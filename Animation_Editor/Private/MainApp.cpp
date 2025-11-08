#include "pch.h"
#include "MainApp.h"

#include "GameInstance.h"
#include "Tool_Manager.h"

#include "Level_Loading.h"

#include "Camera_AnimationEditor.h"

Animation_Editor::CMainApp::CMainApp()
	: m_pGameInstance{ CGameInstance::GetInstance() }
	, m_pTool_Manager{ CTool_Manager::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pTool_Manager);
}

HRESULT Animation_Editor::CMainApp::Initialize()
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

	if (FAILED(Ready_Default_Setting()))
		return E_FAIL;

	if (FAILED(Ready_Prototypes()))
		return E_FAIL;

	if (FAILED(Start_Level(LEVEL::EDITOR)))
		return E_FAIL;

	m_pTool_Manager->Initialize(m_pDevice, m_pContext);

	srand(time(NULL));

	m_isDebuggingActive = FALSE;

	return S_OK;
}

void Animation_Editor::CMainApp::Update(_float fTimeDelta)
{
	//fTimeDelta = fTimeDelta * 0.5f;
	m_pTool_Manager->Priority_Update(fTimeDelta);
	m_pGameInstance->Update_Engine(fTimeDelta);
	m_pTool_Manager->Update(fTimeDelta);
	m_pTool_Manager->Late_Update(fTimeDelta);
}

HRESULT Animation_Editor::CMainApp::Render()
{
	_float4			vClearColor = _float4(1.f, 0.f, 1.f, 1.f);

	m_pGameInstance->Render_Begin(&vClearColor);

	m_pGameInstance->Draw();

	m_pTool_Manager->Render();

	//m_pGameInstance->Render_Text(TEXT("Font_GyeongGiThousandBold"), TEXT("SevenKnights Rebirth"), _float2(g_iWinSizeX/2 - 180, 0), XMVectorSet(1.f, 1.f, 1.f, 0.1f));

	m_pGameInstance->Render_End();

	return S_OK;
}

HRESULT CMainApp::Ready_Default_Setting()
{
	/*MakeSpriteFont "³Ø½¼Lv1°íµñ Bold" /FontSize:20 /FastPack /CharacterRegion:0x0020-0x00FF /CharacterRegion:0x3131-0x3163 /CharacterRegion:0xAC00-0xD800 /DefaultCharacter:0xAC00 154ex.spritefont */
	//if (FAILED(m_pGameInstance->Add_Font(TEXT("Font_GyeongGiThousandBold"), TEXT("../Bin/Resources/Fonts/GyeongGiThousandBold.spritefont"))))
	//	return E_FAIL;
	//
	//if (FAILED(m_pGameInstance->Add_Font(TEXT("Font_GyeongGiThousandBoldLarge"), TEXT("../Bin/Resources/Fonts/GyeongGiThousandBoldLarge.spritefont"))))
	//	return E_FAIL;
	//
	//if (FAILED(m_pGameInstance->Add_Font(TEXT("Font_GyeongGiThousandBoldSmall"), TEXT("../Bin/Resources/Fonts/GyeongGiThousandBoldSmall.spritefont"))))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Start_Level(LEVEL eLevelID)
{
	if (FAILED(m_pGameInstance->Change_Level(CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL::LOADING, eLevelID))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_Prototypes()
{
	/* For.Prototype_Component_Transform*/
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Transform"),
		CTransform::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	///* For.Prototype_Component_Texture_Cursor */
	//if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Cursor"),
	//	CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Atl_Cursor.png"), 1))))
	//	return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Rect */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxPosTex */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPosTex.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
		return E_FAIL;

	/* For.Prototype_GameObject_Camera_AnimationEditor */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Camera_AnimationEditor"),
		CCamera_AnimationEditor::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	return S_OK;
}

CMainApp* Animation_Editor::CMainApp::Create()
{
	CMainApp* pInstance = new CMainApp();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CMainApp");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void Animation_Editor::CMainApp::Free()
{
	__super::Free();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	m_pTool_Manager->DestroyInstance();
	Safe_Release(m_pTool_Manager);

	m_pGameInstance->Release_Engine();
	Safe_Release(m_pGameInstance);
}
