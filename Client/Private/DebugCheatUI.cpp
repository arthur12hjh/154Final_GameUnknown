#include "pch.h"
#include "DebugCheatUI.h"

#include "GameInstance.h"
#include "ClientStruct.h"

#include "ImGuiManager.h"
#include "DebugHierarchy.h"

#include "Nayitba.h"
#include "Player.h"
#include "Camera_Free.h"
#include "Camera_Player.h"

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

    if (m_pSelectCamera)
    {
        if (m_pSelectCamera->isDead())
            m_pSelectCamera = nullptr;
    }
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

void CDebugCheatUI::SetLevelMainCamera()
{
#ifdef _DEBUG
    m_pSelectCamera = m_pGameInstance->GetMainCamera();
    if (nullptr == m_pSelectCamera || m_pSelectCamera->isDead())
    {
        strcpy_s(m_szSelectCamera, "Not Find Main Camera");
    }
    else
    {
        Safe_Release(m_pSelectCamera);
        auto pCameras = m_pGameInstance->GetAllCamera();
        for (auto& iter : *pCameras)
        {
            CStringHelper::ConvertWideToUTF(iter.first.c_str(), m_szCameraComboTag);
            if (iter.second == m_pSelectCamera)
                strcpy_s(m_szSelectCamera, m_szCameraComboTag);

            auto pFree_Camera = static_cast<CCamera_Free*>(m_pSelectCamera);
            pFree_Camera->GetCameraLock(m_bIsCameraLock);
        }
    }

    
#endif // _DEBUG
}

void CDebugCheatUI::ResetLevelCamera()
{
#ifdef _DEBUG
    m_pSelectCamera = nullptr;
#endif // _DEBUG
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

    ImGui::InputFloat3("Teleport Point", m_vTeleportPoint);
    if (ImGui::Button("Player Teleport Button"))
    {
        auto pPlayer = CGameManager::GetInstance()->GetGameCharacter();
        if (nullptr == pPlayer)
            return;

        pPlayer->GetTransform()->Set_State(STATE::POSITION, XMVectorSet(m_vTeleportPoint[0], m_vTeleportPoint[1], m_vTeleportPoint[2], 1.f));
        static_cast<CCharacterController*>(pPlayer->Find_Component(TEXT("Com_CCT")))->Set_Position(pPlayer->GetTransform()->Get_State(STATE::POSITION));
        Safe_Release(pPlayer);
    }

    ImGui::Dummy(ImVec2(0.f, 10.f));
    // 12.09 서민석 - 텔포 포인트 하나 임의로 추가해뒀음
    // 얘는 치우지 말아다오
    if (ImGui::Button("Teleport Player To Elevator"))
    {

        auto pPlayer = CGameManager::GetInstance()->GetGameCharacter();
        if (nullptr == pPlayer)
            return;

        pPlayer->GetTransform()->Set_State(STATE::POSITION, XMVectorSet(479.678f, 24.858f, 328.388f, 1.f));
        static_cast<CCharacterController*>(pPlayer->Find_Component(TEXT("Com_CCT")))->Set_Position(pPlayer->GetTransform()->Get_State(STATE::POSITION));
        Safe_Release(pPlayer);
    }

    if (ImGui::Button("Teleport Player To Gigas"))
    {

        auto pPlayer = CGameManager::GetInstance()->GetGameCharacter();
        if (nullptr == pPlayer)
            return;

        pPlayer->GetTransform()->Set_State(STATE::POSITION, XMVectorSet(686.786f, 14.520f, 558.247f, 1.f));
        static_cast<CCharacterController*>(pPlayer->Find_Component(TEXT("Com_CCT")))->Set_Position(pPlayer->GetTransform()->Get_State(STATE::POSITION));
        Safe_Release(pPlayer);
    }

    //12.10 서민석 - 테스트용 몬스터 소환로직
    //이건 나중에 치워도 될듯
    if (ImGui::Button("Spawn Monster Front"))
    {
        auto pPlayer = CGameManager::GetInstance()->GetGameCharacter();
        if (nullptr == pPlayer)
            return;

        _vector vPos = pPlayer->GetTransform()->Get_State(STATE::POSITION) + pPlayer->GetTransform()->Get_State(STATE::LOOK) * 5.f;

        Safe_Release(pPlayer);

        CNaytiba::NAYITBA_DESC Desc = {};
        Desc.bIsApplyTransform = true;
        Desc.vScale = { 1.f, 1.f, 1.f };
        Desc.iMonsterID = 4;
        Desc.bIsSuperMonster = false;

        Desc.vPosition = { XMVectorGetX(vPos), XMVectorGetY(vPos), XMVectorGetZ(vPos) };
        if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Nayitba"),
            ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_Monster"), &Desc)))
            return;
    }

#endif // _DEBUG
}

void CDebugCheatUI::DrawCameraDebug()
{
#ifdef _DEBUG
    ImGui::Text("Camera Debug");


    // 12.09 서민석 - 트랜스폼 보는거 없길래 추가해둠 
    // 문제생기면 말 ㄱㄱ
    if (nullptr != m_pSelectCamera)
    {
        _float3 vCamPosition = {};
        _float3 vCamLook = {};
        _float3 vCamLookPosition = {};
        XMStoreFloat3(&vCamPosition, m_pSelectCamera->GetTransform()->Get_State(STATE::POSITION));
        XMStoreFloat3(&vCamLook, m_pSelectCamera->GetTransform()->Get_State(STATE::LOOK));
		XMStoreFloat3(&vCamLookPosition, XMLoadFloat3(&vCamPosition) + XMLoadFloat3(&vCamLook) * 50.f);


        ImGui::Dummy(ImVec2(0.f, 5.f));
        ImGui::Text("Camera Position : %.3f , %.3f , %.3f", vCamPosition.x, vCamPosition.y, vCamPosition.z);
        ImGui::Dummy(ImVec2(0.f, 5.f));
        ImGui::Text("Camera Gara Look : %.3f , %.3f , %.3f", vCamLookPosition.x, vCamLookPosition.y, vCamLookPosition.z);
        ImGui::Dummy(ImVec2(0.f, 5.f));
    }

    if (ImGui::InputFloat("Camera Transform", &m_fFreeCamSpeed))
        static_cast<CCamera_Free*>(m_pSelectCamera)->SetCameraSpeed(m_fFreeCamSpeed);

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
                    m_pGameInstance->SetMainCamera(iter.first.c_str(), &PrePosMatrix);
                    m_pSelectCamera = m_pGameInstance->GetMainCamera();
                    Safe_Release(m_pSelectCamera);
                }

                auto pFree_Camera = dynamic_cast<CCamera_Free*>(m_pSelectCamera);
                if (pFree_Camera)
                {
                    pFree_Camera->GetCameraLock(m_bIsCameraLock);
                    pFree_Camera->SetCameraAnimation(&PrePosMatrix, m_pSelectCamera->GetTransform()->Get_WorldMatrixPtr(), m_bIsCamLerp);
                    m_bIsFreeCamera = true;
                }
                else
                    m_bIsFreeCamera = false;
            }
        }

        ImGui::EndCombo();
    }

    if (m_bIsFreeCamera)
    {
        if (ImGui::InputFloat("FreeCam Speed", &m_fFreeCamSpeed))
            static_cast<CCamera_Free*>(m_pSelectCamera)->SetCameraSpeed(m_fFreeCamSpeed);
    }
    else
    {
        if (ImGui::DragFloat("PlayerCam Distance", &m_fCamDistance, 0.01f, 2.f, 50.f))
            static_cast<CCamera_Player*>(m_pSelectCamera)->Set_Distance(m_fCamDistance);
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
    Safe_Release(m_pGameManager);
#endif // _DEBUG
}
