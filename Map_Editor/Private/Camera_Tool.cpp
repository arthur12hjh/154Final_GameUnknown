#include "pch.h"
#include "Camera_Tool.h"

#include "GameInstance.h"
#include "StringHelper.h"

#include "CinemaData.h"
#include "Camera_Free.h"

const WCHAR* m_szCinemaComponentName = TEXT("Prototype_Component_CinemaComponent");

CCamera_Tool::CCamera_Tool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) :
    m_pDevice(pDevice),
    m_pContext(pContext),
    m_pGameInstance(CGameInstance::GetInstance())
{
    Safe_AddRef(pDevice);
    Safe_AddRef(pContext);
    Safe_AddRef(m_pGameInstance);
}

HRESULT CCamera_Tool::Initialize()
{
    return S_OK;
}

void CCamera_Tool::Priority_Update(_float fTimeDelta)
{
}

void CCamera_Tool::Update(_float fTimeDelta)
{
}

void CCamera_Tool::Late_Update(_float fTimeDelta)
{
}

HRESULT CCamera_Tool::Render()
{
    ImGui::Begin("Camera Editor");

    Save_Cinematic_Action();
    ImGui::Separator();
    Load_Cinematic_Actions();

    if (ImGui::Button("ADD_Scene"))
        m_bIsADDCinemaScene = true;

    if (m_bIsADDCinemaScene)
        ADD_CinemaScene();

    if (m_pSelectSceneDatas)
    {
        ImGui::SameLine();
        if (ImGui::Button("ADD CinemaData"))
            m_bIsAddCinemaData = true;
    }

    if (ImGui::BeginCombo("Cinema Scene List", m_szViewCinemaScene))
    {
        auto& pCinemaScene = *m_pGameInstance->GetCinemaAllScenes();
        for (auto& pair : pCinemaScene)
        {
            CStringHelper::ConvertWideToUTF(pair.first.c_str(), m_szSelectable);
            if (ImGui::Selectable(m_szSelectable, false))
            {
                strcpy_s(m_szViewCinemaScene, m_szSelectable);
                m_pSelectSceneDatas = m_pGameInstance->GetCinemaSceneAllDatas(pair.first.c_str());
            }
        }
        ImGui::EndCombo();
    }
    
    if (m_pSelectSceneDatas)
    {
        ImGui::Separator();
        if (ImGui::BeginCombo("Cinema Data", m_szViewCinemaData))
        {
            if (m_pSelectSceneDatas->empty())
                strcpy_s(m_szViewCinemaData, "None");
            else
            {
                for (auto& pair : *m_pSelectSceneDatas)
                {
                    CStringHelper::ConvertWideToUTF(pair.first.c_str(), m_szSelectable);
                    if (ImGui::Selectable(m_szSelectable, false))
                    {
                        strcpy_s(m_szViewCinemaData, m_szSelectable);

                         auto iter = m_pSelectSceneDatas->find(pair.first.c_str());
                         if (iter == m_pSelectSceneDatas->end())
                             m_pCameraAnimation = nullptr;
                         else
                             m_pCameraAnimation = iter->second;
                    }
                }
            }
            ImGui::EndCombo();
        }

        if (m_bIsAddCinemaData)
            ADD_CinemaData();
    }

    if (m_bShowKeyInfo)
        Show_KeyFrameInfo();

    if (m_pCameraAnimation)
    {
        ImGui::Separator();

        if (ImGui::Button("ADD Key Frame"))
            m_bIsKeyFrame = true;

        if(m_bIsKeyFrame)
            ADD_KeyFrame();

        ImGui::SetNextItemWidth(150.f);
        ImGui::InputFloat("Loop Time", &m_fTime.y);
        ImGui::SameLine();
        ImGui::SetNextItemWidth(150.f);
        ImGui::InputFloat("Rate Time", &m_fRateTime);
        Show_KeyFrame(m_pGameInstance->Get_TimeDelta(TEXT("Timer_60")));
    }

    ImGui::End();
    return S_OK;
}

HRESULT CCamera_Tool::Save_Cinematic_Action()
{
    ImGui::InputText("Save File Name", m_szSaveFielPath, MAX_PATH);
    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::Text("File Path");
        ImGui::Text("ex : ../Bin/Save/DataFile/File.bin");
        ImGui::EndTooltip();
    }


    if (ImGui::Button("Save"))
    {
        WCHAR Conv_str[MAX_PATH] = {};
        CStringHelper::ConvertUTFToWide(m_szSaveFielPath, Conv_str);
        m_pGameInstance->SaveCinemaSceneData(Conv_str);
    }
  

    return S_OK;
}

HRESULT CCamera_Tool::Load_Cinematic_Actions()
{
    ImGui::InputText("Load File Name", m_szLoadFielPath, MAX_PATH);
    if (ImGui::Button("Load"))
    {

    }


    return S_OK;
}

void CCamera_Tool::ADD_CinemaScene()
{
    ImGui::Begin("ADD Chinema Scene");
    ImGui::InputText("Chinema Scene Tag", m_szCinemaSceneTag, MAX_PATH);
    if (ImGui::Button("Create"))
    {
        WCHAR Cnv_Str[MAX_PATH] = {};

        CStringHelper::ConvertUTFToWide(m_szCinemaSceneTag, Cnv_Str);
        m_pGameInstance->ADD_ChinemaSceneData(Cnv_Str, TEXT(""), nullptr);
        MSG_BOX("씬 생성 했습니다");
        m_bIsADDCinemaScene = false;
    }
    ImGui::SameLine();
    if (ImGui::Button("Cancel"))
        m_bIsADDCinemaScene = false;
    ImGui::End();
}

void CCamera_Tool::ADD_CinemaData()
{
    ImGui::Begin("ADD Chinema Data");
    ImGui::InputText("Chinema Scene Tag", m_szCinemaSceneTag, MAX_PATH);
    ImGui::InputText("Chinema Data Tag", m_szCinemaDataTag, MAX_PATH);
    if (ImGui::Button("Create"))
    {
        WCHAR Cnv_Str[MAX_PATH]{}, Cnv_Str2[MAX_PATH]{};
        CStringHelper::ConvertUTFToWide(m_szCinemaSceneTag, Cnv_Str);
        CStringHelper::ConvertUTFToWide(m_szCinemaDataTag, Cnv_Str2);

        auto pCinemaData = CCinemaData::Create(m_pDevice, m_pContext);
        if (nullptr == pCinemaData)
        {
            MSG_BOX("시네마틱 데이터 생성 실패");
            m_bIsAddCinemaData = false;
        }
        m_pGameInstance->ADD_ChinemaSceneData(Cnv_Str, Cnv_Str2, pCinemaData);
    }
    ImGui::SameLine();
    if (ImGui::Button("Cancel"))
        m_bIsAddCinemaData = false;
    ImGui::End();
}

void CCamera_Tool::ADD_KeyFrame()
{
    ImGui::Begin("ADD KeyFrame Data");

    // 이거 나중에 타입으로 바꿔서 보여줄거임
    // 카메라인지 모델인지 해서
    ImGui::Separator();
    ImGui::DragFloat3("Roation", m_vRotation);
    ImGui::DragFloat3("Translate", m_vTranslate);

    ImGui::Text("CameraInfo");
    ImGui::Separator();
    ImGui::InputFloat("Fov", &m_fFov);
    ImGui::InputFloat("Far", &m_fFar);
    ImGui::InputFloat("Near", &m_fNear);
    if (ImGui::Button("Create"))
    {
        auto pKeyFrame = new CAMERA_KEYFRAME();
        pKeyFrame->fFov = m_fFov;
        pKeyFrame->fFar = m_fFar;
        pKeyFrame->fNear = m_fNear;

        pKeyFrame->vScale = { 1.f, 1.f, 1.f };
        memcpy(&pKeyFrame->vRotation, m_vRotation, sizeof(_float3));
        memcpy(&pKeyFrame->vTranslation, m_vTranslate, sizeof(_float3));
        m_pCameraAnimation->Insert_KeyFrame(pKeyFrame);
    }
    ImGui::SameLine();
    if (ImGui::Button("Cancel"))
        m_bIsKeyFrame = false;
    ImGui::End();
}

void CCamera_Tool::Show_KeyFrameInfo()
{
    ImGui::Begin("Show KeyFrame Info");
    auto pKeyFrame = m_pCameraAnimation->GetFrameList();

    //일단 카메라만 전경 찍을수 있게
    if (ImGui::BeginCombo("KeyFrame Num :", m_KeyFrameNum))
    {
        _uint iNumKeyFrame = (_uint)pKeyFrame->size();
        for (_uint i = 0; i < iNumKeyFrame; ++i)
        {
            if (ImGui::Selectable(to_string(i).c_str(), false))
            {
                // 여기서 타입 나눠요 카메라 키프레임만 심게 해둘게요
                m_CameraKeyFrameInfo = *static_cast<CAMERA_KEYFRAME*>((*pKeyFrame)[i]);
                strcpy_s(m_KeyFrameNum, to_string(i).c_str());
            }
        }
        ImGui::EndCombo();
    }

    ImGui::Separator();
    ImGui::Text("Cam KeyFrame Info");
    ImGui::Text("Rotation : %.2f %.2f %.2f", m_CameraKeyFrameInfo.vRotation.x, m_CameraKeyFrameInfo.vRotation.y, m_CameraKeyFrameInfo.vRotation.z);
    ImGui::Text("Rotation : %.2f %.2f %.2f", m_CameraKeyFrameInfo.vTranslation.x, m_CameraKeyFrameInfo.vTranslation.y, m_CameraKeyFrameInfo.vTranslation.z);
    ImGui::Text("Fov : %.2f", m_CameraKeyFrameInfo.fFov);
    ImGui::Text("Far : %.2f", m_CameraKeyFrameInfo.fFar);
    ImGui::Text("Near : %.2f", m_CameraKeyFrameInfo.fNear);
    ImGui::End();
}

void CCamera_Tool::Show_KeyFrame(_float fTimeDelta)
{
    if (ImGui::Button("Play"))
        m_bIsPause = false;

    ImGui::SameLine();
    if (ImGui::Button("Pause"))
        m_bIsPause = true;

    ImGui::SameLine();
    if (ImGui::Button("Show KeyFrame Info"))
        m_bShowKeyInfo = true;

    if (ImGui::Button("Test Action"))
    {
        auto pCamera = m_pGameInstance->GetMainCamera();
        static_cast<CCamera_Free*>(pCamera)->CameraAnimtaionTest(m_pCameraAnimation, m_fTime.y);
        Safe_Release(pCamera);
    }

    //사각형 영역의 가장 작은점 두개
    ImVec2 LT_Size = ImGui::GetCursorScreenPos();
    
    //현재 내가 그려진 창의 가로 사이즈를 가져오는 함수
    _float timelineWidth = ImGui::GetContentRegionAvail().x;;
    _float timelineHeight = 50;

    // 내가 그릴 타임라인 영역의 Width와 Hegiht를 입력해주면된다.
    // 이때 ImGui 특성상 Start 부터 더해줘야 정상적인 결과가 나온다.
    ImVec2 RB_Size = { LT_Size.x + timelineWidth, LT_Size.y + timelineHeight };

    // 현재 그리는 ImGui 패널의 DrawList(그리기 버퍼) 가져오는 함수라고함
    auto draw = ImGui::GetWindowDrawList();
    draw->AddRectFilled(LT_Size, RB_Size, IM_COL32(40, 40, 40, 255));

    _float iNumKeyFrame = m_pCameraAnimation->GetNumKeyFrame();
    if (0 == iNumKeyFrame)
        iNumKeyFrame += 1;

    _float LinePadding = (timelineWidth / iNumKeyFrame);
    for (_uint i = 0; i < iNumKeyFrame; ++i)
    {
        ImVec2 p0 = ImVec2(LT_Size.x + i * LinePadding, LT_Size.y);
        ImVec2 p1 = ImVec2(p0.x + 2, p0.y + timelineHeight);

        draw->AddRectFilled(p0, p1, IM_COL32(200, 200, 200, 255));
        ImVec2 CenterP = ImVec2(p0.x, (p1.y + p0.y) * 0.5f);
        draw->AddCircle(CenterP, 5.f, IM_COL32(0, 255, 0, 255));
    }

    ImVec2 p0 = ImVec2(LT_Size.x + timelineWidth - 2, LT_Size.y);
    ImVec2 p1 = ImVec2(p0.x + 2, p0.y + timelineHeight);
    draw->AddRectFilled(p0, p1, IM_COL32(200, 200, 200, 255));

    ImGuiIO& io = ImGui::GetIO();
    if (io.MouseClicked[0])
    {
        if (LT_Size.x <= io.MousePos.x && io.MousePos.x <= RB_Size.x &&
            LT_Size.y <= io.MousePos.y && io.MousePos.y <= RB_Size.y)
        {
            m_vTimeLinePos = io.MousePos;
            m_vTimeLinePos.x -= LT_Size.x;
            m_vTimeLinePos.y -= LT_Size.y;
        }
    }

    // 현재 프레임 받아서 그리기
    if (false == m_bIsPause)
    {
        m_fTime.x += fTimeDelta * m_fRateTime;
        if (m_fTime.y <= m_fTime.x)
            m_fTime.x = 0.f;

        m_vTimeLinePos = p0 = ImVec2(LT_Size.x + m_fTime.x * LinePadding, LT_Size.y);
        p1 = ImVec2(p0.x + 2, p0.y + timelineHeight);

        draw->AddRectFilled(p0, p1, IM_COL32(255, 0, 0, 255));

        p0 = ImVec2(LT_Size.x + m_fTime.x * LinePadding, LT_Size.y);
        p1 = ImVec2(p0.x + 2, p0.y + timelineHeight);
    }
    else
    {
        p0 = { LT_Size.x + m_vTimeLinePos.x,  LT_Size.y };
        p1 = { p0.x + 2, p0.y + timelineHeight };
    }

    draw->AddRectFilled(p0, p1, IM_COL32(255, 0, 0, 255));
}

CCamera_Tool* CCamera_Tool::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CCamera_Tool* pCamera_Tool = new CCamera_Tool(pDevice, pContext);
    if (FAILED(pCamera_Tool->Initialize()))
    {
        Safe_Release(pCamera_Tool);
        MSG_BOX("Create Fail : Camera Tool");
    }
    return pCamera_Tool;
}

void CCamera_Tool::Free()
{
    __super::Free();

    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
    Safe_Release(m_pGameInstance);
}
