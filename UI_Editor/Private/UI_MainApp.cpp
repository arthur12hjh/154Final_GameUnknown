#include "pch.h"

#include "UI_MainApp.h"

#include "GameInstance.h"
#include "UI_Level_Loading.h"

#include "GUIManager.h"

/********************************
* ´ëÀçÈÆÀÇ ÀºÃÑ ÀÌ ¾ó¸¶³ª °ü´ëÇÑ°¡ *
* ´ë¹Î¼®ÀÇ ÀºÃÑ ÀÌ ¾ó¸¶³ª Âù¶õÇÑ°¡ *
********************************/

CUI_MainApp::CUI_MainApp()	
	: m_pGameInstance { CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CUI_MainApp::Initialize()
{
	ENGINE_DESC				EngineDesc{};
	EngineDesc.hInstance = g_hInstance;
	EngineDesc.hWnd = g_hWnd;
	EngineDesc.eWindowMode = WINMODE::WIN;
	EngineDesc.iWinSizeX = g_iWinSizeX;
	EngineDesc.iWinSizeY = g_iWinSizeY;
	EngineDesc.iNumLevels = ENUM_CLASS(LEVEL::END);

	if (FAILED(m_pGameInstance->Initialize_Engine(EngineDesc, &m_pDevice, &m_pContext)))
		return E_FAIL;
	
	m_pGuiManager = CGUIManager::GetInstance();
	m_pGuiManager->Initialize(m_pDevice, m_pContext);

	if (nullptr == m_pGuiManager)
		return E_FAIL;

	if (FAILED(Ready_Default_Setting()))
		return E_FAIL;

	if (FAILED(Ready_Prototypes()))
		return E_FAIL;

	if (FAILED(Start_Level(LEVEL::LOGO)))
		return E_FAIL;

	return S_OK;
}

void CUI_MainApp::Update(_float fTimeDelta)
{
	m_pGameInstance->Update_Engine(fTimeDelta);

	m_pGuiManager->Update(fTimeDelta);
}

HRESULT CUI_MainApp::Render()
{
	_float4			vClearColor = _float4(0.02f, 0.02f, 0.02f, 1.f);
	
	m_pGameInstance->Render_Begin(&vClearColor);

	m_pGameInstance->Draw();

	m_pGuiManager->Render();

	m_pGameInstance->Render_End();

	return S_OK;
}

HRESULT CUI_MainApp::Ready_Default_Setting()
{
	/* [Âü°í] MakeSpriteFont "³Ø½¼Lv1°íµñ Bold" /FontSize:20 /FastPack /CharacterRegion:0x0020-0x00FF /CharacterRegion:0x3131-0x3163 /CharacterRegion:0xAC00-0xD800 /DefaultCharacter:0xAC00 154ex.spritefont */
	/*if (FAILED(m_pGameInstance->Add_Font(TEXT("Spoqa_16"), TEXT("../Bin/Resources/Fonts/Spoqa_Han_Sans_Neo_Medium16.spritefont"))))
		return E_FAIL;*/

	return S_OK;
}

HRESULT CUI_MainApp::Start_Level(LEVEL eLevelID)
{
	if (FAILED(m_pGameInstance->Change_Level(CUI_Level_Loading::Create(m_pDevice, m_pContext, LEVEL::LOADING, eLevelID))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_MainApp::Ready_Prototypes()
{
	/* For.Prototype_Component_VIBuffer_Rect */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxPosTex */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/ShaderFiles/Shader_VtxPosTex.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
		return E_FAIL;

	return S_OK;
}

CUI_MainApp* CUI_MainApp::Create()
{
	CUI_MainApp* pInstance = new CUI_MainApp();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CUI_MainApp");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_MainApp::Free()
{
	__super::Free();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	m_pGuiManager->Release_GUI_Manager();
	Safe_Release(m_pGuiManager);

	m_pGameInstance->Release_Engine();
	Safe_Release(m_pGameInstance);	
}
