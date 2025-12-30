#include "pch.h"
#include "UI_Level_Loading.h"

#include "UI_Loader.h"
#include "GameInstance.h"
#include "GameManager.h"

#include "UI_Level_Logo.h"
#include "UI_Level_GamePlay.h"

#include "UIHUD.h"
#include "UIPanel.h"
#include "UIWrapper.h"
#include "UIImage.h"
#include "UIText.h"
#include "UILoadingBlur.h"
#include "UILoadingBlock.h"
#include "UIScript.h"

CUI_Level_Loading::CUI_Level_Loading(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevelID)
	: CLevel { pDevice, pContext, ENUM_CLASS(eLevelID)}
{

}

HRESULT CUI_Level_Loading::Initialize(LEVEL eNextLevelID)
{
	m_eNextLevelID = eNextLevelID;

	/* 다음 레벨에 대한 자원을 로드하여 준비해둔다. */
	m_pLoader = CUI_Loader::Create(m_pDevice, m_pContext, eNextLevelID);
	if (nullptr == m_pLoader)
		return E_FAIL;

	/* 이 레벨을 구성하기위한 객체를 만든다. */
	if (FAILED(Ready_Prototypes()))
		return E_FAIL;
	if (FAILED(Ready_Layer_BackGround()))
		return E_FAIL;

	return S_OK;
}

void CUI_Level_Loading::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

	if (true == m_pLoader->isFinished()
		&& !m_bLevelTransitioning
		&& GetKeyState(VK_F1) & 0x8000)
	{
		dynamic_cast<CUIHUD*>(m_pHUD)->Anim_Play(TEXT("Layer_Loading"), TEXT("Loading_Overlay"), TEXT("Outro"));

		dynamic_cast<CUIHUD*>(m_pHUD)->Anim_Stop(TEXT("Layer_Loading"), TEXT("LoadingBlur"));
		/*dynamic_cast<CUIHUD*>(m_pHUD)->Anim_Stop(TEXT("Layer_Loading"), TEXT("Loading_Block_0"));
		dynamic_cast<CUIHUD*>(m_pHUD)->Anim_Stop(TEXT("Layer_Loading"), TEXT("Loading_Block_1"));
		dynamic_cast<CUIHUD*>(m_pHUD)->Anim_Stop(TEXT("Layer_Loading"), TEXT("Loading_Block_2"));*/
		
		/*CUIBase* pUI = dynamic_cast<CUIHUD*>(m_pHUD)->Get_UIObject(TEXT("Layer_Loading"), TEXT("LoadingBlocks"));

		if (!pUI)
			return;

		pUI->SetVisibility(VISIBILITY::HIDDEN);

		auto Children = pUI->Get_Children();

		for (auto& pChild : *Children)
			dynamic_cast<CUIHUD*>(m_pHUD)->Get_UIObject(TEXT("Layer_Loading"), TEXT("LoadingBlocks"))->Update_Children(pChild);*/

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
			pNewLevel = CUI_Level_Logo::Create(m_pDevice, m_pContext, m_eNextLevelID);
			break;
		case LEVEL::GAMEPLAY:
			pNewLevel = CUI_Level_GamePlay::Create(m_pDevice, m_pContext, m_eNextLevelID);
			break;
		}

		if (FAILED(m_pGameInstance->Change_Level(pNewLevel)))
			return;
	}

}

HRESULT CUI_Level_Loading::Render()
{
	m_pLoader->Output();

	return S_OK;
}

HRESULT CUI_Level_Loading::Ready_Prototypes()
{
	/* For.Prototype_Component_UI_Texture_StellaSymbol */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::LOADING), TEXT("Prototype_Component_UI_Texture_StellaSymbol"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/UI/BackGround/StellaSymbol.dds"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_UI_Texture_LoadingBG */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::LOADING), TEXT("Prototype_Component_UI_Texture_LoadingBG"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/UI/BackGround/Loading_BG_%d.dds"), 3))))
		return E_FAIL;

	/* For.Prototype_Component_UI_Texture_LoadingBlock */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::LOADING), TEXT("Prototype_Component_UI_Texture_LoadingBlock"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/UI/Loading/Loading_Block.dds"), 1))))
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

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::LOADING), TEXT("Prototype_GameObject_UI_LoadingBlur"),
		CUILoadingBlur::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::LOADING), TEXT("Prototype_GameObject_UI_Loading_Block"),
		CUILoadingBlock::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::LOADING), TEXT("Prototype_GameObject_UI_Script"),
		CUIScript::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Level_Loading::Ready_Layer_BackGround()
{
	CUIHUD* pUIHUD = CUIHUD::Create(m_pDevice, m_pContext);

	if (pUIHUD == nullptr)
		return E_FAIL;

	SetHUD(pUIHUD);

	if (FAILED(pUIHUD->Load_Data(TEXT("Layer_Loading"))))
		return E_FAIL;

	CUIScript* pScript = dynamic_cast<CUIScript*>(pUIHUD->Get_UIObject(TEXT("Layer_Loading"), TEXT("UI_Scripts")));

	if (!pScript)
		return E_FAIL;

	auto pGameManager = CGameManager::GetInstance();

	if (!pGameManager)
		return E_FAIL;

	pScript->Begin_Script(pGameManager->Get_ScriptData(TEXT("Loading")));
	Safe_Release(pGameManager);

	pUIHUD->Anim_Play(TEXT("Layer_Loading"), TEXT("Loading_Overlay"), TEXT("Intro"));

	pUIHUD->Anim_Play(TEXT("Layer_Loading"), TEXT("LoadingBlur"), TEXT("Loading_FX"));

	/*pUIHUD->Anim_Play(TEXT("Layer_Loading"), TEXT("Loading_Block_0"), TEXT("Loading_BeapBeap"));
	pUIHUD->Anim_Play(TEXT("Layer_Loading"), TEXT("Loading_Block_1"), TEXT("Loading_BeapBeap"), 0.3f);
	pUIHUD->Anim_Play(TEXT("Layer_Loading"), TEXT("Loading_Block_2"), TEXT("Loading_BeapBeap"), 0.6f);*/

	return S_OK;
}

CUI_Level_Loading* CUI_Level_Loading::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevelID, LEVEL eNextLevelID)
{
	CUI_Level_Loading* pInstance = new CUI_Level_Loading(pDevice, pContext, eLevelID);

	if (FAILED(pInstance->Initialize(eNextLevelID)))
	{
		MSG_BOX("Failed to Created : CUI_Level_Loading");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Level_Loading::Free()
{
	__super::Free();

	Safe_Release(m_pLoader);
}
