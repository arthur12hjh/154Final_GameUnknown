#include "pch.h"
#include "CameraActionInserter.h"

#include "GameInstance.h"
#include "ClientStruct.h"

#include "ImGuiManager.h"
#include "CameraActionInserter.h"

#include "Player.h"
#include "Camera_Free.h"
#include "Camera_Player.h"
#include "Camera_Action.h"

#include "StringHelper.h"
#include "GameManager.h"

CCameraActionInserter::CCameraActionInserter(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) :
    CGameObject(pDevice, pContext)
{
}

HRESULT CCameraActionInserter::Initialize()
{
#ifdef _DEBUG
    m_pGameManager = CGameManager::GetInstance();
    Safe_AddRef(m_pGameManager);

    m_pImGuiManager = CImGuiManager::GetInstance();
#endif

    return S_OK;
}

void CCameraActionInserter::Update(_float fTimeDeleta)
{
#ifdef _DEBUG
    if (nullptr == m_pCameraAnimationDatas)
        m_pCameraAnimationDatas = m_pGameManager->Get_CameraAnimationMap();


    _bool bIsOpen = 1 - ENUM_CLASS(m_eVisibility);
    ImGui::Begin("Camera Action Maker", &bIsOpen);

    WriteCameraActionDesc();

    ImGui::End();

    if (!bIsOpen)
        m_eVisibility = VISIBILITY::HIDDEN;
#endif // _DEBUG
}

HRESULT CCameraActionInserter::Render()
{
    return S_OK;
}

void CCameraActionInserter::WriteCameraActionDesc()
{
#ifdef _DEBUG
    static CAMERA_ANIMATION_DATA tCameraData{};
    static _int iSelectedTrackType = 0;    // 0=FOV, 1=Pivot, 2=BonePos, 3=BoneRot

    /* Base Info */
    ImGui::InputInt("CameraAnimationID", (_int*)&tCameraData.iCameraAnimationID);
    ImGui::InputInt("CameraAnimationFlag", (_int*)&tCameraData.iCameraAnimationFlag);

    ImGui::InputFloat("BaseFOV", &tCameraData.fBaseFOV);
    ImGui::InputFloat3("BaseCameraPivot", (_float*)&tCameraData.vBaseCameraPivot);
    ImGui::InputFloat3("BaseBonePosition", (_float*)&tCameraData.vBaseBonePosition);
    ImGui::InputFloat3("BaseBoneRotation", (_float*)&tCameraData.vBaseBoneRotation);

    ImGui::InputText("AnimationName", tCameraData.szCameraAnimationName, MAX_PATH);

    ImGui::Separator();

    /* Track Type Selection */
    const char* szTrackNames[] =
    {
        "FOV Track",
        "Pivot Track",
        "Bone Position Track",
        "Bone Rotation Track"
    };

    ImGui::Combo("TrackType", &iSelectedTrackType, szTrackNames, IM_ARRAYSIZE(szTrackNames));

    vector<CAMERA_TRACK_DESC>* pSelectedTrackList = nullptr;

    switch (iSelectedTrackType)
    {
    case 0: pSelectedTrackList = &tCameraData.FOVTrackList; break;
    case 1: pSelectedTrackList = &tCameraData.PivotTrackList; break;
    case 2: pSelectedTrackList = &tCameraData.BonePositionTrackList; break;
    case 3: pSelectedTrackList = &tCameraData.BoneRotationTrackList; break;
    }

    ImGui::Separator();

    /* Track List */
    if (pSelectedTrackList)
    {
        ImGui::Text("TrackCount : %d", (_int)pSelectedTrackList->size());

        if (ImGui::Button("AddTrack"))
        {
            CAMERA_TRACK_DESC tNewTrack{};
            pSelectedTrackList->push_back(tNewTrack);
        }

        ImGui::Separator();

        for (_uint i = 0; i < pSelectedTrackList->size(); ++i)
        {
            CAMERA_TRACK_DESC& tTrack = (*pSelectedTrackList)[i];
            string szHeader = "Track_" + std::to_string(i);

            if (ImGui::CollapsingHeader(szHeader.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::InputFloat("TrackPosition", &tTrack.fTrackPosition);
                ImGui::InputFloat3("TrackValue", (_float*)&tTrack.vTrackValue);
                ImGui::InputFloat("TangentStart", &tTrack.fTangentStart);
                ImGui::InputFloat("TangentEnd", &tTrack.fTangentEnd);
                ImGui::InputInt("InterpolationFlag", (_int*)&tTrack.iInterpolationFlag);

                ImGui::PushID(i);
                if (ImGui::Button("Delete"))
                {
                    pSelectedTrackList->erase(pSelectedTrackList->begin() + i);
                    ImGui::PopID();
                    break;
                }
                ImGui::PopID();
            }
        }
    }

    ImGui::Separator();

    

    /* Save Button */
    if (ImGui::Button("SaveCameraAnimation"))
    {
        m_pCameraAnimationDatas->emplace(tCameraData.iCameraAnimationID, tCameraData);

        m_pGameManager->Save_CameraAnimationData();
    }

    ImGui::Separator();
    
    static int iSelectedPlayCameraAnimationIndex;
    ImGui::InputInt("Playing CameraAnimationIndex", &iSelectedPlayCameraAnimationIndex);

    /* Play Button */
    if (ImGui::Button("PlayCameraAnimation"))
    {
        Play_CameraAnimationData(iSelectedPlayCameraAnimationIndex);
    }
#endif // _DEBUG
}

void CCameraActionInserter::Play_CameraAnimationData(int iSelectedPlayCameraAnimationIndex)
{
#ifdef _DEBUG
    _float4x4 PrePosMatrix = {};

    Safe_Release(m_pSelectCamera);
    m_pGameInstance->SetMainCamera(TEXT("ActionCamera"), &PrePosMatrix);
    m_pSelectCamera = m_pGameInstance->GetMainCamera();

    dynamic_cast<CCamera_Action*>(m_pSelectCamera)->Initialize_CameraAnimationData(iSelectedPlayCameraAnimationIndex);
#endif
}

CCameraActionInserter* CCameraActionInserter::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CCameraActionInserter* pCameraActionInserter = new CCameraActionInserter(pDevice, pContext);
    if (FAILED(pCameraActionInserter->Initialize()))
    {
        Safe_Release(pCameraActionInserter);
        MSG_BOX("Create Fail : Debug Cheat UI");
    }
    return pCameraActionInserter;
}

CGameObject* CCameraActionInserter::Clone(void* pArg)
{
    return nullptr;
}

void CCameraActionInserter::Free()
{
    __super::Free();

#ifdef _DEBUG
    Safe_Release(m_pSelectCamera);
    Safe_Release(m_pGameManager);
#endif // _DEBUG
}
