#include "pch.h"
#include "NpcInteractionController.h"

#include "GameInstance.h"
#include "NpcFSM.h"

#pragma region State
#include "NpcIdleState.h"
#include "NpcInteractionState.h"
#pragma endregion

CNpcInteractionController::CNpcInteractionController(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) :
	CAIController(pDevice, pContext)
{
}

CNpcInteractionController::CNpcInteractionController(const CNpcInteractionController& Prototype) :
	CAIController(Prototype)
{
}

HRESULT CNpcInteractionController::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CNpcInteractionController::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_FSM()))
		return E_FAIL;

	return S_OK;
}

void CNpcInteractionController::Priority_Update(_float fTimeDelta)
{
}

void CNpcInteractionController::Update(_float fTimeDelta)
{
	m_pFSM->Update(fTimeDelta);
}

void CNpcInteractionController::Late_Update(_float fTimeDelta)
{
}

HRESULT CNpcInteractionController::Render()
{
	return S_OK;
}

HRESULT CNpcInteractionController::Ready_FSM()
{
	CStateMachine::STATEMACHINE_DESC Desc = {};
	Desc.pOwner = m_pParent;

	auto pClone = m_pGameInstance->Clone_Prototype(PROTOTYPE::COMPONENT, ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Npc_FSM"), &Desc);
	if (nullptr == pClone)
		return E_FAIL;

	m_pFSM = static_cast<CNpcFSM*>(pClone);

	// 여기서 상태를 넣자
	// 특정몬스터가 상태를 가져야한다면 여기서 상태를 추가해줄수잇음
	if (FAILED(m_pFSM->Add_State(TEXT("Idle"), CNpcIdleState::Create(&Desc))))
		return E_FAIL;

	if (FAILED(m_pFSM->Add_State(TEXT("Interaction"), CNpcInteractionState::Create(&Desc))))
		return E_FAIL;

	m_pFSM->Change_State(TEXT("Idle"));
	return S_OK;
}

CNpcInteractionController* CNpcInteractionController::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CNpcInteractionController* pNpcInteractionController = new CNpcInteractionController(pDevice, pContext);
	if (FAILED(pNpcInteractionController->Initialize_Prototype()))
	{
		Safe_Release(pNpcInteractionController);
		MSG_BOX("Create Fail : Npc Interaction Controller");
	}
	return pNpcInteractionController;
}

CGameObject* CNpcInteractionController::Clone(void* pArg)
{
	CNpcInteractionController* pNpcInteractionController = new CNpcInteractionController(*this);
	if (FAILED(pNpcInteractionController->Initialize(pArg)))
	{
		Safe_Release(pNpcInteractionController);
		MSG_BOX("Clone Fail : Npc Interaction Controller");
	}
	return pNpcInteractionController;
}

void CNpcInteractionController::Free()
{
	__super::Free();

	Safe_Release(m_pFSM);
}
