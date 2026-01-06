#include "pch.h"
#include "UI_Level_Beatsaber.h"

#include "Actor.h"
#include "UI_Camera.h"

#include "GameInstance.h"
#include "UIHUD.h"

#include "GameManager.h"

CUI_Level_Beatsaber::CUI_Level_Beatsaber(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevelID)
	: CLevel { pDevice, pContext, ENUM_CLASS(eLevelID)}
{
}

HRESULT CUI_Level_Beatsaber::Initialize()
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
	StaticShadowDesc.fFar = 3000.f;
	StaticShadowDesc.fNear = 0.1f;
	StaticShadowDesc.vAt = _float4(400.f, 300.f, 0.f, 1.f);

	if (FAILED(m_pGameInstance->Ready_StaticShadow_Light(StaticShadowDesc)))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;
	
	//if (FAILED(Ready_Layer_BackGround(TEXT("BackGround"))))
	//	return E_FAIL;

	if (FAILED(Ready_UI(TEXT("Layer_UI"))))
		return E_FAIL;

	return S_OK;
}

void CUI_Level_Beatsaber::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

	if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_5))
	{
		static_cast<CUIHUD*>(m_pHUD)->Open_Result();
	}
	if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_6))
	{
		static_cast<CUIHUD*>(m_pHUD)->Close_Result();
	}
	if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_7))
	{
		static_cast<CUIHUD*>(m_pHUD)->Open_Song_Selector();
	}

	/*if (m_isOverlay && m_pHUD)
	{
		static_cast<CUIHUD*>(m_pHUD)->Anim_Play(TEXT("Layer_Combat"), TEXT("GamePlay_Overlay"), TEXT("Intro"));
		m_isOverlay = false;
	}

	if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_5))
	{
		static_cast<CUIHUD*>(m_pHUD)->Open_Shop();
	}
	if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_6))
	{
		static_cast<CUIHUD*>(m_pHUD)->Close_Shop();
	}*/
}

HRESULT CUI_Level_Beatsaber::Render()
{
	SetWindowText(g_hWnd, TEXT("게임플레이레벨이빈다"));
	return S_OK;
}

HRESULT CUI_Level_Beatsaber::Ready_Layer_Camera(const _wstring& strLayerTag)
{
	CUI_Camera::UI_CAMERA_DESC			CameraDesc{};
	CameraDesc.fFov = XMConvertToRadians(60.0f);
	CameraDesc.fNear = 0.1f;
	CameraDesc.fFar = 500.f;
	CameraDesc.vEye = _float3(0.f, 10.f, -10.f);
	CameraDesc.vAt = _float3(0.f, 0.f, 0.f);
	CameraDesc.fSpeedPerSec = 5.f;
	CameraDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	CameraDesc.fMouseSensor = 0.1f;

	auto pCamera = m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_Camera"), &CameraDesc);
	m_pGameInstance->Add_Camera(TEXT("UI_Camera"), static_cast<CCamera*>(pCamera));
	m_pGameInstance->SetMainCamera(TEXT("UI_Camera"));

	return S_OK;
}

HRESULT CUI_Level_Beatsaber::Ready_UI(const _wstring& strLayerTag)
{
	CUIHUD* pUIHUD = CUIHUD::Create(m_pDevice, m_pContext);

	if (pUIHUD == nullptr)
		return E_FAIL;

	SetHUD(pUIHUD);

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

HRESULT CUI_Level_Beatsaber::Ready_Layer_BackGround(const _wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::LEVEL_PROB), TEXT("Prototype_GameObject_Sky"),
		ENUM_CLASS(LEVEL::BEATSABER_GAME), strLayerTag)))
		return E_FAIL;

	return S_OK;
}


CUI_Level_Beatsaber* CUI_Level_Beatsaber::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevelID)
{
	CUI_Level_Beatsaber* pInstance = new CUI_Level_Beatsaber(pDevice, pContext, eLevelID);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CUI_Level_Beatsaber");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Level_Beatsaber::Free()
{
	__super::Free();
}
