#include "pch.h"

#include "PlayerFSM.h"
#include "Player_IdleState.h"

CPlayerFSM::CPlayerFSM(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent{ pDevice, pContext }
{
}

CPlayerFSM::CPlayerFSM(const CPlayerFSM& rhs)
	: CComponent { rhs }
{
}

HRESULT CPlayerFSM::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPlayerFSM::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_State()))
		return E_FAIL;

	return S_OK;
}

void CPlayerFSM::Update(_float fTimeDelta)
{
	CPlayerState* pState = m_pCurrentState->Update(fTimeDelta);

	if (nullptr != pState)
	{
		m_pCurrentState->End();

		Safe_Release(m_pCurrentState);
		
		pState->Start();
		m_pCurrentState = pState;
	}
}

HRESULT CPlayerFSM::Ready_State()
{
	CPlayer_IdleState* pState = CPlayer_IdleState::Create(nullptr);
	pState->Start();

	m_pCurrentState = pState;

	return S_OK;
}

CPlayerFSM* CPlayerFSM::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPlayerFSM* pInstance = new CPlayerFSM(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Create Failed : Player FSM");
	}

	return pInstance;
}

CComponent* CPlayerFSM::Clone(void* pArg)
{
	CPlayerFSM* pInstance = new CPlayerFSM(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Clone Failed : Player FSM");
	}

	return pInstance;
}

void CPlayerFSM::Free()
{
	__super::Free();
	
	if(nullptr != m_pCurrentState)
		Safe_Release(m_pCurrentState);
}
