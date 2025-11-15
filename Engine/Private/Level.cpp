#include "Level.h"

#include "GameInstance.h"
#include "GameHUD.h"

CLevel::CLevel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevelID)
    : m_pDevice{ pDevice }
    , m_pContext{ pContext }
    , m_pGameInstance { CGameInstance::GetInstance() }
    , m_iID { iLevelID }
{
    Safe_AddRef(m_pGameInstance);
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
}

HRESULT CLevel::Initialize()
{
    return S_OK;
}

void CLevel::Update(_float fTimeDelta)
{
    if (nullptr != m_pHUD)
        m_pHUD->Update(fTimeDelta);
}

HRESULT CLevel::Render()
{
    return S_OK;
}

void CLevel::SetHUD(CGameHUD* pGameHUD)
{
    m_pHUD = pGameHUD;
}

CGameHUD* CLevel::GetHUD()
{
    if (nullptr == m_pHUD)
        return nullptr;

    Safe_AddRef(m_pHUD);
    return m_pHUD;
}

void CLevel::Free()
{
    __super::Free();

    Safe_Release(m_pHUD);
    Safe_Release(m_pGameInstance);
    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
}
