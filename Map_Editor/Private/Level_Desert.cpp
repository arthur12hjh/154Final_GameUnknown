#include "Level_Desert.h"
#include "pch.h"
#include "GameInstance.h"
#include "Level_Desert.h"
#include "Camera_Free.h"
#include "Level_Loading.h"
#include "Imgui_Manager.h"
#include "DesertObject.h"

CLevel_Desert::CLevel_Desert(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevelID)
	: CLevel{ pDevice, pContext, ENUM_CLASS(eLevelID) }

{

}

HRESULT CLevel_Desert::Initialize()
{
	if (FAILED(Ready_Lights()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Terrain(TEXT("Layer_Terrain"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Player(TEXT("Layer_Player_Test"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;

	CImgui_Manager::GetInstance()->Initialize(m_pDevice, m_pContext);

	CImgui_Manager* pManager = CImgui_Manager::GetInstance();
	if (pManager->Get_MapTool_Desert() == nullptr) // MapTool이 아직 생성되지 않았다면
	{
		if (FAILED(pManager->Create_MapTool_For_Desert(m_pDevice, m_pContext)))
			return E_FAIL;
	}

	return S_OK;
}

void CLevel_Desert::Update(_float fTimeDelta)
{
	/*if (GetKeyState(VK_F12) & 0x8000)
	{
		m_pGameInstance->Change_Level(CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL::LOADING, LEVEL::VILLAGE));
	}*/

	CImgui_Manager::GetInstance()->Update(fTimeDelta);

}

HRESULT CLevel_Desert::Render()
{
	SetWindowText(g_hWnd, TEXT("데저트레벨입니다"));

	CImgui_Manager::GetInstance()->Render();

	return S_OK;
}

HRESULT CLevel_Desert::Ready_Lights()
{
	LIGHT_DESC			LightDesc{};

	LightDesc.eType = LIGHT_TYPE::DIRECTIONAL;
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(0.5f, 0.5f, 0.5f, 1.f);
	LightDesc.vSpecular = _float4(0.2f, 0.2f, 0.2f, 0.2f);
	LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);

	if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
		return E_FAIL;

	/*LightDesc.eType = LIGHT_TYPE::POINT;
	LightDesc.vDiffuse = _float4(1.f, 0.0f, 0.f, 1.f);
	LightDesc.vAmbient = _float4(0.4f, 0.2f, 0.2f, 1.f);
	LightDesc.vSpecular = LightDesc.vDiffuse;
	LightDesc.vPosition = _float4(20.f, 5.f, 20.f, 1.f);
	LightDesc.fRange = 10.f;

	if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
		return E_FAIL;*/
	/*
	LightDesc.eType = LIGHT_TYPE::POINT;
	LightDesc.vDiffuse = _float4(0.f, 1.f, 0.f, 1.f);
	LightDesc.vAmbient = _float4(0.2f, 0.4f, 0.2f, 1.f);
	LightDesc.vSpecular = LightDesc.vDiffuse;
	LightDesc.vPosition = _float4(30.f, 5.f, 20.f, 1.f);
	LightDesc.fRange = 10.f;

	if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
		return E_FAIL;*/

	/*SHADOW_LIGHT_DESC		ShadowDesc{};
	ShadowDesc.vEye = _float4(0.f, 15.f, 0.f, 1.f);
	ShadowDesc.vAt = _float4(10.f, 0.f, 10.f, 1.f);
	ShadowDesc.fFovy = XMConvertToRadians(120.0f);
	ShadowDesc.fAspect = static_cast<_float>(g_iWinSizeX) / g_iWinSizeY;
	ShadowDesc.fNear = 0.1f;
	ShadowDesc.fFar = 500.f;

	if (FAILED(m_pGameInstance->Ready_Shadow_Light(ShadowDesc)))
		return E_FAIL;*/

	return S_OK;
}

HRESULT CLevel_Desert::Ready_Layer_BackGround(const _wstring& strLayerTag)
{
	CDesertObject::DesertObjectDesc pDesc = {};
	pDesc.pComponentTag = TEXT("Prototype_Component_Texture_Sky_Desert6");

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::DESERT), TEXT("Prototype_GameObject_Sky_Desert"),
		ENUM_CLASS(LEVEL::DESERT), strLayerTag, &pDesc)))
		return E_FAIL;

	return S_OK;

}

HRESULT CLevel_Desert::Ready_Layer_Terrain(const _wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::DESERT), TEXT("Prototype_GameObject_Terrain_Desert"),
		ENUM_CLASS(LEVEL::DESERT), strLayerTag)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Desert::Ready_Layer_Camera(const _wstring& strLayerTag)
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

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Camera_Free"),
		ENUM_CLASS(LEVEL::DESERT), strLayerTag, &CameraDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Desert::Ready_Layer_Player(const _wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::DESERT), TEXT("Prototype_GameObject_Player_Test"),
		ENUM_CLASS(LEVEL::DESERT), strLayerTag)))
		return E_FAIL;

	return S_OK;
}


CLevel_Desert* CLevel_Desert::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevelID)
{
	CLevel_Desert* pInstance = new CLevel_Desert(pDevice, pContext, eLevelID);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_Desert");
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CLevel_Desert::Free()
{
	__super::Free();
	CImgui_Manager::GetInstance()->DestroyInstance();
}
