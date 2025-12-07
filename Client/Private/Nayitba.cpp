#include "pch.h"
#include "Nayitba.h"

#include "GameInstance.h"

#include "StringHelper.h"
#include "NayitbaPartBody.h"

#include "TargetComponent.h"
#include "GameManager.h"
#include "BossController.h"
#include "MonsterHitState.h"

#include "Notify.h"
#include "AttackHitBox.h"
#include "GameManager.h"

#include "Player.h"
#include "PlayerCCTHitReporter.h"
#include "PlayerBehaviorCallback.h"

#include "UIHUD.h"
#include "UIBase.h"

CNayitba::CNayitba(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) :
	CCharacter(pDevice, pContext)
{
}

CNayitba::CNayitba(const CNayitba& Prototype) :
	CCharacter(Prototype)
{
}

HRESULT CNayitba::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CNayitba::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	NAYITBA_DESC* pDesc = static_cast<NAYITBA_DESC*>(pArg);
	m_iMonsterID = pDesc->iMonsterID;
	m_bIsSuperMonster = pDesc->bIsSuperMonster;

	m_SkillCandidates.reserve(30);
	if (FAILED(Ready_CharacterData()))
		return E_FAIL;

	if (FAILED(ADD_PartObjects()))
		return E_FAIL;

	if (FAILED(ADD_Components()))
		return E_FAIL;

	// 아래 세개중에서 하나
	// Bip001-Spine
	// Bip001_Spine1
	// Bip001_Spine2

	m_pLockOnMatrix = m_pBodyModelCom->Get_BoneMatrixPtr("Bip001-Spine");
	m_pHeadBoneMatrix = m_pBodyModelCom->Get_BoneMatrixPtr("Bip001-Head");

	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(196.f, 55.f, 243.f, 1.f));

	return S_OK;
}

void CNayitba::Priority_Update(_float fTimeDelta)
{
	m_pAIController->Priority_Update(fTimeDelta);
	m_pCCT->Update_PrePxPosition(m_pTransformCom);

	__super::Priority_Update(fTimeDelta);
}

void CNayitba::Update(_float fTimeDelta)
{
	if (NAYTIBA_STATE::BATTLE == m_MonsterInfo.eNaytibaState)
	{
		if (m_pAISenceCom->IsTagetEmpty())
		{
			BattleEvent(nullptr, NAYTIBA_STATE::DEFAULT);
		}

		if (NAYTIBA_TYPE::ELITE > m_pInitMonsterInfo->eNaytiba_Type)
			VisibleStatusUI(fTimeDelta);
	}

	m_MonsterPreState = m_MonsterInfo.eNaytibaState;
	// 이건 말해봐야할듯 락온이 플레이어 기준으로 반경을 체크하는데
	// 락온보고 일단 고정상수로 두고 하는데 어디서 받아오거나 했으면함
	
	m_pAIController->Update(fTimeDelta);
	if (NAYTIBA_STATE::DEAD != m_MonsterInfo.eNaytibaState)
	{
		m_pAISenceCom->UpdatSenceComponent(fTimeDelta);
		m_pColliderCom->UpdateColiision(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));
	}
	
	if (m_pGameInstance->isIn_WorldFrustum(m_pColliderCom))
	{
		_matrix WorldMatrix = XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr());
		XMStoreFloat3(&m_MonsterInfo.vLockOnPoint, (XMLoadFloat4x4(m_pLockOnMatrix) * WorldMatrix).r[3]);
		XMStoreFloat3(&m_MonsterInfo.vStatusBarPoint, (XMLoadFloat4x4(m_pHeadBoneMatrix) * WorldMatrix).r[3]);
	}
	__super::Update(fTimeDelta);
}

void CNayitba::Late_Update(_float fTimeDelta)
{
	//모든 트랜스폼의 이동이 끝난 후 실행되어야 함.
	m_pCCT->Update_PxPosition(fTimeDelta, m_pTransformCom);
	if (NAYTIBA_STATE::DEAD != m_MonsterInfo.eNaytibaState )
		m_pGameInstance->ADD_Collider(m_pColliderCom);

	if (m_pGameInstance->isIn_WorldFrustum(m_pColliderCom))
	{
		__super::Late_Update(fTimeDelta);

#ifdef _DEBUG
		m_pAISenceCom->Update_Debuge();
		m_pGameInstance->Add_DebugComponent(m_pColliderCom);
		m_pGameInstance->Add_PhysxGeometry(m_pCCT->Get_PxActor(), m_pCCT->Get_PxShape());
#endif // _DEBUG
	}
}

HRESULT CNayitba::Render()
{
	return S_OK;
}

HRESULT CNayitba::Damaged(void* pArg)
{
	DEFAULT_DAMAGE_DESC* pDesc = static_cast<DEFAULT_DAMAGE_DESC*>(pArg);
	const CHARACTER_SKILL_DESC* pSkillDesc = static_cast<const CHARACTER_SKILL_DESC*>(pDesc->pSkillData);

	if (NAYTIBA_STATE::BATTLE != m_MonsterInfo.eNaytibaState)
		m_MonsterInfo.eNaytibaState = NAYTIBA_STATE::BATTLE;

	switch (m_pInitMonsterInfo->eAI_Type)
	{
	case AI_TYPE::DEFENSIVE : // 방어형
		pDesc->bIsHitMotion = DefenseTypeDamage(pDesc);
		break;
	case AI_TYPE::AGGRESSIVE :
	case AI_TYPE::PASSIVE :  // 공격형
	{
		if (m_bIsSuperMonster)
			pDesc->bIsHitMotion = m_pGameManager->ComputeDamageLogic(&m_MonsterInfo, 0);
		else
			pDesc->bIsHitMotion = m_pGameManager->ComputeDamageLogic(&m_MonsterInfo, pSkillDesc->iSkillDamage);
	}
		break;
	}
	
	m_pAISenceCom->Add_SenceTargetObject(pDesc->pAttacker);
	m_pAIController->Damage(pArg);

	m_vHitVisibleDuration.x = 0.f;

	if (NAYTIBA_TYPE::ELITE > m_pInitMonsterInfo->eNaytiba_Type)
	{
		if (nullptr == m_pStatusUI)
		{
			auto pCurHUD = static_cast<CUIHUD*>(m_pGameInstance->GetCurrentLevelHUD());
			m_pStatusUI = pCurHUD->Rent_WorldUI(TEXT("Pool_MonsterVital"), this, &m_MonsterInfo.vStatusBarPoint);
			Safe_Release(pCurHUD);
		}
		if (0 >= m_MonsterInfo.iCurrentHealth)
		{
			m_MonsterInfo.eNaytibaState = NAYTIBA_STATE::DEAD;
			auto pCurHUD = static_cast<CUIHUD*>(m_pGameInstance->GetCurrentLevelHUD());

			// 몬스터 체력
			pCurHUD->Return_WorldUI(m_pStatusUI);
			m_pStatusUI = nullptr;
			Safe_Release(pCurHUD);
		}
	}
	
	// 이제 진짜라고 합니다.
	m_pGameManager->Start_Lockon();
	return S_OK;
}

HRESULT CNayitba::ActionSuccess(void* pArg)
{
	m_pAIController->ActionSuccess(pArg);

	return S_OK;
}

HRESULT CNayitba::CallNotify(_uint iNotiType, const AnimNotify* pNotify)
{
	CNotify::NOTIFY_TYPE NotiType = CNotify::NOTIFY_TYPE(iNotiType);
	if (CNotify::NOTIFY_TYPE::ACTIVE_COLLISION == NotiType)
	{
		CreateHitBox(pNotify);
	}


	return S_OK;
}

_uint CNayitba::GetMonsterID()
{
	return m_iMonsterID;
}

const list<CGameObject*>* CNayitba::GetTargetList()
{
	return m_pAISenceCom->GetSearchAllObject();
}

const CHARACTER_SKILL_DESC* CNayitba::FindSkillData(_uint iTypeIndex, _uint iSkillIndex)
{
	if (0 > iSkillIndex || (_uint)m_MonsterInfo.iAttackList[iTypeIndex].size() <= iSkillIndex)
		return nullptr;

	return  m_MonsterInfo.iAttackList[iTypeIndex][iSkillIndex];
}

const CHARACTER_SKILL_DESC* CNayitba::GetSkillData(_bool bIsRandom, _uint iTypeIndex)
{
	const CHARACTER_SKILL_DESC* pSkill = { nullptr };
	if(-1 == iTypeIndex)
	{
		if (0 == m_iNumCandidate)
			return nullptr;

		if (bIsRandom)
		{
			_uint iRandomIndex = (_uint)m_pGameInstance->Random(0.f, (_float)m_iNumCandidate);
			pSkill = m_SkillCandidates[iRandomIndex];
		}
		else
		{
			if (m_iNumCandidate <= m_iSkillIndex)
				m_iSkillIndex = 0;

			pSkill = m_SkillCandidates[m_iSkillIndex];
			m_iSkillIndex++;
		}
	}
	else
	{
		size_t iNumSize = m_MonsterInfo.iAttackList[iTypeIndex].size();
		_uint iRandomIndex = (_uint)m_pGameInstance->Random(0, iNumSize);
		pSkill = m_MonsterInfo.iAttackList[iTypeIndex][iRandomIndex];
	}

	return pSkill;
}

void CNayitba::SetAttackData(const CHARACTER_SKILL_DESC* pATKDesc)
{
	m_pAttack_Data = pATKDesc;
}

CAIController* CNayitba::GetController()
{
	Safe_AddRef(m_pAIController);
	return m_pAIController;
}

const list<CGameObject*>* CNayitba::GetTraceObejectList()
{
	return m_pAISenceCom->GetSearchAllObject();
}

void CNayitba::Active_SFX(const _wstring& strPartTag, const _wstring& strObjectTag, const ANIM_NOTIFY& NotifyReference)
{
	if (strPartTag.empty())
	{
	}
	else
	{
		Find_PartObject(strPartTag)->Active_SFX(strObjectTag, NotifyReference);
	}
}

HRESULT CNayitba::Ready_CharacterData()
{
	auto pNayitbaInfo = m_pGameManager->Find_BossData(m_iMonsterID);
	if (nullptr != pNayitbaInfo)
	{
		m_pInitMonsterInfo = pNayitbaInfo;
		size_t iNumSkill = m_pInitMonsterInfo->iAttackList.size();
	
		for(_uint i = 0; i < ENUM_CLASS(SKILL_TYPE::END); ++i)
			m_MonsterInfo.iAttackList[i].reserve(iNumSkill);

		for (size_t j = 0; j < iNumSkill; ++j)
		{
			auto pSkillData = m_pGameManager->Find_SkillData(m_pInitMonsterInfo->iAttackList[j]);
			_uint iIndex = ENUM_CLASS(pSkillData->eSkillType);
			m_MonsterInfo.iAttackList[iIndex].push_back(pSkillData);
		}
	
		if (AI_TYPE::PASSIVE == pNayitbaInfo->eAI_Type)
			m_MonsterInfo.eNaytibaState = NAYTIBA_STATE::MIMESSIS;
		else
			m_MonsterInfo.eNaytibaState = NAYTIBA_STATE::DEFAULT;

		m_MonsterInfo.iCurrentHealth = m_pInitMonsterInfo->iMaxHealth;
		m_MonsterInfo.iCurrentShield = m_pInitMonsterInfo->iMaxShield;

		m_MonsterInfo.fAttackCoolTime.y = m_pInitMonsterInfo->fAttackCoolTime;
		m_MonsterInfo.fAttackRange = m_pInitMonsterInfo->fAttackRange;
		m_MonsterInfo.fMoveSpeed = m_pInitMonsterInfo->fMoveSpeed;
		m_MonsterInfo.iCurrentPhase = m_pInitMonsterInfo->iNumPhase;

		m_eTeam = OBJECT_TEAM::ENEMY;

		_uint iEndIndex = ENUM_CLASS(SKILL_TYPE::INTERACTION_SKILL);
		for (_uint i = 0; i <= iEndIndex; ++i)
		{
			for (auto& iter : m_MonsterInfo.iAttackList[i])
				m_SkillCandidates.push_back(iter);
		}
		m_iNumCandidate = m_SkillCandidates.size();
	}

	return S_OK;
}

HRESULT CNayitba::ADD_Components()
{
	/* Com_Collider_AABB */
	CBoxCollider::BOX_COLLIDER_DESC		AABBDesc{};
	AABBDesc.vSize = m_pInitMonsterInfo->fColliderExtents;
	AABBDesc.vCenter = _float3(0.f, AABBDesc.vSize.y, 0.f);
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_Collider_AABB"), reinterpret_cast<CComponent**>(&m_pColliderCom), &AABBDesc)))
		return E_FAIL;

	m_pColliderCom->SetColliderHitType(HIT_TYPE::MONSTER);

	
	WCHAR	ControllerProtoType[MAX_PATH] = {};
	CStringHelper::ConvertUTFToWide(m_pInitMonsterInfo->szAIControllerPrototype, ControllerProtoType);

	CBase* pInstnace = nullptr;
	if (NAYTIBA_TYPE::ELITE == m_pInitMonsterInfo->eNaytiba_Type)
	{
		WCHAR	BehaviorTreePrototpye[MAX_PATH] = {};
		CStringHelper::ConvertUTFToWide(m_pInitMonsterInfo->szAIBehaviorPrototype, BehaviorTreePrototpye);

		CBossController::BOSS_CONTROLLER_DESC ControllerDesc = { };
		ControllerDesc.pParent = this;
		ControllerDesc.szBehaviorProtoType = BehaviorTreePrototpye;
		pInstnace = m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::GAMEPLAY), ControllerProtoType, &ControllerDesc);
		if (nullptr == pInstnace)
			return E_FAIL;

		CAISenceComponent::AI_SENCE_COMPONENT_DESC SenceComDesc = {};
		SenceComDesc.fAiSearchRadius = 360.f;
		SenceComDesc.fAiTargetSearchDistance = 20.f;
		SenceComDesc.m_fAiTargetLostTime = 20.f;

		/* Prototype_Component_TargetComponent */
		if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_AISence"),
			TEXT("Com_AI_SenceCom"), reinterpret_cast<CComponent**>(&m_pAISenceCom), &SenceComDesc)))
			return E_FAIL;

		m_pAISenceCom->SetTraceHitType(HIT_TYPE::SENCE);
		m_pAISenceCom->ADD_SenceOnlyTraceObject(HIT_TYPE::PLAYER);
		m_pAISenceCom->Bind_TargetSearch([&](CGameObject* pTarget) { BattleEvent(pTarget, NAYTIBA_STATE::BATTLE); });
	}
	else
	{
		CAIController::GAMEOBJECT_DESC ControllerDesc = { };
		ControllerDesc.pParent = this;

		pInstnace = m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::GAMEPLAY), ControllerProtoType, &ControllerDesc);
		if (nullptr == pInstnace)
			return E_FAIL;

		CAISenceComponent::AI_SENCE_COMPONENT_DESC SenceComDesc = {};
		SenceComDesc.fAiSearchRadius = 60.f;
		SenceComDesc.fAiTargetSearchDistance = 10.f;
		SenceComDesc.m_fAiTargetLostTime = 20.f;

		/* Prototype_Component_TargetComponent */
		if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_AISence"),
			TEXT("Com_AI_SenceCom"), reinterpret_cast<CComponent**>(&m_pAISenceCom), &SenceComDesc)))
			return E_FAIL;

		m_pAISenceCom->SetTraceHitType(HIT_TYPE::SENCE);
		m_pAISenceCom->ADD_SenceOnlyTraceObject(HIT_TYPE::PLAYER);
		m_pAISenceCom->Bind_TargetSearch([&](CGameObject* pTarget) { BattleEvent(pTarget, NAYTIBA_STATE::BATTLE); });

	}

	/* Com_CCT */
	CCharacterController::CCT_DESC Desc;
	PxUserData tUserData;
	tUserData.szActorTag = TEXT("Player_CCT");

	Desc.eCharacterControllerType = CCharacterController::CCT_SHAPE::CAPSULE;
	Desc.tUserData = tUserData;
	//캡슐 컨트롤러에서 x는 구 성분 y는 기둥 성분
	Desc.vSize = { m_pInitMonsterInfo->fColliderExtents.x, m_pInitMonsterInfo->fColliderExtents.y, 0.f };
	XMStoreFloat4(&Desc.vStartPos, m_pTransformCom->Get_State(STATE::POSITION));
	Desc.vMaterial = _float3(0.5f, 0.5f, 0.f);
	Desc.pHitReporter = CPlayerCCTHitReporter::Create();
	Desc.pBehaviorCallback = CPlayerBehaviorCallback::Create();

	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_CharacterController"),
		TEXT("Com_CCT"), reinterpret_cast<CComponent**>(&m_pCCT), &Desc)))
		return E_FAIL;

	m_pGameInstance->Add_CCT_ToPhysx(this, m_pCCT);
	m_pAIController = static_cast<CAIController*>(pInstnace);

	m_pNotifyCom->Set_ModelCom(m_pBodyModelCom);
	return S_OK;
}

HRESULT CNayitba::ADD_PartObjects()
{
	WCHAR	ModelProtoType[MAX_PATH] = {};
	CStringHelper::ConvertUTFToWide(m_pInitMonsterInfo->szModelPrototype, ModelProtoType);

	CNayitbaPartBody::NAYITBA_PART_BODY_DESC BodyDesc = { };
	BodyDesc.pParentTransform = m_pTransformCom;
	BodyDesc.vScale = { 1.f, 1.f, 1.f };
	BodyDesc.szBodyModel = ModelProtoType;
	BodyDesc.fSpeedPerSec = 5.f;
	if(FAILED(__super::Add_PartObject(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Nayitba_Body"), TEXT("Part_Body"), &BodyDesc)))
		return E_FAIL;
	Import_ModelPtr();

	return S_OK;
}

void CNayitba::BattleEvent(CGameObject* pTarget, NAYTIBA_STATE eState)
{
	m_MonsterInfo.eNaytibaState = eState;
	m_szEntryAnim = m_pInitMonsterInfo->szAnimationName;

	if (NAYTIBA_TYPE::ELITE <= m_pInitMonsterInfo->eNaytiba_Type)
	{
		m_pAISenceCom->Add_SenceTargetObject(pTarget);

		CUIHUD* pUIHUD = dynamic_cast<CUIHUD*>(m_pGameInstance->GetCurrentLevelHUD());

		pUIHUD->Set_Boss_Desc(m_pInitMonsterInfo, &m_MonsterInfo);

		Safe_Release(pUIHUD);
	}
}

void CNayitba::VisibleStatusUI(_float fTimeDelta)
{
	if (m_bIsTimeVisible)
	{
		if (m_vHitVisibleDuration.x <= m_vHitVisibleDuration.y)
		{
			if(VISIBILITY::HIDDEN == m_pStatusUI->GetVisibility())
				m_pStatusUI->SetVisibility(VISIBILITY::VISIBLE);

			m_vHitVisibleDuration.x += fTimeDelta;
		}
		else
			m_pStatusUI->SetVisibility(VISIBILITY::HIDDEN);
	}
}

_bool CNayitba::DefenseTypeDamage(const DEFAULT_DAMAGE_DESC* pDamageDesc, _float fDamageReductionRate)
{
	const CHARACTER_SKILL_DESC* pSkillDesc = static_cast<const CHARACTER_SKILL_DESC*>(pDamageDesc->pSkillData);

	long long iDamage = pSkillDesc->iSkillDamage;
	if (m_bIsSuperMonster)
		iDamage = 0;

	_bool bIsCheckDefenceLogic = false;
	if (m_pAttack_Data)
	{
		if (false == (SKILL_PROPERTY::GUARD & m_pAttack_Data->eProPerty))
		{
			m_pGameManager->ComputeDamageLogic(&m_MonsterInfo, iDamage);
		}
		else
			bIsCheckDefenceLogic = true;
	}
	else
		bIsCheckDefenceLogic = true;

	if (bIsCheckDefenceLogic)
	{
		_vector vOwnerPos{}, vTempOwnerPos{}, vTargetPos{}, vDir{};
		vOwnerPos = vTempOwnerPos = m_pTransformCom->Get_State(STATE::POSITION);
		vTargetPos = pDamageDesc->pAttacker->GetTransform()->Get_State(STATE::POSITION);

		vTempOwnerPos.m128_f32[1] = vTargetPos.m128_f32[1] = 0.f;
		vDir = XMVector3Normalize(vTargetPos - vTempOwnerPos);

		_vector vOwnerLook = m_pTransformCom->Get_State(STATE::LOOK);

		_float fScalar = XMVectorGetX(XMVector3Dot(vOwnerLook, vDir));
		fScalar = Clamp<_float>(fScalar, -1.0f, 1.0f);   // NaN 방지
		_float fRadian = acosf(fScalar);

		// 앞
		if (0 < fScalar)
		{
			if (fRadian < m_pAISenceCom->GetSenceRadiusRadian())
			{
				long long iFrontDamage = iDamage * fDamageReductionRate;
				m_pGameManager->ComputeDamageLogic(&m_MonsterInfo, iFrontDamage);
				return false;
			}
		}
		else
			m_pGameManager->ComputeDamageLogic(&m_MonsterInfo, iDamage);
	}
	
	return true;
}

void CNayitba::CreateHitBox(const AnimNotify* pNotify)
{
	CAttackHitBox::HIT_BOX_DESC HitBoxDesc = {};
	HitBoxDesc.pAttacker = this;

	_uint iGameLevel = ENUM_CLASS(LEVEL::GAMEPLAY);
	_wstring szProtoType(pNotify->szNotifyArg01.begin(), pNotify->szNotifyArg01.end());
	_wstring szLayerName(pNotify->szNotifyArg01.begin(), pNotify->szNotifyArg01.end());

	CAttackHitBox::HIT_BOX_DESC pHitBoxDesc = {};
	auto pSkillData = m_pGameManager->Find_SkillData(pNotify->iNumData01);
	pHitBoxDesc.pData = pSkillData;

	pHitBoxDesc.eColType = COLLIDER(pNotify->iNumData02);
	pHitBoxDesc.eHitBoxType = HIT_TYPE(pNotify->iNumData03);
	pHitBoxDesc.eHitObjectType = HIT_TYPE(pNotify->iNumData04);
	pHitBoxDesc.bIsApplyTransform = true;
	pHitBoxDesc.pAttacker = this;

	pHitBoxDesc.vScale = pSkillData->vHitBoxExtents;
	pHitBoxDesc.fImpactForce = m_fImpactForce;

	_vector vCharacterPos = GetTransform()->Get_State(STATE::POSITION);
	_vector vCharacterLook = GetTransform()->Get_State(STATE::LOOK);
	vCharacterPos += vCharacterLook * pSkillData->fRange;
	XMStoreFloat3(&pHitBoxDesc.vPosition, vCharacterPos);

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(iGameLevel, szProtoType.c_str(),
		iGameLevel, szLayerName.c_str(), &pHitBoxDesc)))
		return;
}

CNayitba* CNayitba::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CNayitba* pNayitba = new CNayitba(pDevice, pContext);
	if (FAILED(pNayitba->Initialize_Prototype()))
	{
		Safe_Release(pNayitba);
		MSG_BOX("Create Fail : NayitBa");
	}
	return pNayitba;
}

CGameObject* CNayitba::Clone(void* pArg)
{
	CNayitba* pNayitba = new CNayitba(*this);
	if (FAILED(pNayitba->Initialize(pArg)))
	{
		Safe_Release(pNayitba);
		MSG_BOX("Clone Fail : NayitBa");
	}
	return pNayitba;
}

void CNayitba::Free()
{
	__super::Free();


	//Safe_Release(m_pStatusUI);
	Safe_Release(m_pAISenceCom);
	Safe_Release(m_pAIController);
}
