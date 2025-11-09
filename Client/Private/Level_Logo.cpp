#include "pch.h"
#include "Level_Logo.h"

#include "GameInstance.h"
#include "Level_Loading.h"

#include "UIHUD.h"

CLevel_Logo::CLevel_Logo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevelID)
	: CLevel { pDevice, pContext, ENUM_CLASS(eLevelID)}
	
{

}

HRESULT CLevel_Logo::Initialize()
{
	//if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
	//	return E_FAIL;	

	/*if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;*/

	if (FAILED(Ready_Layer_UI(TEXT("Layer_UI"))))
		return E_FAIL;

	m_pGameInstance->Manager_PlayBGM(TEXT("BGM_TrainingRoom_01_A.OGG"), 1.f);

	return S_OK;
}

void CLevel_Logo::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

	if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_SPACE))
	{
		if (FAILED(m_pGameInstance->Change_Level(CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL::LOADING, LEVEL::GAMEPLAY))))
			return;
	}
}

HRESULT CLevel_Logo::Render()
{
	SetWindowText(g_hWnd, TEXT("로고레벨이빈다"));

	return S_OK;
}

HRESULT CLevel_Logo::Ready_Layer_Camera(const _wstring& strLayerTag)
{
	/*CCamera::CAMERA_DESC			CameraDesc{};
	CameraDesc.fFov = D3DXToRadian(60.0f);
	CameraDesc.fNear = 0.1f;
	CameraDesc.fFar = 300.f;
	CameraDesc.vEye = _float3(0.f, 1.f, -1.f);
	CameraDesc.vAt = _float3(0.f, 0.f, 0.f);
	CameraDesc.fSpeedPerSec = 5.f;
	CameraDesc.fRotationPerSec = D3DXToRadian(90.0f);

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Camera"),
		ENUM_CLASS(LEVEL::LOGO), strLayerTag, &CameraDesc)))
		return E_FAIL;*/

	return S_OK;
}

HRESULT CLevel_Logo::Ready_Layer_BackGround(const _wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::LOGO), TEXT("Prototype_GameObject_BackGround"),
		ENUM_CLASS(LEVEL::LOGO), strLayerTag)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Logo::Ready_Layer_UI(const _wstring& strLayerTag)
{
	CUIHUD* pUIHUD = CUIHUD::Create(m_pDevice, m_pContext);

	if (pUIHUD == nullptr)
		return E_FAIL;

	SetHUD(pUIHUD);

	if(FAILED(pUIHUD->Add_UserInterface(ENUM_CLASS(LEVEL::LOGO), TEXT("Prototype_GameObject_UI_Button"), TEXT("UIHUDLayer_Logo"), TEXT("UI_Button"))))
		return E_FAIL;

	return S_OK;
}

CLevel_Logo* CLevel_Logo::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevelID)
{
	CLevel_Logo* pInstance = new CLevel_Logo(pDevice, pContext, eLevelID);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_Logo");
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CLevel_Logo::Free()
{
	__super::Free();
}
