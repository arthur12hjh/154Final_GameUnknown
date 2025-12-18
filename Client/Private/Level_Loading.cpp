#include "pch.h"
#include "Level_Loading.h"

#include "Loader.h"
#include "GameInstance.h"
#include "GameManager.h"

#include "Level_Logo.h"
#include "Level_GamePlay.h"
#include "Level_Scarlet.h"

#include "UIHUD.h"
#include "UIPanel.h"
#include "UIWrapper.h"
#include "UIImage.h"
#include "UIText.h"
#include "UIScript.h"
#include "UILoadingBlur.h"

#ifdef _DEBUG
#include "ImGuiManager.h"
#endif // _DEBUG

CLevel_Loading::CLevel_Loading(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevelID)
	: CLevel { pDevice, pContext, ENUM_CLASS(eLevelID)}
{

}

HRESULT CLevel_Loading::Initialize(LEVEL eNextLevelID, _bool bIsResetProtoTypes)
{
	m_eNextLevelID = eNextLevelID;
	m_bIsProtoTypes = bIsResetProtoTypes;

	/* 다음 레벨에 대한 자원을 로드하여 준비해둔다. */
	m_pLoader = CLoader::Create(m_pDevice, m_pContext, eNextLevelID);
	if (nullptr == m_pLoader)
		return E_FAIL;

	m_pGameInstance->Clear_Resources(ENUM_CLASS(LEVEL::LEVEL_PROB), true);
	m_pGameInstance->Clear_LevelResource(bIsResetProtoTypes);

	/* 이 레벨을 구성하기위한 객체를 만든다. */
	if (FAILED(Ready_Prototypes()))
		return E_FAIL;
	if (FAILED(Ready_Layer_BackGround()))
		return E_FAIL;

	return S_OK;
}

void CLevel_Loading::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

	if (true == m_pLoader->isFinished()
		&& !m_bLevelTransitioning
		&& (LEVEL::LOGO == m_eNextLevelID || m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_F1)))
	{
		dynamic_cast<CUIHUD*>(m_pHUD)->Anim_Play(TEXT("Layer_Loading"), TEXT("Loading_Overlay"), TEXT("Outro"));

		dynamic_cast<CUIHUD*>(m_pHUD)->Anim_Stop(TEXT("Layer_Loading"), TEXT("LoadingBlur"));
		dynamic_cast<CUIHUD*>(m_pHUD)->Anim_Stop(TEXT("Layer_Loading"), TEXT("Loading_Block_0"));
		dynamic_cast<CUIHUD*>(m_pHUD)->Anim_Stop(TEXT("Layer_Loading"), TEXT("Loading_Block_1"));
		dynamic_cast<CUIHUD*>(m_pHUD)->Anim_Stop(TEXT("Layer_Loading"), TEXT("Loading_Block_2"));

		CUIBase* pUI = dynamic_cast<CUIHUD*>(m_pHUD)->Get_UIObject(TEXT("Layer_Loading"), TEXT("LoadingBlocks"));

		if (!pUI)
			return;

		pUI->SetVisibility(VISIBILITY::HIDDEN);

		auto Children = pUI->Get_Children();

		for (auto& pChild : *Children)
			dynamic_cast<CUIHUD*>(m_pHUD)->Get_UIObject(TEXT("Layer_Loading"), TEXT("LoadingBlocks"))->Update_Children(pChild);

		m_bLevelTransitioning = true;
	}

	if (m_bLevelTransitioning
		&& dynamic_cast<CUIHUD*>(m_pHUD)->Check_AnimFinish(TEXT("Layer_Loading"), TEXT("Loading_Overlay"), TEXT("Outro")))
	{
		m_pGameInstance->Clear_LevelResource();

		CLevel* pNewLevel = { nullptr };
		switch (m_eNextLevelID)
		{
		case LEVEL::LOGO:
			pNewLevel = CLevel_Logo::Create(m_pDevice, m_pContext, m_eNextLevelID);
			break;
		case LEVEL::GAMEPLAY:
			pNewLevel = CLevel_GamePlay::Create(m_pDevice, m_pContext, m_eNextLevelID);
			break;
		case LEVEL::SCARLET:
			pNewLevel = CLevel_Scarlet::Create(m_pDevice, m_pContext, m_eNextLevelID);
			break;
		}
		if (FAILED(m_pGameInstance->Change_Level(pNewLevel)))
			return;
	}
}

HRESULT CLevel_Loading::Render()
{
	m_pLoader->Output();

	return S_OK;
}

HRESULT CLevel_Loading::Ready_Prototypes()
{
	/* For.Prototype_Component_UI_Texture_StellaSymbol */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::LOADING), TEXT("Prototype_Component_UI_Texture_StellaSymbol"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/UI/BackGround/StellaSymbol.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_UI_Texture_LoadingBG */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::LOADING), TEXT("Prototype_Component_UI_Texture_LoadingBG"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/UI/BackGround/Loading_BG_%d.png"), 3))))
		return E_FAIL;

	/* For.Prototype_Component_UI_Texture_LoadingBlock */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::LOADING), TEXT("Prototype_Component_UI_Texture_LoadingBlock"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/UI/Loading/Loading_Block.png"), 1))))
		return E_FAIL;

	/*==============================================================================*/

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::LOADING), TEXT("Prototype_GameObject_UI_Panel"),
		CUIPanel::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::LOADING), TEXT("Prototype_GameObject_UI_Wrapper"),
		CUIWrapper::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::LOADING), TEXT("Prototype_GameObject_UI_Text"),
		CUIText::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::LOADING), TEXT("Prototype_GameObject_UI_Image"),
		CUIImage::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::LOADING), TEXT("Prototype_GameObject_UI_Script"),
		CUIScript::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::LOADING), TEXT("Prototype_GameObject_UI_LoadingBlur"),
		CUILoadingBlur::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	return S_OK;
}

HRESULT CLevel_Loading::Ready_Layer_BackGround()
{
	CUIHUD* pUIHUD = CUIHUD::Create(m_pDevice, m_pContext);

	if (pUIHUD == nullptr)
		return E_FAIL;

	SetHUD(pUIHUD);

	if (FAILED(pUIHUD->Load_Data(TEXT("Layer_Loading"))))
		return E_FAIL;

	pUIHUD->Set_Show_Debug_Rect(false);

	CUIScript* pScript = dynamic_cast<CUIScript*>(pUIHUD->Get_UIObject(TEXT("Layer_Loading"), TEXT("UI_Scripts")));
	
	if (!pScript)
		return E_FAIL;

	auto pGameManager = CGameManager::GetInstance();

	if (!pGameManager)
		return E_FAIL;

	pScript->Set_Script(*pGameManager->Get_ScriptData(TEXT("Loading")));
	Safe_Release(pGameManager);

	pUIHUD->Anim_Play(TEXT("Layer_Loading"), TEXT("Loading_Overlay"), TEXT("Intro"));

	pUIHUD->Anim_Play(TEXT("Layer_Loading"), TEXT("LoadingBlur"), TEXT("Loading_FX"));

	pUIHUD->Anim_Play(TEXT("Layer_Loading"), TEXT("Loading_Block_0"), TEXT("Loading_BeapBeap"));
	pUIHUD->Anim_Play(TEXT("Layer_Loading"), TEXT("Loading_Block_1"), TEXT("Loading_BeapBeap"), 0.3f);
	pUIHUD->Anim_Play(TEXT("Layer_Loading"), TEXT("Loading_Block_2"), TEXT("Loading_BeapBeap"), 0.6f);

	return S_OK;
}

CLevel_Loading* CLevel_Loading::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevelID, LEVEL eNextLevelID, _bool bIsResetProtoTypes)
{
	CLevel_Loading* pInstance = new CLevel_Loading(pDevice, pContext, eLevelID);

	if (FAILED(pInstance->Initialize(eNextLevelID, bIsResetProtoTypes)))
	{
		MSG_BOX("Failed to Created : CLevel_Loading");
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CLevel_Loading::Free()
{
	__super::Free();

	Safe_Release(m_pLoader);
}
