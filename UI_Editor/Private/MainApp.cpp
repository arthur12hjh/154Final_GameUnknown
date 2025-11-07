#include "pch.h"

#include "MainApp.h"

#include "GameInstance.h"

#include "GUIManager.h"

/********************************
* ´ëÀçÈÆÀÇ ÀºÃÑ ÀÌ ¾ó¸¶³ª °ü´ëÇÑ°¡ *
* ´ë¹Î¼®ÀÇ ÀºÃÑ ÀÌ ¾ó¸¶³ª Âù¶õÇÑ°¡ *
********************************/

CMainApp::CMainApp()	
	: m_pGameInstance { CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CMainApp::Initialize()
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

	return S_OK;
}

void CMainApp::Update(_float fTimeDelta)
{
	m_pGameInstance->Update_Engine(fTimeDelta);

	m_pGuiManager->Update(fTimeDelta);
}

HRESULT CMainApp::Render()
{
	_float4			vClearColor = _float4(0.02f, 0.02f, 0.02f, 1.f);
	
	m_pGameInstance->Render_Begin(&vClearColor);

	m_pGameInstance->Draw();

	m_pGuiManager->Render();

	m_pGameInstance->Render_End();

	return S_OK;
}

HRESULT CMainApp::Ready_Default_Setting()
{
	/* [Âü°í] MakeSpriteFont "³Ø½¼Lv1°íµñ Bold" /FontSize:20 /FastPack /CharacterRegion:0x0020-0x00FF /CharacterRegion:0x3131-0x3163 /CharacterRegion:0xAC00-0xD800 /DefaultCharacter:0xAC00 154ex.spritefont */
	/*if (FAILED(m_pGameInstance->Add_Font(TEXT("Spoqa_16"), TEXT("../Bin/Resources/Fonts/Spoqa_Han_Sans_Neo_Medium16.spritefont"))))
		return E_FAIL;*/

	return S_OK;
}

HRESULT CMainApp::Ready_Prototypes()
{
	/* For.Prototype_Component_Texture_BackGround */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_BackGround"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Backgrounds/Background_%d.png"), 2))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Rect */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxPosTex */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPosTex.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
		return E_FAIL;

	return S_OK;
}

CMainApp* CMainApp::Create()
{
	CMainApp* pInstance = new CMainApp();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CMainApp");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMainApp::Free()
{
	__super::Free();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	m_pGuiManager->Release_GUI_Manager();
	Safe_Release(m_pGuiManager);

	m_pGameInstance->Release_Engine();
	Safe_Release(m_pGameInstance);	
}
