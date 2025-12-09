#include "pch.h"
#include "Level_GamePlay.h"
#include "GameInstance.h"
#include "GameManager.h"
#include "Level_Loading.h"

#include "Actor.h"
#include "Nayitba.h"
#include "Camera_Free.h"
#include "Player.h"
#include "UIHUD.h"

#ifdef _DEBUG
#include "ImGuiManager.h"
#endif // _DEBUG

CLevel_GamePlay::CLevel_GamePlay(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevelID)
	: CLevel { pDevice, pContext, ENUM_CLASS(eLevelID)}	
{
}

HRESULT CLevel_GamePlay::Initialize()
{
	m_pGameInstance->Manager_StopSound(CHANNELID::BGM);
	m_pGameInstance->Manager_PlayBGM(TEXT("BGM_WASTELAND_UNDISCOVER_LOOP_100_C.wav"), 0.5f);

	if (FAILED(Ready_Lights()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Player(TEXT("Layer_Player"))))
 		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Terrain(TEXT("Layer_Terrain"))))
 		return E_FAIL;

	if (FAILED(Ready_Layer_Sky(TEXT("Layer_Sky"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Monster(TEXT("Layer_Monster"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_UI(TEXT("Layer_UI"))))
		return E_FAIL;

	Load_Map_Desert_Data("../../Map_Editor/Bin/DataFiles/MapData_Desert10.bin");
	Load_Monster_Desert_Data("../Bin/DataFiles/MonsterData_Desert.bin");

	auto pGameCharacter = CGameManager::GetInstance()->GetGameCharacter();
	m_pGameInstance->SetInteractionBaseObject(pGameCharacter);
	Safe_Release(pGameCharacter);

#ifdef _DEBUG
	CImGuiManager::GetInstance()->SetLevelFreeCamera();
#endif // _DEBUG

	return S_OK;
}

void CLevel_GamePlay::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

	if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_F12))
	{
		m_pGameInstance->Clear_LevelResource();
		if (FAILED(m_pGameInstance->Change_Level(CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL::LOADING, LEVEL::SCARLET))))
			return;
	}
}

HRESULT CLevel_GamePlay::Render()
{
#ifdef _DEBUG
	SetWindowText(g_hWnd, TEXT("게임플레이레벨이빈다"));
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
	LIGHT_DESC			LightDesc{};

	LightDesc.eType = LIGHT_TYPE::DIRECTIONAL;
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);

	if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
		return E_FAIL;


	// 빛 정보 로딩 함수. 나중에 반드시 켜야됩니다
	//Load_Light_Data();

	/*LIGHT_DESC			LightDesc{};

	LightDesc.eType = LIGHT_TYPE::DIRECTIONAL;
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(0.5f, 0.5f, 0.5f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);

	if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
		return E_FAIL;*/

	/*LightDesc.eType = LIGHT_TYPE::POINT;
	LightDesc.vDiffuse = _float4(1.f, 0.0f, 0.f, 1.f);
	LightDesc.vAmbient = _float4(0.4f, 0.2f, 0.2f, 1.f);
	LightDesc.vSpecular = LightDesc.vDiffuse;
	LightDesc.vPosition = _float4(20.f, 5.f, 20.f, 1.f);
	LightDesc.fRange = 10.f;

	if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
		return E_FAIL;

	LightDesc.eType = LIGHT_TYPE::POINT;
	LightDesc.vDiffuse = _float4(0.f, 1.f, 0.f, 1.f);
	LightDesc.vAmbient = _float4(0.2f, 0.4f, 0.2f, 1.f);
	LightDesc.vSpecular = LightDesc.vDiffuse;
	LightDesc.vPosition = _float4(30.f, 5.f, 20.f, 1.f);
	LightDesc.fRange = 10.f;

	if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
		return E_FAIL;*/


	SHADOW_LIGHT_DESC		ShadowDesc{};
	ShadowDesc.vEye = _float4(0.f, 15.f, 0.f, 1.f);
	ShadowDesc.vAt = _float4(10.f, 0.f, 10.f, 1.f);
	ShadowDesc.fFovy = XMConvertToRadians(120.0f);
	ShadowDesc.fAspect = static_cast<_float>(g_iWinSizeX) / g_iWinSizeY;
	ShadowDesc.fNear = 0.1f;
	ShadowDesc.fFar = 500.f;

	if (FAILED(m_pGameInstance->Ready_Shadow_Light(ShadowDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_BackGround(const _wstring& strLayerTag)
{

	CActor::ACTOR_DESC ProbDesc = {};
	ProbDesc.bIsApplyTransform = true;
	ProbDesc.vScale = { 1.f, 1.f, 1.f };

	ProbDesc.szVIBuffer_PrototypeName = TEXT("Prototype_Component_Model_Prob_Box1");
	for (size_t i = 0; i < 5; i++)
	{
		ProbDesc.vPosition = { m_pGameInstance->Random(0, 20),
							  0.3f,
							   m_pGameInstance->Random(0, 20) };

		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Prob_Destory"),
			ENUM_CLASS(LEVEL::GAMEPLAY), strLayerTag, &ProbDesc)))
			return E_FAIL;
	}

	ProbDesc.szVIBuffer_PrototypeName = TEXT("Prototype_Component_Model_Prob_Box2");
	for (size_t i = 0; i < 5; i++)
	{
		ProbDesc.vPosition = { m_pGameInstance->Random(0, 50),
							  0.3f,
							   m_pGameInstance->Random(0, 50) };

		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Prob_Destory"),
			ENUM_CLASS(LEVEL::GAMEPLAY), strLayerTag, &ProbDesc)))
			return E_FAIL;
	}

	ProbDesc.szVIBuffer_PrototypeName = TEXT("Prototype_Component_Model_Prob_CanBox");
	for (size_t i = 0; i < 5; i++)
	{
		ProbDesc.vPosition = { 10.f * i,
							   m_pGameInstance->Random(0, 50),
							   m_pGameInstance->Random(0, 50) };
	
		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Prob_CanBox"),
			ENUM_CLASS(LEVEL::GAMEPLAY), strLayerTag, &ProbDesc)))
			return E_FAIL;
	}

	ProbDesc.szVIBuffer_PrototypeName = TEXT("Prototype_Component_Model_Interaction_Chair117");
	for (size_t i = 0; i < 5; i++)
	{
		ProbDesc.vPosition = { m_pGameInstance->Random(0, 50),
							   0.f,
							   m_pGameInstance->Random(0, 50) };

		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Static_Interaction"),
			ENUM_CLASS(LEVEL::GAMEPLAY), strLayerTag, &ProbDesc)))
			return E_FAIL;
	}

	ProbDesc.szVIBuffer_PrototypeName = TEXT("Prototype_Component_Model_Interaction_Vending7A");
	for (size_t i = 0; i < 5; i++)
	{
		ProbDesc.vPosition = { m_pGameInstance->Random(50, 100),
							   0.f,
							   m_pGameInstance->Random(50, 100) };

		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Static_Interaction"),
			ENUM_CLASS(LEVEL::GAMEPLAY), strLayerTag, &ProbDesc)))
			return E_FAIL;
	}

	ProbDesc.szVIBuffer_PrototypeName = TEXT("Prototype_Component_Model_Pallet02");
	for (size_t i = 0; i < 5; i++)
	{
		ProbDesc.vPosition = { m_pGameInstance->Random(150, 200),
							   0.3f,
							   m_pGameInstance->Random(150, 200) };

		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Prob_Destory"),
			ENUM_CLASS(LEVEL::GAMEPLAY), strLayerTag, &ProbDesc)))
			return E_FAIL;
	}

	ProbDesc.szVIBuffer_PrototypeName = TEXT("Prototype_Component_Model_Poster02");
	for (size_t i = 0; i < 5; i++)
	{
		ProbDesc.vPosition = { m_pGameInstance->Random(200, 250),
							   0.f,
							   m_pGameInstance->Random(200, 250) };

		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Prob_Static"),
			ENUM_CLASS(LEVEL::GAMEPLAY), strLayerTag, &ProbDesc)))
			return E_FAIL;
	}
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

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Terrain_Desert"),
		ENUM_CLASS(LEVEL::GAMEPLAY), strLayerTag)))
		return E_FAIL;



	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Sky(const _wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Sky"),
		ENUM_CLASS(LEVEL::GAMEPLAY), strLayerTag)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Moon"),
		ENUM_CLASS(LEVEL::GAMEPLAY), strLayerTag)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Player(const _wstring& strLayerTag)
{	
	CGameObject::GAMEOBJECT_DESC Desc = {};
	Desc.bIsApplyTransform = true;
	Desc.vScale = { 1.f, 1.f, 1.f };
	Desc.vPosition = { 696.44f, 2.f, 560.87f };
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

	CNayitba::NAYITBA_DESC Desc = {};
	Desc.bIsApplyTransform = true;
	Desc.vScale = { 1.f, 1.f, 1.f };

	//Desc.iMonsterID = 1;
	//Desc.vPosition = { 62.f, 1.f, 62.f };
	//if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Nayitba"),
	//	ENUM_CLASS(LEVEL::GAMEPLAY), strLayerTag, &Desc)))
	//	return E_FAIL;
	//
	//Desc.iMonsterID = 3;
	//{
	//	for (_uint i = 0; i < 10; ++i)
	//	{
	//		Desc.vPosition = { 15.f * i, 1.f, 105.f};
	//		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Nayitba"),
	//			ENUM_CLASS(LEVEL::GAMEPLAY), strLayerTag, &Desc)))
	//			return E_FAIL;
	//	}
	//	
	//
	//}
	
	//Desc.iMonsterID = 4;
	//Desc.vPosition = { m_pGameInstance->Random(0.f, 30.f), 1.f, m_pGameInstance->Random(0.f, 30.f) };
	//Desc.bIsSuperMonster = true;
	//if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Nayitba"),
	//	ENUM_CLASS(LEVEL::GAMEPLAY), strLayerTag, &Desc)))
	//	return E_FAIL;

	Desc.iMonsterID = 4;
	Desc.bIsSuperMonster = true;
	Desc.vPosition = { m_pGameInstance->Random(0.f, 30.f), 1.f, m_pGameInstance->Random(0.f, 30.f) };
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Nayitba"),
		ENUM_CLASS(LEVEL::GAMEPLAY), strLayerTag, &Desc)))
		return E_FAIL;


	//	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Test_InstanceModel"),
	//		ENUM_CLASS(LEVEL::GAMEPLAY), strLayerTag)))
	//		return E_FAIL;

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
	pUIHUD->Register_WorldUI(TEXT("Pool_MonsterVital"), TEXT("UI_Monster_Vital"), 10, ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_World"));
	//pUIHUD->Register_WorldUI(TEXT("Pool_Simple_Interaction"), TEXT("UI_Simple_Interaction"), 10, ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_World"));
	pUIHUD->Anim_Play(TEXT("Layer_World"), TEXT("MonsterHp_Fx"), TEXT("Hp_Fx_BeapBeap"));


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
	if (FAILED(Load_Map_Desert_Format(ifs, TEXT("Prototype_GameObject_Lift_Controller"), TEXT("Layer_Lift_Controller")))) return S_OK;
	if (FAILED(Load_Map_Desert_Format(ifs, TEXT("Prototype_GameObject_Lift_Platform"), TEXT("Layer_Lift_Platform")))) return S_OK;
	if (FAILED(Load_Map_Desert_Format(ifs, TEXT("Prototype_GameObject_Iron_Floor"), TEXT("Layer_Iron_Floor")))) return S_OK;
	if (FAILED(Load_Map_Desert_Format(ifs, TEXT("Prototype_GameObject_Deco"), TEXT("Layer_Deco")))) return S_OK;
	
	if (FAILED(Load_Map_Desert_Format(ifs, TEXT("Prototype_GameObject_Chair"), TEXT("Layer_Chair")))) return S_OK;
	if (FAILED(Load_Map_Desert_Format(ifs, TEXT("Prototype_GameObject_VendingMachine"), TEXT("Layer_VendingMachine")))) return S_OK;
	if (FAILED(Load_Map_Desert_Format(ifs, TEXT("Prototype_GameObject_Top_Roof"), TEXT("Layer_Top_Roof")))) return S_OK;
	if (FAILED(Load_Map_Desert_Format(ifs, TEXT("Prototype_GameObject_RepairConsole"), TEXT("Layer_RepairConsole")))) return S_OK;
	if (FAILED(Load_Map_Desert_Format(ifs, TEXT("Prototype_GameObject_Desert_Tree"), TEXT("Layer_Desert_Tree")))) return S_OK;
	if (FAILED(Load_Map_Desert_Format(ifs, TEXT("Prototype_GameObject_Corpse"), TEXT("Layer_Corpse")))) return S_OK;
	if (FAILED(Load_Map_Desert_Format(ifs, TEXT("Prototype_GameObject_Container"), TEXT("Layer_Container")))) return S_OK;
	if (FAILED(Load_Map_Desert_Format(ifs, TEXT("Prototype_GameObject_SciFi_Door"), TEXT("Layer_SciFi_Door")))) return S_OK;
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

		HRESULT hr = m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), protoTag,
			ENUM_CLASS(LEVEL::GAMEPLAY), pLayerTag, &Desc);

		//if (SUCCEEDED(hr))
		//{
		//	list<CGameObject*>* pObjs = m_pGameInstance->GetAllObejctToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), pLayerTag);
		//	if (pObjs && !pObjs->empty())
		//	{
		//		CGameObject* pObj = pObjs->back();
		//		CTransform* pTransform = dynamic_cast<CTransform*>(pObj->Find_Component(TEXT("Com_Transform")));
		//		if (pTransform)
		//		{
		//			_matrix matWorld = XMLoadFloat4x4(&info.worldMatrix);

		//			_vector vScale = {};
		//			_vector vRotation = {};
		//			_vector vPosition = {};
		//			XMMatrixDecompose(&vScale, &vRotation, &vPosition, matWorld);

		//			_matrix matScale = XMMatrixScaling(XMVectorGetX(vScale), XMVectorGetY(vScale), XMVectorGetZ(vScale));
		//			_matrix matRotation = XMMatrixRotationQuaternion(vRotation);
		//			_matrix matTranslation = XMMatrixTranslationFromVector(vPosition);

		//			// 순서: Scale * Rotation * Translation (SRT 순서)
		//			_matrix matFinalWorld = matScale * matRotation * matTranslation;

		//			_float4x4* pWorldMatrixDest = const_cast<_float4x4*>(pTransform->Get_WorldMatrixPtr());
		//			XMStoreFloat4x4(pWorldMatrixDest, matFinalWorld);

		//		}
		//	}
		//}
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

		CNayitba::NAYITBA_DESC Desc = {};
		Desc.bIsApplyTransform = true;
		//Desc.vScale = { 1.f, 1.f, 1.f };
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

		HRESULT hr = m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY),
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
	__super::Free();


}
