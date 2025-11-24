#include "pch.h"
#include "Nayitba.h"

#include "GameInstance.h"

#include "StringHelper.h"
#include "NayitbaPartBody.h"

#include "TargetComponent.h"
#include "BossController.h"
#include "MonsterHitState.h"
#include "GameManager.h"

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

	if (FAILED(Ready_CharacterData()))
		return E_FAIL;

	if (FAILED(ADD_PartObjects()))
		return E_FAIL;

	if (FAILED(ADD_Components()))
		return E_FAIL;

	return S_OK;
}

void CNayitba::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
	m_pAIController->Priority_Update(fTimeDelta);
}

void CNayitba::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

	if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_PGDN))
	{
		
	}

	if (NAYTIBA_STATE::BATTLE == m_MonsterInfo.eNaytiba)
	{
		if (m_pAISenceCom->IsTagetEmpty())
		{
			BattleEvent(nullptr, NAYTIBA_STATE::DEFAULT);
		}
	}

	m_pAISenceCom->UpdatSenceComponent(fTimeDelta);
	m_pAIController->Update(fTimeDelta);
}

void CNayitba::Late_Update(_float fTimeDelta)
{
	// 여기에서 컬링 할거임
	__super::Late_Update(fTimeDelta);

#ifdef _DEBUG
	m_pAISenceCom->Update_Debuge();
#endif // _DEBUG

}

HRESULT CNayitba::Render()
{
	return S_OK;
}

HRESULT CNayitba::Damaged(void* pArg)
{
	if (nullptr == pArg)
	{
		// 이거 나중에 재훈이형이랑 연동할거임
		CHARACTER_SKILL_DESC SkillDesc = {};
		SkillDesc.eATK_Direction = ATTACK_DIRECTION::ATK_LEFT;

		DEFAULT_DAMAGE_DESC pHitDesc = {};
		pHitDesc.pSkillData = &SkillDesc;
		pHitDesc.pAttacker = m_pGameManager->GetGameCharacter();

		m_MonsterInfo.iCurrentHealth -= 50.f;
		m_pAISenceCom->Add_SenceTargetObject(pHitDesc.pAttacker);
		m_pAIController->Damage(&pHitDesc);

		Safe_Release(pHitDesc.pAttacker);
	}
	else
	{
		DEFAULT_DAMAGE_DESC* pDesc = static_cast<DEFAULT_DAMAGE_DESC*>(pArg);
		CHARACTER_SKILL_DESC* pSkillDesc = static_cast<CHARACTER_SKILL_DESC*>(pDesc->pSkillData);

		//if(pSkillDesc->eSkillType)

		m_MonsterInfo.iCurrentHealth -= pSkillDesc->iSkillDamage;
		m_pAISenceCom->Add_SenceTargetObject(pDesc->pAttacker);
		m_pAIController->Damage(pArg);
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

CAIController* CNayitba::GetController()
{
	Safe_AddRef(m_pAIController);
	return m_pAIController;
}

const list<CGameObject*>* CNayitba::GetTraceObejectList()
{
	return m_pAISenceCom->GetSearchAllObject();
}

HRESULT CNayitba::Ready_CharacterData()
{
	auto pNayitbaInfo = m_pGameManager->Find_BossData(m_iMonsterID);
	if (nullptr != pNayitbaInfo)
	{
		m_pInitMonsterInfo = pNayitbaInfo;
		size_t iNumSkill = m_pInitMonsterInfo->iAttackList.size();

		m_MonsterInfo.iAttackList.resize(iNumSkill);
		for (size_t i = 0; i < iNumSkill; ++i)
		{
			m_MonsterInfo.iAttackList[i] = m_pGameManager->Find_SkillData(m_pInitMonsterInfo->iAttackList[i]);
		}
	}

	if (AI_TYPE::PASSIVE == pNayitbaInfo->eAI_Type)
		m_MonsterInfo.eNaytiba = NAYTIBA_STATE::MIMESSIS;
	else
		m_MonsterInfo.eNaytiba = NAYTIBA_STATE::DEFAULT;

	// 체력 설정
	m_MonsterInfo.iCurrentHealth = m_pInitMonsterInfo->iMaxHealth;
	m_MonsterInfo.iCurrentShield = m_pInitMonsterInfo->iMaxShield;

	// 공격 설정
	m_MonsterInfo.fAttackCoolTime.y = m_pInitMonsterInfo->fAttackCoolTime;
	m_MonsterInfo.fAttackRange = m_pInitMonsterInfo->fAttackRange;

	// Phase 설정하는거
	m_MonsterInfo.iCurrentPhase = m_pInitMonsterInfo->iNumPhase;

	// 팀 설정
	m_eTeam = OBJECT_TEAM::ENEMY;

	return S_OK;
}

HRESULT CNayitba::ADD_Components()
{
	// 여기서 충돌처리용 콜라이더 달고
	// AI 센서 달아서 충돌 처리한번 보자
	/*if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT(""), TEXT("AI_Sence"), (CComponent**)&m_pAISenceCom)))
		return E_FAIL;*/

	CAISenceComponent::AI_SENCE_COMPONENT_DESC SenceComDesc = {};
	SenceComDesc.fAiSearchRadius = 60.f;
	SenceComDesc.fAiTargetSearchDistance = 10.f;
	SenceComDesc.m_fAiTargetLostTime = 20.f;

	/* Prototype_Component_TargetComponent */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_AISence"),
		TEXT("Com_AI_SenceCom"), reinterpret_cast<CComponent**>(&m_pAISenceCom), &SenceComDesc)))
		return E_FAIL;

	m_pAISenceCom->Bind_TargetSearch([&](CGameObject* pTarget) { BattleEvent(pTarget, NAYTIBA_STATE::BATTLE); });

	// 그다음 여기서 FSM 인지 행동트리아
	WCHAR	ControllerProtoType[MAX_PATH] = {};
	CStringHelper::ConvertUTFToWide(m_pInitMonsterInfo->szAIControllerPrototype, ControllerProtoType);

	CBase* pInstnace = nullptr;
	if (NAYTIBA_TYPE::ELITE == m_pInitMonsterInfo->eNaytiba_Type)
	{
		WCHAR	BehaviorTreePrototpye[MAX_PATH] = {};
		CStringHelper::ConvertUTFToWide(m_pInitMonsterInfo->szAIBehaviorPrototype, BehaviorTreePrototpye);

		CBossController::BOSS_CONTROLLER_DESC ControllerDesc = { };
		ControllerDesc.pOwner = this;
		ControllerDesc.szBehaviorProtoType = BehaviorTreePrototpye;
		pInstnace = m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::GAMEPLAY), ControllerProtoType, &ControllerDesc);
		if (nullptr == pInstnace)
			return E_FAIL;
	}
	else
	{
		CAIController::AI_CONTROLLER_DESC ControllerDesc = { };
		ControllerDesc.pOwner = this;

		pInstnace = m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::GAMEPLAY), ControllerProtoType, &ControllerDesc);
		if (nullptr == pInstnace)
			return E_FAIL;
	}

	m_pAIController = static_cast<CAIController*>(pInstnace);
	m_pAISenceCom->ADD_SenceIgnoreTraceObejct(HIT_TYPE::STATIC);

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
	m_MonsterInfo.eNaytiba = eState;
	// 이거 다른 플래그 넘겨서
	// 타겟을 찾으면 바로 확인해서 달려와야할거같음
	// Battle Start & Battle End 상태 애니메이션 넣어 주자

	m_szEntryAnim = m_pInitMonsterInfo->szAnimationName;
	if (NAYTIBA_STATE::BATTLE == m_MonsterInfo.eNaytiba)
		m_szEntryAnim += "_BattleStart";
	else
		m_szEntryAnim += "_BattleEnd";
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

	Safe_Release(m_pAISenceCom);
	Safe_Release(m_pAIController);
}
