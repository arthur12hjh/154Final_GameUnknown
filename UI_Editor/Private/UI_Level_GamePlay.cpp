#include "pch.h"
#include "UI_Level_GamePlay.h"

#include "Actor.h"
#include "UI_Camera.h"

#include "GameInstance.h"
#include "UIHUD.h"

#include "GameManager.h"

CUI_Level_GamePlay::CUI_Level_GamePlay(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevelID)
	: CLevel { pDevice, pContext, ENUM_CLASS(eLevelID)}
{
}

HRESULT CUI_Level_GamePlay::Initialize()
{
	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

	if (FAILED(Ready_UI(TEXT("Layer_UI"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_BackGround(TEXT("BackGround"))))
		return E_FAIL;

	/*auto pGameCharacter = m_pGameInstance->GetMainCamera();
	m_pGameInstance->SetInteractionBaseObject(pGameCharacter);
	Safe_Release(pGameCharacter);*/

	return S_OK;
}

void CUI_Level_GamePlay::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

	if (m_isOverlay && m_pHUD)
	{
		static_cast<CUIHUD*>(m_pHUD)->Anim_Play(TEXT("Layer_Combat"), TEXT("GamePlay_Overlay"), TEXT("Intro"));
		m_isOverlay = false;
	}
}

HRESULT CUI_Level_GamePlay::Render()
{
	SetWindowText(g_hWnd, TEXT("게임플레이레벨이빈다"));
	return S_OK;
}

HRESULT CUI_Level_GamePlay::Ready_Layer_Camera(const _wstring& strLayerTag)
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

HRESULT CUI_Level_GamePlay::Ready_UI(const _wstring& strLayerTag)
{
	CUIHUD* pUIHUD = CUIHUD::Create(m_pDevice, m_pContext);

	if (pUIHUD == nullptr)
		return E_FAIL;

	SetHUD(pUIHUD);

	if (FAILED(pUIHUD->Load_Data(TEXT("Layer_Combat"))))
		return E_FAIL;

	pUIHUD->Anim_Play(TEXT("Layer_Combat"), TEXT("Hp_Fx"), TEXT("Hp_Fx_BeapBeap"));

	if (FAILED(pUIHUD->Load_Data(TEXT("Layer_Boss"))))
		return E_FAIL;

	pUIHUD->Anim_Play(TEXT("Layer_Boss"), TEXT("Boss_Hp_Fx"), TEXT("Hp_Fx_BeapBeap"));

	if (FAILED(pUIHUD->Load_Data(TEXT("Layer_World"))))
		return E_FAIL;

	pUIHUD->Anim_Play(TEXT("Layer_World"), TEXT("MonsterHp_Fx"), TEXT("Hp_Fx_BeapBeap"));
	
	if (FAILED(pUIHUD->Load_Data(TEXT("Layer_Combat_Info"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Level_GamePlay::Ready_Layer_BackGround(const _wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Sky"),
		ENUM_CLASS(LEVEL::GAMEPLAY), strLayerTag)))
		return E_FAIL;

	//CActor::ACTOR_DESC ProbDesc = {};
	//ProbDesc.bIsApplyTransform = true;
	//ProbDesc.vScale = { 1.f, 1.f, 1.f };
	////ProbDesc.vPosition = { 0.f, 0.f, 0.f };

	//ProbDesc.szVIBuffer_PrototypeName = TEXT("Prototype_Component_Model_Prob_CanBox");

	///*if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Prob_CanBox"),
	//	ENUM_CLASS(LEVEL::GAMEPLAY), strLayerTag, &ProbDesc)))
	//	return E_FAIL;*/

	//for (size_t i = 0; i < 5; i++)
	//{
	//	ProbDesc.vPosition = { 10.f * i,
	//						   m_pGameInstance->Random(0, 50),
	//						   m_pGameInstance->Random(0, 50) };

	//	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Prob_CanBox"),
	//		ENUM_CLASS(LEVEL::GAMEPLAY), strLayerTag, &ProbDesc)))
	//		return E_FAIL;
	//}

	return S_OK;
}


CUI_Level_GamePlay* CUI_Level_GamePlay::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevelID)
{
	CUI_Level_GamePlay* pInstance = new CUI_Level_GamePlay(pDevice, pContext, eLevelID);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CUI_Level_GamePlay");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Level_GamePlay::Free()
{
	__super::Free();
}
