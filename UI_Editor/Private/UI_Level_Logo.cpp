#include "pch.h"
#include "UI_Level_Logo.h"

#include "GameInstance.h"
#include "UI_Level_Loading.h"

#include "UIHUD.h"

#include "GUIManager.h"

CUI_Level_Logo::CUI_Level_Logo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevelID)
	: CLevel { pDevice, pContext, ENUM_CLASS(eLevelID)}
	
{

}

HRESULT CUI_Level_Logo::Initialize()
{
	m_pGuiManager = CGUIManager::GetInstance();

	if (FAILED(Ready_Layer_UI(TEXT("Layer_UI"))))
		return E_FAIL;

	//if (FAILED(Ready_Textures()))
	//	return E_FAIL;

	//m_pGameInstance->Manager_PlayBGM(TEXT("BGM_TrainingRoom_01_A.OGG"), 1.f);

	return S_OK;
}

void CUI_Level_Logo::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

	if (GetKeyState(VK_F3) & 0x8000)
	{
		if (FAILED(m_pGameInstance->Change_Level(CUI_Level_Loading::Create(m_pDevice, m_pContext, LEVEL::LOADING, LEVEL::GAMEPLAY))))
			return;
	}
}

HRESULT CUI_Level_Logo::Render()
{
	SetWindowText(g_hWnd, TEXT("로고레벨이빈다"));

	return S_OK;
}

HRESULT CUI_Level_Logo::Ready_Layer_UI(const _wstring& strLayerTag)
{
	CUIHUD* pUIHUD = CUIHUD::Create(m_pDevice, m_pContext);

	if (pUIHUD == nullptr)
		return E_FAIL;

	SetHUD(pUIHUD);

	/*if (FAILED(pUIHUD->Add_UserInterface(ENUM_CLASS(LEVEL::LOGO), TEXT("Prototype_GameObject_UI_Wrapper"), TEXT("UIHUDLayer_Logo"), TEXT("UI_Logo_Wrapper"))))
		return E_FAIL;*/

	/*if(FAILED(pUIHUD->Load_Data(TEXT("Test"))))
		return E_FAIL;*/
	
	if(FAILED(pUIHUD->Load_Data(TEXT("Test2"))))
		return E_FAIL;

	return S_OK;
}

//HRESULT CUI_Level_Logo::Ready_Textures()
//{
//	CUIHUD* pUIHUD = dynamic_cast<Client::CUIHUD*>(m_pGameInstance->GetCurrentLevelHUD());
//
//	_wstring prefix = TEXT("Prototype_Component_UI_Texture_");
//	for (auto& pTexture : *m_pGameInstance->Get_Prototypes_InLevel(ENUM_CLASS(LEVEL::STATIC)))
//	{
//		if (pTexture.first.find(prefix) == 0)
//		{
//			size_t pos = pTexture.first.find(prefix);
//			_wstring szTextureTag = pTexture.first.substr(pos + prefix.length());
//
//			if(FAILED(pUIHUD->Add_Texture(ENUM_CLASS(LEVEL::STATIC), pTexture.first, szTextureTag)))
//				return E_FAIL;
//		}
//	}
//	for (auto& pTexture : *m_pGameInstance->Get_Prototypes_InLevel(ENUM_CLASS(LEVEL::LOGO)))
//	{
//		if (pTexture.first.find(prefix) == 0)
//		{
//			size_t pos = pTexture.first.find(prefix);
//			_wstring szTextureTag = pTexture.first.substr(pos + prefix.length());
//
//			if (FAILED(pUIHUD->Add_Texture(ENUM_CLASS(LEVEL::LOGO), pTexture.first, szTextureTag)))
//				return E_FAIL;
//		}
//	}
//
//	return S_OK;
//}

CUI_Level_Logo* CUI_Level_Logo::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevelID)
{
	CUI_Level_Logo* pInstance = new CUI_Level_Logo(pDevice, pContext, eLevelID);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CUI_Level_Logo");
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CUI_Level_Logo::Free()
{
	__super::Free();


}
