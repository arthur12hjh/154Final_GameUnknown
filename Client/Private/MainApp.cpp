#include "pch.h"
#include "MainApp.h"

#include "GameInstance.h"

#include "Level_Loading.h"
#include "Camera_Free.h"
#include "Camera_Player.h"

#include "GameManager.h"
#include "JsonParser.h"
#include "MousePointer.h"

#ifdef _DEBUG
#include "ImGuiMain.h"

#endif
#include "Model.h"
#include "RigidBody.h"
#include "CharacterController.h"
#include "TestEveHead.h"

#include "Notify.h"
#include "EffectSRV.h"

CMainApp::CMainApp()	
	: m_pGameInstance { CGameInstance::GetInstance() },
	m_pEffectSRV{ CEffectSRV::GetInstance() }
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

	if (FAILED(m_pEffectSRV->Initialize(m_pDevice, m_pContext)))
		return E_FAIL;

	if (FAILED(Ready_Default_Setting()))
		return E_FAIL;

	if (FAILED(Ready_Manager_Setting()))
		return E_FAIL;

	if (FAILED(Ready_Prototypes()))
		return E_FAIL;

	if (FAILED(Ready_Mouse()))
		return E_FAIL;

	if (FAILED(Start_Level(LEVEL::LOGO)))
		return E_FAIL;		

#ifdef _DEBUG
	m_pImGuiDebug = CImGuiMain::Create(m_pDevice, m_pContext);
	if (nullptr == m_pImGuiDebug)
		return E_FAIL;
#endif

	return S_OK;
}

void CMainApp::Update(_float fTimeDelta)
{
	if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_GRAVE))
		m_bIsMouseLock = !m_bIsMouseLock;

	if (m_bIsMouseLock)
		MouseLock();

	m_pGameInstance->Update_Engine(fTimeDelta);


	m_pEffectSRV->Reset();
#ifdef _DEBUG
	m_pImGuiDebug->Update(fTimeDelta);
#endif


}

HRESULT CMainApp::Render()
{
	_float4			vClearColor = _float4(0.f, 0.f, 1.f, 1.f);

	m_pGameInstance->Render_Begin(&vClearColor);

	m_pGameInstance->Draw();

#ifdef _DEBUG
	m_pImGuiDebug->Render();
#endif

	m_pGameInstance->Render_End();

	return S_OK;
}

HRESULT CMainApp::Ready_Default_Setting()
{
	/*MakeSpriteFont "넥슨Lv1고딕 Bold" /FontSize:20 /FastPack /CharacterRegion:0x0020-0x00FF /CharacterRegion:0x3131-0x3163 /CharacterRegion:0xAC00-0xD800 /DefaultCharacter:0xAC00 155ex.spritefont */
	if (FAILED(m_pGameInstance->Add_Font(TEXT("KoPub"), TEXT("../../Client/Bin/Resources/Fonts/KoPub.spritefont"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Font(TEXT("Iceberg"), TEXT("../../Client/Bin/Resources/Fonts/Iceberg.spritefont"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_Manager_Setting()
{
	auto pGameManager = CGameManager::GetInstance();
	pGameManager->Initialize(m_pDevice, m_pContext);

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
	///* For.Prototype_Component_Transform*/
	//if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Transform"),
	//	CTransform::Create(m_pGraphic_Device))))
	//	return E_FAIL;

	/* For.Prototype_Component_Notify */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Notify"),
		CNotify::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Rect */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Point */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Point"),
		CVIBuffer_Point::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	CVIBuffer_Rect_Instance::RECT_INSTANCE_DESC InstanceDesc{};
	InstanceDesc.iNumInstance = 1;

	/* For.Prototype_Component_VIBuffer_Rect_Instance */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect_Instance"),
		CVIBuffer_Rect_Instance::Create(m_pDevice, m_pContext, &InstanceDesc))))
		return E_FAIL;

#pragma region Shader
	/* For.Prototype_Component_Shader_VtxPosTex */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPosTex.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_UI */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_UI"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_UI.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_Point */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_Mouse"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Mouse.hlsl"), VTXPOS::Elements, VTXPOS::iNumElements))))
		return E_FAIL;

#pragma endregion

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Mouse"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Cursor/cursor_%d.png"), 4))))
		return E_FAIL;
	

	/* For.Prototype_GameObject_Camera_Free */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Camera_Free"),
		CCamera_Free::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Camera_Player */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Camera_Player"),
		CCamera_Player::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_RigidBody */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_RigidBody"),
		CRigidBody::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_CharacterController */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_CharacterController"),
		CCharacterController::Create(m_pDevice, m_pContext))))
		return E_FAIL;

#ifdef _DEBUG

#endif
	_fmatrix HeadPreWorlMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_TestEveHead"),
		CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Models/MS_EVE_HEAD/Eve_Face.fbx", HeadPreWorlMatrix))))
		return E_FAIL;

	/* For.Prototype_GameObject_TestEveHead */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_TestEveHead"), 
		CTestEveHead::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_Mouse()
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

	// 이브 대가리 SSSAO & SpecDetail 적용 테스트 코드. 혹시 지우고 싶으면 말씀좀
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Eve_Head_SSSAO"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Models/Character/PC/Eve/CH_P_HEAD_EVE/Tex_P_EVE_Head_SSSAO.png"), 1))))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Eve_Head_SpecDetail"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Models/Character/PC/Eve/CH_P_HEAD_EVE/Tex_P_EVE_Head_S.png"), 1))))
		return E_FAIL;

	return S_OK;
}

void CMainApp::MouseLock()
{
	// 내 윈도우 핸들 hWnd 기준
	POINT center = {};
	RECT rc;

	// 내 윈도우 핸들 hWnd 기준
	GetClientRect(g_hWnd, &rc);  // 클라이언트 크기 가져오기

	center.x = (rc.right - rc.left) / 2;  // 가로 중심
	center.y = (rc.bottom - rc.top) / 2;  // 세로 중심

	ClientToScreen(g_hWnd, &center);        // 화면 좌표로 변환 (마우스 세팅용)
	SetCursorPos(center.x, center.y);
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

	CGameManager::DestroyInstance();
	m_pEffectSRV->DestroyInstance();
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

#ifdef _DEBUG
	Safe_Release(m_pImGuiDebug);
#endif

	m_pGameInstance->Release_Engine();

	Safe_Release(m_pGameInstance);
}
