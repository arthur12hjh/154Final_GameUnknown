#include "GameHUD.h"

#include "GameInstance.h"
#include "GameObject.h"
#include "HUDLayer.h"

CGameHUD::CGameHUD(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) :
    m_pGameInstance(CGameInstance::GetInstance()),
    m_pDevice(pDevice),
    m_pContext(pContext)
{
    Safe_AddRef(m_pGameInstance);
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
}

HRESULT CGameHUD::Initialize()
{
    return S_OK;
}

void CGameHUD::Update(_float fDeletaTime)
{
    for (auto& iter : m_pLayers)
    {
        iter.second->Priority_Update(fDeletaTime);
        iter.second->Update(fDeletaTime);
        iter.second->Late_Update(fDeletaTime);
    }
}

HRESULT CGameHUD::Add_UserInterface(_uint iLevel, const WCHAR* PrototypeTag, const WCHAR* LayerTag, const WCHAR* UITag, CGameObject** ppOut, void* pArg)
{
    auto pLayer = Find_Layer(LayerTag);
    if (nullptr == pLayer)
    {
        pLayer = CHUDLayer::Create();
        m_pLayers.emplace(LayerTag, pLayer);
    }

    CGameObject* pGameObject = static_cast<CGameObject *>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, iLevel, PrototypeTag, pArg));
    if (nullptr == pGameObject)
        return E_FAIL;

    pLayer->ADD_GameObject(UITag, pGameObject);

    if (ppOut != nullptr)
        *ppOut = pGameObject;

    return S_OK;
}

HRESULT CGameHUD::Remove_UserInterface(const WCHAR* LayerTag, const WCHAR* UITag)
{
    auto pLayer = Find_Layer(LayerTag);
    if (nullptr == pLayer)
        return E_FAIL;

    pLayer->Remove_GameObject(UITag);
    return S_OK;
}

CHUDLayer* CGameHUD::Find_Layer(const WCHAR* LayerTag)
{
    auto iter = m_pLayers.find(LayerTag);
    if (iter == m_pLayers.end())
        return nullptr;

    return iter->second;
}

void CGameHUD::Clear_HUD()
{
    for (auto& iter : m_pLayers)
        Safe_Release(iter.second);

    m_pLayers.clear();
}

void CGameHUD::Free()
{
    __super::Free();

    for (auto& iter : m_pLayers)
        Safe_Release(iter.second);

    m_pLayers.clear();

    Safe_Release(m_pGameInstance);
    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
}
