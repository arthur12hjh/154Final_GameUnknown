#include "pch.h"
#include "Level_BeatSaber.h"

#include "GameInstance.h"
#include "Camera_Free.h"

#include "UIHUD.h"
#include "Level_Loading.h"
#include "StringHelper.h"
#include "BeatSaberSpawner.h"
#include "Note.h"

CLevel_BeatSaber::CLevel_BeatSaber(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevelID) :
    CLevel(pDevice, pContext, ENUM_CLASS(eLevelID))
{
}

HRESULT CLevel_BeatSaber::Initialize()
{
    m_pGameInstance->Manager_StopSound(CHANNELID::BGM);
    m_pGameInstance->Manager_PlayBGM(TEXT("CountingStar.mp3"), 0.7f);

    if (FAILED(Load_SongList("../Bin/DataFiles/NoteData/SongList.csv")))
        return E_FAIL;

    if (FAILED(Ready_Lights()))
        return E_FAIL;

    if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
        return E_FAIL;

    /*if (FAILED(Ready_Layer_Terrain(TEXT("Layer_Terrian"))))
        return E_FAIL;*/

    if (FAILED(Ready_Layer_Sky(TEXT("Layer_Sky"))))
        return E_FAIL;

    if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
        return E_FAIL;

    if (FAILED(Ready_Layer_Player(TEXT("Layer_Player"))))
        return E_FAIL;

    if (FAILED(Ready_Layer_BeatSpawner(TEXT("Layer_BeatSaberSpawner"))))

        return E_FAIL;
    if (FAILED(Ready_Layer_UI(TEXT("Layer_UserInterface"))))
        return E_FAIL;

    Load_Dororong_Saber_Objects("../../Map_Editor/Bin/DataFiles/Dororong_Saber.bin");

    return S_OK;
}

void CLevel_BeatSaber::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);

    /*if (m_isOverlay && m_pHUD)
    {
        static_cast<CUIHUD*>(m_pHUD)->Anim_Play(TEXT("Layer_Combat"), TEXT("GamePlay_Overlay"), TEXT("Intro"));
        m_isOverlay = false;
    }*/

    if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_INSERT))
        Play_GameBGM(TEXT("Test"), 1.f);

    if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_F12))
    {
        if(m_pHUD)
            dynamic_cast<CUIHUD*>(m_pHUD)->Reset_AllWorldUI_State();

        if (FAILED(m_pGameInstance->Change_Level(CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL::LOADING, LEVEL::GAMEPLAY, false))))
            return;

        return;
    }

    /*if (m_bChangeLevel && !m_bLevelTransitioning)
    {
        dynamic_cast<CUIHUD*>(m_pHUD)->Anim_Play(TEXT("Layer_Combat"), TEXT("GamePlay_Overlay"), TEXT("Outro"));
        m_bLevelTransitioning = true;
    }*/

    // 결과창 테스트용
    if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_5))
    {
        static_cast<CUIHUD*>(m_pHUD)->Open_Result();
    }
    if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_6))
    {
        static_cast<CUIHUD*>(m_pHUD)->Close_Result();
    }
}

HRESULT CLevel_BeatSaber::Render()
{
#ifdef _DEBUG
    SetWindowText(g_hWnd, TEXT("Game Play Level"));
#else
    SetWindowText(g_hWnd, m_pGameInstance->GetFrameText());
#endif // _DEBUG


    return S_OK;
}

HRESULT CLevel_BeatSaber::Ready_Lights()
{
    LIGHT_DESC			LightDesc{};

    LightDesc.eType = LIGHT_TYPE::DIRECTIONAL;
    LightDesc.vDiffuse = _float4(1.05f, 1.02f, 0.93f, 1.f);
    LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
    LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);
    LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);

    if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
        return E_FAIL;

    CASCADE_SHADOW_DESC		CascadeShadowDesc{};
    CascadeShadowDesc.vDir = _float4(1.f, -1.f, 1.f, 0.f);
    if (FAILED(m_pGameInstance->Ready_CascadeShadow_Light(CascadeShadowDesc)))
        return E_FAIL;

    STATIC_SHADOW_DESC		StaticShadowDesc{};
    StaticShadowDesc.fFar = 2000.f;
    StaticShadowDesc.fNear = 0.1f;
    StaticShadowDesc.vAt = _float4(400.f, 300.f, 0.f, 1.f);

    if (FAILED(m_pGameInstance->Ready_StaticShadow_Light(StaticShadowDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_BeatSaber::Ready_Layer_BackGround(const _wstring& strLayerTag)
{
    return S_OK;
}

HRESULT CLevel_BeatSaber::Ready_Layer_Terrain(const _wstring& strLayerTag)
{
    if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Terrain"),
        ENUM_CLASS(LEVEL::BEATSABER_GAME), strLayerTag)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_BeatSaber::Ready_Layer_Sky(const _wstring& strLayerTag)
{
    if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::LEVEL_PROB), TEXT("Prototype_GameObject_Sky_Dororong"),
        ENUM_CLASS(LEVEL::BEATSABER_GAME), strLayerTag)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_BeatSaber::Ready_Layer_Camera(const _wstring& strLayerTag)
{
    CCamera_Free::CAMERA_FREE_DESC			CameraDesc{};
    CameraDesc.fFov = XMConvertToRadians(60.0f);
    CameraDesc.fNear = 0.1f;
    CameraDesc.fFar = 500.f;
    CameraDesc.vEye = _float3(0.f, 10.f, -10.f);
    CameraDesc.vAt = _float3(0.f, 0.f, 0.f);
    CameraDesc.fSpeedPerSec = 5.f;
    CameraDesc.fRotationPerSec = XMConvertToRadians(90.0f);
    CameraDesc.fMouseSensor = 0.1f;

    auto pCamera = m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Camera_Free"), &CameraDesc);
    m_pGameInstance->Add_Camera(TEXT("FreeCamera"), static_cast<CCamera*>(pCamera));

    CameraDesc.fFov = XMConvertToRadians(60.0f);
    CameraDesc.fNear = 0.1f;
    CameraDesc.fFar = 100.f;
    //CameraDesc.vEye = _float3(-1.1f, 1.9f, -3.8f);
    CameraDesc.vEye = _float3(-3.871f, 9.062f, -4.583f);
    CameraDesc.vAt = _float3(30.f, -5.5f, 30.f);
    CameraDesc.fSpeedPerSec = 5.f;
    CameraDesc.fRotationPerSec = XMConvertToRadians(90.0f);
    CameraDesc.fMouseSensor = 0.1f;

    pCamera = m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::BEATSABER_GAME), TEXT("Prototype_GameObject_Camera_BeatSaber"), &CameraDesc);
    m_pGameInstance->Add_Camera(TEXT("GameCamera"), static_cast<CCamera*>(pCamera));
    m_pGameInstance->SetMainCamera(TEXT("GameCamera"));
    return S_OK;
}

HRESULT CLevel_BeatSaber::Ready_Layer_Player(const _wstring& strLayerTag)
{
    CGameObject::GAMEOBJECT_DESC Desc = {};
    Desc.bIsApplyTransform = true;
    Desc.vScale = { 1.f, 1.f, 1.f };
    Desc.vRotation = { 0.f , XMConvertToRadians(210.f), 0.f, 0.f };
    Desc.vPosition = { 0.f, 4.f, -0.6f };
    Desc.fRotationPerSec = XMConvertToRadians(180.0f);
    Desc.fSpeedPerSec = 10.f;

    if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::BEATSABER_GAME), TEXT("Prototype_GameObject_BeatSaberCharacter"),
        ENUM_CLASS(LEVEL::BEATSABER_GAME), strLayerTag, &Desc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_BeatSaber::Ready_Layer_UI(const _wstring& strLayerTag)
{
    CUIHUD* pUIHUD = CUIHUD::Create(m_pDevice, m_pContext);

    if (pUIHUD == nullptr)
        return E_FAIL;

    SetHUD(pUIHUD);
    pUIHUD->Set_Show_Debug_Rect(false);

    if (FAILED(pUIHUD->Load_Data(TEXT("Layer_BeatSaber_Overlay"))))
        return E_FAIL;

    if (FAILED(pUIHUD->Load_Data(TEXT("Layer_Song_Selector"))))
        return E_FAIL;

    if (FAILED(pUIHUD->Load_Data(TEXT("Layer_BeatSaber"))))
        return E_FAIL;

    if (FAILED(pUIHUD->Load_Data(TEXT("Layer_BeatSaber_Result"))))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_BeatSaber::Ready_Layer_BeatSpawner(const _wstring& strLayerTag)
{
    auto pLayerList = m_pGameInstance->GetAllObejctToLayer(ENUM_CLASS(LEVEL::BEATSABER_GAME), TEXT("Layer_Player"));
    if (nullptr == pLayerList)
        return E_FAIL;

    CBeatSaberSpawner::BEATSABER_SPAWNER_DESC Desc = {};
    Desc.bIsApplyTransform = true;
    Desc.vScale = { 1.f, 1.f, 1.f };
    Desc.vRotation = { 0.f , XMConvertToRadians(210.f), 0.f, 0.f };
    Desc.vPosition = { 30.f, 4.f, 30.f };
    Desc.fRotationPerSec = XMConvertToRadians(180.0f);
    Desc.fSpeedPerSec = 10.f;
    Desc.pPlayerTransform = pLayerList->front()->GetTransform()->Get_WorldMatrixPtr();

    if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::BEATSABER_GAME), TEXT("Prototype_GameObject_BeatSaberSpawner"),
        ENUM_CLASS(LEVEL::BEATSABER_GAME), strLayerTag, &Desc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_BeatSaber::Load_SongList(const char* szDataFile)
{
    WCHAR            szKeyName[MAX_PATH] = {};
    NOTE_DATA_NAME Data = {};

    vector<string> DataList;
    CStringHelper::CSVRead(szDataFile, DataList);

    _uint iLastIndex = (_uint)DataList.size();
    for (_uint i = 3; i < iLastIndex;)
    {
        CStringHelper::ConvertUTFToWide(DataList[i++].c_str(), szKeyName);
        CStringHelper::ConvertUTFToWide(DataList[i++].c_str(), Data.SongFileName);
        strcpy_s(Data.NoteFileName, DataList[i++].c_str());

        m_SongList.emplace(szKeyName, Data);
    }

    return S_OK;
}

void CLevel_BeatSaber::Play_GameBGM(const wstring& szFileTag, _float fVolume)
{
    m_pGameInstance->Manager_StopAll();
    NOTE_DATA_NAME* pData = Get_FindSongFile(szFileTag);
    if (nullptr == pData)
        return;

    auto pList = m_pGameInstance->GetAllObejctToLayer(ENUM_CLASS(LEVEL::BEATSABER_GAME), TEXT("Layer_BeatSaberSpawner"));
    if (nullptr == pList)
        return;

    m_pGameInstance->Manager_PlayBGM(pData->SongFileName, fVolume, 0,
        [&](FMOD_CHANNELCONTROL* channelcontrol,
            FMOD_CHANNELCONTROL_TYPE controltype,
            FMOD_CHANNELCONTROL_CALLBACK_TYPE callbacktype,
            void* commanddata1,
            void* commanddata2)
        {
            Finished_GameBGM(channelcontrol, controltype, callbacktype, commanddata1, commanddata2);
        });

    static_cast<CBeatSaberSpawner*>(pList->front())->Load_BeatData(pData->NoteFileName);
}

CLevel_BeatSaber::NOTE_DATA_NAME* CLevel_BeatSaber::Get_FindSongFile(const wstring& szFileTag)
{
    auto iter = m_SongList.find(szFileTag);
    if (iter == m_SongList.end())
        return nullptr;

    return &iter->second;
}

FMOD_RESULT CLevel_BeatSaber::Finished_GameBGM(FMOD_CHANNELCONTROL* channelcontrol, FMOD_CHANNELCONTROL_TYPE controltype, FMOD_CHANNELCONTROL_CALLBACK_TYPE callbacktype, void* commanddata1, void* commanddata2)
{
    if(m_pGameInstance)
    {
        m_pGameInstance->Manager_StopAll();
        m_pGameInstance->Manager_PlayBGM(TEXT("CountingStar.mp3"), 0.5f);
    }

    return FMOD_RESULT::FMOD_OK;
}

HRESULT CLevel_BeatSaber::Load_Light_Data()
{
    return S_OK;
}

HRESULT CLevel_BeatSaber::Load_Dororong_Saber_Objects(const _char* szFilePath)
{
    std::ifstream ifs(szFilePath, std::ios::binary);
    if (!ifs.is_open())
    {
        MessageBoxW(g_hWnd, L"Failed to open MapData", L"Error", MB_OK | MB_ICONERROR);
        return E_FAIL;
    }

    if (FAILED(Load_Dororong_Saber_By_Layer(ifs, TEXT("Prototype_GameObject_Pad"), TEXT("Layer_Pad")))) return S_OK;
    if (FAILED(Load_Dororong_Saber_By_Layer(ifs, TEXT("Prototype_GameObject_Rail"), TEXT("Layer_Rail")))) return S_OK;
    if (FAILED(Load_Dororong_Saber_By_Layer(ifs, TEXT("Prototype_GameObject_Beat_Indicator"), TEXT("Layer_Beat_Indicator")))) return S_OK;

    ifs.close();

    return S_OK;
}

HRESULT CLevel_BeatSaber::Load_Dororong_Saber_By_Layer(ifstream& ifs, const _tchar* protoTag, const _tchar* pLayerTag)
{
    _uint iNumObjs = 0;
    ifs.read(reinterpret_cast<char*>(&iNumObjs), sizeof(_uint));

    for (_uint i = 0; i < iNumObjs; ++i)
    {
        SAVEDDORORONGSABERINFO info;
        ifs.read(reinterpret_cast<char*>(&info), sizeof(SAVEDDORORONGSABERINFO));

        CGameObject::GAMEOBJECT_DESC Desc = {};
        Desc.bIsApplyTransform = true;
        Desc.bIsQuaternion = true;

        _vector vScale = {};
        _vector vRotation = {};
        _vector vPosition = {};
        XMMatrixDecompose(&vScale, &vRotation, &vPosition, XMLoadFloat4x4(&info.worldMatrix));

        XMStoreFloat3(&Desc.vScale, vScale);
        XMStoreFloat4(&Desc.vRotation, vRotation);
        XMStoreFloat3(&Desc.vPosition, vPosition);

        HRESULT hr = m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::LEVEL_PROB), protoTag,
            ENUM_CLASS(LEVEL::BEATSABER_GAME), pLayerTag, &Desc);
    }

    return S_OK;
}

CLevel_BeatSaber* CLevel_BeatSaber::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevelID)
{
    CLevel_BeatSaber* pInstance = new CLevel_BeatSaber(pDevice, pContext, eLevelID);

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Failed to Created : BeatSaberLevel");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CLevel_BeatSaber::Free()
{
    m_pGameInstance->Manager_StopAll();
    __super::Free();
}
