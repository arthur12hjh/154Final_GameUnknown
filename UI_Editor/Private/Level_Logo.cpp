#include "pch.h"
#include "Level_Logo.h"

#include "GameInstance.h"
#include "Level_Loading.h"

#include "UIHUD.h"

#include "GUIManager.h"

CLevel_Logo::CLevel_Logo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevelID)
	: CLevel { pDevice, pContext, ENUM_CLASS(eLevelID)}
	
{

}

HRESULT CLevel_Logo::Initialize()
{
	m_pGuiManager = CGUIManager::GetInstance();

	if (FAILED(Ready_Layer_UI(TEXT("Layer_UI"))))
		return E_FAIL;

	//m_pGameInstance->Manager_PlayBGM(TEXT("BGM_TrainingRoom_01_A.OGG"), 1.f);

	m_pGuiManager->Set_Current_HUD(dynamic_cast<CUIHUD*>(GetHUD()));

	return S_OK;
}

void CLevel_Logo::Update(_float fTimeDelta)
{
	if (GetKeyState(VK_SPACE) & 0x8000)
	{
		if (FAILED(m_pGameInstance->Change_Level(CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL::LOADING, LEVEL::GAMEPLAY))))
			return;
	}

	__super::Update(fTimeDelta);
}

HRESULT CLevel_Logo::Render()
{
	SetWindowText(g_hWnd, TEXT("로고레벨이빈다"));

	return S_OK;
}

HRESULT CLevel_Logo::Ready_Layer_UI(const _wstring& strLayerTag)
{
	CUIHUD* pUIHUD = CUIHUD::Create(m_pDevice, m_pContext);

	SetHUD(pUIHUD);

	if (pUIHUD == nullptr)
		return E_FAIL;

	if (FAILED(pUIHUD->Add_UserInterface(ENUM_CLASS(LEVEL::LOGO), TEXT("Prototype_GameObject_UI_Button"), TEXT("UIHUDLayer_Logo"), TEXT("UI_Button"))))
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
