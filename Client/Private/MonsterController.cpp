#include "pch.h"
#include "MonsterController.h"

#include "GameInstance.h"
#include "StateMachine.h"
#include "MonsterFSM.h"

CMonsterController::CMonsterController(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) :
	CAIController(pDevice, pContext)
{
}

CMonsterController::CMonsterController(const CMonsterController& Prototype) :
	CAIController(Prototype)
{
}

HRESULT CMonsterController::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMonsterController::Initialize(void* pArg)
{
	if(FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_FSM()))
		return E_FAIL;

	return S_OK;
}

void CMonsterController::Priority_Update(_float fTimeDelta)
{
	
}

void CMonsterController::Update(_float fTimeDelta)
{
	m_pFSM->Update(fTimeDelta);
}

void CMonsterController::Late_Update(_float fTimeDelta)
{
}

HRESULT CMonsterController::Render()
{
	return S_OK;
}

HRESULT CMonsterController::Ready_FSM()
{
	CStateMachine::STATEMACHINE_DESC Desc = {};
	Desc.pOwner = m_pOwner;

	auto pClone = m_pGameInstance->Clone_Prototype(PROTOTYPE::COMPONENT, ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Monster_FSM"), &Desc);
	if (nullptr == pClone)
		return E_FAIL;

	m_pFSM = static_cast<CStateMachine*>(pClone);

	// 여기서 상태를 넣자
	// 특정몬스터가 상태를 가져야한다면 여기서 상태를 추가해줄수잇음




	return S_OK;
}

CMonsterController* CMonsterController::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMonsterController* pMonsterController = new CMonsterController(pDevice, pContext);
	if (FAILED(pMonsterController->Initialize_Prototype()))
	{
		Safe_Release(pMonsterController);
		MSG_BOX("Create Fail : Monster Controller");
	}
	return pMonsterController;
}

CGameObject* CMonsterController::Clone(void* pArg)
{
	CMonsterController* pMonsterController = new CMonsterController(*this);
	if (FAILED(pMonsterController->Initialize(pArg)))
	{
		Safe_Release(pMonsterController);
		MSG_BOX("Clone Fail : Monster Controller");
	}
	return pMonsterController;
}

void CMonsterController::Free()
{
	__super::Free();

	Safe_Release(m_pFSM);
}
