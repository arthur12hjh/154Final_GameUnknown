#include "pch.h"
#include "DebugCheatUI.h"

#include "GameInstance.h"
#include "ImGuiManager.h"
#include "DebugHierarchy.h"
#include "StringHelper.h"
#include "GameManager.h"

CDebugCheatUI::CDebugCheatUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) :
    CGameObject(pDevice, pContext)
{
}

HRESULT CDebugCheatUI::Initialize()
{
#ifdef _DEBUG
    m_pGameManager = CGameManager::GetInstance();
    Safe_AddRef(m_pGameManager);


    m_pImGuiManager = CImGuiManager::GetInstance();

    strcpy_s(m_szSelectCamera, "FreeCamera");
#endif

    return S_OK;
}

void CDebugCheatUI::Update(_float fTimeDeleta)
{
#ifdef _DEBUG
    _bool bIsOpen = 1 - ENUM_CLASS(m_eVisibility);
    ImGui::Begin("Cheat List", &bIsOpen);
    ImGui::Checkbox("Object Teleport Picking", &m_bIsTeleport);
    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::Text("Button Select");
        ImGui::Text("Mesh or Terrian Picking Move Object");
        ImGui::EndTooltip();
    }

    if (ImGui::BeginCombo("Camera Change", m_szSelectCamera))
    {
        auto pCameras = m_pGameInstance->GetAllCamera();
        for (auto& iter : *pCameras)
        {
            CStringHelper::ConvertWideToUTF(iter.first.c_str(), m_szCameraComboTag);
            if (ImGui::Selectable(m_szCameraComboTag, false))
            {
                strcpy_s(m_szSelectCamera, m_szCameraComboTag);
                m_pGameInstance->SetMainCamera(iter.first.c_str());
            }
        }

        ImGui::EndCombo();
    }

    ImGui::End();

    if (m_bIsTeleport)
    {
        _float3 vPickingPoint = {};
        if (m_pGameInstance->KeyDown(KEY_INPUT::MOUSE, 0) &&
            m_pGameInstance->isPicking(&vPickingPoint))
        {
            auto pHierarchy = m_pImGuiManager->Find_ImGuiObject(TEXT("ImGui_Hierarchy"));
            static_cast<CDebugHierarchy*>(pHierarchy)->UpdateTransform(vPickingPoint);
        }
    }

    if (!bIsOpen)
        m_eVisibility = VISIBILITY::HIDDEN;
#endif // _DEBUG
}

HRESULT CDebugCheatUI::Render()
{
    return S_OK;
}

CDebugCheatUI* CDebugCheatUI::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CDebugCheatUI* pDebugCheatUI = new CDebugCheatUI(pDevice, pContext);
    if (FAILED(pDebugCheatUI->Initialize()))
    {
        Safe_Release(pDebugCheatUI);
        MSG_BOX("Create Fail : Debug Cheat UI");
    }
    return pDebugCheatUI;
}

CGameObject* CDebugCheatUI::Clone(void* pArg)
{
    return nullptr;
}

void CDebugCheatUI::Free()
{
    __super::Free();

#ifdef _DEBUG
    Safe_Release(m_pGameManager);
#endif // _DEBUG
}
