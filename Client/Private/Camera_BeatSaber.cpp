#include "pch.h"
#include "Camera_BeatSaber.h"

#include "GameInstance.h"
#include "GameManager.h"
#include "Player.h"

CCamera_BeatSaber::CCamera_BeatSaber(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) :
    CCamera(pDevice, pContext)
    , m_pGameManager{ CGameManager::GetInstance() }
{
}

CCamera_BeatSaber::CCamera_BeatSaber(const CCamera_BeatSaber& Prototype) :
    CCamera(Prototype)
    , m_pGameManager{ CGameManager::GetInstance() }
{
}

HRESULT CCamera_BeatSaber::Initialize_Prototype()
{

    return S_OK;
}

HRESULT CCamera_BeatSaber::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    return S_OK;
}

void CCamera_BeatSaber::Priority_Update(_float fTimeDelta)
{
    if (false == m_pGameInstance->IsMainCamera(this))
        return;

    __super::Bind_Matrices(fTimeDelta);
}

void CCamera_BeatSaber::Update(_float fTimeDelta)
{
}

void CCamera_BeatSaber::Late_Update(_float fTimeDelta)
{
}

HRESULT CCamera_BeatSaber::Render()
{
    return S_OK;
}

CCamera_BeatSaber* CCamera_BeatSaber::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CCamera_BeatSaber* pInstance = new CCamera_BeatSaber(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : Camera BeatSaber");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CCamera_BeatSaber::Clone(void* pArg)
{
    CCamera_BeatSaber* pInstance = new CCamera_BeatSaber(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : Camera BeatSaber");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CCamera_BeatSaber::Free()
{
    __super::Free();
}
