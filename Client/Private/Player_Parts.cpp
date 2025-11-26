#include "pch.h"
#include "Player_Parts.h"

#include "GameManager.h"

CPlayer_Parts::CPlayer_Parts(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CPartObject { pDevice, pContext }
    , m_pGameManager { CGameManager::GetInstance() }
{
    Safe_AddRef(m_pGameManager);
}

CPlayer_Parts::CPlayer_Parts(const CPlayer_Parts& rhs)
    : CPartObject { rhs }
    , m_pGameManager{ CGameManager::GetInstance() }
{
    Safe_AddRef(m_pGameManager);
}

HRESULT CPlayer_Parts::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CPlayer_Parts::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    return S_OK;
}

void CPlayer_Parts::Priority_Update(_float fTimeDelta)
{
    __super::Priority_Update(fTimeDelta);
}

void CPlayer_Parts::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);
}

void CPlayer_Parts::Late_Update(_float fTimeDelta)
{
    __super::Late_Update(fTimeDelta);
}

HRESULT CPlayer_Parts::Render()
{
    return S_OK;
}

void CPlayer_Parts::Free()
{
    __super::Free();

    Safe_Release(m_pGameManager);
}
