#include "pch.h"
#include "UI_Loader.h"

#pragma region UI Header
#include "UIWrapper.h"
#include "UIPanel.h"
#include "UIButton.h"
#include "UIText.h"
#include "UIImage.h"
#include "UIPotion.h"
#include "UIPotionCount.h"
#include "UIPotionStack.h"
#include "UIHPBar.h"
#include "UIHPFX.h"
#include "UIShield.h"
#include "UIBeta.h"
#include "UIBetaFX.h"
#include "UISkillWrapper.h"
#include "UISkillWrapperLineFX.h"
#include "UISkillWrapperOnFX.h"
#include "UISkillSlot.h"
#include "UIRushSlot.h"

#include "UIWorldWrapper.h"
#include "UIMonsterHPBar.h"
#include "UIMonsterHPFX.h"
#include "UIMonsterShield.h"
#include "UIMonsterStamina.h"
#include "UIMonsterStaminaFX.h"
#include "UIBossVitalWrapper.h"
#include "UIBossHPBar.h"
#include "UIBossHPBarFX.h"
#include "UIBossShield.h"
#include "UIBossStamina.h"
#include "UIBossStaminaFX.h"
#include "UIBossName.h"
#include "UISimpleKey.h"
#include "UIInteractionFX.h"
#include "UILockOn.h"
#include "UIGetterQueue.h"
#include "UIOwnGold.h"
#include "UIScript.h"
#include "UIUnlockFX.h"

#include "UIPopup.h"
#include "UICostumePuzzleAnswer.h"
#include "UICostumePuzzleButtons.h"
#include "UICostumePuzzleHint.h"
#include "UIMapSelector.h"

#pragma endregion

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
	
	/* For.Prototype_Component_UI_Texture_Overlay */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::LOGO), TEXT("Prototype_Component_UI_Texture_Overlay"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/UI/BackGround/Loading_BG_0.png"), 1))))
		return E_FAIL;

	m_pUIResourceStore->Add_UI_Texture(ENUM_CLASS(LEVEL::LOGO), TEXT("Prototype_Component_UI_Texture_Overlay"),
		TEXT("Com_Texture_UI_Overlay"), TEXT("../../Client/Bin/Resources/Textures/UI/BackGround/Loading_BG_0.png"), 1);

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

	if (FAILED(Loading_UI_For_World()))
		return E_FAIL;

	if (FAILED(Loading_UI_For_Popups()))
		return E_FAIL;

	/* For.Prototype_Component_UI_Texture_Overlay */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Overlay"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/UI/BackGround/Loading_BG_0.png"), 1))))
		return E_FAIL;

	m_pUIResourceStore->Add_UI_Texture(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Overlay"),
		TEXT("Com_Texture_UI_Overlay"), TEXT("../../Client/Bin/Resources/Textures/UI/BackGround/Loading_BG_0.png"), 1);

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

	/* For.Prototype_Component_UI_Texture_Icon_Gold */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Icon_Gold"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/UI/Combat_HUD/Info/Icon_Gold.png"), 1))))
		return E_FAIL;

	m_pUIResourceStore->Add_UI_Texture(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Icon_Gold"),
		TEXT("Com_Texture_UI_Icon_Gold"), TEXT("../../Client/Bin/Resources/Textures/UI/Combat_HUD/Info/Icon_Gold.png"), 1);

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

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_UI_Script"),
		CUIScript::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_UI_Texture_Boss_Vital_Shadow */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Boss_Vital_Shadow"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/UI/Combat_HUD/Boss/Boss_Vital_Shadow.png"), 1))))
		return E_FAIL;

	m_pUIResourceStore->Add_UI_Texture(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Boss_Vital_Shadow"),
		TEXT("Com_Texture_UI_Boss_Vital_Shadow"), TEXT("../../Client/Bin/Resources/Textures/UI/Combat_HUD/Boss/Boss_Vital_Shadow.png"), 1);
	
	/* For.Prototype_Component_UI_Texture_Boss_Name */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Boss_Name"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/UI/Combat_HUD/Boss/Boss_Name_%d.png"), 1))))
		return E_FAIL;

	m_pUIResourceStore->Add_UI_Texture(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Boss_Name"),
		TEXT("Com_Texture_UI_Boss_Name"), TEXT("../../Client/Bin/Resources/Textures/UI/Combat_HUD/Boss/Boss_Name_%d.png"), 1);

	/* For.Prototype_Component_UI_Texture_Boss_Stamina */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Boss_Stamina"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/UI/Combat_HUD/Boss/Stamina_%d.png"), 3))))
		return E_FAIL;

	m_pUIResourceStore->Add_UI_Texture(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Boss_Stamina"),
		TEXT("Com_Texture_UI_Boss_Stamina"), TEXT("../../Client/Bin/Resources/Textures/UI/Combat_HUD/Boss/Stamina_%d.png"), 3);

	/* For.Prototype_Component_UI_Texture_Boss_StaminaFX */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Boss_StaminaFX"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/UI/Combat_HUD/Boss/Stamina_FX_%d.png"), 2))))
		return E_FAIL;

	m_pUIResourceStore->Add_UI_Texture(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Boss_StaminaFX"),
		TEXT("Com_Texture_UI_Boss_Stamina_FX"), TEXT("../../Client/Bin/Resources/Textures/UI/Combat_HUD/Boss/Stamina_FX_%d.png"), 2);

	/*=========================================================================================================================*/

	/* For.Prototype_GameObject_UI_WorldWrapper */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_UI_BossVitalWrapper"),
		CUIBossVitalWrapper::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_Boss_HPBar */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_UI_Boss_HPBar"),
		CUIBossHPBar::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_Boss_HPFX */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_UI_Boss_HPFX"),
		CUIBossHPBarFX::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_Boss_Shield */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_UI_Boss_Shield"),
		CUIBossShield::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_BossName */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_UI_BossName"),
		CUIBossName::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_BossStamina */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_UI_BossStamina"),
		CUIBossStamina::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_BossStaminaFX */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_UI_BossStaminaFX"),
		CUIBossStaminaFX::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_GetterQueue */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_UI_GetterQueue"),
		CUIGetterQueue::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_OwnGold */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_UI_OwnGold"),
		CUIOwnGold::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	m_strMessage = TEXT("UI객체원형들 로딩 중 입니다.");

	/* ------------------------------------------------------------------------------------------- */

	_matrix PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
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

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Shader_VtxMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/ShaderFiles/Shader_VtxMesh.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Sky */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::LEVEL_PROB), TEXT("Prototype_Component_Model_Sky"),
		CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../../Client/Bin/Resources/Maps/Sky/Sky1.bin", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Sky1 */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::LEVEL_PROB), TEXT("Prototype_Component_Texture_Sky1"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Maps/Sky/T_Sky_RockyHills.png"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::LEVEL_PROB), TEXT("Prototype_GameObject_Sky"),
		CSky::Create(m_pDevice, m_pContext))))
		return E_FAIL;

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

	/* For.Prototype_Component_UI_Texture_Player_BetaFX */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Player_BetaFX"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/UI/Combat_HUD/Player_HP/BetaFx_%d.png"), 2))))
		return E_FAIL;

	m_pUIResourceStore->Add_UI_Texture(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Player_BetaFX"),
		TEXT("Com_Texture_UI_Player_BetaFX"), TEXT("../../Client/Bin/Resources/Textures/UI/Combat_HUD/Player_HP/BetaFx_%d.png"), 2);

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

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_UI_BetaFX"),
		CUIBetaFX::Create(m_pDevice, m_pContext))))
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
	
	/* For.Prototype_Component_UI_Texture_Beta_Cost_Deco */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Beta_Cost_Deco"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/UI/Combat_HUD/SkillFrame/Beta_Cost_Deco.png"), 1))))
		return E_FAIL;

	m_pUIResourceStore->Add_UI_Texture(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Beta_Cost_Deco"),
		TEXT("Com_Texture_UI_Beta_Cost_Deco"), TEXT("../../Client/Bin/Resources/Textures/UI/Combat_HUD/SkillFrame/Beta_Cost_Deco.png"), 1);
	
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
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/UI/Combat_HUD/SkillFrame/Skill_On_Fx_0.png"), 1))))
		return E_FAIL;

	m_pUIResourceStore->Add_UI_Texture(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Skill_On_Fx"),
		TEXT("Com_Texture_UI_Skill_On_Fx"), TEXT("../../Client/Bin/Resources/Textures/UI/Combat_HUD/SkillFrame/Skill_On_Fx_0.png"), 1);
	
	/* For.Prototype_Component_UI_Texture_SkillWrapper_On_Fx */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_SkillWrapper_On_Fx"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/UI/Combat_HUD/SkillFrame/SkillWrapper_On_Fx_%d.png"), 2))))
		return E_FAIL;

	m_pUIResourceStore->Add_UI_Texture(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_SkillWrapper_On_Fx"),
		TEXT("Com_Texture_UI_SkillWrapper_On_Fx"), TEXT("../../Client/Bin/Resources/Textures/UI/Combat_HUD/SkillFrame/SkillWrapper_On_Fx_%d.png"), 2);

	/* For.Prototype_Component_UI_Texture_Skill_Line_Fx */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Skill_Line_Fx"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/UI/Combat_HUD/SkillFrame/On_Line_%d.png"), 2))))
		return E_FAIL;

	m_pUIResourceStore->Add_UI_Texture(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Skill_Line_Fx"),
		TEXT("Com_Texture_UI_Skill_Line_Fx"), TEXT("../../Client/Bin/Resources/Textures/UI/Combat_HUD/SkillFrame/On_Line_%d.png"), 2);

	/* For.Prototype_Component_UI_Texture_Skills */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Skills"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/UI/Skills/Skill_%d.png"), 4))))
		return E_FAIL;

	m_pUIResourceStore->Add_UI_Texture(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Skills"),
		TEXT("Com_Texture_UI_Skills"), TEXT("../../Client/Bin/Resources/Textures/UI/Skills/Skill_%d.png"), 4);
	
	/* For.Prototype_Component_UI_Texture_Rush */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Rush"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/UI/Combat_HUD/SkillFrame/Rush.png"), 1))))
		return E_FAIL;

	m_pUIResourceStore->Add_UI_Texture(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Rush"),
		TEXT("Com_Texture_UI_Rush"), TEXT("../../Client/Bin/Resources/Textures/UI/Combat_HUD/SkillFrame/Rush.png"), 1);
	
	/* For.Prototype_Component_UI_Texture_Rush_Frame */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Rush_Frame"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/UI/Combat_HUD/SkillFrame/Rush_Frame.png"), 1))))
		return E_FAIL;

	m_pUIResourceStore->Add_UI_Texture(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Rush_Frame"),
		TEXT("Com_Texture_UI_Rush_Frame"), TEXT("../../Client/Bin/Resources/Textures/UI/Combat_HUD/SkillFrame/Rush_Frame.png"), 1);
	
	/* For.Prototype_Component_UI_Texture_Rush_CoolTime */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Rush_CoolTime"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/UI/Combat_HUD/SkillFrame/Rush_CoolTime.png"), 1))))
		return E_FAIL;

	m_pUIResourceStore->Add_UI_Texture(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Rush_CoolTime"),
		TEXT("Com_Texture_UI_Rush_CoolTime"), TEXT("../../Client/Bin/Resources/Textures/UI/Combat_HUD/SkillFrame/Rush_CoolTime.png"), 1);

	/* For.Prototype_Component_UI_Texture_Rush_Glow */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Rush_Glow"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/UI/Combat_HUD/SkillFrame/Rush_Glow_%d.png"), 2))))
		return E_FAIL;

	m_pUIResourceStore->Add_UI_Texture(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Rush_Glow"),
		TEXT("Com_Texture_UI_Rush_Glow"), TEXT("../../Client/Bin/Resources/Textures/UI/Combat_HUD/SkillFrame/Rush_Glow_.png"), 2);

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
	
	/* For.Prototype_Component_UI_Texture_Skill_Slot_Covor */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Skill_Slot_Covor"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/UI/Combat_HUD/SkillFrame/SkillCover.png"), 1))))
		return E_FAIL;

	m_pUIResourceStore->Add_UI_Texture(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Skill_Slot_Covor"),
		TEXT("Com_Texture_UI_Skill_Slot_Covor"), TEXT("../../Client/Bin/Resources/Textures/UI/Combat_HUD/SkillFrame/SkillCover.png"), 1);
	
	/* For.Prototype_Component_UI_Texture_On_BetaText */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_On_BetaText"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/UI/Combat_HUD/SkillFrame/On_BetaText.png"), 1))))
		return E_FAIL;
	
	m_pUIResourceStore->Add_UI_Texture(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_On_BetaText"),
		TEXT("Com_Texture_UI_On_BetaText"), TEXT("../../Client/Bin/Resources/Textures/UI/Combat_HUD/SkillFrame/On_BetaText.png"), 1);

	/* For.Prototype_GameObject_UI_SkillWrapper */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_UI_SkillWrapper"),
		CUISkillWrapper::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_SkillWrapperLineFX */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_UI_SkillWrapperLineFX"),
		CUISkillWrapperLineFX::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
	/* For.Prototype_GameObject_UI_SkillWrapperOnFX */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_UI_SkillWrapperOnFX"),
		CUISkillWrapperOnFX::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_SkillSlot */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_UI_SkillSlot"),
		CUISkillSlot::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_RushSlot */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_UI_RushSlot"),
		CUIRushSlot::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Loader::Loading_UI_For_Popups()
{
	// 팝업
	/* For.Prototype_Component_UI_Texture_PopupBG */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_PopupBG"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/UI/Popup/Popup_Bg_%d.png"), 2))))
		return E_FAIL;

	m_pUIResourceStore->Add_UI_Texture(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_PopupBG"),
		TEXT("Com_Texture_UI_PopupBG"), TEXT("../../Client/Bin/Resources/Textures/UI/Popup/Popup_Bg_%d.png"), 2);
	
	/* For.Prototype_Component_UI_Texture_Popup_Inner_Frame */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Popup_Inner_Frame"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/UI/Puzzle/Inner_Frame_%d.png"), 2))))
		return E_FAIL;

	m_pUIResourceStore->Add_UI_Texture(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Popup_Inner_Frame"),
		TEXT("Com_Texture_UI_Popup_Inner_Frame"), TEXT("../../Client/Bin/Resources/Textures/UI/Puzzle/Inner_Frame_%d.png"), 2);
	
	/* For.Prototype_Component_UI_Texture_Costume_Puzzle_Button */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Costume_Puzzle_Button"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/UI/Puzzle/Puzzle_Button_%d.png"), 4))))
		return E_FAIL;

	m_pUIResourceStore->Add_UI_Texture(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Puzzle_Button"),
		TEXT("Com_Texture_UI_Puzzle_Button"), TEXT("../../Client/Bin/Resources/Textures/UI/Puzzle/Puzzle_Button_%d.png"), 4);
	
	/* For.Prototype_Component_UI_Texture_Suit_Icon */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Suit_Icon"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/UI/Puzzle/Suit_%d.png"), 12))))
		return E_FAIL;

	m_pUIResourceStore->Add_UI_Texture(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Suit_Icon"),
		TEXT("Com_Texture_UI_Suit_Icon"), TEXT("../../Client/Bin/Resources/Textures/UI/Puzzle/Suit_%d.png"), 12);
	
	/* For.Prototype_Component_UI_Texture_Suit_Icons */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Suit_Icons"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/UI/Puzzle/Suit_Icons.png"), 1))))
		return E_FAIL;

	m_pUIResourceStore->Add_UI_Texture(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Suit_Icons"),
		TEXT("Com_Texture_UI_Suit_Icons"), TEXT("../../Client/Bin/Resources/Textures/UI/Puzzle/Suit_Icons.png"), 1);

	/* For.Prototype_Component_UI_Texture_Puzzle_Answer */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Puzzle_Answer"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/UI/Puzzle/Answer_%d.png"), 1))))
		return E_FAIL;

	m_pUIResourceStore->Add_UI_Texture(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Puzzle_Answer"),
		TEXT("Com_Texture_UI_Puzzle_Answer"), TEXT("../../Client/Bin/Resources/Textures/UI/Puzzle/Answer_%d.png"), 1);
	
	/* For.Prototype_Component_UI_Texture_Puzzle_Unlock */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Puzzle_Unlock"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/UI/Puzzle/Lock_Glow_%d.png"), 2))))
		return E_FAIL;

	m_pUIResourceStore->Add_UI_Texture(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Puzzle_Unlock"),
		TEXT("Com_Texture_UI_Puzzle_Unlock"), TEXT("../../Client/Bin/Resources/Textures/UI/Puzzle/Lock_Glow_%d.png"), 2);

	/* For.Prototype_Component_UI_Texture_Map_Thumbnails */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Map_Thumbnails"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/UI/Maps/Map_Thumbnail_%d.png"), 3))))
		return E_FAIL;

	m_pUIResourceStore->Add_UI_Texture(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Map_Thumbnails"),
		TEXT("Com_Texture_UI_Map_Thumbnail"), TEXT("../../Client/Bin/Resources/Textures/UI/Maps/Map_Thumbnail_%d.png"), 3);
	
	/* For.Prototype_Component_UI_Texture_ItemIcons */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_ItemIcons"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/UI/ItemIcons/Item_Icon_%d.png"), 3))))
		return E_FAIL;

	m_pUIResourceStore->Add_UI_Texture(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_ItemIcons"),
		TEXT("Com_Texture_UI_ItemIcons"), TEXT("../../Client/Bin/Resources/Textures/UI/ItemIcons/Item_Icon_%d.png"), 3);

	/*=================================================================================================================*/

	/* For.Prototype_GameObject_UI_Popup */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_UI_Popup"),
		CUIPopup::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_Costume_Puzzle_Answer */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_UI_Costume_Puzzle_Answer"),
		CUICostumePuzzleAnswer::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_Costume_Puzzle_Buttons */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_UI_Costume_Puzzle_Buttons"),
		CUICostumePuzzleButtons::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_Costume_Puzzle_Hint */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_UI_Costume_Puzzle_Hint"),
		CUICostumePuzzleHint::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_Map_Selector */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_UI_Map_Selector"),
		CUIMapSelector::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_UnlockFX */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_UI_UnlockFX"),
		CUIUnlockFX::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Loader::Loading_UI_For_World()
{
	/* For.Prototype_Component_UI_Texture_LockOn */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_LockOn"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/UI/LockOn/LockOnMark.png"), 1))))
		return E_FAIL;

	m_pUIResourceStore->Add_UI_Texture(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_LockOn"),
		TEXT("Com_Texture_UI_LockOn"), TEXT("../../Client/Bin/Resources/Textures/UI/LockOn/LockOnMark.png"), 1);
	
	/* For.Prototype_Component_UI_Texture_Interaction_Key */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Interaction_Key"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/UI/Interaction/KeyIcon_%d.png"), 2))))
		return E_FAIL;

	m_pUIResourceStore->Add_UI_Texture(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Interaction_Key"),
		TEXT("Com_Texture_UI_Interaction_Key"), TEXT("../../Client/Bin/Resources/Textures/UI/Interaction/KeyIcon_%d.png"), 2);
	
	/* For.Prototype_Component_UI_Texture_KeyIcon_Mouse */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_KeyIcon_Mouse"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/UI/LockOn/KeyIcon_Mouse_0.png"), 1))))
		return E_FAIL;

	m_pUIResourceStore->Add_UI_Texture(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_KeyIcon_Mouse"),
		TEXT("Com_Texture_UI_KeyIcon_Mouse"), TEXT("../../Client/Bin/Resources/Textures/UI/LockOn/KeyIcon_Mouse_0.png"), 1);
	
	/* For.Prototype_Component_UI_Texture_Shadow_Ball */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Shadow_Ball"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/UI/LockOn/Shadow_Ball.png"), 1))))
		return E_FAIL;

	m_pUIResourceStore->Add_UI_Texture(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Shadow_Ball"),
		TEXT("Com_Texture_UI_Shadow_Ball"), TEXT("../../Client/Bin/Resources/Textures/UI/LockOn/Shadow_Ball.png"), 1);
	
	/* For.Prototype_Component_UI_Texture_Shadow_Block */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Shadow_Block"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/UI/Combat_HUD/ETC/Shadow_Block.png"), 1))))
		return E_FAIL;

	m_pUIResourceStore->Add_UI_Texture(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Shadow_Block"),
		TEXT("Com_Texture_UI_Shadow_Block"), TEXT("../../Client/Bin/Resources/Textures/UI/Combat_HUD/ETC/Shadow_Block.png"), 1);

	/* For.Prototype_Component_UI_Texture_Finish_Ring */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Finish_Ring"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/UI/LockOn/Finish_Ring.png"), 1))))
		return E_FAIL;

	m_pUIResourceStore->Add_UI_Texture(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Finish_Ring"),
		TEXT("Com_Texture_UI_Finish_Ring"), TEXT("../../Client/Bin/Resources/Textures/UI/LockOn/Finish_Ring.png"), 1);
	
	/* For.Prototype_Component_UI_Texture_Interaction_Hold_Gauge */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Interaction_Hold_Gauge"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/UI/Interaction/Hold_Gauge.png"), 1))))
		return E_FAIL;

	m_pUIResourceStore->Add_UI_Texture(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Interaction_Hold_Gauge"),
		 TEXT("Com_Texture_UI_Hold_Gauge"), TEXT("../../Client/Bin/Resources/Textures/UI/Interaction/Hold_Gauge.png"), 1);

	/* For.Prototype_Component_UI_Texture_Interaction_Shadow */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Interaction_Shadow"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/UI/Interaction/Interaction_Shadow.png"), 1))))
		return E_FAIL;

	m_pUIResourceStore->Add_UI_Texture(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Interaction_Shadow"),
		TEXT("Com_Texture_UI_Interaction_Shadow"), TEXT("../../Client/Bin/Resources/Textures/UI/Interaction/Interaction_Shadow.png"), 1);

	/* For.Prototype_Component_UI_Texture_Interaction_FX */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Interaction_FX"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/UI/Interaction/Interaction_FX_%d.png"), 3))))
		return E_FAIL;

	m_pUIResourceStore->Add_UI_Texture(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Interaction_FX"),
		TEXT("Com_Texture_UI_Interaction_FX"), TEXT("../../Client/Bin/Resources/Textures/UI/Interaction/Interaction_FX_%d.png"), 3);

	/*=====================================================================================================*/

	/* For.Prototype_GameObject_UI_WorldWrapper */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_UI_WorldWrapper"),
		CUIWorldWrapper::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_Monster_HPBar */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_UI_Monster_HPBar"),
		CUIMonsterHPBar::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_Monster_HPFX */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_UI_Monster_HPFX"),
		CUIMonsterHPFX::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_Monster_Shield */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_UI_Monster_Shield"),
		CUIMonsterShield::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_Monster_Stamina */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_UI_Monster_Stamina"),
		CUIMonsterStamina::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_Monster_StaminaFX */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_UI_Monster_StaminaFX"),
		CUIMonsterStaminaFX::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_SimpleKey */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_UI_SimpleKey"),
		CUISimpleKey::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_InteractionFX */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_UI_InteractionFX"),
		CUIInteractionFX::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_LockOn */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_UI_LockOn"),
		CUILockOn::Create(m_pDevice, m_pContext))))
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
//////////////////////////////////////////////////