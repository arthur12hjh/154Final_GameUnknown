#include "State.h"
#include "GameInstance.h"

CState::CState() :
    m_pGameInstance(CGameInstance::GetInstance())
{
    Safe_AddRef(m_pGameInstance);
}

HRESULT CState::Initialize(void* pArg)
{
    STATE_DESC* pDesc = static_cast<STATE_DESC*>(pArg);
    m_pOwner = pDesc->pOwner;

    return S_OK;
}

void CState::Start(void* pArg)
{
}

void CState::Update(_float fTimeDelta)
{

}

void CState::End()
{

}

void CState::Free()
{
    __super::Free();

    Safe_Release(m_pGameInstance);
}
