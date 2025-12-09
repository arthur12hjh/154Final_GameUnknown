#include "pch.h"

#include "Player.h"
#include "Nayitba.h"

#include "Body_Player.h"
#include "Face_Player.h"
#include "Hair_Player.h"
#include "PonyTail_Player.h"
#include "Weapon.h"
#include "CameraBone_Player.h"

#include "GameInstance.h"
#include "GameManager.h"
#include "Interaction_Component.h"
#include "Effect.h"
#include "Notify.h"
#include "AttackHitBox.h"
#include "PlayerCCTHitReporter.h"
#include "PlayerBehaviorCallback.h"

#include "PlayerFSM.h"
#include "PlayerState.h"
 
CPlayer::CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCharacter {pDevice, pContext}
{
}

CPlayer::CPlayer(const CPlayer& Prototype)
	: CCharacter{ Prototype }
{
}

void CPlayer::Handle_Notify(void* pArg)
{
	ANIM_NOTIFY* pNotify = static_cast<ANIM_NOTIFY*>(pArg);

}

_float CPlayer::Get_AnimationRatio()
{
	return m_pBodyModelCom->Get_AnimationRatio();
}

// 인자로 들어온 스킬 id의 스킬이 사용 가능한지 확인.
// 사용가능하다면 게이지 감소 후 값 리턴.
_bool CPlayer::Use_BetaSkill(_uint iSkillID)
{
	_uint iIdx = 0;
	for (_uint i = 0; i < m_PlayerDesc.iBetaSkillCount; ++i)
	{
		if (m_PlayerDesc.iBetaSkillId[i] == iSkillID)
		{
			_uint iGauge = m_pGameManager->Find_BetaSkillData(iSkillID)->iRequiredBetaGauge;

			if (iGauge <= m_PlayerDesc.iCurrentBetaEnergy)
			{
				m_PlayerDesc.iCurrentBetaEnergy -= iGauge;
				m_PlayerDesc.eBetaSkillState[i] = SKILL_STATE::USE;

				return true;
			}
		}
	}

	return false;
}

_bool CPlayer::Use_RushSkill()
{
	if (m_PlayerDesc.fCurrentRushCoolTime < m_PlayerDesc.fMaxRushCoolTime)
		return false;

	m_PlayerDesc.eRushState = SKILL_STATE::USE; // [JU] Rush Use로 상태 변환
	m_PlayerDesc.fCurrentRushCoolTime = 0.f;

	return S_OK;
}

void CPlayer::Active_SFX(const _wstring& strPartTag, const _wstring& strObjectTag, const ANIM_NOTIFY& NotifyReference)
{
	// 어떤 방식으로 짜야하지?
	// 1) strPartTag가 empty일 경우, Player에서 탐색
	//    strPartTag가 있을 경우, strPartTag로 모델을 탐색
	// 2) strObjectTag라는 매핑된 값을 문자열 탐색해서 찾고, Play() 함수를 실행함
	// 3) 그 모델들은 시간이 지난 후 알아서 Stop()

	if (strPartTag.empty())
	{

	}
	else
	{
		Find_PartObject(strPartTag)->Active_SFX(strObjectTag, NotifyReference);
	}

}

void CPlayer::Activate_PartObject_Collider(const _wstring& strPartTag, const _wstring& strColliderTag, const ANIM_NOTIFY& NotifyRef)
{
	auto pPartObject = Find_PartObject(strPartTag);
	if (nullptr == pPartObject)
		return;

	pPartObject->Activate_PartObject_Collider(strColliderTag, NotifyRef);
}

HRESULT CPlayer::CallNotify(_uint iNotiType, const AnimNotify* pNotify)
{
	CNotify::NOTIFY_TYPE NotiType = CNotify::NOTIFY_TYPE(iNotiType);
	if (CNotify::NOTIFY_TYPE::ACTIVE_COLLISION == NotiType)
	{
		CreateHitBox(pNotify);
	}


	return S_OK;
}

HRESULT CPlayer::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPlayer::Initialize(void* pArg)
{
	CGameManager::GetInstance()->Bind_GameCharacter(this);
	if(FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_PartObjects()))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_PlayerDesc()))
		return E_FAIL;

	if (FAILED(Ready_BetaSkillDesc()))
		return E_FAIL;

	if (FAILED(Ready_FSM()))
		return E_FAIL;

	m_pColliderCom->SetOwner(this);

	return S_OK;
}

void CPlayer::Priority_Update(_float fTimeDelta)
{
	m_pTransformCom->Update_PreWorldMatrix();
	m_pCCT->Update_PrePxPosition(m_pTransformCom);

	__super::Priority_Update(fTimeDelta);
}

void CPlayer::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

	m_pGameManager->Lockon(fTimeDelta);

	Update_TestLogic(fTimeDelta);
	Update_RushSkill(fTimeDelta);
	Update_BetaSkill();
	Update_FSM(fTimeDelta);
	
	// [JU] Use_RushSkill 테스트(마우스 우클릭)
	if (m_pGameInstance->KeyDown(KEY_INPUT::MOUSE, 1))
		Use_RushSkill();

	if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_F))
	{
		auto pInteraction = m_pGameInstance->GetNearInteraction();
		if(pInteraction)
			pInteraction->Action_InteractionEvent(this);
	}

	m_pColliderCom->UpdateColiision(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));
}

void CPlayer::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta); 
	
	//m_pCCT->Update_PxPosition(fTimeDelta, m_pTransformCom);

	m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
	m_pGameInstance->ADD_Collider(m_pColliderCom);

#ifdef _DEBUG
	m_pGameInstance->Add_DebugComponent(m_pColliderCom);
	m_pGameInstance->Add_PhysxGeometry(m_pCCT->Get_PxActor(), m_pCCT->Get_PxShape());
#endif
}

HRESULT CPlayer::Render()
{
	return S_OK;
}

HRESULT CPlayer::Damaged(void* pArg)
{
	DEFAULT_DAMAGE_DESC* pDamageDesc = static_cast<DEFAULT_DAMAGE_DESC*>(pArg);
	const CHARACTER_SKILL_DESC* pSkillDesc = static_cast<const CHARACTER_SKILL_DESC*>(pDamageDesc->pSkillData);

	if (SKILL_TYPE::INTERACTION_SKILL == pSkillDesc->eSkillType)
	{
		CCharacter* pCharacter = static_cast<CCharacter*>(pDamageDesc->pAttacker);

		// 임시입니다 잡기 테스트용 나중에 넘겨받거나 넘겨줄데이터 생기면 말좀해주세요
		// ㄴ 여기서 아마 상태 추가할거같긴 한데 몬스터 본이랑 몬스터 애니메이션 정보 연동해야 될 듯?
		pCharacter->ActionSuccess(nullptr);
	}
	else
	{
		// Interaction 아니라면 따로 뻈음.
		// 안에서 플레이어 모션 제어 중
		Handle_Hit(pDamageDesc, pSkillDesc);
	}

	return S_OK;
}

void CPlayer::RecoveryPoint(RECOVERY_TYPE eRecoveryType, long long iCost)
{
	switch (eRecoveryType)
	{
	case RECOVERY_TYPE::RECOVERY_HP:
	{
		if (0 == iCost)
			m_PlayerDesc.iCurrentHealth = m_PlayerDesc.iMaxHealth;
		else
		{
			m_PlayerDesc.iCurrentHealth += iCost;
			m_PlayerDesc.iCurrentHealth = Clamp<long long>(m_PlayerDesc.iCurrentHealth, 0, m_PlayerDesc.iMaxHealth);
		}
	}
		break;
	case RECOVERY_TYPE::RECOVERY_SHILED:
	{
		if (0 == iCost)
			m_PlayerDesc.iCurrentShield = m_PlayerDesc.iMaxShield;
		else
		{
			m_PlayerDesc.iCurrentShield += iCost;
			m_PlayerDesc.iCurrentShield = Clamp<long long>(m_PlayerDesc.iCurrentShield, 0, m_PlayerDesc.iMaxShield);
		}
	}
		break;
	default :
		return;
	}
}

void CPlayer::SetSkillDataID(_uint iSkillID)
{
	m_iSkillID = iSkillID;
}

_int CPlayer::GetSkillDataID()
{
	return m_iSkillID;
}

void CPlayer::Update_TestLogic(_float fTimeDelta)
{
	m_fTestTimer += fTimeDelta;

	if (m_fTestTimer >= 2.f && m_PlayerDesc.iCurrentBetaEnergy < m_PlayerDesc.iMaxBetaEnergy)
	{
		m_PlayerDesc.iCurrentBetaEnergy++;
		m_fTestTimer = 0.f;
	}
}

HRESULT CPlayer::Ready_Components()
{
	/* Com_Collider_AABB */
	CBoxCollider::BOX_COLLIDER_DESC		AABBDesc{};
	AABBDesc.vSize = _float3(1.5f, 2.f, 1.5f);
	AABBDesc.vCenter = _float3(0.f, AABBDesc.vSize.y * 0.5f, 0.f);

	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_Collider_AABB"), reinterpret_cast<CComponent**>(&m_pColliderCom), &AABBDesc)))
		return E_FAIL;
	m_pColliderCom->SetColliderHitType(HIT_TYPE::PLAYER);

	/* Com_CCT */
	CCharacterController::CCT_DESC Desc;
	PxUserData tUserData;
	tUserData.szActorTag = TEXT("Player_CCT");

	Desc.eCharacterControllerType = CCharacterController::CCT_SHAPE::CAPSULE;
	Desc.tUserData = tUserData;
	Desc.vSize = _float3(2.5f, 2.5f, 0.f);
	XMStoreFloat4(&Desc.vStartPos, m_pTransformCom->Get_State(STATE::POSITION));
	Desc.vMaterial = _float3(0.5f, 0.5f, 0.f);
	Desc.pHitReporter = CPlayerCCTHitReporter::Create();
	Desc.pBehaviorCallback = CPlayerBehaviorCallback::Create();

	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_CharacterController"),
		TEXT("Com_CCT"), reinterpret_cast<CComponent**>(&m_pCCT), &Desc)))
		return E_FAIL;

	m_pGameInstance->Add_CCT_ToPhysx(this, m_pCCT);

	return S_OK;
}

HRESULT CPlayer::Ready_PartObjects()
{
	CBody_Player::BODY_PLAYER_DESC BodyDesc{};
	BodyDesc.pParentTransform = m_pTransformCom;

	/* Part_Body */
	if (FAILED(__super::Add_PartObject(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Body_Player"),
		TEXT("Part_Body"), &BodyDesc)))
		return E_FAIL;

	CBody_Player* pBody = dynamic_cast<CBody_Player*>(Find_PartObject(TEXT("Part_Body")));

	CWeapon::WEAPON_DESC	WeaponDesc{};
	WeaponDesc.pParent = this;
	WeaponDesc.pSocketMatrix = pBody->Get_BoneMatrixPtr("Weapon");
	WeaponDesc.pParentTransform = m_pTransformCom;
	
	/* Part_Weapon */
	if (FAILED(__super::Add_PartObject(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Weapon"),
		TEXT("Part_Weapon"), &WeaponDesc)))
		return E_FAIL;

	m_pWeapon = static_cast<CWeapon *>(Find_PartObject(TEXT("Part_Weapon")));

	CFace_Player::FACE_PLAYER_DESC FaceDesc{};
	FaceDesc.pParentTransform = m_pTransformCom;
	FaceDesc.pBodyPtr = pBody;

	/* Part_Face */
	if (FAILED(__super::Add_PartObject(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Face_Player"),
		TEXT("Part_Face"), &FaceDesc)))
		return E_FAIL;

	CHair_Player::HAIR_PLAYER_DESC HairDesc{};
	HairDesc.pParentTransform = m_pTransformCom;
	HairDesc.pBodyPtr = pBody;
	
	/* Part_Hair */
	if (FAILED(__super::Add_PartObject(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Hair_Player"),
		TEXT("Part_Hair"), &HairDesc)))
		return E_FAIL;
	
	CPonyTail_Player::PONYTAIL_PLAYER_DESC PonyTailDesc{};
	PonyTailDesc.pParentTransform = m_pTransformCom;
	PonyTailDesc.pBodyPtr = pBody;
	
	/* Part_PonyTail */
	if (FAILED(__super::Add_PartObject(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_PonyTail_Player"),
		TEXT("Part_PonyTail"), &PonyTailDesc)))
		return E_FAIL;
	
	CCameraBone_Player::CAMERABONE_DESC CameraBoneDesc{};
	CameraBoneDesc.pParentTransform = m_pTransformCom;
	CameraBoneDesc.pSocketMatrix = pBody->Get_BoneMatrixPtr("Root");
	CameraBoneDesc.pCharacter = this;
	
	/* Part_CameraBone */
	if (FAILED(__super::Add_PartObject(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_CameraBone_Player"),
		TEXT("Part_CameraBone"), &CameraBoneDesc)))
		return E_FAIL;

	Import_ModelPtr();
	m_pNotifyCom->Set_ModelCom(m_pBodyModelCom);

	return S_OK;
}

HRESULT CPlayer::Ready_PlayerDesc()
{
	m_PlayerDesc.iMaxHealth = 100;
	m_PlayerDesc.iMaxShield = 100;
	
	//베타 에너지
	m_PlayerDesc.iMaxBetaEnergy = 20;
	m_PlayerDesc.iCurrentBetaEnergy = 0;

	m_PlayerDesc.iCurrentHealth = 100;
	m_PlayerDesc.iCurrentShield = 100;
	m_PlayerDesc.iCurrentShieldATK = 100;

	m_PlayerDesc.fCurrentCTDamage = 100.f;
	m_PlayerDesc.fCurrentCTPercent = 100.f;
	
	m_PlayerDesc.fCurrentLinkApplyDamage = 100.f;
	m_PlayerDesc.iCurrentAttackPoint = 100;

	m_PlayerDesc.iCurrentPotions = 3;
	m_PlayerDesc.iMaxPotions = 3;

	m_PlayerDesc.eRushState = SKILL_STATE::DEFAULT;
	m_PlayerDesc.fMaxRushCoolTime = 5.f;
	m_PlayerDesc.fCurrentRushCoolTime = 0.f;

	//Default가 비활성화 상태
	memset(m_PlayerDesc.iBetaSkillId, 0, sizeof(_uint) * 4);

	m_PlayerDesc.eBetaSkillState[0] = SKILL_STATE::DEFAULT;
	m_PlayerDesc.eBetaSkillState[1] = SKILL_STATE::DEFAULT;
	m_PlayerDesc.eBetaSkillState[2] = SKILL_STATE::DEFAULT;
	m_PlayerDesc.eBetaSkillState[3] = SKILL_STATE::DEFAULT;

	m_PlayerDesc.pPlayerController = m_pCCT;
	m_PlayerDesc.pPlayerTransform   = m_pTransformCom;
	m_PlayerDesc.ePlayerMode = PLAYER_MODE::IDLE;

	return S_OK;
}

HRESULT CPlayer::Ready_BetaSkillDesc()
{
	//추후 추가 예정
	m_PlayerDesc.iBetaSkillCount = 2;
	m_PlayerDesc.iBetaSkillId[0] = 1004;
	m_PlayerDesc.iBetaSkillId[1] = 1005;

	return S_OK;
}

HRESULT CPlayer::Ready_FSM()
{
	m_pFSM = CPlayerFSM::Create();

	return S_OK;
}

void CPlayer::Update_FSM(_float fTimeDelta)
{
	m_pFSM->Update(fTimeDelta);
}

void CPlayer::Update_RushSkill(_float fTimeDelta)
{
	// active on 상태라면 active로 다시 전환
	if (SKILL_STATE::ACTIVE_ON == m_PlayerDesc.eRushState)
		m_PlayerDesc.eRushState = SKILL_STATE::ACTIVE;

	// use 상태라면 Default로 다시 전환
	if (SKILL_STATE::USE == m_PlayerDesc.eRushState)
	{
		m_PlayerDesc.fCurrentRushCoolTime = 0.f;
		m_PlayerDesc.eRushState = SKILL_STATE::DEFAULT;
	}

	// [JU] Default일 때만 쿨타임 증가
	if (m_PlayerDesc.fCurrentRushCoolTime < m_PlayerDesc.fMaxRushCoolTime)
	{
		m_PlayerDesc.fCurrentRushCoolTime += fTimeDelta;
		m_PlayerDesc.eRushState = SKILL_STATE::DEFAULT;
	}
	else if (m_PlayerDesc.fCurrentRushCoolTime >= m_PlayerDesc.fMaxRushCoolTime
		&& m_PlayerDesc.eRushState != SKILL_STATE::ACTIVE) // [JU] 쿨타임 다 차면 활성화(active on)
	{
		m_PlayerDesc.eRushState = SKILL_STATE::ACTIVE_ON;
		m_PlayerDesc.fCurrentRushCoolTime = m_PlayerDesc.fMaxRushCoolTime;
	}
}

void CPlayer::Update_BetaSkill()
{
	_uint iIdx = 0;

	for (_uint i = 0; i < m_PlayerDesc.iBetaSkillCount; ++i)
	{
		_uint iGauge = m_pGameManager->Find_BetaSkillData(m_PlayerDesc.iBetaSkillId[i])->iRequiredBetaGauge;

		if (SKILL_STATE::USE == m_PlayerDesc.eBetaSkillState[i] && iGauge > m_PlayerDesc.iCurrentBetaEnergy)
			m_PlayerDesc.eBetaSkillState[i] = SKILL_STATE::DEFAULT;

		else if (SKILL_STATE::USE == m_PlayerDesc.eBetaSkillState[i] && iGauge <= m_PlayerDesc.iCurrentBetaEnergy)
			m_PlayerDesc.eBetaSkillState[i] = SKILL_STATE::ACTIVE;

		else if (SKILL_STATE::DEFAULT == m_PlayerDesc.eBetaSkillState[i] && iGauge <= m_PlayerDesc.iCurrentBetaEnergy)
			m_PlayerDesc.eBetaSkillState[i] = SKILL_STATE::ACTIVE_ON;

		else if (SKILL_STATE::ACTIVE_ON == m_PlayerDesc.eBetaSkillState[i] && iGauge <= m_PlayerDesc.iCurrentBetaEnergy)
			m_PlayerDesc.eBetaSkillState[i] = SKILL_STATE::ACTIVE;
	}
}

void CPlayer::Handle_Hit(DEFAULT_DAMAGE_DESC* pDamageDesc, const CHARACTER_SKILL_DESC* pSkillDesc)
{
	_float3 vHitDir{}, vHitPoint{}, vImpactDir{};
	_float4 vAttackerPos{};
	_float fImpactForce;

	XMStoreFloat4(&vAttackerPos, pDamageDesc->pAttacker->GetTransform()->Get_State(STATE::POSITION));

	PLAYER_HIT_DESC HitDesc;
	memcpy(&HitDesc.fImpact, &pDamageDesc->fImpactForce, sizeof(_float));
	memcpy(&HitDesc.vHitDir, &pDamageDesc->vHitDir, sizeof(_float3));
	memcpy(&HitDesc.vHitPoint, &pDamageDesc->vHitPoint, sizeof(_float3));
	memcpy(&HitDesc.vImpactDir, &pDamageDesc->vImpactDir, sizeof(_float3));
	memcpy(&HitDesc.vAttackerPos, &vAttackerPos, sizeof(_float4));

	m_PlayerDesc.iCurrentHealth -= pSkillDesc->iSkillDamage;

	if (0 >= m_PlayerDesc.iCurrentHealth)
		m_PlayerDesc.iCurrentHealth = 0.f;

	// 패리 성공. 만약 몬스터 팅겨나는거 제어하고 싶으면
	// 이 분기문 안에서 Attacker 정보 있으니까 그걸로 제어하면 될듯
	if (true == m_PlayerDesc.isJustParryable)
	{
		PLAYER_TRANSITION_DESC Desc{};
		Desc.isChangeMode = false;
		Desc.eNextState = PLAYER_STATE::PARRY_SUCCESS;
		Desc.pArg = &HitDesc;

		m_pFSM->Handle_Transition(Desc);

		Default_Damage_Desc DamageDesc = {};
		DamageDesc.pAttacker = this;

		auto pNayitba = static_cast<CNayitba*>(pDamageDesc->pAttacker);
		if (NAYTIBA_TYPE::ELITE <= pNayitba->GetStaticMonsterData()->eNaytiba_Type)
		{
			if (ATTACK_DIRECTION::ATK_LEFT == pSkillDesc->eATK_Direction)
				DamageDesc.pSkillData = m_pGameManager->Find_SkillData(1009);
			else if (ATTACK_DIRECTION::ATK_RIGHT == pSkillDesc->eATK_Direction)
				DamageDesc.pSkillData = m_pGameManager->Find_SkillData(1008);
		}
		else
			DamageDesc.pSkillData = m_pGameManager->Find_SkillData(1008);

		pNayitba->Damaged(&DamageDesc);
		m_pGameInstance->GamePauseDurationTime(3.f, 0.1f, 3.f);
	}
	// 가드만 성공
	else if (true == m_PlayerDesc.isParryable)
	{
		PLAYER_TRANSITION_DESC Desc{};
		Desc.isChangeMode = false;
		Desc.eNextState = PLAYER_STATE::PARRY_GUARD;
		Desc.pArg = &HitDesc;

		m_pFSM->Handle_Transition(Desc);
	}

	else if (false == m_PlayerDesc.isSuperArmor)
	{
		PLAYER_TRANSITION_DESC Desc{};
		Desc.isChangeMode = false;
		Desc.eNextState = PLAYER_STATE::HIT;
		Desc.pArg = &HitDesc;

		if (m_pWeapon)
		{
			m_iSkillID = -1;
			m_pWeapon->EnableCollider(false);
		}

		m_pFSM->Handle_Transition(Desc);
	}
}

void CPlayer::CreateHitBox(const AnimNotify* pNotify)
{
	CAttackHitBox::HIT_BOX_DESC HitBoxDesc = {};
	// 공격자. 싱글톤 매니저에 연산할 때 넘겨야함
	HitBoxDesc.pAttacker = this;

	// 현재 레벨
	_uint iGameLevel = ENUM_CLASS(LEVEL::GAMEPLAY);
	// 프로토타입명, 레이어명
	_wstring szProtoType(pNotify->szNotifyArg01.begin(), pNotify->szNotifyArg01.end());
	_wstring szLayerName(pNotify->szNotifyArg02.begin(), pNotify->szNotifyArg02.end());

	CAttackHitBox::HIT_BOX_DESC pHitBoxDesc = {};
	// SkillData 엑셀파일에 있는 첫 번째값. SKill ID
	auto pSkillData = m_pGameManager->Find_SkillData(pNotify->iNumData01);
	pHitBoxDesc.pData = pSkillData;

	// 충돌 타입, 공격타입. SKILL_DESC에 있는 애랑 별개로 세팅해줘야하는 애임
	pHitBoxDesc.eColType = COLLIDER(pNotify->iNumData02);
	pHitBoxDesc.eHitBoxType = HIT_TYPE(pNotify->iNumData03);
	pHitBoxDesc.eHitObjectType = HIT_TYPE(pNotify->iNumData04);
	pHitBoxDesc.bIsApplyTransform = true;
	pHitBoxDesc.pAttacker = this;

	// 얘는 여기 종속돼있네? 옹히려좋아
	pHitBoxDesc.vScale = pSkillData->vHitBoxExtents;
	pHitBoxDesc.fImpactForce = m_fImpactForce;

	// 이건 여기서 알아해주는거같으니까 상관 안해도 되겟다
	_vector vCharacterPos = GetTransform()->Get_State(STATE::POSITION);
	_vector vCharacterLook = GetTransform()->Get_State(STATE::LOOK);
	vCharacterPos += vCharacterLook * pSkillData->fRange;
	XMStoreFloat3(&pHitBoxDesc.vPosition, vCharacterPos);

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(iGameLevel, szProtoType.c_str(),
		iGameLevel, szLayerName.c_str(), &pHitBoxDesc)))
		return;
}

CPlayer* CPlayer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPlayer* pInstance = new CPlayer(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CPlayer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPlayer::Clone(void* pArg)
{
	CPlayer* pInstance = new CPlayer(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CPlayer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);

	Safe_Release(m_pFSM);
}


//m_fTime += fTimeDelta;
//if (5 <= m_fTime) {
//	CEffect::EFFECT_TRANSFORM_DESC desc;
//	desc.fRotationPerSec = 1.f;
//	desc.fSpeedPerSec = 1.f;
//	desc.pRootMatrix = m_pTransformCom->Get_WorldMatrixPtr();
//	desc.vPos = XMVectorSet(0, 0, 0, 1);
//	desc.fRot = _float3(0, 0, 0);
//	desc.fSize = 1.f;
//	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Effect_Slash"),
//		ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_Effect"), &desc)))
//		return;
//
//	desc.fRotationPerSec = 1.f;
//	desc.fSpeedPerSec = 1.f;
//	desc.pWorldMatrix = m_pTransformCom->Get_WorldMatrixPtr();
//	desc.pRootMatrix = m_pBody->Get_BoneMatrixPtr("Bip001-R-Hand");
//	desc.vPos = XMVectorSet(0, 0, 0, 1);
//	desc.fRot = _float3(0, 0, 0);
//	desc.fSize = 30.f;
//	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Effect_SheildBreak_Yellow"),
//		ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_Effect"), &desc)))
//		return;
//
//	desc.fRotationPerSec = 1.f;
//	desc.fSpeedPerSec = 1.f;
//	desc.pWorldMatrix = m_pTransformCom->Get_WorldMatrixPtr();
//	desc.pRootMatrix = m_pBody->Get_BoneMatrixPtr("Bip001-L-Hand");
//	desc.vPos = XMVectorSet(0, 0, 0, 1);
//	desc.fRot = _float3(0, 0, 0);
//	desc.fSize = 15.f;
//	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Effect_Spectrum_Test"),
//		ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_Effect"), &desc)))
//		return;
//
//	desc.fRotationPerSec = 1.f;
//	desc.fSpeedPerSec = 1.f;
//	desc.pWorldMatrix = nullptr;
//	desc.pRootMatrix = nullptr;
//	desc.vPos = XMVectorSet(5, 3, 0, 1);
//	desc.fRot = _float3(0, 0, 0);
//	desc.fSize = 2.f;
//	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Effect_Hit_Spark"),
//		ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_Effect"), &desc)))
//		return;
//
//	m_fTime = 0.f;
//}
//m_pTrail->Update_Trail(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()), fTimeDelta, true);