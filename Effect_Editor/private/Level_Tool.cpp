#include "pch.h"
#include "Level_Tool.h"

#include "GameInstance.h"
#include "Level_Loading.h"
#include "Img_Manager.h"


CLevel_Tool::CLevel_Tool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevelID)
	: CLevel{ pDevice, pContext, ENUM_CLASS(eLevelID) }

{

}

HRESULT CLevel_Tool::Initialize()
{
	//if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
	//	return E_FAIL;	

	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
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

HRESULT CLevel_Tool::Ready_Layer_BackGround(const _wstring& strLayerTag)
{
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
