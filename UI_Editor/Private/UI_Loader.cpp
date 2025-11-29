#include "pch.h"
#include "UI_Loader.h"

#include "UIPanel.h"
#include "UIWrapper.h"
#include "UIButton.h"
#include "UIText.h"
#include "UIImage.h"
#include "UIHPBar.h"
#include "UIHPFX.h"
#include "UIPotion.h"
#include "UIPotionCount.h"
#include "UIPotionStack.h"
#include "UIShield.h"
#include "UIBeta.h"

#include "GameInstance.h"
#include "UIResourceStore.h"

#include "Sky.h"
#include "CanBox.h"
#include "Interaction_Component.h"

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

	m_pUIResourceStore = CUIResourceStore::GetInstance();

	if (!m_pUIResourceStore)
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
	m_pUIResourceStore->Clear_UI_Texture_Descs();

	if(FAILED(Loading_UI_For_Logo_Level()))
		return E_FAIL;

	m_strMessage = TEXT("로딩이 완료되었습니다..");

	m_isFinished = true;

	return S_OK;
}

HRESULT CUI_Loader::Loading_For_GamePlay()
{
	m_pUIResourceStore->Clear_UI_Texture_Descs();

	if (FAILED(Loading_UI_For_GamePlay_Level()))
		return E_FAIL;

	m_strMessage = TEXT("텍스쳐를(을) 로딩 중 입니다.");

	m_strMessage = TEXT("셰이더를(을) 로딩 중 입니다.");

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
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/UI/BackGround/BackGround_%d.png"), 1))))
		return E_FAIL;

	m_pUIResourceStore->Add_UI_Texture(ENUM_CLASS(LEVEL::LOGO), TEXT("Prototype_Component_UI_Texture_BackGround"),
		TEXT("Com_Texture_UI_BackGround"), TEXT("../../Client/Bin/Resources/Textures/UI/BackGround/BackGround_%d.png"), 1);

	/* For.Prototype_Component_UI_Texture_Default */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::LOGO), TEXT("Prototype_Component_UI_Texture_Default"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/Default0.png"), 1))))
		return E_FAIL;

	m_pUIResourceStore->Add_UI_Texture(ENUM_CLASS(LEVEL::LOGO), TEXT("Prototype_Component_UI_Texture_Default"),
		TEXT("Com_Texture_UI_Default"), TEXT("../../Client/Bin/Resources/Textures/Default0.png"), 1);

	/* For.Prototype_Component_UI_Texture_Logo_Main_Title */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::LOGO), TEXT("Prototype_Component_UI_Texture_Logo_Main_Title"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/UI/Logo/Main_Title.png"), 1))))
		return E_FAIL;

	m_pUIResourceStore->Add_UI_Texture(ENUM_CLASS(LEVEL::LOGO), TEXT("Prototype_Component_UI_Texture_Logo_Main_Title"),
		TEXT("Com_Texture_UI_Logo_Main_Title"), TEXT("../../Client/Bin/Resources/Textures/UI/Logo/Main_Title.png"), 1);

	/* For.Prototype_Component_UI_Texture_Dot_Select */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::LOGO), TEXT("Prototype_Component_UI_Texture_Dot_Select"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/UI/Dots/Dot_Select.png"), 1))))
		return E_FAIL;

	m_pUIResourceStore->Add_UI_Texture(ENUM_CLASS(LEVEL::LOGO), TEXT("Prototype_Component_UI_Texture_Dot_Select"),
		TEXT("Com_Texture_UI_Dot_Select"), TEXT("../../Client/Bin/Resources/Textures/UI/Dots/Dot_Select.png"), 1);

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

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::LOGO), TEXT("Prototype_GameObject_UI_Image"),
		CUIImage::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Loader::Loading_UI_For_GamePlay_Level()
{
	if (FAILED(Loading_UI_For_Combat_HUD_Vitals()))
		return E_FAIL;

	if (FAILED(Loading_UI_For_Combat_HUD_Skills()))
		return E_FAIL;

	/* For.Prototype_Component_UI_Texture_Center_Pivot */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Center_Pivot"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/UI/Aim/Center_Pivot.png"), 1))))
		return E_FAIL;

	m_pUIResourceStore->Add_UI_Texture(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Center_Pivot"),
		TEXT("Com_Texture_UI_Center_Pivot"), TEXT("../../Client/Bin/Resources/Textures/UI/Aim/Center_Pivot.png"), 1);

	/* For.Prototype_Component_UI_Texture_Number */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Number"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/UI/Number/Number_%d.png"), 10))))
		return E_FAIL;

	m_pUIResourceStore->Add_UI_Texture(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Number"),
		TEXT("Com_Texture_UI_Number"), TEXT("../../Client/Bin/Resources/Textures/UI/Number/Number_%d.png"), 10);

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_UI_Panel"),
		CUIPanel::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_UI_Wrapper"),
		CUIWrapper::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_UI_Button"),
		CUIButton::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_UI_Text"),
		CUIText::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_UI_Image"),
		CUIImage::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	m_strMessage = TEXT("UI객체원형들 로딩 중 입니다.");

	/* ------------------------------------------------------------------------------------------- */

	//_matrix PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	//
	//if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_Prob_CanBox"),
	//	CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::ANIM, "../../Client/Bin/Resources/Models/Box/CanBox/CanBox.binx", PreTransformMatrix))))
	//	return E_FAIL;
	///* Can Box */
	//if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Prob_CanBox"),
	//	CCanBox::Create(m_pDevice, m_pContext))))
	//	return E_FAIL;

	//if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Interaction"),
	//	CInteraction_Component::Create(m_pDevice, m_pContext))))
	//	return E_FAIL;

	//if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
	//	CShader::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/ShaderFiles/Shader_VtxAnimMesh.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements))))
	//	return E_FAIL;

	//if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Shader_VtxMesh"),
	//	CShader::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/ShaderFiles/Shader_VtxMesh.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
	//	return E_FAIL;

	///* For.Prototype_Component_Model_Sky */
	//PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	//if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_Sky"),
	//	CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Client/Bin/Resources/Maps/Sky/Sky1.bin", PreTransformMatrix))))
	//	return E_FAIL;

	///* For.Prototype_Component_Texture_Sky1 */
	//if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Texture_Sky1"),
	//	CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Maps/Sky/T_Sky_RockyHills.png"), 1))))
	//	return E_FAIL;

	//if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Sky"),
	//	CSky::Create(m_pDevice, m_pContext))))
	//	return E_FAIL;

	//if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Collider_AABB"),
	//	CBoxCollider::Create(m_pDevice, m_pContext))))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CUI_Loader::Loading_UI_For_Combat_HUD_Vitals()
{
	/* For.Prototype_Component_UI_Texture_Vital */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Vital"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/UI/Combat_HUD/Vital/Vital_Tag_%d.png"), 3))))
		return E_FAIL;

	m_pUIResourceStore->Add_UI_Texture(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Vital"),
		TEXT("Com_Texture_UI_Vital_Tag"), TEXT("../../Client/Bin/Resources/Textures/UI/Combat_HUD/Vital/Vital_Tag_%d.png"), 3);

	/* For.Prototype_Component_UI_Texture_Player_Hp */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Player_Hp"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/UI/Combat_HUD/Player_HP/Player_HP_%d.png"), 2))))
		return E_FAIL;

	m_pUIResourceStore->Add_UI_Texture(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Player_Hp"),
		TEXT("Com_Texture_UI_Player_Hp"), TEXT("../../Client/Bin/Resources/Textures/UI/Combat_HUD/Player_HP/Player_HP_%d.png"), 2);

	/* For.Prototype_Component_UI_Texture_Player_Hp_FX */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Player_Hp_FX"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/UI/Combat_HUD/Player_HP/Player_HP_FX.png"), 1))))
		return E_FAIL;

	m_pUIResourceStore->Add_UI_Texture(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Player_Hp_FX"),
		TEXT("Com_Texture_UI_Player_Hp_Fx"), TEXT("../../Client/Bin/Resources/Textures/UI/Combat_HUD/Player_HP/Player_HP_FX.png"), 1);

	/* For.Prototype_Component_UI_Texture_Player_Beta */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Player_Beta"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/UI/Combat_HUD/Player_HP/Player_Beta_%d.png"), 6))))
		return E_FAIL;

	m_pUIResourceStore->Add_UI_Texture(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Player_Beta"),
		TEXT("Com_Texture_UI_Player_Beta"), TEXT("../../Client/Bin/Resources/Textures/UI/Combat_HUD/Player_HP/Player_Beta_%d.png"), 6);

	/* For.Prototype_Component_UI_Texture_Potion */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Potion"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/UI/Combat_HUD/Potion/Potion.png"), 1))))
		return E_FAIL;

	m_pUIResourceStore->Add_UI_Texture(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Potion"),
		TEXT("Com_Texture_UI_Potion"), TEXT("../../Client/Bin/Resources/Textures/UI/Combat_HUD/Potion/Potion.png"), 1);
	
	/* For.Prototype_Component_UI_Texture_Potion_Stack */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Potion_Stack"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/UI/Combat_HUD/Potion/Potion_Stack.png"), 1))))
		return E_FAIL;

	m_pUIResourceStore->Add_UI_Texture(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Potion_Stack"),
		TEXT("Com_Texture_UI_Potion_Stack"), TEXT("../../Client/Bin/Resources/Textures/UI/Combat_HUD/Potion/Potion_Stack.png"), 1);

	/* For.Prototype_Component_UI_Texture_Btn_Empty */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Btn_Empty"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/UI/Combat_HUD/ETC/Btn_Empty.png"), 1))))
		return E_FAIL;

	m_pUIResourceStore->Add_UI_Texture(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Btn_Empty"),
		TEXT("Com_Texture_UI_Btn_Empty"), TEXT("../../Client/Bin/Resources/Textures/UI/Combat_HUD/ETC/Btn_Empty.png"), 1);

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_UI_HPBar"),
		CUIHPBar::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_UI_HP_FX"),
		CUIHPFX::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_UI_Potion"),
		CUIPotion::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_UI_PotionCount"),
		CUIPotionCount::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_UI_PotionStack"),
		CUIPotionStack::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_UI_Shield"),
		CUIShield::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_UI_Beta"),
		CUIBeta::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	return S_OK;
}

HRESULT CUI_Loader::Loading_UI_For_Combat_HUD_Skills()
{
	/* For.Prototype_Component_UI_Texture_SkillFrame */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_SkillFrame"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/UI/Combat_HUD/SkillFrame/SkillFrame.png"), 1))))
		return E_FAIL;

	m_pUIResourceStore->Add_UI_Texture(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_SkillFrame"),
		TEXT("Com_Texture_UI_SkillFrame"), TEXT("../../Client/Bin/Resources/Textures/UI/Combat_HUD/SkillFrame/SkillFrame.png"), 1);
	
	/* For.Prototype_Component_UI_Texture_SkillFrame_Shadow */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_SkillFrame_Shadow"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/UI/Combat_HUD/SkillFrame/SkillFrame_Shadow_%d.png"), 2))))
		return E_FAIL;

	m_pUIResourceStore->Add_UI_Texture(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_SkillFrame_Shadow"),
		TEXT("Com_Texture_UI_SkillFrame_Shadow"), TEXT("../../Client/Bin/Resources/Textures/UI/Combat_HUD/SkillFrame/SkillFrame_Shadow_%d.png"), 2);
	
	/* For.Prototype_Component_UI_Texture_Skill_Focus_Glow */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Skill_Focus_Glow"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/UI/Combat_HUD/SkillFrame/Ring_Focus_OutGlow.png"), 1))))
		return E_FAIL;

	m_pUIResourceStore->Add_UI_Texture(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Skill_Focus_Glow"),
		TEXT("Com_Texture_UI_Skill_Focus_Glow"), TEXT("../../Client/Bin/Resources/Textures/UI/Combat_HUD/SkillFrame/Ring_Focus_OutGlow.png"), 1);
	
	/* For.Prototype_Component_UI_Texture_Skill_On_Fx */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Skill_On_Fx"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/UI/Combat_HUD/SkillFrame/Skill_On_Fx_%d.png"), 2))))
		return E_FAIL;

	m_pUIResourceStore->Add_UI_Texture(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Skill_On_Fx"),
		TEXT("Com_Texture_UI_Skill_On_Fx"), TEXT("../../Client/Bin/Resources/Textures/UI/Combat_HUD/SkillFrame/Skill_On_Fx_%d.png"), 2);

	/* For.Prototype_Component_UI_Texture_Skills */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Skills"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/UI/Skills/Skill_%d.png"), 2))))
		return E_FAIL;

	m_pUIResourceStore->Add_UI_Texture(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Skills"),
		TEXT("Com_Texture_UI_Skills"), TEXT("../../Client/Bin/Resources/Textures/UI/Skills/Skill_%d.png"), 2);
	
	/* For.Prototype_Component_UI_Texture_Rush */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Rush"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/UI/Combat_HUD/SkillFrame/Rush.png"), 1))))
		return E_FAIL;

	m_pUIResourceStore->Add_UI_Texture(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Rush"),
		TEXT("Com_Texture_UI_Rush"), TEXT("../../Client/Bin/Resources/Textures/UI/Combat_HUD/SkillFrame/Rush.png"), 1);

	/* For.Prototype_Component_UI_Texture_Rush_Glow */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Rush_Glow"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/UI/Combat_HUD/SkillFrame/Rush_Glow.png"), 1))))
		return E_FAIL;

	m_pUIResourceStore->Add_UI_Texture(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Rush_Glow"),
		TEXT("Com_Texture_UI_Rush_Glow"), TEXT("../../Client/Bin/Resources/Textures/UI/Combat_HUD/SkillFrame/Rush_Glow.png"), 1);

	/* For.Prototype_Component_UI_Texture_Rush_Frame_Glow */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Rush_Frame_Glow"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/UI/Combat_HUD/SkillFrame/Rush_Frame_Glow.png"), 1))))
		return E_FAIL;

	m_pUIResourceStore->Add_UI_Texture(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Rush_Frame_Glow"),
		TEXT("Com_Texture_UI_Rush_Frame_Glow"), TEXT("../../Client/Bin/Resources/Textures/UI/Combat_HUD/SkillFrame/Rush_Frame_Glow.png"), 1);
	
	/* For.Prototype_Component_UI_Texture_On_Ring */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_On_Ring"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/UI/Combat_HUD/SkillFrame/On_Ring.png"), 1))))
		return E_FAIL;

	m_pUIResourceStore->Add_UI_Texture(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_On_Ring"),
		TEXT("Com_Texture_UI_On_Ring"), TEXT("../../Client/Bin/Resources/Textures/UI/Combat_HUD/SkillFrame/On_Ring.png"), 1);

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
//////////////////////////////////////////////////