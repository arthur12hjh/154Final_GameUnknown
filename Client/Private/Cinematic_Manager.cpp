#include "pch.h"
#include "Cinematic_Manager.h"

#include "GameInstance.h"
#include "GameManager.h"
#include "StringHelper.h"
#include "JsonParser.h"

#include "CinematicObject.h"
#include "Camera_Free.h"
#include "Camera_Action.h"
#include "Npc.h"
#include "TriggerBox.h"

#include "UIHUD.h"
#include "UIBase.h"
#include "UIScript.h"

#include "UIActionEvent.h"
#include "PonyTail_Player.h"
#include "Body_Player.h"
#include "Player.h"

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
    m_bIsCinematicSkip = FALSE;

    m_pUIActionEvent = CUIActionEvent::Create([&](void* pArg) {});

    return S_OK;
}

HRESULT CCinematicManager::Update(_float fTimeDelta)
{
    if (FALSE == m_bIsCinematicPlaying)
        return S_OK;

    if (m_bIsCinematicSkip == TRUE)
    {
        Reset_Cinematic();
        return S_OK;
    }

    Active_CinematicCameraQueue();

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

    if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_SPACE))
    {
        Skip_Cinematic();
    }

    /* Dear. 대재훈
        Script_Action(SCRIPT_ACTION::BEGIN); // 스크립트 삽입 입니다 호출하면 알아서 들어갈겁니다 (현재 시네마틱 실행하면 알아서 들어가고 있어요)
        Script_Action(SCRIPT_ACTION::PLAY); // 스크립트 다음 대사 재생
        Script_Action(SCRIPT_ACTION::STOP); // 스크립트 멈추기(안보이기)
        Script_Action(SCRIPT_ACTION::SHOW); // 스크립트 보이기(기본값)
        Script_Action(SCRIPT_ACTION::HIDE); // 스크립트 숨기기
        Script_Action(SCRIPT_ACTION::END); // 스크립트 해제 (현재 스킵 적용 되고 있습니다)
    */

    // 스크립트 테스트
    if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_N))
    {
        Script_Action(SCRIPT_ACTION::PLAY);
    }
    if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_M))
    {
        
    }
    if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_K))
    {
        Script_Action(SCRIPT_ACTION::SHOW);
    }
    if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_L))
    {
        Script_Action(SCRIPT_ACTION::HIDE);
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

    auto pHUD = static_cast<CUIHUD*>(m_pGameInstance->GetCurrentLevelHUD());

    if (!pHUD)
    {
        Safe_Release(pHUD);
        return S_OK;
    }

    auto pTriggerKey = pHUD->Get_UIObject(TEXT("Layer_Cinematic"), TEXT("UI_SpaceKey"));

    if (!pTriggerKey)
        return S_OK;

    pTriggerKey->SetVisibility(VISIBILITY::VISIBLE);

    for (auto& pChild : *pTriggerKey->Get_Children())
        pTriggerKey->Update_Children(pChild);


    m_pGameInstance->Add_Event(TEXT("Cinematic_Skip"), m_pUIActionEvent);

    Script_Action(SCRIPT_ACTION::BEGIN);
    Script_Action(SCRIPT_ACTION::HIDE);

    Safe_Release(pHUD);

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

HRESULT CCinematicManager::Skip_Cinematic()
{
    m_bIsCinematicSkip = TRUE;

    _bool bActive = true;
    UI_EVENT_ARG_DESC Arg{};
    Arg.Type = UI_EVENT_ARG_DESC::BOOL;
    Arg.pData = &bActive;
    m_pUIActionEvent->Notify(&Arg);

    return S_OK;
}

map<_wstring, CCinematicObject*>* CCinematicManager::Get_CinematicObjectsMap()
{
    return &m_CinematicObjectsMap;
}

HRESULT CCinematicManager::Reset_Cinematic()
{
    Reset_UI();

    m_bIsCinematicPlaying = FALSE;
    m_bIsCinematicSkip = FALSE;
    while (m_iCurrentCinematicNodeIndex < m_pCurrentCinematicDesc->CinematicNodeTrackList.size())
    {
        if (m_pCurrentCinematicDesc->CinematicNodeTrackList[m_iCurrentCinematicNodeIndex].eState != CINEMATICNODE_STATE::PLAY_SOUND)
            Play_Node(m_pCurrentCinematicDesc->CinematicNodeTrackList[m_iCurrentCinematicNodeIndex]);
        ++m_iCurrentCinematicNodeIndex;
    }

    m_pGameInstance->SetMainCamera(TEXT("PlayerCamera"));

    for (auto& pCinematicObjectPair : m_CinematicObjectsMap)
    {
        pCinematicObjectPair.second->DeActiveCinematicObject();
    }

    if (m_FinishedCinematic)
        m_FinishedCinematic();

    return S_OK;
}

HRESULT CCinematicManager::Reset_UI()
{
    auto pHUD = static_cast<CUIHUD*>(m_pGameInstance->GetCurrentLevelHUD());

    if (!pHUD)
    {
        Safe_Release(pHUD);
        return S_OK;
    }

    auto pTriggerKey = pHUD->Get_UIObject(TEXT("Layer_Cinematic"), TEXT("UI_SpaceKey"));

    if (!pTriggerKey)
        return S_OK;

    pTriggerKey->SetVisibility(VISIBILITY::HIDDEN);

    for (auto& pChild : *pTriggerKey->Get_Children())
        pTriggerKey->Update_Children(pChild);

    m_pGameInstance->Remove_Event(TEXT("Cinematic_Skip"));

    Script_Action(SCRIPT_ACTION::END);

    Safe_Release(pHUD);

    return S_OK;
}

void CCinematicManager::Script_Action(SCRIPT_ACTION eAction)
{
    auto pHUD = static_cast<CUIHUD*>(m_pGameInstance->GetCurrentLevelHUD());

    if (!pHUD)
    {
        Safe_Release(pHUD);
        return;
    }

    CUIScript* pScript = dynamic_cast<CUIScript*>(pHUD->Get_UIObject(TEXT("Layer_Script"), TEXT("UI_Scripts")));

    if (!pScript)
        return;

    switch (eAction)
    {
    case SCRIPT_ACTION::BEGIN:
    {
        pScript->Begin_Script(CGameManager::GetInstance()->Get_ScriptData(to_wstring(m_iCurrentCinematicID)));
        pScript->Set_Show_Script(false);
        break;
    }
    case SCRIPT_ACTION::PLAY:
    {
        pScript->Play_Next_Script();
        break;
    }
    case SCRIPT_ACTION::STOP:
    {
        pScript->Stop_Script();
        break;
    }
    case SCRIPT_ACTION::SHOW:
    {
        pScript->Set_Show_Script(true);
        break;
    }
    case SCRIPT_ACTION::HIDE:
    {
        pScript->Set_Show_Script(false);
        break;
    }
    case SCRIPT_ACTION::END:
    {
        pScript->End_Script();
        break;
    }
    }

    Safe_Release(pHUD);
}

void CCinematicManager::Play_Node(const CINEMATIC_NODE_DESC& CinematicNodeDesc)
{
    _tchar szText[MAX_PATH];
    _wstring strObjectName;
    CTriggerBox::TRIGGER_BOX_DESC pTriggerBoxDesc = {};
    list<CGameObject*>* pObjectList = nullptr;
    _float fVolume;
    _TCHAR szSoundTag[MAX_PATH];

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
        /*break;*/
        CStringHelper::ConvertUTFToWide(CinematicNodeDesc.szObjectTag, szText);
        strObjectName = szText;

        m_ActionCameraMap.find(strObjectName)->second->Initialize_CameraAnimationData(CinematicNodeDesc.iActiveIndex);

        Add_CinematicCameraQueue(strObjectName);

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
        Reset_UI();
        CUIHUD* pHUD = static_cast<CUIHUD*>(m_pGameInstance->GetCurrentLevelHUD());
        static_cast<CUIHUD*>(pHUD)->Anim_Play(TEXT("Layer_Cinematic"), TEXT("Cinematic_Overlay"), TEXT("Cinema_Intro"));
        Safe_Release(pHUD);

        break;
    }
    case CINEMATICNODE_STATE::FADE_OUT:
    {

        CUIHUD* pHUD = static_cast<CUIHUD*>(m_pGameInstance->GetCurrentLevelHUD());

        _float fDelay = (_float)CinematicNodeDesc.iActiveIndex;
        static_cast<CUIHUD*>(pHUD)->Anim_Play(TEXT("Layer_Cinematic"), TEXT("Cinematic_Overlay"), TEXT("Cinema_Outro"), fDelay);
        Safe_Release(pHUD);

        break;
    }
    case CINEMATICNODE_STATE::PLAY_SOUND:
        CStringHelper::ConvertUTFToWide(CinematicNodeDesc.szObjectTag, szSoundTag);
        fVolume = (_float)CinematicNodeDesc.iActiveIndex / 10.f;
        m_pGameInstance->Manager_PlaySound(szSoundTag, CHANNELID::EFFECT, fVolume);
        break;
    case CINEMATICNODE_STATE::MOVE_CHARACTER:
        CStringHelper::ConvertUTFToWide(CinematicNodeDesc.szObjectTag, szText);
        pObjectList = m_pGameInstance->GetAllObejctToLayer(m_pGameInstance->GetCurrentLevelID(), szText);
        if (pObjectList != nullptr)
        {
            _int iObjectIndex = 0;
            for (auto& pObject : *pObjectList)
            {
                pObject->GetTransform()->Set_State(STATE::POSITION, XMVectorSetW(XMLoadFloat3(&CinematicNodeDesc.CinematicIndexDataList[iObjectIndex].vPosition), 1.f));
                static_cast<CCharacterController*>(pObject->Find_Component(TEXT("Com_CCT")))->Set_Active(false);
                static_cast<CCharacterController*>(pObject->Find_Component(TEXT("Com_CCT")))->Set_Position(pObject->GetTransform()->Get_State(STATE::POSITION));

                if (CinematicNodeDesc.CinematicIndexDataList[iObjectIndex].vRotation.w == 0.f)
                {
                    pObject->GetTransform()->Rotation(
                        CinematicNodeDesc.CinematicIndexDataList[iObjectIndex].vRotation.x,
                        CinematicNodeDesc.CinematicIndexDataList[iObjectIndex].vRotation.y,
                        CinematicNodeDesc.CinematicIndexDataList[iObjectIndex].vRotation.z);
                }
                else if (CinematicNodeDesc.CinematicIndexDataList[iObjectIndex].vRotation.w == 1.f)
                {
                    _vector vLookAt = XMVectorSetW(XMLoadFloat4(&CinematicNodeDesc.CinematicIndexDataList[iObjectIndex].vRotation), 1.f);
                    pObject->GetTransform()->LookAt(vLookAt);
                }

                ++iObjectIndex;

                static_cast<CCharacterController*>(pObject->Find_Component(TEXT("Com_CCT")))->Set_Active(true);
            }
        }

        if (pObjectList != nullptr && _wstring(szText) == TEXT("Layer_Player"))
        {
            CPlayer* pPlayer = CGameManager::GetInstance()->GetGameCharacter();
            Safe_Release(pPlayer);

            if (nullptr != pPlayer)
            {
                CBody_Player* pBody = static_cast<CBody_Player*>(pPlayer->Get_PartObject(TEXT("Part_Body")));
                pBody->Teleport_JointChains();

                CPonyTail_Player* pPonytail = static_cast<CPonyTail_Player*>(pPlayer->Get_PartObject(TEXT("Part_PonyTail")));
                pPonytail->Teleport_JointChains();
            }
        }

        //if (pObjectList != nullptr && _wstring(szText) == TEXT("Layer_Player"))
        //{
        //    m_pGameInstance->ADD_FrameFinalFunction([&]() {
        //        CPlayer* pPlayer = CGameManager::GetInstance()->GetGameCharacter();
        //        Safe_Release(pPlayer);

        //        if (nullptr != pPlayer)
        //        {
        //            CBody_Player* pBody = static_cast<CBody_Player*>(pPlayer->Get_PartObject(TEXT("Part_Body")));
        //            pBody->Teleport_JointChains();

        //            CPonyTail_Player* pPonytail = static_cast<CPonyTail_Player*>(pPlayer->Get_PartObject(TEXT("Part_PonyTail")));
        //            pPonytail->Teleport_JointChains();
        //        }
        //    }, 2);
        //}

        break;
    case CINEMATICNODE_STATE::MOVE_NPC:
        CStringHelper::ConvertUTFToWide(CinematicNodeDesc.szObjectTag, szText);
        pObjectList = m_pGameInstance->GetAllObejctToLayer(m_pGameInstance->GetCurrentLevelID(), TEXT("Layer_Npc"));
        if (pObjectList != nullptr)
        {
            for (auto& pObject : *pObjectList)
            {
                if (dynamic_cast<CNpc*>(pObject)->Get_NpcDesc()->iNpcID == CinematicNodeDesc.iActiveIndex)
                {
                    pObject->GetTransform()->Set_State(STATE::POSITION, XMVectorSetW(XMLoadFloat3(&CinematicNodeDesc.CinematicIndexDataList[0].vPosition), 1.f));

                    static_cast<CCharacterController*>(pObject->Find_Component(TEXT("Com_CCT")))->Set_Position(pObject->GetTransform()->Get_State(STATE::POSITION));

                    if (CinematicNodeDesc.CinematicIndexDataList[0].vRotation.w == 0.f)
                    {
                        pObject->GetTransform()->Rotation(
                            CinematicNodeDesc.CinematicIndexDataList[0].vRotation.x,
                            CinematicNodeDesc.CinematicIndexDataList[0].vRotation.y,
                            CinematicNodeDesc.CinematicIndexDataList[0].vRotation.z);
                    }
                    else if (CinematicNodeDesc.CinematicIndexDataList[0].vRotation.w == 1.f)
                    {
                        _vector vLookAt = XMVectorSetW(XMLoadFloat4(&CinematicNodeDesc.CinematicIndexDataList[0].vRotation), 1.f);
                        pObject->GetTransform()->LookAt(vLookAt);
                    }

                }
            }
        }
        break;
    case CINEMATICNODE_STATE::CREATE_TRIGGERBOX:
        pTriggerBoxDesc.iTriggerCode = CinematicNodeDesc.iActiveIndex;
        pTriggerBoxDesc.eColType = COLLIDER::OBB;
        pTriggerBoxDesc.vScale = { CinematicNodeDesc.CinematicIndexDataList[0].vScale.x,
                            CinematicNodeDesc.CinematicIndexDataList[0].vScale.y,
                            CinematicNodeDesc.CinematicIndexDataList[0].vScale.z };
        pTriggerBoxDesc.vRotation = { CinematicNodeDesc.CinematicIndexDataList[0].vRotation.x,
                            CinematicNodeDesc.CinematicIndexDataList[0].vRotation.y,
                            CinematicNodeDesc.CinematicIndexDataList[0].vRotation.z };
        pTriggerBoxDesc.vPosition = { CinematicNodeDesc.CinematicIndexDataList[0].vPosition.x,
                            CinematicNodeDesc.CinematicIndexDataList[0].vPosition.y,
                            CinematicNodeDesc.CinematicIndexDataList[0].vPosition.z };
        pTriggerBoxDesc.fDelayTime = -1.f;

        if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_TriggerBox"),
            ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_Trigger"), &pTriggerBoxDesc)))
        {
            int a = 10;
        }

        break;
    case CINEMATICNODE_STATE::CREATE_TARGETLIGHT:
        CGameManager::GetInstance()->Change_ShaderSetting(static_cast<LEVEL>(m_pGameInstance->GetCurrentLevelID()), CinematicNodeDesc.iActiveIndex);
        break;
        // 13번
    case CINEMATICNODE_STATE::SET_CINEMATICLIGHT:
        CGameManager::GetInstance()->Set_CinematicLights(CinematicNodeDesc.iActiveIndex);
        break;
        // 14번
    case CINEMATICNODE_STATE::LOCKON_START:
        CGameManager::GetInstance()->Force_Lockon(m_pGameInstance->Get_TimeDelta(TEXT("GameLoopTime")));
        break;
        // 15번
    case CINEMATICNODE_STATE::LOCKON_END:
        CGameManager::GetInstance()->Force_LockOff();
        break;
        // 16번
    case CINEMATICNODE_STATE::BAKE_VILLAGE_SHADOW:
        m_pGameInstance->ADD_DelayFunction(TEXT("Bake_Shadow"), 0.8f, [&]() {
            /*그림자 세팅도 여기서 해주자. */
            STATIC_SHADOW_DESC		StaticShadowDesc{};
            StaticShadowDesc.fFar = 2000.f;
            StaticShadowDesc.fNear = 0.1f;
            StaticShadowDesc.vAt = _float4(913.586f, 105.541f, 1456.260f, 1.f);

            if (FAILED(m_pGameInstance->Ready_StaticShadow_Light(StaticShadowDesc)))
                return;

            m_pGameInstance->Bake_StaticShadow();
            return;
            });
        break;
        // 17번
    case CINEMATICNODE_STATE::PLAY_BGM:
        m_pGameInstance->Manager_StopSound(CHANNELID::BGM);
        CStringHelper::ConvertUTFToWide(CinematicNodeDesc.szObjectTag, szSoundTag);
        fVolume = (_float)CinematicNodeDesc.iActiveIndex / 10.f;
        m_pGameInstance->Manager_PlayBGM(szSoundTag, fVolume, 1);
        break;
        // 18번
    case CINEMATICNODE_STATE::PLAY_BOSSBGM:
        //m_pGameInstance->Manager_StopSound(CHANNELID::BGM);
        CGameManager::GetInstance()->Play_BossBGM(CinematicNodeDesc.iActiveIndex / 10, CinematicNodeDesc.iActiveIndex % 10);
        break;
        // 19번
    case CINEMATICNODE_STATE::CONTROL_DIALOGUE:
        /// 일의 자리수
        ///  - 0 : 아무 것도 안함
        ///  - 1 : Hide
        ///  - 2 : Show
        /// 
        /// 십의 자리수
        ///  - 0 : 아무 것도 앟마
        ///  - 1 : 다음 대사 재생
        ///  - 2 : 엔딩
        /// 
        /// Script_Action(SCRIPT_ACTION::BEGIN); // 스크립트 삽입 입니다 호출하면 알아서 들어갈겁니다 (현재 시네마틱 실행하면 알아서 들어가고 있어요)
        /// Script_Action(SCRIPT_ACTION::PLAY); // 스크립트 다음 대사 재생
        /// Script_Action(SCRIPT_ACTION::STOP); // 스크립트 멈추기(안보이기)
        /// Script_Action(SCRIPT_ACTION::SHOW); // 스크립트 보이기(기본값)
        /// Script_Action(SCRIPT_ACTION::HIDE); // 스크립트 숨기기
        /// Script_Action(SCRIPT_ACTION::END); // 스크립트 해제 (현재 스킵 적용 되고 있습니다)
        /// 

        switch (CinematicNodeDesc.iActiveIndex / 10)
        {
        case 0 :
            break;
        case 1 :
            Script_Action(SCRIPT_ACTION::PLAY);
            break;
        case 2 :
            Script_Action(SCRIPT_ACTION::END);
            break;
        }

        switch (CinematicNodeDesc.iActiveIndex % 10)
        {
        case 0:
            break;
        case 1:
            Script_Action(SCRIPT_ACTION::HIDE);
            break;
        case 2:
            Script_Action(SCRIPT_ACTION::SHOW);
            break;
        }
        break;
    }
}

void CCinematicManager::Add_CinematicCameraQueue(_wstring szText)
{
    m_CinematicCameraQueue.push(szText);
}

void CCinematicManager::Active_CinematicCameraQueue()
{
    if (m_CinematicCameraQueue.empty())
        return;

    _float4x4 PrePosMatrix = {};
    CCamera* pCamera = nullptr; 

    m_pGameInstance->SetMainCamera(m_CinematicCameraQueue.front().c_str(), &PrePosMatrix);
    pCamera = m_pGameInstance->GetMainCamera();
    m_CinematicCameraQueue.pop();
    Safe_Release(pCamera);

    //m_pGameInstance->ADD_FrameFinalFunction([&]() {
    //    if (m_CinematicCameraQueue.empty())
    //        return;

    //    m_pGameInstance->SetMainCamera(m_CinematicCameraQueue.front().c_str(), &PrePosMatrix);
    //    pCamera = m_pGameInstance->GetMainCamera();
    //    m_CinematicCameraQueue.pop();
    //    Safe_Release(pCamera);
    //}, 6);
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

    //for (auto& pActionCamera : m_ActionCameraMap)
    //{
    //    Safe_Release(pActionCamera.second);
    //}
    //m_ActionCameraMap.clear();

    Safe_Release(m_pGameInstance);

    m_pGameInstance->Remove_Event(TEXT("Cinematic_Skip"));

    Safe_Release(m_pUIActionEvent);
}