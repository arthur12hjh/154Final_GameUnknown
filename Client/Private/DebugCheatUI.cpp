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
    DrawCameraDebug();
    ImGui::Separator();
    DrawLightDebug();
    ImGui::Separator();
    DrawGameSpeedDebug();

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
#ifdef _DEBUG
    ImGui::Checkbox("Object Teleport Picking", &m_bIsTeleport);
    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::Text("Button Select");
        ImGui::Text("Mesh or Terrian Picking Move Object");
        ImGui::EndTooltip();
    }

#endif // _DEBUG
}

void CDebugCheatUI::DrawCameraDebug()
{
#ifdef _DEBUG

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
                if (m_pSelectCamera != iter.second)
                {
                    Safe_Release(m_pSelectCamera);
                    m_pGameInstance->SetMainCamera(iter.first.c_str(), &PrePosMatrix);
                    m_pSelectCamera = m_pGameInstance->GetMainCamera();
                }

                auto pFree_Camera = static_cast<CCamera_Free*>(m_pSelectCamera);
                pFree_Camera->GetCameraLock(m_bIsCameraLock);
                pFree_Camera->SetCameraAnimation(&PrePosMatrix, m_pSelectCamera->GetTransform()->Get_WorldMatrixPtr(), m_bIsCamLerp);
            }
        }

        ImGui::EndCombo();
    }

    if (ImGui::Checkbox("KeyBoard Lock", &m_bIsCameraLock[0]))
    {
        // 이거 일단 프리카메라만 했는데 필요하다면 Camera쪽에다가 이전하겠음
        if (m_pSelectCamera)
            static_cast<CCamera_Free*>(m_pSelectCamera)->CameraLock(m_bIsCameraLock[0], m_bIsCameraLock[1]);
        else
            MSG_BOX("Not Select Camera");
    }
    if (ImGui::Checkbox("Mouse Lock", &m_bIsCameraLock[1]) || m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_G))
    {
        // 이거 일단 프리카메라만 했는데 필요하다면 Camera쪽에다가 이전하겠음
        if (m_pSelectCamera)
            static_cast<CCamera_Free*>(m_pSelectCamera)->CameraLock(m_bIsCameraLock[0], m_bIsCameraLock[1]);
        else
            MSG_BOX("Not Select Camera");
    }
#endif // _DEBUG
}

void CDebugCheatUI::DrawLightDebug()
{
#ifdef _DEBUG
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
                    strcpy_s(m_szVisbility, g_szVisibility[1]);
                else
                    strcpy_s(m_szVisbility, g_szVisibility[0]);
            }

            iIndex++;
        }

        ImGui::EndCombo();
    }

    if (m_pSelectLight)
    {
        ImGui::Checkbox("Show Light Info ", &m_ShowLightInfo);
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

#pragma region Light Info
        if (m_ShowLightInfo)
        {
            ImGui::Begin("LIGHT INFO");
            ImGui::Text("Type :");
            ImGui::SameLine();
            auto pLightInfo = m_pSelectLight->Get_LightDesc();
            switch (pLightInfo->eType)
            {
            case LIGHT_TYPE::DIRECTIONAL:
                ImGui::Text("DIRECTIONAL");
                break;
            case LIGHT_TYPE::POINT:
                ImGui::Text("POINT");
                break;
            case LIGHT_TYPE::SPOT:
                ImGui::Text("SPOT");
                break;
            }
            ImGui::Separator();
            ImGui::Text("Diffuse : %.2f %.2f %.2f %.2f", pLightInfo->vDiffuse.x, pLightInfo->vDiffuse.y, pLightInfo->vDiffuse.z, pLightInfo->vDiffuse.w);
            ImGui::Separator();
            ImGui::Text("Ambient : %.2f %.2f %.2f %.2f", pLightInfo->vAmbient.x, pLightInfo->vAmbient.y, pLightInfo->vAmbient.z, pLightInfo->vAmbient.w);
            ImGui::Separator();
            ImGui::Text("Specular : %.2f %.2f %.2f %.2f", pLightInfo->vSpecular.x, pLightInfo->vSpecular.y, pLightInfo->vSpecular.z, pLightInfo->vSpecular.w);
            ImGui::Separator();
            if (LIGHT_TYPE::DIRECTIONAL == pLightInfo->eType)
            {
                ImGui::Text("Direction : %.2f %.2f %.2f %.2f", pLightInfo->vDirection.x, pLightInfo->vDirection.y, pLightInfo->vDirection.z, pLightInfo->vDirection.w);
                ImGui::Separator();
            }
            ImGui::Text("vPosition : %.2f %.2f %.2f %.2f", pLightInfo->vPosition.x, pLightInfo->vPosition.y, pLightInfo->vPosition.z, pLightInfo->vPosition.w);
            ImGui::End();
        }
#pragma endregion
    }

#endif // _DEBUG
}

void CDebugCheatUI::DrawGameSpeedDebug()
{
#ifdef _DEBUG
    ImGui::Text("Game Speed Debug");

    if(ImGui::SliderFloat("Game Speed Ratio", &m_fGameSpeed, 0.001f, 3.f))
    {
       m_pGameInstance->SetGameSpeed(m_fGameSpeed);
    }
    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::Text("DeltaTime Mulitply Ratio");
        ImGui::EndTooltip();
    }


#endif // _DEBUG
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
    Safe_Release(m_pSelectCamera);
    Safe_Release(m_pGameManager);
#endif // _DEBUG
}
