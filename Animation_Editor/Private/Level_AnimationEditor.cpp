#include "pch.h"
#include "Level_AnimationEditor.h"

#include "GameInstance.h"

#include "Camera_AnimationEditor.h"

CLevel_AnimationEditor::CLevel_AnimationEditor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevelID)
	: CLevel{ pDevice, pContext, ENUM_CLASS(eLevelID) }

{

}

HRESULT CLevel_AnimationEditor::Initialize()
{
	if (FAILED(Ready_Lights()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Player(TEXT("Layer_Player"))))
		return E_FAIL;

	//if (FAILED(Ready_Layer_Monster(TEXT("Layer_Monster"))))
	//	return E_FAIL;

	//if (FAILED(Ready_Layer_Effect(TEXT("Layer_Effect"))))
	//	return E_FAIL;

	return S_OK;
}

void CLevel_AnimationEditor::Update(_float fTimeDelta)
{
}

HRESULT CLevel_AnimationEditor::Render()
{
	SetWindowText(g_hWnd, TEXT("게임플레이레벨이빈다"));
	return S_OK;
}

HRESULT CLevel_AnimationEditor::Ready_Lights()
{
	LIGHT_DESC			LightDesc{};

	LightDesc.eType = LIGHT_TYPE::DIRECTIONAL;
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(0.5f, 0.5f, 0.5f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);

	if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
		return E_FAIL;

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

HRESULT CLevel_AnimationEditor::Ready_Layer_BackGround(const _wstring& strLayerTag)
{
	//if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::EDITOR), TEXT("Prototype_GameObject_Terrain"),
	//	ENUM_CLASS(LEVEL::EDITOR), strLayerTag)))
	//	return E_FAIL;
	//
	//for (size_t i = 0; i < 10; i++)
	//{
	//	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::EDITOR), TEXT("Prototype_GameObject_ForkLift"),
	//		ENUM_CLASS(LEVEL::EDITOR), strLayerTag)))
	//		return E_FAIL;
	//
	//}
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::EDITOR), TEXT("Prototype_GameObject_Sky"),
		ENUM_CLASS(LEVEL::EDITOR), strLayerTag)))
		return E_FAIL;
	
	return S_OK;

}

HRESULT CLevel_AnimationEditor::Ready_Layer_Camera(const _wstring& strLayerTag)
{
	CCamera_AnimationEditor::CAMERA_DESC			CameraDesc{};
	CameraDesc.fFovy = XMConvertToRadians(60.0f);
	CameraDesc.fNear = 0.1f;
	CameraDesc.fFar = 500.f;
	CameraDesc.vEye = _float3(0.f, 10.f, -10.f);
	CameraDesc.vAt = _float3(0.f, 0.f, 0.f);
	CameraDesc.fSpeedPerSec = 5.f;
	CameraDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	CameraDesc.fMouseSensor = 0.1f;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Camera_AnimationEditor"),
		ENUM_CLASS(LEVEL::EDITOR), strLayerTag, &CameraDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_AnimationEditor::Ready_Layer_Player(const _wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::EDITOR), TEXT("Prototype_GameObject_Character"),
		ENUM_CLASS(LEVEL::EDITOR), strLayerTag)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_AnimationEditor::Ready_Layer_Monster(const _wstring& strLayerTag)
{
	//for (size_t i = 0; i < 5; i++)
	//{
	//	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::EDITOR), TEXT("Prototype_GameObject_Monster"),
	//		ENUM_CLASS(LEVEL::EDITOR), strLayerTag)))
	//		return E_FAIL;
	//}


	return S_OK;
}

HRESULT CLevel_AnimationEditor::Ready_Layer_Effect(const _wstring& strLayerTag)
{

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::EDITOR), TEXT("Prototype_GameObject_Snow"),
		ENUM_CLASS(LEVEL::EDITOR), strLayerTag)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::EDITOR), TEXT("Prototype_GameObject_Explosion"),
		ENUM_CLASS(LEVEL::EDITOR), strLayerTag)))
		return E_FAIL;

	for (size_t i = 0; i < 50; i++)
	{
		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::EDITOR), TEXT("Prototype_GameObject_Sprite_Explosion"),
			ENUM_CLASS(LEVEL::EDITOR), strLayerTag)))
			return E_FAIL;
	}


	return S_OK;
}


CLevel_AnimationEditor* CLevel_AnimationEditor::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevelID)
{
	CLevel_AnimationEditor* pInstance = new CLevel_AnimationEditor(pDevice, pContext, eLevelID);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_AnimationEditor");
		Safe_Release(pInstance);
	}

	return pInstance;
}





void CLevel_AnimationEditor::Free()
{
	__super::Free();


}
