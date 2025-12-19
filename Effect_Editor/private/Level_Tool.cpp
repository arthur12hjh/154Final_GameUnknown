#include "pch.h"
#include "Level_Tool.h"

#include "GameInstance.h"
#include "Level_Loading.h"
#include "Img_Manager.h"
#include "Camera_Free.h"


CLevel_Tool::CLevel_Tool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevelID)
	: CLevel{ pDevice, pContext, ENUM_CLASS(eLevelID) }

{

}

HRESULT CLevel_Tool::Initialize()
{
	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;	

	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;

	LIGHT_DESC			LightDesc{};

	LightDesc.eType = LIGHT_TYPE::DIRECTIONAL;
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(0.5f, 0.5f, 0.5f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vDirection = _float4(1.f, -1.f, 0.f, 0.f);

	if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
		return E_FAIL;

	SHADOW_LIGHT_DESC		ShadowDesc{};
	ShadowDesc.vEye = _float4(0.f, 15.f, 0.f, 1.f);
	ShadowDesc.vAt = _float4(10.f, 0.f, 10.f, 1.f);
	ShadowDesc.fFovy = XMConvertToRadians(120.0f);
	ShadowDesc.fAspect = static_cast<_float>(g_iWinSizeX) / g_iWinSizeY;
	ShadowDesc.fNear = 0.1f;
	ShadowDesc.fFar = 500.f;
	ShadowDesc.vDir = _float4(1.f, -1.f, 1.f, 0.f);

	if (FAILED(m_pGameInstance->Ready_Shadow_Light(ShadowDesc)))
		return E_FAIL;


	m_pImgManager = CImg_Manager::GetInstance();
	m_pImgManager->Ready_Manager(m_pDevice, m_pContext);
	return S_OK;
}

void CLevel_Tool::Update(_float fTimeDelta)
{
	m_pImgManager->Update_Manager(fTimeDelta);
	//if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_SPACE))
	//{
	//	if (FAILED(m_pGameInstance->Change_Level(CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL::LOADING, LEVEL::GAMEPLAY))))
	//		return;
	//}
}

HRESULT CLevel_Tool::Render()
{
	SetWindowText(g_hWnd, TEXT("ÀÌÆåÆ® ÅøÀÌºó´Ù"));

	m_pImgManager->Render_Manager();
	return S_OK;
}



HRESULT CLevel_Tool::Ready_Layer_Camera(const _wstring& strLayerTag)
{
	CCamera_Free::CAMERA_FREE_DESC			CameraDesc{};
	CameraDesc.fFov = XMConvertToRadians(60.0f);
	CameraDesc.fNear = 0.1f;
	CameraDesc.fFar = 500.f;
	CameraDesc.vEye = _float3(0.f, 10.f, -10.f);
	CameraDesc.vAt = _float3(0.f, 0.f, 0.f);
	CameraDesc.fSpeedPerSec = 30.f;
	CameraDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	CameraDesc.fMouseSensor = 0.1f;

	//if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Camera_Free"),
	//	ENUM_CLASS(LEVEL::STATIC), strLayerTag, &CameraDesc)))
	//	return E_FAIL;

	auto pCamera = m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Camera_Free"), &CameraDesc);
	m_pGameInstance->Add_Camera(TEXT("FreeCamera"), static_cast<CCamera*>(pCamera));
	m_pGameInstance->SetMainCamera(TEXT("FreeCamera"));

	return S_OK;

	return S_OK;
}

HRESULT CLevel_Tool::Ready_Layer_BackGround(const _wstring& strLayerTag)
{

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::TOOL), TEXT("Prototype_GameObject_Sky"),
		ENUM_CLASS(LEVEL::TOOL), strLayerTag)))
		return E_FAIL;
	//if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::TOOL), TEXT("Prototype_GameObject_BackGround"),
	//	ENUM_CLASS(LEVEL::LOGO), strLayerTag)))
	//	return E_FAIL;

	return S_OK;
}

CLevel_Tool* CLevel_Tool::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevelID)
{
	CLevel_Tool* pInstance = new CLevel_Tool(pDevice, pContext, eLevelID);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_Tool");
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CLevel_Tool::Free()
{
	__super::Free();
	Safe_Release(m_pImgManager);
}
