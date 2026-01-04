#include "pch.h"

#include "UI_MainApp.h"

#include "GameInstance.h"

#include "UI_Camera.h"

#include "UI_Level_Loading.h"
#include "GUIManager.h"
#include "UIResourceStore.h"
#include "GameManager.h"

#include "RigidBody.h"

#include "UIInstanceBuffer.h"

#include "MousePointer.h"

/********************************
* ´ëÀçÈÆÀÇ ÀºÃÑ ÀÌ ¾ó¸¶³ª °ü´ëÇÑ°¡ *
* ´ë¹Î¼®ÀÇ ÀºÃÑ ÀÌ ¾ó¸¶³ª Âù¶õÇÑ°¡ *
********************************/

CUI_MainApp::CUI_MainApp()	
	: m_pGameInstance { CGameInstance::GetInstance() },
	m_pGameManager{ CGameManager::GetInstance() }
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

	if(FAILED(Ready_Manager_Setting()))
		return E_FAIL;

	if (FAILED(Ready_Prototypes()))
		return E_FAIL;

	if (FAILED(Ready_Mouse()))
		return E_FAIL;

	if (FAILED(Start_Level(LEVEL::BEATSABER_GAME)))
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

	if (FAILED(m_pGameInstance->Add_Font(TEXT("KoPub"), TEXT("../../Client/Bin/Resources/Fonts/KoPub.spritefont"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Font(TEXT("Iceberg"), TEXT("../../Client/Bin/Resources/Fonts/Iceberg.spritefont"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_MainApp::Ready_Manager_Setting()
{
	m_pUIResourceStore = CUIResourceStore::GetInstance();

	if (!m_pUIResourceStore)
		return E_FAIL;

	m_pUIResourceStore->Initialize(m_pDevice, m_pContext);

	auto pGameManager = CGameManager::GetInstance();
	pGameManager->Initialize(m_pDevice, m_pContext);

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

	/* For.Prototype_Component_VIBuffer_Point */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Point"),
		CVIBuffer_Point::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_UIDebug */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_UIDebug"),
		CVIBuffer_Point::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	CVIBuffer_Rect_Instance::RECT_INSTANCE_DESC InstanceDesc{};
	InstanceDesc.iNumInstance = 1;

	/* For.Prototype_Component_VIBuffer_Rect_Instance */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect_Instance"),
		CVIBuffer_Rect_Instance::Create(m_pDevice, m_pContext, &InstanceDesc))))
		return E_FAIL;

	CUIInstanceBuffer::UI_INSTANCE_DESC UIInstanceDesc{};
	UIInstanceDesc.iNumInstance = 1;

	/* For.Prototype_Component_UI_Instance_Buffer */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_UI_Instance_Buffer"),
		CUIInstanceBuffer::Create(m_pDevice, m_pContext, &UIInstanceDesc))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_UI */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_UI"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/ShaderFiles/Shader_UI.hlsl"), VTX_POSTEX_UI_INSTANCE::Elements, VTX_POSTEX_UI_INSTANCE::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_Point */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_Mouse"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/ShaderFiles/Shader_Mouse.hlsl"), VTXPOS::Elements, VTXPOS::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_RigidBody */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_RigidBody"),
		CRigidBody::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_Camera */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_Camera"),
		CUI_Camera::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Mouse"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/UI/Cursor/cursor_%d.png"), 4))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_MainApp::Ready_Mouse()
{
	/* For.Prototype_GameObject_Camera_Free */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Mouse"),
		CMousePointer::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	CUIObject::UIOBJECT_DESC UIDesc = {};
	UIDesc.fSizeX = 32.f;
	UIDesc.fSizeY = 32.f;
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Mouse"),
		ENUM_CLASS(LEVEL::STATIC), TEXT("Static_Level_Layer_Mouse"), &UIDesc)))
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

	m_pGameManager->Release_GameMgr();
	CGameManager::DestroyInstance();

	m_pGuiManager->Release_GUI_Manager();
	Safe_Release(m_pGuiManager);

	m_pGameInstance->Release_Engine();
	Safe_Release(m_pGameInstance);	
}
