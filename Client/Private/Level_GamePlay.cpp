#include "pch.h"
#include "Level_GamePlay.h"

#include "GameInstance.h"
#include "GameManager.h"

#include "Actor.h"
#include "Nayitba.h"
#include "Camera_Free.h"
#include "Player.h"
#include "UIHUD.h"

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

	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Terrain(TEXT("Layer_Terrain"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Sky(TEXT("Layer_Sky"))))
		return E_FAIL;
	
	if (FAILED(Ready_Layer_Monster(TEXT("Layer_Monster"))))
		return E_FAIL;

	//m_pGameInstance->ADD_DelayFunction(TEXT("Effect_Create"), 10.f, [&]()
	//	{
	//		Ready_Layer_Effect(TEXT("Layer_Effect"));
	//	});

	if (FAILED(Ready_Layer_Effect(TEXT("Layer_Effect"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_UI(TEXT("Layer_UI"))))
		return E_FAIL;

	//Load_Map_Data();
	auto pGameCharacter = CGameManager::GetInstance()->GetGameCharacter();
	m_pGameInstance->SetInteractionBaseObject(pGameCharacter);
	Safe_Release(pGameCharacter);

	return S_OK;
}

void CLevel_GamePlay::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
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
	/*LIGHT_DESC			LightDesc{};

	LightDesc.eType = LIGHT_TYPE::DIRECTIONAL;
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(0.5f, 0.5f, 0.5f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);

	if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
		return E_FAIL;*/

	Load_Light_Data();

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
							   m_pGameInstance->Random(0, 20),
							   m_pGameInstance->Random(0, 20) };

		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Prob_Box"),
			ENUM_CLASS(LEVEL::GAMEPLAY), strLayerTag, &ProbDesc)))
			return E_FAIL;
	}

	ProbDesc.szVIBuffer_PrototypeName = TEXT("Prototype_Component_Model_Prob_Box2");
	for (size_t i = 0; i < 5; i++)
	{
		ProbDesc.vPosition = { m_pGameInstance->Random(0, 50),
							   m_pGameInstance->Random(0, 50),
							   m_pGameInstance->Random(0, 50) };

		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Prob_Box"),
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
	m_pGameInstance->SetMainCamera(TEXT("FreeCamera"));

	CameraDesc.fSpeedPerSec = 15.f;
	CameraDesc.fRotationPerSec = XMConvertToRadians(120.0f);

	pCamera = m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Camera_Player"), &CameraDesc);
	m_pGameInstance->Add_Camera(TEXT("PlayerCamera"), static_cast<CCamera*>(pCamera));

	/*if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Camera_Free"),
		ENUM_CLASS(LEVEL::GAMEPLAY), strLayerTag, &CameraDesc)))
		return E_FAIL;*/

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Terrain(const _wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Terrain"),
		ENUM_CLASS(LEVEL::GAMEPLAY), strLayerTag)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Sky(const _wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Sky"),
		ENUM_CLASS(LEVEL::GAMEPLAY), strLayerTag)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Player(const _wstring& strLayerTag)
{	
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Player"),
		ENUM_CLASS(LEVEL::GAMEPLAY), strLayerTag)))
		return E_FAIL;	

	//for (size_t i = 0; i < 30; i++)
	//{
	//	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_PxTestProp"),
	//		ENUM_CLASS(LEVEL::GAMEPLAY), strLayerTag)))
	//		return E_FAIL;
	//}

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_TestEveHead"),
		ENUM_CLASS(LEVEL::GAMEPLAY), strLayerTag)))
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

	Desc.iMonsterID = 2;
	Desc.vPosition = { 12.f, 0.f, 12.f };
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Nayitba"),
		ENUM_CLASS(LEVEL::GAMEPLAY), strLayerTag, &Desc)))
		return E_FAIL;

	//Desc.iMonsterID = 3;
	//Desc.vPosition = { 15.f, 0.f, 15.f };
	//if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Nayitba"),
	//	ENUM_CLASS(LEVEL::GAMEPLAY), strLayerTag, &Desc)))
	//	return E_FAIL;

	//Desc.iMonsterID = 4;
	//Desc.vPosition = { 21.f, 0.f, 25.f };
	//if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Nayitba"),
	//	ENUM_CLASS(LEVEL::GAMEPLAY), strLayerTag, &Desc)))
	//	return E_FAIL;

	//Desc.iMonsterID = 5;
	//Desc.vPosition = { 25.f, 0.f, 31.f };
	//if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Nayitba"),
	//	ENUM_CLASS(LEVEL::GAMEPLAY), strLayerTag, &Desc)))
	//	return E_FAIL;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Test_InstanceModel"),
		ENUM_CLASS(LEVEL::GAMEPLAY), strLayerTag)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Effect(const _wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Snow"),
		ENUM_CLASS(LEVEL::GAMEPLAY), strLayerTag)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Effect_Test"),
		ENUM_CLASS(LEVEL::GAMEPLAY), strLayerTag)))
		return E_FAIL;

	//for (size_t i = 0; i < 50; i++)
	//{
	//	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Sprite_Explosion"),
	//		ENUM_CLASS(LEVEL::GAMEPLAY), strLayerTag)))
	//		return E_FAIL;
	//}


	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_UI(const _wstring& strLayerTag)
{
	CUIHUD* pUIHUD = CUIHUD::Create(m_pDevice, m_pContext);

	if (pUIHUD == nullptr)
		return E_FAIL;

	SetHUD(pUIHUD);

	if (FAILED(pUIHUD->Load_Data(TEXT("Layer_Combat"))))
		return E_FAIL;
	
	pUIHUD->Set_Show_Debug_Rect(false);

	return S_OK;
}

HRESULT CLevel_GamePlay::Load_Map_Data()
{
	std::ifstream ifs("../Bin/DataFiles/MapData.bin", std::ios::binary);
	if (!ifs.is_open())
	{
		MessageBoxW(g_hWnd, L"Failed to open MapData", L"Error", MB_OK | MB_ICONERROR);
		return E_FAIL;
	}

	if (FAILED(Load_Map_Format(ifs, TEXT("Prototype_GameObject_Giwajip"), TEXT("Layer_Giwajip")))) return S_OK;

	if (FAILED(Load_Map_Format(ifs, TEXT("Prototype_GameObject_StoneLantern1"), TEXT("Layer_StoneLantern1")))) return S_OK;
	if (FAILED(Load_Map_Format(ifs, TEXT("Prototype_GameObject_StoneLantern2"), TEXT("Layer_StoneLantern2")))) return S_OK;

	if (FAILED(Load_Map_Format(ifs, TEXT("Prototype_GameObject_Stair"), TEXT("Layer_Stair")))) return S_OK;

	if (FAILED(Load_Map_Format(ifs, TEXT("Prototype_GameObject_Stone1"), TEXT("Layer_Stone1")))) return S_OK;
	if (FAILED(Load_Map_Format(ifs, TEXT("Prototype_GameObject_Stone2"), TEXT("Layer_Stone2")))) return S_OK;
	if (FAILED(Load_Map_Format(ifs, TEXT("Prototype_GameObject_Stone3"), TEXT("Layer_Stone3")))) return S_OK;
	if (FAILED(Load_Map_Format(ifs, TEXT("Prototype_GameObject_Stone4"), TEXT("Layer_Stone4")))) return S_OK;

	if (FAILED(Load_Map_Format(ifs, TEXT("Prototype_GameObject_StoneWall1"), TEXT("Layer_StoneWall1")))) return S_OK;
	if (FAILED(Load_Map_Format(ifs, TEXT("Prototype_GameObject_StoneWall2"), TEXT("Layer_StoneWall2")))) return S_OK;

	if (FAILED(Load_Map_Format(ifs, TEXT("Prototype_GameObject_StoneTile"), TEXT("Layer_StoneTile")))) return S_OK;

	if (FAILED(Load_Map_Format(ifs, TEXT("Prototype_GameObject_Inscription_L"), TEXT("Layer_Inscription_L")))) return S_OK;
	if (FAILED(Load_Map_Format(ifs, TEXT("Prototype_GameObject_Inscription_R"), TEXT("Layer_Inscription_R")))) return S_OK;
	if (FAILED(Load_Map_Format(ifs, TEXT("Prototype_GameObject_TombStone"), TEXT("Layer_TombStone")))) return S_OK;
	if (FAILED(Load_Map_Format(ifs, TEXT("Prototype_GameObject_TombStoneBase1"), TEXT("Layer_TombStoneBase1")))) return S_OK;
	if (FAILED(Load_Map_Format(ifs, TEXT("Prototype_GameObject_TombStoneBase2"), TEXT("Layer_TombStoneBase2")))) return S_OK;

	if (FAILED(Load_Map_Format(ifs, TEXT("Prototype_GameObject_DryGrass1"), TEXT("Layer_DryGrass1")))) return S_OK;
	if (FAILED(Load_Map_Format(ifs, TEXT("Prototype_GameObject_DryGrass2"), TEXT("Layer_DryGrass2")))) return S_OK;
	if (FAILED(Load_Map_Format(ifs, TEXT("Prototype_GameObject_DryGrass3"), TEXT("Layer_DryGrass3")))) return S_OK;
	if (FAILED(Load_Map_Format(ifs, TEXT("Prototype_GameObject_Grass"), TEXT("Layer_Grass")))) return S_OK;

	if (FAILED(Load_Map_Format(ifs, TEXT("Prototype_GameObject_Reed"), TEXT("Layer_Reed")))) return S_OK;

	if (FAILED(Load_Map_Format(ifs, TEXT("Prototype_GameObject_Rock1"), TEXT("Layer_Rock1")))) return S_OK;
	if (FAILED(Load_Map_Format(ifs, TEXT("Prototype_GameObject_Rock2"), TEXT("Layer_Rock2")))) return S_OK;
	if (FAILED(Load_Map_Format(ifs, TEXT("Prototype_GameObject_Rock3"), TEXT("Layer_Rock3")))) return S_OK;
	if (FAILED(Load_Map_Format(ifs, TEXT("Prototype_GameObject_Rock4"), TEXT("Layer_Rock4")))) return S_OK;
	if (FAILED(Load_Map_Format(ifs, TEXT("Prototype_GameObject_Rock5"), TEXT("Layer_Rock5")))) return S_OK;
	if (FAILED(Load_Map_Format(ifs, TEXT("Prototype_GameObject_Rock6"), TEXT("Layer_Rock6")))) return S_OK;
	if (FAILED(Load_Map_Format(ifs, TEXT("Prototype_GameObject_Rock7"), TEXT("Layer_Rock7")))) return S_OK;
	if (FAILED(Load_Map_Format(ifs, TEXT("Prototype_GameObject_Rock8"), TEXT("Layer_Rock8")))) return S_OK;

	if (FAILED(Load_Map_Format(ifs, TEXT("Prototype_GameObject_Bamboo"), TEXT("Layer_Bamboo")))) return S_OK;
	if (FAILED(Load_Map_Format(ifs, TEXT("Prototype_GameObject_CherryBlossom1"), TEXT("Layer_CherryBlossom1")))) return S_OK;
	if (FAILED(Load_Map_Format(ifs, TEXT("Prototype_GameObject_CherryBlossom2"), TEXT("Layer_CherryBlossom2")))) return S_OK;
	if (FAILED(Load_Map_Format(ifs, TEXT("Prototype_GameObject_CherryBlossom3"), TEXT("Layer_CherryBlossom3")))) return S_OK;
	if (FAILED(Load_Map_Format(ifs, TEXT("Prototype_GameObject_CherryBlossom4"), TEXT("Layer_CherryBlossom4")))) return S_OK;

	if (FAILED(Load_Map_Format(ifs, TEXT("Prototype_GameObject_Giwajip2"), TEXT("Layer_Giwajip2")))) return S_OK;

	if (FAILED(Load_Map_Format(ifs, TEXT("Prototype_GameObject_CM_Rock1"), TEXT("Layer_CM_Rock1")))) return S_OK;
	if (FAILED(Load_Map_Format(ifs, TEXT("Prototype_GameObject_CM_Rock2"), TEXT("Layer_CM_Rock2")))) return S_OK;
	if (FAILED(Load_Map_Format(ifs, TEXT("Prototype_GameObject_CM_Rock3"), TEXT("Layer_CM_Rock3")))) return S_OK;
	if (FAILED(Load_Map_Format(ifs, TEXT("Prototype_GameObject_CM_Rock4"), TEXT("Layer_CM_Rock4")))) return S_OK;
	if (FAILED(Load_Map_Format(ifs, TEXT("Prototype_GameObject_CM_Rock5"), TEXT("Layer_CM_Rock5")))) return S_OK;
	if (FAILED(Load_Map_Format(ifs, TEXT("Prototype_GameObject_CM_Rock6"), TEXT("Layer_CM_Rock6")))) return S_OK;
	if (FAILED(Load_Map_Format(ifs, TEXT("Prototype_GameObject_CM_Rock7"), TEXT("Layer_CM_Rock7")))) return S_OK;
	if (FAILED(Load_Map_Format(ifs, TEXT("Prototype_GameObject_CM_Rock8"), TEXT("Layer_CM_Rock8")))) return S_OK;
	if (FAILED(Load_Map_Format(ifs, TEXT("Prototype_GameObject_CM_Rock9"), TEXT("Layer_CM_Rock9")))) return S_OK;
	if (FAILED(Load_Map_Format(ifs, TEXT("Prototype_GameObject_CM_Rock10"), TEXT("Layer_CM_Rock10")))) return S_OK;
	if (FAILED(Load_Map_Format(ifs, TEXT("Prototype_GameObject_CM_Rock11"), TEXT("Layer_CM_Rock11")))) return S_OK;
	if (FAILED(Load_Map_Format(ifs, TEXT("Prototype_GameObject_CM_Rock12"), TEXT("Layer_CM_Rock12")))) return S_OK;
	if (FAILED(Load_Map_Format(ifs, TEXT("Prototype_GameObject_CM_Rock13"), TEXT("Layer_CM_Rock13")))) return S_OK;
	if (FAILED(Load_Map_Format(ifs, TEXT("Prototype_GameObject_CM_Rock14"), TEXT("Layer_CM_Rock14")))) return S_OK;

	ifs.close();

	return S_OK;
}

HRESULT CLevel_GamePlay::Load_Map_Format(std::ifstream& ifs, const _tchar* protoTag, const _tchar* pLayerTag)
{
	_uint iNumObjs = 0;
	ifs.read(reinterpret_cast<char*>(&iNumObjs), sizeof(_uint));

	for (_uint i = 0; i < iNumObjs; ++i)
	{
		SAVEDOBJECTINFO info;
		ifs.read(reinterpret_cast<char*>(&info), sizeof(SAVEDOBJECTINFO));

		HRESULT hr = m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), protoTag,
			ENUM_CLASS(LEVEL::GAMEPLAY), pLayerTag);

		if (SUCCEEDED(hr))
		{
			list<CGameObject*>* pObjs = m_pGameInstance->GetAllObejctToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), pLayerTag);
			if (pObjs && !pObjs->empty())
			{
				CGameObject* pObj = pObjs->back();
				CTransform* pTransform = dynamic_cast<CTransform*>(pObj->Find_Component(TEXT("Com_Transform")));
				if (pTransform)
				{
					_matrix matWorld = XMLoadFloat4x4(&info.worldMatrix);

					_vector vScale = {};
					_vector vRotation = {};
					_vector vPosition = {};
					XMMatrixDecompose(&vScale, &vRotation, &vPosition, matWorld);

					_matrix matScale = XMMatrixScaling(XMVectorGetX(vScale), XMVectorGetY(vScale), XMVectorGetZ(vScale));
					_matrix matRotation = XMMatrixRotationQuaternion(vRotation);
					_matrix matTranslation = XMMatrixTranslationFromVector(vPosition);

					_matrix matFinalWorld = matScale * matRotation * matTranslation;

					_float4x4* pWorldMatrixDest = const_cast<_float4x4*>(pTransform->Get_WorldMatrixPtr());
					XMStoreFloat4x4(pWorldMatrixDest, matFinalWorld);

				}
			}
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
