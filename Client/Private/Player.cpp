#include "pch.h"

#include "Player.h"
#include "Nayitba.h"

#include "Body_Player.h"
#include "Face_Player.h"
#include "Hair_Player.h"
#include "PonyTail_Player.h"
#include "Hairpin_Player.h"
#include "Weapon.h"
#include "CameraBone_Player.h"

#include "GameInstance.h"
#include "GameManager.h"
#include "InteractionBinder.h"
#include "Effect.h"
#include "Notify.h"
#include "AttackHitBox.h"
#include "PlayerCCTHitReporter.h"
#include "PlayerBehaviorCallback.h"
#include "PlayerCCTQueryFilterCallback.h"

#include "PlayerFSM.h"
#include "Player_Parts.h"
#include "PlayerState.h"
#include "Prob_Interaction.h"
 
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
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	SAVE_LEVEL_PLAYERDATA SaveData = {  };
	if (false == m_pGameManager->LoadPlayerDesc(SaveData))
	{
		if (FAILED(Ready_PlayerDesc()))
			return E_FAIL;
	}
	else
	{
		m_PlayerDesc = SaveData.PlayerData;

		_vector vOldPos = XMLoadFloat3(&SaveData.vOldPosition);
		m_pTransformCom->Set_State(STATE::POSITION, vOldPos);
	}

	if (FAILED(Ready_PartObjects()))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_BetaSkillDesc()))
		return E_FAIL;

	if (FAILED(Ready_FSM()))
		return E_FAIL;

	m_pColliderCom->SetOwner(this);
	m_PlayerDesc.pPlayerTransform = m_pTransformCom;
	m_PlayerDesc.pPlayerController = m_pCCT;

	/* 시작할땐 꺼두자 */
	SetVisibility(VISIBILITY::HIDDEN);
	MotionTrailEnable(false);

	//CEffect::EFFECT_TRANSFORM_DESC EffectDesc;
	//EffectDesc.fRotationPerSec = 1.f;
	//EffectDesc.fSpeedPerSec = 1.f;
	//
	//EffectDesc.pRootMatrix = nullptr;
	//EffectDesc.pWorldMatrix = nullptr;
	//
	//EffectDesc.vPos = GetTransform()->Get_State(STATE::POSITION);
	//EffectDesc.fRot = _float3(0, 0, 0);
	//EffectDesc.fSize = 0.9f;
	//
	//static_cast<CEffect*>(m_pGameInstance->Add_Get_GameObject(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Effect_Sakura"),
	//	ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_Effect"), &EffectDesc));


	return S_OK;
}

void CPlayer::Priority_Update(_float fTimeDelta)
{
	if (false == m_bIsActive)
		return;

	m_pTransformCom->Update_PreWorldMatrix();
	m_pCCT->Update_PrePxPosition(m_pTransformCom);

	__super::Priority_Update(fTimeDelta);
}

void CPlayer::Update(_float fTimeDelta)
{
	if (false == m_bIsActive)
		return;

	__super::Update(fTimeDelta);

	m_pGameManager->Lockon(fTimeDelta);

	Update_TestLogic(fTimeDelta);

	//링크 어택이 가장 최우선 판정으로 들어간다.
	Update_LinkAttack(fTimeDelta);
	Update_RushSkill(fTimeDelta);
	Update_BetaSkill(fTimeDelta);
	Update_FSM(fTimeDelta);
	Update_Interaction(fTimeDelta);
	Update_PotionUse(fTimeDelta);
	Update_ReactionSkills(fTimeDelta);
	//일단 테스트 입력 최우선 처리
	Update_ReactionSkillInput(fTimeDelta);

	// [JU] Use_RushSkill 테스트(키보드 R키)
	if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_R))
		Use_RushSkill();

	m_pColliderCom->UpdateColiision(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));
}

void CPlayer::Late_Update(_float fTimeDelta)
{
	if (false == m_bIsActive)
		return;

	m_pCCT->Update_PxPosition(fTimeDelta, m_pTransformCom);

	m_pGameInstance->ADD_Collider(m_pColliderCom);

#ifdef _DEBUG
	m_pGameInstance->Add_DebugComponent(m_pColliderCom);
#endif
	__super::Late_Update(fTimeDelta);
}

HRESULT CPlayer::Render()
{
	return S_OK;
}

HRESULT CPlayer::Render_Shadow()
{

	return S_OK;
}

HRESULT CPlayer::Render_MotionBlur()
{
	return S_OK;
}

HRESULT CPlayer::Damaged(void* pArg)
{
	if (true == m_PlayerDesc.isInvincible || VISIBILITY::HIDDEN == m_eVisibility || false == m_bIsActive)
		return S_OK;

	DEFAULT_DAMAGE_DESC* pDamageDesc = static_cast<DEFAULT_DAMAGE_DESC*>(pArg);
	const CHARACTER_SKILL_DESC* pSkillDesc = static_cast<const CHARACTER_SKILL_DESC*>(pDamageDesc->pSkillData);

	// Interaction 아니라면 따로 뻈음.
	// 안에서 플레이어 모션 제어 중
	Calc_Damage(pDamageDesc, pSkillDesc);
	Handle_Hit(pDamageDesc, pSkillDesc);

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
/*
데스크 받아와서 iFrame 만큼의 프레임 동안은 
저회, 리펄스, 블링크가 가능한 상태로 바꿔준다.
*/
void CPlayer::Attack_Interaction(void* pArg)
{
	ATK_INTERACTION_DESC* pNotifyDesc = static_cast<ATK_INTERACTION_DESC*>(pArg);
	// 보이드 포인터는 혹시 몰라서 받아온거니까 따로 당장 처리하지 않음.
	// PERFECT_DOGE, BLINK, REPULSE;
	ATK_INTERACTION_TYPE eType = pNotifyDesc->eInteraction_Type;
	// 프레임 단위 판정이니까.. 키 입력을 프레임 단위로 판정해야되나?
	_uint iFrame = pNotifyDesc->iFrameCnt;

	m_PlayerDesc.iLeftReactionSkillFrameAcc = iFrame;
	m_PlayerDesc.eReactionType = eType;
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
	m_fShieldTimer += fTimeDelta;

	if (m_PlayerDesc.iCurrentBetaEnergy < m_PlayerDesc.iMaxBetaEnergy && m_fTestTimer >= 2.0)
	//if (m_fTestTimer >= 5.f && m_PlayerDesc.iCurrentBetaEnergy < m_PlayerDesc.iMaxBetaEnergy)
	{
		m_PlayerDesc.iCurrentBetaEnergy++;
		m_fTestTimer = 0.f;
	}

	if (m_PlayerDesc.iCurrentShield < m_PlayerDesc.iMaxShield && m_fShieldTimer >= 10.0f)
		//if (m_fTestTimer >= 5.f && m_PlayerDesc.iCurrentBetaEnergy < m_PlayerDesc.iMaxBetaEnergy)
	{
		m_PlayerDesc.iCurrentShield += 4;
		m_fShieldTimer = 0.f;
	}
}

/* 이 함수 내용물 건드리기 ㄴㄴ 건드릴거면 디코좀 */
void CPlayer::Update_ReactionSkillInput(_float fTimeDelta)
{
	// 테스트 코드
	if (true == m_PlayerDesc.HasTarget)
	{
		if (true == m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_NUMPAD7))
		{
			PLAYER_TRANSITION_DESC Desc;
			Desc.eNextState = PLAYER_STATE::REPULSE;

			m_pFSM->Handle_Transition(Desc);
		}
		if (true == m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_NUMPAD9))
		{
			PLAYER_TRANSITION_DESC Desc;
			Desc.eNextState = PLAYER_STATE::BLINK_START;

			m_pFSM->Handle_Transition(Desc);
		}
	}

	/* 
	실제 로직이니까 날리지 마세요
	*/  
	if (true == m_PlayerDesc.HasTarget)
	{
		if (true == m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_W) &&
			true == m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_LSHIFT) &&
			ATK_INTERACTION_TYPE::BLINK == m_PlayerDesc.eReactionType)
		{
			PLAYER_TRANSITION_DESC Desc;
			Desc.eNextState = PLAYER_STATE::BLINK_START;

			m_pFSM->Handle_Transition(Desc);
		}
		if (true == m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_S) &&
			true == m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_LSHIFT) &&
			ATK_INTERACTION_TYPE::REPULSE == m_PlayerDesc.eReactionType)
		{
			PLAYER_TRANSITION_DESC Desc;
			Desc.eNextState = PLAYER_STATE::REPULSE;

			m_pFSM->Handle_Transition(Desc);
		}
	}
}

HRESULT CPlayer::Ready_Components()
{
	/* Com_Collider_AABB */
	CBoxCollider::BOX_COLLIDER_DESC		AABBDesc{};
	AABBDesc.vSize = _float3(1.5f, 2.5f, 1.5f);
	AABBDesc.vCenter = _float3(0.f, AABBDesc.vSize.y, 0.f);

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
	Desc.pQueryFilterCallback = CPlayerCCTQueryFilterCallback::Create();
	Desc.iCollisionGroup = PHYSX_CCT;
	Desc.iCollisionMask &= ~(PHYSX_CUSTOM_3);

	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_CharacterController"),
		TEXT("Com_CCT"), reinterpret_cast<CComponent**>(&m_pCCT), &Desc)))
		return E_FAIL;

	m_pGameInstance->Add_CCT_ToPhysx(this, m_pCCT);

#ifdef _DEBUG
	m_pGameInstance->Set_PVDRender_Off();
#endif
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
	// 바디가 생성 되자마자 세팅.
	// 이래야 다른 파트오브젝트에서 바디를 참조 가능하지 ㅇㅇ
	Import_ModelPtr();
	m_pNotifyCom->Set_ModelCom(m_pBodyModelCom);

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
	
	
	CHairpin_Player::HAIRPIN_PLAYER_DESC HairpinDesc{};
	HairpinDesc.pParentTransform = m_pTransformCom;
	HairpinDesc.pBodyPtr = pBody;
	
	/* Part_PonyTail */
	if (FAILED(__super::Add_PartObject(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Hairpin_Player"),
		TEXT("Part_Hairpin"), &HairpinDesc)))
		return E_FAIL;
	
	CCameraBone_Player::CAMERABONE_DESC CameraBoneDesc{};
	CameraBoneDesc.pParentTransform = m_pTransformCom;
	CameraBoneDesc.pSocketMatrix = pBody->Get_BoneMatrixPtr("Root");
	CameraBoneDesc.pCharacter = this;
	
	/* Part_CameraBone */
	if (FAILED(__super::Add_PartObject(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_CameraBone_Player"),
		TEXT("Part_CameraBone"), &CameraBoneDesc)))
		return E_FAIL;

	m_pCameraBone = static_cast<CCameraBone_Player*>(Find_PartObject(TEXT("Part_CameraBone")));

	return S_OK;
}

HRESULT CPlayer::Ready_PlayerDesc()
{
	
	m_PlayerDesc.iMaxHealth = 250;
	m_PlayerDesc.iMaxShield = 100;
	
	//베타 에너지
	m_PlayerDesc.iMaxBetaEnergy = 20;
	m_PlayerDesc.iCurrentBetaEnergy = 0;

	m_PlayerDesc.iCurrentHealth = 250;
	m_PlayerDesc.iCurrentShield = 100;
	m_PlayerDesc.iCurrentShieldATK = 100;

	m_PlayerDesc.fCurrentCTDamage = 100.f;
	m_PlayerDesc.fCurrentCTPercent = 100.f;
	
	m_PlayerDesc.fCurrentLinkApplyDamage = 100.f;
	m_PlayerDesc.iCurrentAttackPoint = 100;

	m_PlayerDesc.iCurrentPotions = 5;
	m_PlayerDesc.iMaxPotions = 5;

	m_PlayerDesc.eRushState = SKILL_STATE::DEFAULT;
	m_PlayerDesc.fMaxRushCoolTime = 5.f;
	m_PlayerDesc.fCurrentRushCoolTime = 0.f;

	//Default가 비활성화 상태
	memset(m_PlayerDesc.iBetaSkillId, 0, sizeof(_uint) * 4);

	m_PlayerDesc.eBetaSkillState[0] = SKILL_STATE::DEFAULT;
	m_PlayerDesc.eBetaSkillState[1] = SKILL_STATE::DEFAULT;
	m_PlayerDesc.eBetaSkillState[2] = SKILL_STATE::DEFAULT;
	m_PlayerDesc.eBetaSkillState[3] = SKILL_STATE::DEFAULT;

	m_PlayerDesc.ePlayerMode		= PLAYER_MODE::IDLE;

	m_PlayerDesc.iOwnGold			= 5000;

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

//베타스킬 상태 변경
void CPlayer::Update_BetaSkill(_float fTimeDelta)
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

		else if (SKILL_STATE::ACTIVE == m_PlayerDesc.eBetaSkillState[i] && iGauge > m_PlayerDesc.iCurrentBetaEnergy)
			m_PlayerDesc.eBetaSkillState[i] = SKILL_STATE::DEFAULT;

	}
}

//인터랙션 관련 처리 (키입력)
void CPlayer::Update_Interaction(_float fTimeDelta)
{
	if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_F))
	{
		auto pInteractionCom = dynamic_cast<CInteractionBinder*>(m_pGameInstance->GetNearInteraction());
		if (nullptr == pInteractionCom)
			return;

		//CProb_Interaction* pInteractionObject = static_cast<CProb_Interaction*>(pInteractionCom->GetOwner());

 		const INTERACTION_DATA* pInteractionData = pInteractionCom->Get_InterDesc();
		INTERACTION_STATE InteractionState = pInteractionCom->Get_InterState();

		switch (InteractionState)
		{
		case INTERACTION_STATE::DEFAULT:
			pInteractionCom->Action_InteractionEvent(fTimeDelta, this);
			break;
		case INTERACTION_STATE::CONTACT:
		{
			PLAYER_TRANSITION_DESC Desc;
			Desc.isChangeMode = false;
			Desc.pArg = pInteractionCom;

			switch (pInteractionData->eType)
			{
			// 만약 서플라이 박스라면 (발로 차는 모션)
			case INTERACTION_TYPE::SUPPLY_BOX:
			{
				Desc.eNextState = PLAYER_STATE::SUPPLYBOX_INTERACTION;
				m_pFSM->Handle_Transition(Desc);
				break;
			}
			case INTERACTION_TYPE::CORPSE:
			{
				Desc.eNextState = PLAYER_STATE::CORPSE_INTERACTION;
				m_pFSM->Handle_Transition(Desc);
				break;

			}
			case INTERACTION_TYPE::ITEM:
			case INTERACTION_TYPE::DOOR:
			case INTERACTION_TYPE::LIFT_CONTROLLER:
			case INTERACTION_TYPE::NPC:
			case INTERACTION_TYPE::VENDING_MACINE:
			{
				pInteractionCom->Action_InteractionEvent(fTimeDelta, this);
				break;
			}
			}
		}
		// 끝났거나 잠겨있다면, 그냥 Break 처리.
		// 디폴트여도 상호작용은 안되니까 Break 처리.
		case INTERACTION_STATE::LOCK:
		case INTERACTION_STATE::END:
			break;
		}
	}
	else if (m_pGameInstance->KeyUp(KEY_INPUT::KEYBOARD, DIK_F))
	{
		auto pInteractionCom = dynamic_cast<CInteractionBinder*>(m_pGameInstance->GetNearInteraction());
		if (nullptr == pInteractionCom)
			return;

		if (0.f < pInteractionCom->Get_InterDesc()->fInteractionTime)
			pInteractionCom->Reset_Interaction();
	}
}

void CPlayer::Update_PotionUse(_float fTimeDelta)
{
	m_PlayerDesc.fCurrentPotionCoolDown += fTimeDelta;

	if (m_PlayerDesc.fCurrentPotionCoolDown >= m_PlayerDesc.fPotionCoolDown)
		m_PlayerDesc.fCurrentPotionCoolDown = m_PlayerDesc.fPotionCoolDown;

	if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_Q))
	{
		/* 여기서 포션 사용 이펙트 쏴줘 */
		if (m_PlayerDesc.fPotionCoolDown <= m_PlayerDesc.fCurrentPotionCoolDown &&
			m_PlayerDesc.iCurrentPotions > 0 && m_PlayerDesc.iCurrentHealth < m_PlayerDesc.iMaxHealth)
		{
			m_PlayerDesc.iCurrentPotions--;
			m_PlayerDesc.fPotionCoolDown = 0.f;
			m_PlayerDesc.iCurrentHealth += m_PlayerDesc.iMaxHealth / 2.f;
			
			CEffect::EFFECT_TRANSFORM_DESC EffectDesc;
			EffectDesc.fRotationPerSec = 1.f;
			EffectDesc.fSpeedPerSec = 1.f;

			EffectDesc.pRootMatrix = m_pTransformCom->Get_WorldMatrixPtr();
			EffectDesc.vPos = XMVectorSet(0, 1.5f, 0, 1);
			EffectDesc.fRot = _float3(0, 0, 0);
			EffectDesc.fSize = 9.f;
			CEffect* pEffect = static_cast<CEffect*>(m_pGameInstance->Add_Get_GameObject(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Effect_Heal"),
				ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_Effect"), &EffectDesc));
			
			if(m_PlayerDesc.iCurrentHealth >= m_PlayerDesc.iMaxHealth)
				m_PlayerDesc.iCurrentHealth = m_PlayerDesc.iMaxHealth;
		}
	}
}

void CPlayer::Update_LinkAttack(_float fTimeDelta)
{
	if (false == m_PlayerDesc.isLinkAttackAvailable || nullptr == m_PlayerDesc.pLinkAttackTarget)
		return;

	if (m_pGameInstance->KeyDown(KEY_INPUT::MOUSE, ENUM_CLASS(MOUSEKEYSTATE::RBUTTON)))
	{
		NAYITBA_EXECUTION_TYPE eExecutionState = m_PlayerDesc.pLinkAttackTarget->bIsThesholdAction();

		if (NAYITBA_EXECUTION_TYPE::EXECUTION_ATTACK == eExecutionState)
			Execution_Nayitba();
		else
			LinkAttack_Nayitba(eExecutionState);
	}
}

void CPlayer::Update_ReactionSkills(_float fTimeDelta)
{
	//0보다 크다면 프레임 계속 감소.
	if (0 < m_PlayerDesc.iLeftReactionSkillFrameAcc)
		m_PlayerDesc.iLeftReactionSkillFrameAcc--; 
	// END로 바꿔
	else if (0 == m_PlayerDesc.iLeftReactionSkillFrameAcc)
		m_PlayerDesc.eReactionType = ATK_INTERACTION_TYPE::END;
}

void CPlayer::Handle_Hit(DEFAULT_DAMAGE_DESC* pDamageDesc, const CHARACTER_SKILL_DESC* pSkillDesc)
{
	//무적이면 충돌처리 안하게 처리

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

	PLAYER_TRANSITION_DESC Desc{};
	CHARACTER_SKILL_DESC SkillDescCopy{};
	Default_Damage_Desc DamageDesc = {};
	//만약 그랩스킬이라면
	//폐기했으니까 무시
	switch (pSkillDesc->eSkillType)
	{
	case SKILL_TYPE::INTERACTION_SKILL:
		{
		//memcpy(&SkillDescCopy, pSkillDesc, sizeof(CHARACTER_SKILL_DESC));

		//Desc.isChangeMode = false;
		//Desc.eNextState = PLAYER_STATE::GRAB;
		//Desc.pArg = &SkillDescCopy;

		////공격한 녀석의 본 이름 & 객체 포인터 들고옴
		//auto pNayitba = static_cast<CNaytiba*>(pDamageDesc->pAttacker);
		//m_PlayerDesc.pGrabBone = pNayitba->Get_BodyModelCom()
		//	->Get_BoneMatrixPtr(pSkillDesc->szLinkBoneName);

		//m_PlayerDesc.pGrabAttackter = pDamageDesc->pAttacker;

		//pNayitba->ActionSuccess(nullptr);
		//m_pFSM->Handle_Transition(Desc);
		}
		break;
	default:
		if (true == m_PlayerDesc.isJustParryable)
		{
			Desc.isChangeMode = false;
			Desc.eNextState = PLAYER_STATE::PARRY_SUCCESS;
			Desc.pArg = &HitDesc;
			
			m_pFSM->Handle_Transition(Desc);
			DamageDesc.pAttacker = this;

			auto pNayitba = static_cast<CNaytiba*>(pDamageDesc->pAttacker);
			if (0 != pSkillDesc->iSkillID)
			{
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
			}
		
			//m_pGameInstance->GamePauseDurationTime(2.f, 0.7f, 2.5f);
		}
		// 가드만 성공
		else if (true == m_PlayerDesc.isParryable)
		{
			Desc.isChangeMode = false;
			Desc.eNextState = PLAYER_STATE::PARRY_GUARD;
			Desc.pArg = &HitDesc;

			m_pFSM->Handle_Transition(Desc);
		}

		else if (false == m_PlayerDesc.isSuperArmor)
		{
			//타격당할때 공격자를 바라보게 한다.
			m_pTransformCom->LookAt(XMVectorSetY((XMLoadFloat4(&HitDesc.vAttackerPos)), 
				XMVectorGetY(m_pTransformCom->Get_State(STATE::POSITION))));

			Desc.isChangeMode = false;
			Desc.eNextState = PLAYER_STATE::HIT;
			Desc.pArg = &HitDesc;

			if (m_pWeapon)
			{
				m_iSkillID = -1;
				m_pWeapon->EnableCollider(false);
			}

			m_pFSM->Handle_Transition(Desc);
			m_pGameInstance->Shake_Camera(0.2f, 0.2f);
		}
		break;
	}

}

/* 실드 연산 로직 */
void CPlayer::Calc_Damage(DEFAULT_DAMAGE_DESC* pDamageDesc, const CHARACTER_SKILL_DESC* pSkillDesc)
{
	_uint fOriginDamage = pSkillDesc->iSkillDamage;
	_float fShieldDamage = { 0 };
	_float fRemainDamage = { 0 };

	_float fHealthDamage = { 0 };

	fShieldDamage = ceil(fOriginDamage * 0.3f);

	m_PlayerDesc.iCurrentShield -= fShieldDamage;
	if (0 >= m_PlayerDesc.iCurrentShield)
	{
		fRemainDamage = fabsf(m_PlayerDesc.iCurrentShield);
		m_PlayerDesc.iCurrentShield = 0.f;
	}

	//실제로 적용된 데미지만 뺴준다.
	fHealthDamage = fOriginDamage - fShieldDamage + fRemainDamage;
	m_PlayerDesc.iCurrentHealth -= fHealthDamage;
	if (0 >= m_PlayerDesc.iCurrentHealth)
		m_PlayerDesc.iCurrentHealth = 0.f;
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

	auto pHitBox = m_pGameManager->SetActivePoolObject(ENUM_CLASS(LEVEL::STATIC), ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("GamePlay_Layer_HitBox"), TEXT("Hit_Box"));
	if(pHitBox)
		static_cast<CAttackHitBox*>(pHitBox)->Initialize(pHitBoxDesc);
}

void CPlayer::MotionTrailEnable(_bool bIsEnable)
{
	for (auto& iter : m_PartObjects)
	{
		auto pPlayer_Parts = static_cast<CPlayer_Parts*>(iter.second);
		pPlayer_Parts->EnableMotionTrail(bIsEnable);
	}
}

void CPlayer::MotionTrailRimLight(_float fRimLightPower, _float fRimLightIntensity, _float4 vColor)
{
	for (auto& iter : m_PartObjects)
	{
		auto pPlayer_Parts = static_cast<CPlayer_Parts*>(iter.second);
		pPlayer_Parts->SetMotionTrailRimLight(fRimLightPower, fRimLightIntensity, vColor);
	}
}

void CPlayer::MotionTrailCoolDown(_float fCoolDown)
{
	for (auto& iter : m_PartObjects)
	{
		auto pPlayer_Parts = static_cast<CPlayer_Parts*>(iter.second);
		pPlayer_Parts->SetMotionTrailCoolDown(fCoolDown);
	}

}

void CPlayer::Execution_Nayitba()
{
	DEFAULT_DAMAGE_DESC Desc;
	Desc.pSkillData = m_pGameManager->Find_SkillData(1010);

	m_PlayerDesc.pLinkAttackTarget->Damaged(&Desc);
}

void CPlayer::LinkAttack_Nayitba(const NAYITBA_EXECUTION_TYPE& eLinkAttackType)
{
	PLAYER_TRANSITION_DESC TransitionDesc{};
	SOCKETMATRIX_DESC TargetDesc{};

	_uint iMonsterID = m_PlayerDesc.pLinkAttackTarget->GetStaticMonsterData()->iMonsetID;

	TargetDesc.pParentTransformMatrix = m_PlayerDesc.pLinkAttackTarget->GetTransform()->Get_WorldMatrixPtr();
	TargetDesc.pSocketMatrix = m_PlayerDesc.pLinkAttackTarget->GetLinkTargetBone();

	switch (iMonsterID)
	{
	case 1:
		TransitionDesc.eNextState = PLAYER_STATE::GIGAS_LINKATTACK;
		//여기서 기가스 정보 꺼내와서 넘겨줘야함
		TransitionDesc.pArg = &TargetDesc;
		break;

	// 일단 기본적으로 홍련 링크어택으로 들어가게 했어
	// 2페이즈 그로기는 아래에 주석 처리한 SCARLET_PHASE2_LINKATTACK 임.
	case 8:
		if (NAYITBA_EXECUTION_TYPE::LINK_ATTACK == eLinkAttackType)
			TransitionDesc.eNextState = PLAYER_STATE::SCARLET_LINKATTACK;
		else if (NAYITBA_EXECUTION_TYPE::PHASE2_LINKATTACK == eLinkAttackType)
			TransitionDesc.eNextState = PLAYER_STATE::SCARLET_PHASE2_LINKATTACK;

		TransitionDesc.pArg = &TargetDesc;
		break;
	default:
		return;
	}

	m_pFSM->Handle_Transition(TransitionDesc);
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
	m_pGameManager->Bind_GameCharacter(nullptr);

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