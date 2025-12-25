#include "pch.h"
#include "Cinematic_Manager.h"

#include "GameInstance.h"
#include "GameManager.h"
#include "StringHelper.h"
#include "JsonParser.h"

#include "CinematicObject.h"
#include "Camera_Free.h"
#include "Camera_Action.h"
#include "Player.h"
#include "Nayitba.h"

#include "UIHUD.h"
#include "UIBase.h"

CCinematicManager::CCinematicManager()
    : m_pGameInstance{ CGameInstance::GetInstance() }
{
    Safe_AddRef(m_pGameInstance);
}

HRESULT CCinematicManager::Initialize()
{
	m_pCinematicDatas = CGameManager::GetInstance()->Get_CinematicDataMap();

    m_fCinematicTimer = 0.f;
    m_iCurrentCinematicNodeIndex = 0;
    m_iCurrentCinematicID = -1;
    m_bIsCinematicPlaying = false;

    return S_OK;
}

HRESULT CCinematicManager::Update(_float fTimeDelta)
{
    if (FALSE == m_bIsCinematicPlaying)
        return S_OK;

    m_fCinematicTimer += fTimeDelta * m_pGameInstance->GetGameSpeedfRatio();
    while (m_iCurrentCinematicNodeIndex < m_pCurrentCinematicDesc->CinematicNodeTrackList.size())
    {
        if (m_fCinematicTimer >= m_pCurrentCinematicDesc->CinematicNodeTrackList[m_iCurrentCinematicNodeIndex].fTrackPosition)
        {
            Play_Node(m_pCurrentCinematicDesc->CinematicNodeTrackList[m_iCurrentCinematicNodeIndex]);
            ++m_iCurrentCinematicNodeIndex;
        }
        else
            break;

        if (m_iCurrentCinematicNodeIndex >= m_pCurrentCinematicDesc->CinematicNodeTrackList.size())
        {
            m_bIsCinematicPlaying = FALSE;
            if (m_FinishedCinematic)
                m_FinishedCinematic();
        }
    }

	for (auto& pCinematicObjectPair : m_CinematicObjectsMap)
    {
        pCinematicObjectPair.second->Priority_Update(fTimeDelta * m_pGameInstance->GetGameSpeedfRatio());
    }

	for (auto& pCinematicObjectPair : m_CinematicObjectsMap)
    {
        pCinematicObjectPair.second->Update(fTimeDelta * m_pGameInstance->GetGameSpeedfRatio());
    }

	for (auto& pCinematicObjectPair : m_CinematicObjectsMap)
    {
        pCinematicObjectPair.second->Late_Update(fTimeDelta * m_pGameInstance->GetGameSpeedfRatio());
    }

    return S_OK;
}

HRESULT CCinematicManager::Play_Cinematic(_uint iCinematicID, function<void()> FinishedFunc)
{   
	auto iter = m_pCinematicDatas->find(iCinematicID);
	if (iter == m_pCinematicDatas->end())
        return E_FAIL;

    m_fCinematicTimer = 0.f;
    m_iCurrentCinematicID = iCinematicID;
    m_iCurrentCinematicNodeIndex = 0;
    m_pCurrentCinematicDesc = &iter->second;
    m_FinishedCinematic = FinishedFunc;
    m_bIsCinematicPlaying = TRUE;

    return S_OK;
}

HRESULT CCinematicManager::Emplace_CinematicObject(CCinematicObject* pObject)
{
    if (nullptr == pObject)
        return S_OK;

    m_CinematicObjectsMap.emplace(pObject->Get_ObjectTag(), pObject);
    Safe_AddRef(pObject);
    
    return S_OK;
}

HRESULT CCinematicManager::Emplace_ActionCamera(const _tchar* strCameraTag)
{
    m_ActionCameraMap.emplace(strCameraTag, static_cast<CCamera_Action*>(m_pGameInstance->GetCamrea(strCameraTag)));
    return S_OK;
}

HRESULT CCinematicManager::Load_Level_CinematicObjectData(const _char* szFilePath)
{
    // [V] 먼저 시네마틱 오브젝트랑 액션 카메라를 밀어버린다.
    // [V] 받아온 파일명에 따라 json 파일을 읽어온다.
    // [V] 시네마틱 오브젝트와 액션카메라를 생성 및 등록한다.
    // [V] map에 집어넣어준다.

    for (auto& pCinematicObject : m_CinematicObjectsMap)
    {
        Safe_Release(pCinematicObject.second);
    }
    m_CinematicObjectsMap.clear();

    for (auto& pActionCamera : m_ActionCameraMap)
    {
     //   Safe_Release(pActionCamera.second);
    }
    m_ActionCameraMap.clear();

    Json jCinematicObjectDatas{};

    CJsonParser::ReadJsonData(szFilePath, jCinematicObjectDatas);

    for (auto& pLevelCinematicObject : jCinematicObjectDatas)
    {
        _tchar szText[MAX_PATH];
        _wstring strObjectName;

        CStringHelper::ConvertUTFToWide(pLevelCinematicObject["szObjectTag"].get<string>().c_str(), szText);
        strObjectName = szText;

        if (static_cast<CINEMATICOBJECT_TYPE>(pLevelCinematicObject["eType"].get<_int>()) == CINEMATICOBJECT_TYPE::CINEMATICOBJECT)
        {
            // 시네마틱 오브젝트일 경우, szObjectTag를 PrototypeTag로 사용
            CCinematicObject::CINEMATICOBJECT_DESC			CinematicModelDesc{};
            CinematicModelDesc.szObjectTag = strObjectName;
            CinematicModelDesc.fSpeedPerSec = 15.f;
            CinematicModelDesc.fRotationPerSec = XMConvertToRadians(120.0f);
            CinematicModelDesc.vPosition = {
                pLevelCinematicObject["vPosition"][0],
                pLevelCinematicObject["vPosition"][1],
                pLevelCinematicObject["vPosition"][2]
            };
            CinematicModelDesc.vRotation = {
                pLevelCinematicObject["vRotation"][0],
                pLevelCinematicObject["vRotation"][1],
                pLevelCinematicObject["vRotation"][2],
                pLevelCinematicObject["vRotation"][3]
            };
            CinematicModelDesc.vScale = {
                pLevelCinematicObject["vScale"][0],
                pLevelCinematicObject["vScale"][1],
                pLevelCinematicObject["vScale"][2]
            };

            auto pObject = m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::GAMEPLAY), strObjectName, &CinematicModelDesc);

            m_CinematicObjectsMap.emplace(strObjectName, static_cast<CCinematicObject*>(pObject));
            //Safe_AddRef(pObject);
        }
        else if (static_cast<CINEMATICOBJECT_TYPE>(pLevelCinematicObject["eType"].get<_int>()) == CINEMATICOBJECT_TYPE::ACTIONCAMERA)
        {
            // 액션 카메라일 경우, szObjectTag를 카메라 태그로 사용
            CCamera_Free::CAMERA_FREE_DESC			CameraDesc{};
            CameraDesc.fFov = XMConvertToRadians(60.0f);
            CameraDesc.fNear = 0.1f;
            CameraDesc.fFar = 500.f;
            CameraDesc.vEye = _float3(0.f, 10.f, -10.f);
            CameraDesc.vAt = _float3(0.f, 0.f, 0.f);
            CameraDesc.fSpeedPerSec = 15.f;
            CameraDesc.fRotationPerSec = XMConvertToRadians(120.0f);
            CameraDesc.fMouseSensor = 0.1f;

            auto pCamera = m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Camera_Action"), &CameraDesc);
            m_pGameInstance->Add_Camera(strObjectName.c_str(), static_cast<CCamera*>(pCamera));

            m_ActionCameraMap.emplace(strObjectName, static_cast<CCamera_Action*>(pCamera));
            //Safe_AddRef(pCamera);
        }
    }

    return S_OK;
}

HRESULT CCinematicManager::Change_MainCamera()
{
    return S_OK;
}

void CCinematicManager::Play_Node(const CINEMATIC_NODE_DESC& CinematicNodeDesc)
{
    _tchar szText[MAX_PATH];
    _wstring strObjectName;
    CCamera* pCamera = nullptr;
    list<CGameObject*>* pObjectList = nullptr;

    switch (CinematicNodeDesc.eState)
    {
        case CINEMATICNODE_STATE::ACTIVE_CINEOBJ:
            CStringHelper::ConvertUTFToWide(CinematicNodeDesc.szObjectTag, szText);
            strObjectName = szText;

            m_CinematicObjectsMap.find(strObjectName)->second->ActiveCinematicObject(CinematicNodeDesc);
            break;
        case CINEMATICNODE_STATE::PLAY_CINEOBJ:
            CStringHelper::ConvertUTFToWide(CinematicNodeDesc.szObjectTag, szText);
            strObjectName = szText;

            m_CinematicObjectsMap.find(strObjectName)->second->PlayCinematicObject(CinematicNodeDesc);
            break;
        case CINEMATICNODE_STATE::ACTIVE_CHARACTER:
            CStringHelper::ConvertUTFToWide(CinematicNodeDesc.szObjectTag, szText);
            pObjectList = m_pGameInstance->GetAllObejctToLayer(m_pGameInstance->GetCurrentLevelID(), szText);
            if (pObjectList != nullptr)
            {
                for (auto& pObject : *pObjectList)
                {
                    pObject->SetActive(TRUE);
                }
            }
            break;
        case CINEMATICNODE_STATE::ACTIVE_CAMERA:
            CStringHelper::ConvertUTFToWide(CinematicNodeDesc.szObjectTag, szText);
            strObjectName = szText;

            _float4x4 PrePosMatrix = {};

            m_pGameInstance->SetMainCamera(szText, &PrePosMatrix);
            pCamera = m_pGameInstance->GetMainCamera();

			m_ActionCameraMap.find(strObjectName)->second->Initialize_CameraAnimationData(CinematicNodeDesc.iActiveIndex);

            Safe_Release(pCamera);
            break;
        case CINEMATICNODE_STATE::DEACTIVE_CHARACTER:
            CStringHelper::ConvertUTFToWide(CinematicNodeDesc.szObjectTag, szText);
			pObjectList = m_pGameInstance->GetAllObejctToLayer(m_pGameInstance->GetCurrentLevelID(), szText);
            if (pObjectList != nullptr)
            {
                for (auto& pObject : *pObjectList)
                {
                    pObject->SetActive(FALSE);
                }
            }
            break;
        case CINEMATICNODE_STATE::DEACTIVE_CAMERA:
            
            break;
        case CINEMATICNODE_STATE::FADE_IN:
        {
			CUIHUD* pHUD = static_cast<CUIHUD*>(m_pGameInstance->GetCurrentLevelHUD());
            static_cast<CUIHUD*>(pHUD)->Anim_Play(TEXT("Layer_Combat"), TEXT("GamePlay_Overlay"), TEXT("Outro"));
            Safe_Release(pHUD);
            break;
        }
        case CINEMATICNODE_STATE::FADE_OUT:
        {
			CUIHUD* pHUD = static_cast<CUIHUD*>(m_pGameInstance->GetCurrentLevelHUD());
            static_cast<CUIHUD*>(pHUD)->Anim_Play(TEXT("Layer_Combat"), TEXT("GamePlay_Overlay"), TEXT("Intro"));
            Safe_Release(pHUD);
            break;
        }
        case CINEMATICNODE_STATE::PLAY_SOUND:

            break;
        case CINEMATICNODE_STATE::END:

            break;
    }
}

CCinematicManager* CCinematicManager::Create()
{
    CCinematicManager* pInstance = new CCinematicManager();

    if (FAILED(pInstance->Initialize()))
    {
        Safe_Release(pInstance);
        MSG_BOX("Create Failed : Cinematic Manager");
    }

    return pInstance;
}

void CCinematicManager::Free()
{
    __super::Free();

    for (auto& pCinematicObject : m_CinematicObjectsMap)
    {
        Safe_Release(pCinematicObject.second);
    }
    m_CinematicObjectsMap.clear();

    for (auto& pActionCamera : m_ActionCameraMap)
    {
        Safe_Release(pActionCamera.second);
    }
    m_ActionCameraMap.clear();

    Safe_Release(m_pGameInstance);
}
