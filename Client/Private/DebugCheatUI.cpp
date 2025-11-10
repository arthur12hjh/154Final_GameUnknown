#include "pch.h"
#include "DebugCheatUI.h"

#include "GameInstance.h"
#include "ClientStruct.h"

#include "ImGuiManager.h"
#include "DebugHierarchy.h"
#include "Camera_Free.h"
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

    DrawObjectDebug();
    ImGui::Separator();
    DrawCaemraDebug();
    ImGui::Separator();
    DrawLightDebug();

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

void CDebugCheatUI::DrawObjectDebug()
{
    ImGui::Checkbox("Object Teleport Picking", &m_bIsTeleport);
    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::Text("Button Select");
        ImGui::Text("Mesh or Terrian Picking Move Object");
        ImGui::EndTooltip();
    }
}

void CDebugCheatUI::DrawCaemraDebug()
{
    ImGui::Text("Camera Debug");
    ImGui::Checkbox("Camera Lerp Tirrger", &m_bIsCamLerp);
    if (ImGui::BeginCombo("Camera Change", m_szSelectCamera))
    {
        auto pCameras = m_pGameInstance->GetAllCamera();
        for (auto& iter : *pCameras)
        {
            CStringHelper::ConvertWideToUTF(iter.first.c_str(), m_szCameraComboTag);
            if (ImGui::Selectable(m_szCameraComboTag, false))
            {
                strcpy_s(m_szSelectCamera, m_szCameraComboTag);

                _float4x4 PrePosMatrix = {};
                m_pGameInstance->SetMainCamera(iter.first.c_str(), &PrePosMatrix);
                auto pCamera = m_pGameInstance->GetMainCamera();

                static_cast<CCamera_Free*>(pCamera)->SetCameraAnimation(&PrePosMatrix, pCamera->GetTransform()->Get_WorldMatrixPtr(), m_bIsCamLerp);
                Safe_Release(pCamera);
            }
        }

        ImGui::EndCombo();
    }
}

void CDebugCheatUI::DrawLightDebug()
{
    auto pLights = m_pGameInstance->GetAllLight();

    ImGui::Text("Light Debug");
    if (ImGui::BeginCombo("Select Light", m_szSelectLight))
    {
        _uint iIndex = {};
        for (auto& iter : *pLights)
        {
            string Tag = "Light" + to_string(iIndex);
            if (ImGui::Selectable(Tag.c_str(), false))
            {
                strcpy_s(m_szSelectLight, Tag.c_str());
                m_pSelectLight = m_pGameInstance->Find_Light(iIndex);

                if(VISIBILITY::HIDDEN == m_pSelectLight->GetVisbility())
                    strcpy_s(m_szSelectLight, g_szVisibility[1]);
                else
                    strcpy_s(m_szSelectLight, g_szVisibility[0]);
            }

            iIndex++;
        }

        ImGui::EndCombo();
    }

    if (m_pSelectLight)
    {
        if (ImGui::BeginCombo("VISIBILITY", m_szVisbility))
        {
            for (_uint i = 0; i < ENUM_CLASS(VISIBILITY::END); ++i)
            {
                if (ImGui::Selectable(g_szVisibility[i], false))
                {
                    strcpy_s(m_szVisbility, g_szVisibility[i]);
                    if (0 == i)
                        m_pSelectLight->SetVisibility(VISIBILITY::VISIBLE);
                    else
                        m_pSelectLight->SetVisibility(VISIBILITY::HIDDEN);
                }
            }
            ImGui::EndCombo();
        }
    }
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
