#include "pch.h"
#include "Nayitba.h"

#include "GameInstance.h"

#include "StringHelper.h"
#include "NayitbaPartBody.h"

#include "TargetComponent.h"
#include "BossController.h"
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
	auto pNayitbaInfo = m_pGameManager->Find_BossData(m_iMonsterID);
	if (nullptr != pNayitbaInfo)
		m_pInitMonsterInfo = pNayitbaInfo;
	
	if (FAILED(ADD_Components()))
		return E_FAIL;

	if (FAILED(ADD_PartObjects()))
		return E_FAIL;

	m_MonsterInfo.iCurrentHealth = m_pInitMonsterInfo->iMaxHealth;
	m_MonsterInfo.iCurrentShield = m_pInitMonsterInfo->iMaxShield;
	m_MonsterInfo.iCurrentPhase = m_pInitMonsterInfo->iNumPhase;

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
	m_pAIController->Update(fTimeDelta);
}

void CNayitba::Late_Update(_float fTimeDelta)
{
	// 여기에서 컬링 할거임
	__super::Late_Update(fTimeDelta);

}

HRESULT CNayitba::Render()
{
	return S_OK;
}

_uint CNayitba::GetMonsterID()
{
	return _uint();
}

HRESULT CNayitba::ADD_Components()
{
	// 여기서 충돌처리용 콜라이더 달고
	// AI 센서 달아서 충돌 처리한번 보자
	/*if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT(""), TEXT("AI_Sence"), (CComponent**)&m_pAISenceCom)))
		return E_FAIL;*/

	// 그다음 여기서 FSM 인지 행동트리아
	WCHAR	ControllerProtoType[MAX_PATH] = {};
	CStringHelper::ConvertUTFToWide(m_pInitMonsterInfo->szAIControllerPrototype, ControllerProtoType);

	CBase* pInstnace = nullptr;
	if (m_pInitMonsterInfo->bIsBoss)
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

	Safe_Release(m_pTargetCom);
	Safe_Release(m_pAISenceCom);
	Safe_Release(m_pAIController);
}
