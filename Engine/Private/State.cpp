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

void CState::Start(void* pArg, CState* pPreState)
{
}

void CState::Update(_float fTimeDelta)
{

}

void CState::End()
{

}

const char* CState::GetAnimationName()
{
    return m_szAnimationName.c_str();
}

void CState::Free()
{
    __super::Free();

    Safe_Release(m_pGameInstance);
}
