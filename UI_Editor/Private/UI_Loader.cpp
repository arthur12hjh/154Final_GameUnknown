#include "pch.h"
#include "UI_Loader.h"

#include "UIPanel.h"
#include "UIWrapper.h"
#include "UIButton.h"
#include "UIText.h"

#include "GameInstance.h"
#include "GameManager.h"

CUI_Loader::CUI_Loader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice { pDevice }
	, m_pContext { pContext }
	, m_pGameInstance { CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

unsigned int APIENTRY LoadingMain(void* pArg)
{
	CUI_Loader*		pUI_Loader = static_cast<CUI_Loader*>(pArg);

	if (FAILED(pUI_Loader->Loading()))
		return 1;

	return 0;
}

HRESULT CUI_Loader::Initialize(LEVEL eNextLevelID)
{	
	m_eNextLevelID = eNextLevelID;

	/* 세마포어, 뮤텍스, 크리티컬섹션 */

	/* 임계영역(힙, 데이터, 코드)에 접근하기위한 키를 생성한다. */
	InitializeCriticalSection(&m_CriticalSection);

	/* 실제 로딩을 수행하기위한 스레드를 생성한다. */
	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, LoadingMain, this, 0, nullptr);
	if (0 == m_hThread)
		return E_FAIL;

	m_pGameManager = Client::CGameManager::GetInstance();

	if (!m_pGameManager)
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Loader::Loading()
{
	CoInitializeEx(nullptr, 0);

	EnterCriticalSection(&m_CriticalSection);

	HRESULT		hr = {};

	switch (m_eNextLevelID)
	{
	case LEVEL::LOGO:
		hr = Loading_For_Logo();
		break;
	case LEVEL::GAMEPLAY:
		hr = Loading_For_GamePlay();
		break;
	}

	LeaveCriticalSection(&m_CriticalSection);

	if (FAILED(hr))
		return E_FAIL;

	return S_OK;
}

void CUI_Loader::Output()
{
	SetWindowText(g_hWnd, m_strMessage.c_str());
}

HRESULT CUI_Loader::Loading_For_Logo()
{
	Loading_UI_For_Logo_Level();

	m_strMessage = TEXT("로딩이 완료되었습니다..");

	m_isFinished = true;

	return S_OK;
}

HRESULT CUI_Loader::Loading_For_GamePlay()
{
	m_strMessage = TEXT("텍스쳐를(을) 로딩 중 입니다.");

	m_strMessage = TEXT("셰이더를(을) 로딩 중 입니다.");
	///* For.Prototype_Component_Shader_VtxNorTex */
	//if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Shader_VtxNorTex"),
	//	CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX::Elements, VTXNORTEX::iNumElements))))
	//	return E_FAIL;

	///* For.Prototype_Component_Shader_VtxRectParticle */
	//if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Shader_VtxRectParticle"),
	//	CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxRectParticle.hlsl"), VTX_POSTEX_INSTANCE_PARTICLE::Elements, VTX_POSTEX_INSTANCE_PARTICLE::iNumElements))))
	//	return E_FAIL;

	///* For.Prototype_Component_Shader_VtxPointParticle */
	//if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Shader_VtxPointParticle"),
	//	CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPointParticle.hlsl"), VTX_POS_INSTANCE_PARTICLE::Elements, VTX_POS_INSTANCE_PARTICLE::iNumElements))))
	//	return E_FAIL;

	m_strMessage = TEXT("객체원형를(을) 로딩 중 입니다.");

	m_strMessage = TEXT("로딩이 완료되었습니다..");

	m_isFinished = true;

	return S_OK;
}

HRESULT CUI_Loader::Loading_UI_For_Logo_Level()
{
	m_strMessage = TEXT("UI텍스쳐들 로딩 중 입니다.");
	/* For.Prototype_Component_UI_Texture_BackGround */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::LOGO), TEXT("Prototype_Component_UI_Texture_BackGround"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/Default%d.jpg"), 2))))
		return E_FAIL;

	m_pGameManager->Add_UI_Texture(ENUM_CLASS(LEVEL::LOGO), TEXT("Prototype_Component_UI_Texture_BackGround"),
		TEXT("Com_Texture_UI_BackGround"), TEXT("../../Client/Bin/Resources/Textures/Default%d.jpg"), 2);

	/* For.Prototype_Component_UI_Texture_Default */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::LOGO), TEXT("Prototype_Component_UI_Texture_Default"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/Default0.png"), 1))))
		return E_FAIL;

	m_pGameManager->Add_UI_Texture(ENUM_CLASS(LEVEL::LOGO), TEXT("Prototype_Component_UI_Texture_Default"),
		TEXT("Com_Texture_UI_Default"), TEXT("../../Client/Bin/Resources/Textures/Default0.png"), 1);

	m_strMessage = TEXT("UI객체원형들 로딩 중 입니다.");
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::LOGO), TEXT("Prototype_GameObject_UI_Panel"),
		CUIPanel::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::LOGO), TEXT("Prototype_GameObject_UI_Wrapper"),
		CUIWrapper::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::LOGO), TEXT("Prototype_GameObject_UI_Button"),
		CUIButton::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::LOGO), TEXT("Prototype_GameObject_UI_Text"),
		CUIText::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	return S_OK;
}

CUI_Loader* CUI_Loader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevelID)
{
	CUI_Loader* pInstance = new CUI_Loader(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevelID)))
	{
		MSG_BOX("Failed to Created : CUI_Loader");
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CUI_Loader::Free()
{
	__super::Free();

	WaitForSingleObject(m_hThread, INFINITE);

	CloseHandle(m_hThread);

	DeleteCriticalSection(&m_CriticalSection);

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
