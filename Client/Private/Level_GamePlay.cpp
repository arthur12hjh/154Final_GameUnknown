	#include "pch.h"
#include "Level_GamePlay.h"
#include "GameInstance.h"
#include "GameManager.h"
#include "Level_Loading.h"

#include "Actor.h"
#include "Nayitba.h"
#include "Npc.h"
#include "Prob_Interaction.h"
#include "Camera_Free.h"
#include "Lift_Controller.h"
#include "Lift_Platform.h"
#include "Player.h"
#include "Spawner.h"
#include "AttackHitBox.h"
#include "TriggerBox.h"
#include "UIHUD.h"
#include "UIScript.h"
#include "ChangeLevelEvent.h"
#include "TeleportEvent.h"
#include "SoundTriggerBox.h"

#include "SpriteParticle.h"

#ifdef _DEBUG
#include "ImGuiManager.h"
#endif // _DEBUG

CLevel_GamePlay::CLevel_GamePlay(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevelID)
	: CLevel { pDevice, pContext, ENUM_CLASS(eLevelID)}	
{
}

HRESULT CLevel_GamePlay::Initialize()
{
	if (FAILED(Ready_Lights()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Player(TEXT("Layer_Player"))))
 		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

	//if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
	//	return E_FAIL;

	if (FAILED(Ready_Layer_Terrain(TEXT("Layer_Terrain"))))
 		return E_FAIL;

	if (FAILED(Ready_Layer_Sky(TEXT("Layer_Sky"))))
		return E_FAIL;

	//if (FAILED(Ready_Layer_Monster(TEXT("Layer_Monster"))))
	//	return E_FAIL;

	//if (FAILED(Ready_Layer_NPC(TEXT("Layer_Npc"))))
	//	return E_FAIL;

	if (FAILED(Ready_Layer_UI(TEXT("Layer_UI"))))
		return E_FAIL;

	Load_Level_CinematicObjectData("../Bin/DataFiles/LevelCinematicObjectData/CinematicData_Desert.json");

	Load_Map_Desert_Data("../../Map_Editor/Bin/DataFiles/MapData_Desert2.bin");
	Load_Map_Desert_Data("../../Map_Editor/Bin/DataFiles/MapData_Xion.bin");
	Load_Monster_Desert_Data("../../Map_Editor/Bin/DataFiles/MonsterData_Desert.bin");
	Load_Sound_Trigger_Box_Objects("../../Map_Editor/Bin/DataFiles/Desert_SoundTriggerBox.bin");
	
	auto pGameManager = CGameManager::GetInstance();
	CAttackHitBox::HIT_BOX_DESC pHitBoxDesc = {};
	pHitBoxDesc.vScale = { 1.f, 1.f, 1.f };
	pHitBoxDesc.eColType = COLLIDER::OBB;
	pGameManager->ADD_PoolManager(ENUM_CLASS(LEVEL::STATIC), ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_AttackHitBox"), &pHitBoxDesc, TEXT("Hit_Box"), 300);
	pGameManager->Setting_PoolManager(ENUM_CLASS(LEVEL::GAMEPLAY));

	auto pGameCharacter = pGameManager->GetGameCharacter();
	m_pGameInstance->SetInteractionBaseObject(pGameCharacter);
	Safe_Release(pGameCharacter);

	m_pLevelChangeEvent = CChangeLevelEvent::Create([&](void* pArg) {
		UI_EVENT_ARG_DESC Desc = *static_cast<UI_EVENT_ARG_DESC*>(pArg);

		LEVEL_CHANGER LevelChanger = *static_cast<LEVEL_CHANGER*>(Desc.pData);

		m_bChangeLevel = LevelChanger.bChange;
		m_eTargetLevel = LEVEL(LevelChanger.iTargetLevel);
		});
	m_pGameInstance->Bind_Observer(TEXT("Change_Map"), m_pLevelChangeEvent);
	m_pGameInstance->Bind_Observer(TEXT("Go_DororongSaber"), m_pLevelChangeEvent);

	m_pTeleportEvent = CTeleportEvent::Create([&](void* pArg) {
		UI_EVENT_ARG_DESC Desc = *static_cast<UI_EVENT_ARG_DESC*>(pArg);

		m_bTeleport = *static_cast<_bool*>(Desc.pData);
		});
	m_pGameInstance->Bind_Observer(TEXT("TelePort"), m_pTeleportEvent);

#ifdef _DEBUG
	CImGuiManager::GetInstance()->SetLevelFreeCamera();
#endif // _DEBUG

	pGameManager->Change_ShaderSetting(LEVEL::GAMEPLAY, 0);

	return S_OK;
}

void CLevel_GamePlay::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

	//if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_X))
	//{
	//	CGameManager::GetInstance()->Play_Cinematic(126);
	//}

	if (m_isOverlay && m_pHUD)
	{
		static_cast<CUIHUD*>(m_pHUD)->Anim_Play(TEXT("Layer_Combat"), TEXT("GamePlay_Overlay"), TEXT("Intro"), 2.f);
		m_isOverlay = false;
	}

	if (m_bTeleport && m_pHUD)
	{
		static_cast<CUIHUD*>(m_pHUD)->Anim_Play(TEXT("Layer_Combat"), TEXT("GamePlay_Overlay"), TEXT("Intro"), 0.5f);
		m_bTeleport = false;
	}

	if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_F12))
	{
		dynamic_cast<CUIHUD*>(m_pHUD)->Reset_AllWorldUI_State();

		auto pGameManger = CGameManager::GetInstance();

		pGameManger->SavePlayerDesc();
		pGameManger->SetPlayerNextLevelSpawnPosition(ENUM_CLASS(LEVEL::SCARLET), 1);
		if (FAILED(m_pGameInstance->Change_Level(CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL::LOADING, LEVEL::SCARLET, false))))
			return;

		return;
	}

	if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_F11))
	{
		dynamic_cast<CUIHUD*>(m_pHUD)->Reset_AllWorldUI_State();

		CGameManager::GetInstance()->SavePlayerDesc();
		if (FAILED(m_pGameInstance->Change_Level(CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL::LOADING, LEVEL::BEATSABER_GAME, false))))
			return;

		return;
	}

	if (m_bChangeLevel && !m_bLevelTransitioning)
	{
		dynamic_cast<CUIHUD*>(m_pHUD)->Anim_Play(TEXT("Layer_Combat"), TEXT("GamePlay_Overlay"), TEXT("Outro"));
		m_bLevelTransitioning = true;
	}

	if (m_bLevelTransitioning && m_bChangeLevel
		&& dynamic_cast<CUIHUD*>(m_pHUD)->Check_AnimFinish(TEXT("Layer_Combat"), TEXT("GamePlay_Overlay"), TEXT("Outro")))
	{
		dynamic_cast<CUIHUD*>(m_pHUD)->Reset_AllWorldUI_State();

		auto pGameManger = CGameManager::GetInstance();

		pGameManger->SavePlayerDesc();
		if(m_eTargetLevel == LEVEL::SCARLET)
			pGameManger->SetPlayerNextLevelSpawnPosition(ENUM_CLASS(LEVEL::SCARLET), 1);

		if (FAILED(m_pGameInstance->Change_Level(CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL::LOADING, m_eTargetLevel, false))))
			return;

		return;
	}
}

HRESULT CLevel_GamePlay::Render()
{
#ifdef _DEBUG
	SetWindowText(g_hWnd, TEXT("Game Play Level"));
#else
	SetWindowText(g_hWnd, m_pGameInstance->GetFrameText());
#endif // _DEBUG

	return S_OK;
}

void CLevel_GamePlay::FontRender()
{
	SetWindowText(g_hWnd, TEXT("쓰레드 풀 동작"));
}

HRESULT CLevel_GamePlay::Ready_Lights()
{

	// 빛 정보 로딩 함수. 나중에 반드시 켜야됩니다
	//Load_Light_Data();

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

HRESULT CLevel_GamePlay::Ready_Layer_BackGround(const _wstring& strLayerTag)
{
	/*CActor::ACTOR_DESC ProbDesc = {};
	ProbDesc.bIsApplyTransform = true;
	ProbDesc.vScale = { 1.f, 1.f, 1.f };*/

	/*ProbDesc.szVIBuffer_PrototypeName = TEXT("Prototype_Component_Model_Prob_Box1");
	for (size_t i = 0; i < 5; i++)
	{
		ProbDesc.vPosition = { m_pGameInstance->Random(5, 20),
							  0.3f,
							   m_pGameInstance->Random(5, 20) };

		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::LEVEL_PROB), TEXT("Prototype_GameObject_Prob_Destory"),
			ENUM_CLASS(LEVEL::GAMEPLAY), strLayerTag, &ProbDesc)))
			return E_FAIL;
	}

	ProbDesc.szVIBuffer_PrototypeName = TEXT("Prototype_Component_Model_Prob_Box2");
	for (size_t i = 0; i < 5; i++)
	{
		ProbDesc.vPosition = { m_pGameInstance->Random(5, 50),
							  0.3f,
							   m_pGameInstance->Random(5, 50) };

		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::LEVEL_PROB), TEXT("Prototype_GameObject_Prob_Destory"),
			ENUM_CLASS(LEVEL::GAMEPLAY), strLayerTag, &ProbDesc)))
			return E_FAIL;
	}*/

	//CProb_Interaction::PROB_INTERACTION_DESC InteractionDesc = {};
	//InteractionDesc.bIsApplyTransform = true;
	//InteractionDesc.vScale = { 1.f, 1.f, 1.f };
	//InteractionDesc.iInteractionID = 5;
	//InteractionDesc.szVIBuffer_PrototypeName = TEXT("Prototype_Component_Model_CanBox");
	//for (size_t i = 0; i < 5; i++)
	//{
	//	InteractionDesc.vPosition = { 10.f * i,
	//							1.f,
	//						   //m_pGameInstance->Random(0, 50),
	//						   m_pGameInstance->Random(5, 50) };
	//
	//	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::LEVEL_PROB), TEXT("Prototype_GameObject_CanBox"),
	//		ENUM_CLASS(LEVEL::GAMEPLAY), strLayerTag, &InteractionDesc)))
	//		return E_FAIL;
	//}

	/*InteractionDesc.iInteractionID = 2;
	InteractionDesc.szVIBuffer_PrototypeName = TEXT("Prototype_Component_Model_Interaction_Chair117");
	for (size_t i = 0; i < 5; i++)
	{
		InteractionDesc.vPosition = { m_pGameInstance->Random(5, 50),
							   0.f,
							   m_pGameInstance->Random(5, 50) };

		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::LEVEL_PROB), TEXT("Prototype_GameObject_Interaction_NonAnim"),
			ENUM_CLASS(LEVEL::GAMEPLAY), strLayerTag, &InteractionDesc)))
			return E_FAIL;
	}

	InteractionDesc.iInteractionID = 3;
	InteractionDesc.szVIBuffer_PrototypeName = TEXT("Prototype_Component_Model_Interaction_Vending7A");
	for (size_t i = 0; i < 5; i++)
	{
		InteractionDesc.vPosition = { m_pGameInstance->Random(50, 100),
							   0.f,
							   m_pGameInstance->Random(50, 100) };

		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::LEVEL_PROB), TEXT("Prototype_GameObject_Interaction_NonAnim"),
			ENUM_CLASS(LEVEL::GAMEPLAY), strLayerTag, &InteractionDesc)))
			return E_FAIL;
	}*/

	/*ProbDesc.szVIBuffer_PrototypeName = TEXT("Prototype_Component_Model_Pallet02");
	for (size_t i = 0; i < 5; i++)
	{
		ProbDesc.vPosition = { m_pGameInstance->Random(150, 200),
							   0.3f,
							   m_pGameInstance->Random(150, 200) };

		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::LEVEL_PROB), TEXT("Prototype_GameObject_Prob_Destory"),
			ENUM_CLASS(LEVEL::GAMEPLAY), strLayerTag, &ProbDesc)))
			return E_FAIL;
	}

	ProbDesc.szVIBuffer_PrototypeName = TEXT("Prototype_Component_Model_Poster02");
	for (size_t i = 0; i < 5; i++)
	{
		ProbDesc.vPosition = { m_pGameInstance->Random(200, 250),
							   0.f,
							   m_pGameInstance->Random(200, 250) };

		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::LEVEL_PROB), TEXT("Prototype_GameObject_Prob_Static"),
			ENUM_CLASS(LEVEL::GAMEPLAY), strLayerTag, &ProbDesc)))
			return E_FAIL;
	}*/

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Camera(const _wstring& strLayerTag)
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

	CameraDesc.fSpeedPerSec = 15.f;
	CameraDesc.fFov = XMConvertToRadians(45.0f);
	CameraDesc.fRotationPerSec = XMConvertToRadians(120.0f);
	pCamera = m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Camera_Action"), &CameraDesc);
	m_pGameInstance->Add_Camera(TEXT("ActionCamera"), static_cast<CCamera*>(pCamera));
	
	CameraDesc.fSpeedPerSec = 15.f;
	CameraDesc.fFov = XMConvertToRadians(45.0f);
	CameraDesc.fRotationPerSec = XMConvertToRadians(120.0f);
	pCamera = m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Camera_Npc"), &CameraDesc);
	m_pGameInstance->Add_Camera(TEXT("NpcCamera"), static_cast<CCamera*>(pCamera));

	CameraDesc.fSpeedPerSec = 15.f;
	CameraDesc.fFov = XMConvertToRadians(45.0f);
	CameraDesc.fRotationPerSec = XMConvertToRadians(120.0f);

	pCamera = m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Camera_Player"), &CameraDesc);
	m_pGameInstance->Add_Camera(TEXT("PlayerCamera"), static_cast<CCamera*>(pCamera));
	m_pGameInstance->SetMainCamera(TEXT("PlayerCamera"));
	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Terrain(const _wstring& strLayerTag)
{
	/*if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Terrain"),
		ENUM_CLASS(LEVEL::GAMEPLAY), strLayerTag)))
		return E_FAIL;*/

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::LEVEL_PROB), TEXT("Prototype_GameObject_Terrain_Desert"),
		ENUM_CLASS(LEVEL::GAMEPLAY), strLayerTag)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Sky(const _wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::LEVEL_PROB), TEXT("Prototype_GameObject_Sky"),
		ENUM_CLASS(LEVEL::GAMEPLAY), strLayerTag)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Player(const _wstring& strLayerTag)
{	
	////테스트용
	//if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_VelocityExample"),
	//	ENUM_CLASS(LEVEL::GAMEPLAY), strLayerTag)))
	//	return E_FAIL;

	CGameObject::GAMEOBJECT_DESC Desc = {};
	Desc.bIsApplyTransform = true;
	Desc.vScale = { 1.f, 1.f, 1.f };
	Desc.vPosition = { 190.f, 55.f, 233.f};
	//Desc.vPosition = { 800.f, 150.f, 1500.f};
	Desc.fRotationPerSec = XMConvertToRadians(180.0f);
	Desc.fSpeedPerSec = 10.f;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Player"),
		ENUM_CLASS(LEVEL::GAMEPLAY), strLayerTag, &Desc)))
		return E_FAIL;	

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Monster(const _wstring& strLayerTag)
{
	//for (size_t i = 0; i < 1; i++)
	//{
	//	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Monster"),
	//		ENUM_CLASS(LEVEL::GAMEPLAY), strLayerTag)))
	//		return E_FAIL;
	//}

	CNaytiba::NAYITBA_DESC Desc = {};
	Desc.bIsApplyTransform = true;
	Desc.vScale = { 1.f, 1.f, 1.f };

	/*Desc.iMonsterID = 6;
	Desc.vPosition = { 62.f, 1.f, 62.f };
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Nayitba"),
		ENUM_CLASS(LEVEL::GAMEPLAY), strLayerTag, &Desc)))
		return E_FAIL;*/
	
	/*Desc.iMonsterID = 3;
	{
		for (_uint i = 0; i < 10; ++i)
		{
			Desc.vPosition = { 15.f * i, 1.f, 105.f};
			if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Nayitba"),
				ENUM_CLASS(LEVEL::GAMEPLAY), strLayerTag, &Desc)))
				return E_FAIL;
		}
	}*/

	//Desc.iMonsterID = 3;
	//{
	//	for (_uint i = 0; i < 10; ++i)
	//	{
	//		Desc.vPosition = { 15.f * i, 1.f, 105.f };
	//		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Nayitba"),
	//			ENUM_CLASS(LEVEL::GAMEPLAY), strLayerTag, &Desc)))
	//			return E_FAIL;
	//	}
	//}
	
	//Desc.iMonsterID = 9;
	//Desc.vPosition = { 60.f, 1.f, 60.f };
	//Desc.bIsSuperMonster = false;
	//if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Nayitba"),
	//	ENUM_CLASS(LEVEL::GAMEPLAY), strLayerTag, &Desc)))
	//	return E_FAIL;


	/*Desc.iMonsterID = 10;
	Desc.bIsSuperMonster = false;
	Desc.vPosition = { m_pGameInstance->Random(0.f, 30.f), 1.f, m_pGameInstance->Random(0.f, 30.f) };
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Nayitba"),
		ENUM_CLASS(LEVEL::GAMEPLAY), strLayerTag, &Desc)))
		return E_FAIL;*/

	/*Desc.iMonsterID = 6;
	Desc.vPosition = { 350.f, 1.f, 600.f };
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Nayitba"),
		ENUM_CLASS(LEVEL::GAMEPLAY), strLayerTag, &Desc)))
		return E_FAIL;*/


	/*CSpawner::SPAWNER_DESC pSapwnerDesc = {};
	pSapwnerDesc.bIsApplyTransform = true;
	pSapwnerDesc.vScale = { 100.f, 1.f, 100.f };
	pSapwnerDesc.eType = CSpawner::SPAWNER_TYPE::TRIGGER;
	pSapwnerDesc.vPosition = { 350.f, 1.f, 600.f };
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Spawner"),
		ENUM_CLASS(LEVEL::GAMEPLAY), strLayerTag, &pSapwnerDesc)))
		return E_FAIL;*/

	////  시네마틱 테스트용 모델임
	//if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_LinkAttackTester"),
	//	ENUM_CLASS(LEVEL::GAMEPLAY), strLayerTag, nullptr)))
	//	return E_FAIL;

	/*if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Test_InstanceModel"),
		ENUM_CLASS(LEVEL::GAMEPLAY), strLayerTag)))
		return E_FAIL;*/

	//Desc.iMonsterID = 8;
	//Desc.vPosition = { 1000.f, 1.f, 1000.f };
	//if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Nayitba"),
	//	ENUM_CLASS(LEVEL::GAMEPLAY), strLayerTag, &Desc)))
	//	return E_FAIL;


	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_NPC(const _wstring& strLayerTag)
{
	CNpc::NPC_DESC NpcDesc = {};
	NpcDesc.bIsApplyTransform = true;
	NpcDesc.vScale = { 1.f, 1.f, 1.f };
	NpcDesc.iNpcID = 1;
	NpcDesc.vPosition = { 60.f, 1.f, 60.f };
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Npc"),
		ENUM_CLASS(LEVEL::GAMEPLAY), strLayerTag, &NpcDesc)))
		return E_FAIL;

	NpcDesc.iNpcID = 4;
	NpcDesc.vPosition = { 50.f, 1.f, 50.f };
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Npc"),
		ENUM_CLASS(LEVEL::GAMEPLAY), strLayerTag, &NpcDesc)))
		return E_FAIL;

	/*NpcDesc.iNpcID = 5;
	NpcDesc.vPosition = { 40.f, 1.f, 50.f };
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Npc"),
		ENUM_CLASS(LEVEL::GAMEPLAY), strLayerTag, &NpcDesc)))
		return E_FAIL;*/

	NpcDesc.iNpcID = 5;
	NpcDesc.vPosition = { 790.26f, 98.15f, 1517.58f };
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Npc"),
		ENUM_CLASS(LEVEL::GAMEPLAY), strLayerTag, &NpcDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_UI(const _wstring& strLayerTag)
{
	CUIHUD* pUIHUD = CUIHUD::Create(m_pDevice, m_pContext);

	if (pUIHUD == nullptr)
		return E_FAIL;

	SetHUD(pUIHUD);
	pUIHUD->Set_Show_Debug_Rect(false);

	if (FAILED(pUIHUD->Load_Data(TEXT("Layer_Combat")))) 
		return E_FAIL;
	
	pUIHUD->Anim_Play(TEXT("Layer_Combat"), TEXT("Hp_Fx"), TEXT("Hp_Fx_BeapBeap"));

	if (FAILED(pUIHUD->Load_Data(TEXT("Layer_Boss"))))
		return E_FAIL;
	pUIHUD->Anim_Play(TEXT("Layer_Boss"), TEXT("Boss_Hp_Fx"), TEXT("Hp_Fx_BeapBeap"));

	if (FAILED(pUIHUD->Load_Data(TEXT("Layer_World"))))
		return E_FAIL;

	pUIHUD->Register_WorldUI(TEXT("Pool_LockOnMark"), TEXT("UI_LockOnMark"), 1, ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_World"));
	pUIHUD->Register_WorldUI(TEXT("Pool_InteractionDot"), TEXT("UI_InteractionDot"), 10, ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_World"));
	pUIHUD->Register_WorldUI(TEXT("Pool_MonsterVital"), TEXT("UI_Monster_Vital"), 30, ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_World"));

	pUIHUD->Anim_Play(TEXT("Layer_World"), TEXT("MonsterHp_Fx"), TEXT("Hp_Fx_BeapBeap"));

	if (FAILED(pUIHUD->Load_Data(TEXT("Layer_Combat_Info"))))
		return E_FAIL;

	if (FAILED(pUIHUD->Load_Data(TEXT("Layer_Script"))))
		return E_FAIL;

	if (FAILED(pUIHUD->Load_Data(TEXT("Layer_Popup"))))
		return E_FAIL;

	if (FAILED(pUIHUD->Load_Data(TEXT("Layer_Shop"))))
		return E_FAIL;
	
	if (FAILED(pUIHUD->Load_Data(TEXT("Layer_Cinematic"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Trigger(const _wstring& strLayerTag)
{
	CTriggerBox::TRIGGER_BOX_DESC pTriggerBoxDesc = {};
	pTriggerBoxDesc.iTriggerCode = 124;
	pTriggerBoxDesc.eColType = COLLIDER::OBB;
	pTriggerBoxDesc.vScale = { 3.f, 3.f, 3.f };
	pTriggerBoxDesc.vRotation= { 0.f, 0.f, 0.f };
	pTriggerBoxDesc.vPosition = { 715.428f, 38.124f, 491.423f };
	pTriggerBoxDesc.fDelayTime = -1.f;

	//  기가스 조우
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_TriggerBox"),
		ENUM_CLASS(LEVEL::GAMEPLAY), strLayerTag, &pTriggerBoxDesc)))
		return E_FAIL;
	
	pTriggerBoxDesc.iTriggerCode = 123;
	pTriggerBoxDesc.vPosition = { 684.761f, 31.453f, 383.506f };

	//  도로롱 조우
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_TriggerBox"),
		ENUM_CLASS(LEVEL::GAMEPLAY), strLayerTag, &pTriggerBoxDesc)))
		return E_FAIL;
	
	if (CGameManager::GetInstance()->GetLevelTransportIndex() == 2)
	{
		pTriggerBoxDesc.iTriggerCode = 127;
		pTriggerBoxDesc.vPosition = { 812.065f, 98.135f, 1517.918f };
		pTriggerBoxDesc.vScale = { 3.f, 3.f, 3.f };

		//  도로롱 조우
		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_TriggerBox"),
			ENUM_CLASS(LEVEL::GAMEPLAY), strLayerTag, &pTriggerBoxDesc)))
			return E_FAIL;
	}


	return S_OK;
}

HRESULT CLevel_GamePlay::Load_Map_Desert_Data(const _char* szFilePath)
{
	std::ifstream ifs(szFilePath, std::ios::binary);
	if (!ifs.is_open())
	{
		MessageBoxW(g_hWnd, L"Failed to open MapData", L"Error", MB_OK | MB_ICONERROR);
		return E_FAIL;
	}

	if (FAILED(Load_Map_Desert_Format(ifs, TEXT("Prototype_GameObject_Building_Ruin"), TEXT("Layer_Building_Ruin")))) return S_OK;
	if (FAILED(Load_Map_Desert_Format(ifs, TEXT("Prototype_GameObject_Canyon"), TEXT("Layer_Canyon")))) return S_OK;
	if (FAILED(Load_Map_Desert_Format(ifs, TEXT("Prototype_GameObject_Lift_Body"), TEXT("Layer_Lift_Body")))) return S_OK;
	if (FAILED(Load_Map_Desert_Format(ifs, TEXT("Prototype_GameObject_Iron_Floor"), TEXT("Layer_Iron_Floor")))) return S_OK;
	if (FAILED(Load_Map_Desert_Format(ifs, TEXT("Prototype_GameObject_Deco"), TEXT("Layer_Deco")))) return S_OK;
	if (FAILED(Load_Map_Desert_Format(ifs, TEXT("Prototype_GameObject_Desert_Tree"), TEXT("Layer_Desert_Tree")))) return S_OK;
	if (FAILED(Load_Map_Desert_Format(ifs, TEXT("Prototype_GameObject_Architecture"), TEXT("Layer_Architecture")))) return S_OK;
	
	if (FAILED(Load_Interaction_Objects_By_Layer(ifs, TEXT("Prototype_GameObject_Top_Roof"), TEXT("Layer_Top_Roof")))) return S_OK;
	if (FAILED(Load_Interaction_Objects_By_Layer(ifs, TEXT("Prototype_GameObject_RepairConsole"), TEXT("Layer_RepairConsole")))) return S_OK;
	if (FAILED(Load_Interaction_Objects_By_Layer(ifs, TEXT("Prototype_GameObject_SciFi_Door"), TEXT("Layer_SciFi_Door")))) return S_OK;
	if (FAILED(Load_Interaction_Objects_By_Layer(ifs, TEXT("Prototype_GameObject_CanBox"), TEXT("Layer_CanBox"), true))) return S_OK;
	if (FAILED(Load_Interaction_Objects_By_Layer(ifs, TEXT("Prototype_GameObject_Interaction_NonAnim"), TEXT("Layer_Interaction")))) return S_OK;
	
	if (FAILED(Load_Instancing_By_Layer(ifs, TEXT("Layer_Desert_Grass")))) return S_OK;
	
	if (FAILED(Load_Lift_Platform_By_Layer(ifs, TEXT("Prototype_GameObject_Lift_Platform"), TEXT("Layer_Lift_Platform")))) return S_OK;
	if (FAILED(Load_Lift_Controller_By_Layer(ifs, TEXT("Prototype_GameObject_Lift_Controller"), TEXT("Layer_Lift_Controller")))) return S_OK;
	/*if (FAILED(Load_Interaction_Objects_By_Layer(ifs, TEXT("Prototype_GameObject_DisplayBox"), TEXT("Layer_DisplayBox")))) return S_OK;
	if (FAILED(Load_Map_Desert_Format(ifs, TEXT("Prototype_GameObject_Shutter"), TEXT("Layer_Shutter")))) return S_OK;*/
#pragma endregion

	ifs.close();

	return S_OK;
}

HRESULT CLevel_GamePlay::Load_Monster_Desert_Data(const _char* szFilePath)
{
	std::ifstream ifs(szFilePath, std::ios::binary);
	if (!ifs.is_open())
	{
		MessageBoxW(g_hWnd, L"Failed to open MapData", L"Error", MB_OK | MB_ICONERROR);
		return E_FAIL;
	}

	if (FAILED(Load_Monster_Desert_Format(ifs, TEXT("Prototype_GameObject_Nayitba"), TEXT("Layer_Monster")))) return S_OK;
	if (FAILED(Load_Npc_Desert_Format(ifs, TEXT("Prototype_GameObject_Npc"), TEXT("Layer_Npc")))) return S_OK;

	ifs.close();

	return S_OK;
}

HRESULT CLevel_GamePlay::Load_Sound_Trigger_Box_Objects(const _char* szFilePath)
{
	std::ifstream ifs(szFilePath, std::ios::binary);
	if (!ifs.is_open())
	{
		MessageBoxW(g_hWnd, L"Failed to open MapData", L"Error", MB_OK | MB_ICONERROR);
		return E_FAIL;
	}

	if (FAILED(Load_Sound_Trigger_Box_By_Layer(ifs, TEXT("Prototype_GameObject_SoundTriggerBox"), TEXT("Layer_SoundTriggerBox")))) return E_FAIL;

	ifs.close();

	return S_OK;
}

HRESULT CLevel_GamePlay::Load_Map_Desert_Format(std::ifstream& ifs, const _tchar* protoTag, const _tchar* pLayerTag)
{
	_uint iNumObjs = 0;
	ifs.read(reinterpret_cast<char*>(&iNumObjs), sizeof(_uint));

	for (_uint i = 0; i < iNumObjs; ++i)
	{
		SAVEDOBJECTINFO info;
		ifs.read(reinterpret_cast<char*>(&info), sizeof(SAVEDOBJECTINFO));

		CActor::ACTOR_DESC Desc = {};
		Desc.bIsApplyTransform = true;
		Desc.bIsQuaternion = true;
		Desc.iObjectID = info.iObjectID;
		Desc.szVIBuffer_PrototypeName = info.szComponentTag;

		_vector vScale = {};
		_vector vRotation = {};
		_vector vPosition = {};
		XMMatrixDecompose(&vScale, &vRotation, &vPosition, XMLoadFloat4x4(&info.worldMatrix));

		XMStoreFloat3(&Desc.vScale, vScale);
		XMStoreFloat4(&Desc.vRotation, vRotation);
		XMStoreFloat3(&Desc.vPosition, vPosition);

		HRESULT hr = m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::LEVEL_PROB), protoTag,
			ENUM_CLASS(LEVEL::GAMEPLAY), pLayerTag, &Desc);
	}

	return S_OK;
}

HRESULT CLevel_GamePlay::Load_Npc_Desert_Format(ifstream& ifs, const _tchar* protoTag, const _tchar* pLayerTag)
{
	_uint iNumObjs = 0;
	ifs.read(reinterpret_cast<char*>(&iNumObjs), sizeof(_uint));

	for (_uint i = 0; i < iNumObjs; ++i)
	{
		SAVEDNPCINFO info;
		ifs.read(reinterpret_cast<char*>(&info), sizeof(SAVEDNPCINFO));

		CNpc::NPC_DESC Desc = {};
		Desc.bIsApplyTransform = true;
		Desc.bIsQuaternion = true;
		Desc.iNpcID = info.iNpcID;

		_vector vScale = {};
		_vector vRotation = {};
		_vector vPosition = {};
		XMMatrixDecompose(&vScale, &vRotation, &vPosition, XMLoadFloat4x4(&info.worldMatrix));

		XMStoreFloat3(&Desc.vScale, vScale);
		XMStoreFloat4(&Desc.vRotation, vRotation);
		XMStoreFloat3(&Desc.vPosition, vPosition);

		HRESULT hr = m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), protoTag,
			ENUM_CLASS(LEVEL::GAMEPLAY), pLayerTag, &Desc);
	}

	return S_OK;
}


HRESULT CLevel_GamePlay::Load_Interaction_Objects_By_Layer(ifstream& ifs, const _tchar* protoTag, const _tchar* pLayerTag, _bool bIsGamePlay)
{
	_uint iNumObjs = 0;
	ifs.read(reinterpret_cast<char*>(&iNumObjs), sizeof(_uint));

	for (_uint i = 0; i < iNumObjs; ++i)
	{
		SAVEDINTERACTIONOBJECTINFO info;
		ifs.read(reinterpret_cast<char*>(&info), sizeof(SAVEDINTERACTIONOBJECTINFO));

		CProb_Interaction::PROB_INTERACTION_DESC Desc = {};
		Desc.bIsApplyTransform = true;
		Desc.bIsQuaternion = true;
		Desc.iObjectID = info.iObjectID;
		Desc.iInteractionID = info.iInteractionID;
		Desc.szVIBuffer_PrototypeName = info.szComponentTag;

		_vector vScale = {};
		_vector vRotation = {};
		_vector vPosition = {};
		XMMatrixDecompose(&vScale, &vRotation, &vPosition, XMLoadFloat4x4(&info.worldMatrix));

		XMStoreFloat3(&Desc.vScale, vScale);
		XMStoreFloat4(&Desc.vRotation, vRotation);
		XMStoreFloat3(&Desc.vPosition, vPosition);

		HRESULT hr = {};
		if (bIsGamePlay)
		{
			hr = m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), protoTag,
				ENUM_CLASS(LEVEL::GAMEPLAY), pLayerTag, &Desc);
		}
		else
		{
			hr = m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::LEVEL_PROB), protoTag,
				ENUM_CLASS(LEVEL::GAMEPLAY), pLayerTag, &Desc);
		}

	}

	return S_OK;
}

HRESULT CLevel_GamePlay::Load_Lift_Controller_By_Layer(ifstream& ifs, const _tchar* protoTag, const _tchar* pLayerTag)
{
	_uint iNumObjs = 0;
	ifs.read(reinterpret_cast<char*>(&iNumObjs), sizeof(_uint));

	for (_uint i = 0; i < iNumObjs; ++i)
	{
		SAVED_LIFT_CONTROLLER_INFO info;
		ifs.read(reinterpret_cast<char*>(&info), sizeof(SAVED_LIFT_CONTROLLER_INFO));

		CLift_Controller::LIFT_CONTROLLER_DESC Desc = {};
		Desc.bIsApplyTransform = true;
		Desc.bIsQuaternion = true;
		Desc.iObjectID = info.iObjectID;
		Desc.iInteractionID = info.iInteractionID;
		Desc.szVIBuffer_PrototypeName = info.szComponentTag;
		Desc.bIsControllerType = info.bIsControllerType;
		Desc.iPlatformID = info.iPlatformID;
		Desc.iPosition = info.iPosition;

		_vector vScale = {};
		_vector vRotation = {};
		_vector vPosition = {};
		XMMatrixDecompose(&vScale, &vRotation, &vPosition, XMLoadFloat4x4(&info.worldMatrix));

		XMStoreFloat3(&Desc.vScale, vScale);
		XMStoreFloat4(&Desc.vRotation, vRotation);
		XMStoreFloat3(&Desc.vPosition, vPosition);

		HRESULT hr = m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::LEVEL_PROB), protoTag,
			ENUM_CLASS(LEVEL::GAMEPLAY), pLayerTag, &Desc);
	}

	return S_OK;
}

HRESULT CLevel_GamePlay::Load_Lift_Platform_By_Layer(ifstream& ifs, const _tchar* protoTag, const _tchar* pLayerTag)
{
	_uint iNumObjs = 0;
	ifs.read(reinterpret_cast<char*>(&iNumObjs), sizeof(_uint));

	for (_uint i = 0; i < iNumObjs; ++i)
	{
		SAVED_LIFT_PLATFORM_INFO info;
		ifs.read(reinterpret_cast<char*>(&info), sizeof(SAVED_LIFT_PLATFORM_INFO));

		CLift_Platform::LIFT_PLATFORM_DESC Desc = {};
		Desc.bIsApplyTransform = true;
		Desc.bIsQuaternion = true;
		Desc.iObjectID = info.iObjectID;
		Desc.szVIBuffer_PrototypeName = info.szComponentTag;
		Desc.iPlatFormID = info.iPlatformID;
		Desc.fMoveDistance = info.fMoveDistance;

		_vector vScale = {};
		_vector vRotation = {};
		_vector vPosition = {};
		XMMatrixDecompose(&vScale, &vRotation, &vPosition, XMLoadFloat4x4(&info.worldMatrix));

		XMStoreFloat3(&Desc.vScale, vScale);
		XMStoreFloat4(&Desc.vRotation, vRotation);
		XMStoreFloat3(&Desc.vPosition, vPosition);

		HRESULT hr = m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::LEVEL_PROB), protoTag,
			ENUM_CLASS(LEVEL::GAMEPLAY), pLayerTag, &Desc);
	}

	return S_OK;
}

HRESULT CLevel_GamePlay::Load_Monster_Desert_Format(std::ifstream& ifs, const _tchar* protoTag, const _tchar* pLayerTag)
{
	_uint iNumObjs = 0;
	ifs.read(reinterpret_cast<char*>(&iNumObjs), sizeof(_uint));

	for (_uint i = 0; i < iNumObjs; ++i)
	{
		SAVEDMONSTERINFO info;
		ifs.read(reinterpret_cast<char*>(&info), sizeof(SAVEDMONSTERINFO));

		CNaytiba::NAYITBA_DESC Desc = {};
		Desc.bIsApplyTransform = true;
		Desc.bIsQuaternion = true;
		Desc.iMonsterID = info.iMonsterId;

		_vector vScale = {};
		_vector vRotation = {};
		_vector vPosition = {};
		XMMatrixDecompose(&vScale, &vRotation, &vPosition, XMLoadFloat4x4(&info.worldMatrix));

		XMStoreFloat3(&Desc.vScale, vScale);
		XMStoreFloat4(&Desc.vRotation, vRotation);
		XMStoreFloat3(&Desc.vPosition, vPosition);

		HRESULT hr = m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), protoTag,
			ENUM_CLASS(LEVEL::GAMEPLAY), pLayerTag, &Desc);
	}

	return S_OK;
}

HRESULT CLevel_GamePlay::Load_Instancing_By_Layer(ifstream& ifs, const _tchar* pLayerTag)
{
	_uint iNumObjs = 0;
	ifs.read(reinterpret_cast<char*>(&iNumObjs), sizeof(_uint));

	if (iNumObjs == 0) return S_OK;

	map<wstring, pair<vector<VTX_INSTANCE_MODEL>, _uint>> groupedInstanceData;

	for (_uint i = 0; i < iNumObjs; ++i)
	{
		SAVEDOBJECTINFO info;
		ifs.read(reinterpret_cast<char*>(&info), sizeof(SAVEDOBJECTINFO));

		_matrix matWorld = XMLoadFloat4x4(&info.worldMatrix);
		_vector vScale, vRotation, vPosition;
		XMMatrixDecompose(&vScale, &vRotation, &vPosition, matWorld);

		_matrix matFinalWorld = XMMatrixScaling(XMVectorGetX(vScale), XMVectorGetY(vScale), XMVectorGetZ(vScale))
			* XMMatrixRotationQuaternion(vRotation)
			* XMMatrixTranslationFromVector(vPosition);

		VTX_INSTANCE_MODEL InstanceData{};
		XMStoreFloat4(&InstanceData.vRight, matFinalWorld.r[0]);
		XMStoreFloat4(&InstanceData.vUp, matFinalWorld.r[1]);
		XMStoreFloat4(&InstanceData.vLook, matFinalWorld.r[2]);
		XMStoreFloat4(&InstanceData.vTranslation, matFinalWorld.r[3]);

		wstring strTag = info.szComponentTag;
		_uint iObjectID = info.iObjectID;

		groupedInstanceData[strTag].first.push_back(InstanceData);
		groupedInstanceData[strTag].second = iObjectID;
	}

	for (auto& pair : groupedInstanceData)
	{
		Engine::MODEL_INSTANCE_LOAD_DESC FinalLoadDesc = {};
		FinalLoadDesc.iNumInstance = (_uint)pair.second.first.size();

		FinalLoadDesc.pPrototypeTag = pair.first.c_str();
		FinalLoadDesc.iObjectID = pair.second.second;
		FinalLoadDesc.pInstancingData = const_cast<vector<VTX_INSTANCE_MODEL>*>(&pair.second.first);

		HRESULT hr = m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::LEVEL_PROB),
			TEXT("Prototype_GameObject_Instance_Desert"),
			ENUM_CLASS(LEVEL::GAMEPLAY), pLayerTag,
			&FinalLoadDesc);

		if (FAILED(hr))
		{
			return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CLevel_GamePlay::Load_Sound_Trigger_Box_By_Layer(ifstream& ifs, const _tchar* protoTag, const _tchar* pLayerTag)
{
	_uint iNumObjs = 0;
	ifs.read(reinterpret_cast<char*>(&iNumObjs), sizeof(_uint));

	for (_uint i = 0; i < iNumObjs; ++i)
	{
		SAVED_SOUND_TRIGGER_BOX_INFO info;
		ifs.read(reinterpret_cast<char*>(&info), sizeof(SAVED_SOUND_TRIGGER_BOX_INFO));

		CSoundTriggerBox::SOUNDTRIGGER_BOX_DESC Desc = {};
		Desc.bIsApplyTransform = true;
		Desc.bIsQuaternion = true;
		Desc.eSoundBoxType = info.eType;

		_vector vScale = {};
		_vector vRotation = {};
		_vector vPosition = {};
		XMMatrixDecompose(&vScale, &vRotation, &vPosition, XMLoadFloat4x4(&info.worldMatrix));

		XMStoreFloat3(&Desc.vScale, vScale);
		XMStoreFloat4(&Desc.vRotation, vRotation);
		XMStoreFloat3(&Desc.vPosition, vPosition);

		HRESULT hr = m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), protoTag,
			ENUM_CLASS(LEVEL::GAMEPLAY), pLayerTag, &Desc);
	}

	return S_OK;
}

HRESULT CLevel_GamePlay::Load_Level_CinematicObjectData(const _char* szFilePath)
{
	/*
	* == 오할? =====
		1. CinematicData_Desert.json으로 Level_GamePlay등에서 모델 + 카메라 생성하는거 제작
		2. 그거 Cinematic_Manager에서 등록하는 기능 제작
		3. 일단 모델만 불러와서 한번 실행해보기
		4. 이후 카메라 잡기
	*/

	CGameManager::GetInstance()->Load_Level_CinematicObjectData(szFilePath);


	if (FAILED(Ready_Layer_Trigger(TEXT("Layer_Trigger"))))
		return E_FAIL;

	return S_OK;
}


HRESULT CLevel_GamePlay::Load_Light_Data()
{
	std::ifstream ifs("../Bin/DataFiles/LightData.bin", std::ios::binary);
	if (!ifs.is_open())
	{
		MessageBoxW(g_hWnd, L"Failed to open LightData", L"Error", MB_OK | MB_ICONERROR);
		return E_FAIL;
	}

	const list<CLight*>* pLights = m_pGameInstance->GetAllLight();

	if (pLights && !pLights->empty())
	{
		for (auto pLight : *pLights)
		{
			pLight->SetDead(true);
		}
	}

	_uint iNumLights = 0;
	ifs.read(reinterpret_cast<_char*>(&iNumLights), sizeof(_uint));

	for (_uint i = 0; i < iNumLights; ++i)
	{
		LIGHT_DESC LightDesc;
		ifs.read(reinterpret_cast<_char*>(&LightDesc), sizeof(LIGHT_DESC));

		m_pGameInstance->Add_Light(LightDesc);
	}

	ifs.close();

	return S_OK;
}

CLevel_GamePlay* CLevel_GamePlay::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevelID)
{
	CLevel_GamePlay* pInstance = new CLevel_GamePlay(pDevice, pContext, eLevelID);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_GamePlay");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_GamePlay::Free()
{
	m_pGameInstance->SetInteractionBaseObject(nullptr);

	m_pGameInstance->UnBind_Observer(TEXT("Change_Map"), m_pLevelChangeEvent);
	m_pGameInstance->UnBind_Observer(TEXT("Go_DororongSaber"), m_pLevelChangeEvent);
	m_pGameInstance->UnBind_Observer(TEXT("TelePort"), m_pTeleportEvent);

	Safe_Release(m_pLevelChangeEvent);
	Safe_Release(m_pTeleportEvent);

	__super::Free();
}
