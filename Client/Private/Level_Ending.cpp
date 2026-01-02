#include "pch.h"
#include "Level_Ending.h"

#include "GameInstance.h"
#include "Level_Loading.h"
#include "GameManager.h"

#include "UIHUD.h"
#include "HUDLayer.h"
#include "GameObject.h"
#include "ChangeLevelEvent.h"
#include "UIStruct.h"

#ifdef _DEBUG
#include "ImGuiManager.h"
#endif // _DEBUG

CLevel_Ending::CLevel_Ending(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevelID) :
    CLevel(pDevice, pContext, ENUM_CLASS(LEVEL::ENDING))
{
}

HRESULT CLevel_Ending::Initialize()
{
	m_pGameManager = CGameManager::GetInstance();

	if (!m_pGameManager)
		return E_FAIL;

	//if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
	//	return E_FAIL;	

	//if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
	//	return E_FAIL;

	if (FAILED(Ready_Layer_UI(TEXT("Layer_UI"))))
		return E_FAIL;

	m_pGameInstance->Manager_PlayBGM(TEXT("BGM_EVETrailer.wav"), 1.f);

	auto pGameManager = CGameManager::GetInstance();
	pGameManager->Setting_PoolManager(ENUM_CLASS(LEVEL::LOGO));

#ifdef _DEBUG
	CImGuiManager::GetInstance()->SetLevelFreeCamera();
#endif // _DEBUG

	return S_OK;
}

void CLevel_Ending::Update(_float fTimeDelta)
{
}

HRESULT CLevel_Ending::Render()
{
	SetWindowText(g_hWnd, TEXT("¿£µù"));
    return S_OK;
}

HRESULT CLevel_Ending::Ready_Layer_Camera(const _wstring& strLayerTag)
{
    return S_OK;
}

HRESULT CLevel_Ending::Ready_Layer_BackGround(const _wstring& strLayerTag)
{
    return S_OK;
}

HRESULT CLevel_Ending::Ready_Layer_UI(const _wstring& strLayerTag)
{
    return S_OK;
}

CLevel_Ending* CLevel_Ending::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevelID)
{
	CLevel_Ending* pInstance = new CLevel_Ending(pDevice, pContext, eLevelID);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : Level_Ending");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Ending::Free()
{
	__super::Free();
}
