#include "pch.h"
#include "UIRushSlot.h"

#include "GameInstance.h"
#include "GameManager.h"
#include "Player.h"
#ifdef _DEBUG
#include "../../UI_Editor/Public/UI_Camera.h"
#endif // DEBUG

#include "UIHUD.h"
#include "UISkillWrapper.h"

CUIRushSlot::CUIRushSlot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIBase{ pDevice, pContext }
{
}

CUIRushSlot::CUIRushSlot(const CUIRushSlot& Prototype) 
	: CUIBase{ Prototype }
{
}

HRESULT CUIRushSlot::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUIRushSlot::Initialize(void* pArg)
{	
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	auto pCharactor{ CGameManager::GetInstance()->GetGameCharacter() };

	if (pCharactor)
	{
		m_fMaxCoolTime = const_cast<_float*>(&CGameManager::GetInstance()->Get_PlayerDesc()->fMaxRushCoolTime);
		m_fCurrentCoolTime = const_cast<_float*>(&CGameManager::GetInstance()->Get_PlayerDesc()->fCurrentRushCoolTime);
	}
#ifdef _DEBUG
	else
	{
		auto pGaraPlayer = dynamic_cast<CUI_Camera*>(m_pGameInstance->GetMainCamera());

		m_fMaxCoolTime = const_cast<_float*>(&pGaraPlayer->Get_Desc()->fMaxRushCoolTime);
		m_fCurrentCoolTime = const_cast<_float*>(&pGaraPlayer->Get_Desc()->fCurrentRushCoolTime);

		Safe_Release(pGaraPlayer);
	}
#endif // DEBUG

	Safe_Release(pCharactor);
	return S_OK;
}

void CUIRushSlot::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CUIRushSlot::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

	m_fCoolAmount = (*m_fCurrentCoolTime / *m_fMaxCoolTime);
}

void CUIRushSlot::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	if (m_tRushInfo.iPrevSkillState == ENUM_CLASS(SKILL_STATE::DEFAULT)
		&& m_tRushInfo.iSkillState == ENUM_CLASS(SKILL_STATE::DEFAULT)
		&& m_bPlayingAnim == false)
	{
		CUIHUD* pHUD = dynamic_cast<CUIHUD*>(m_pGameInstance->GetCurrentLevelHUD());
		pHUD->Anim_Stop(m_tUIDesc.szLayerTag, m_tUIDesc.szUITag);
		Safe_Release(pHUD);
	}
	else if (m_tRushInfo.iPrevSkillState == ENUM_CLASS(SKILL_STATE::DEFAULT)
		&& m_tRushInfo.iSkillState == ENUM_CLASS(SKILL_STATE::ACTIVE_ON)
		&& m_bPlayingAnim == false)
	{
		CUIHUD* pHUD = dynamic_cast<CUIHUD*>(m_pGameInstance->GetCurrentLevelHUD());

		auto AnimTag = m_tUIDesc.m_AnimTags.find(TEXT("Rush_ActiveOn"));
		if (AnimTag != m_tUIDesc.m_AnimTags.end())
			pHUD->Anim_Play(m_tUIDesc.szLayerTag, m_tUIDesc.szUITag, AnimTag->first);

		m_bRushActiveOn = true;

		Safe_Release(pHUD);
	}
	else if (m_tRushInfo.iPrevSkillState == ENUM_CLASS(SKILL_STATE::ACTIVE_ON)
		&& m_tRushInfo.iSkillState == ENUM_CLASS(SKILL_STATE::ACTIVE)
		&& m_bPlayingAnim == false)
	{
		CUIHUD* pHUD = dynamic_cast<CUIHUD*>(m_pGameInstance->GetCurrentLevelHUD());

		auto AnimTag = m_tUIDesc.m_AnimTags.find(TEXT("Rush_Active"));
		if (AnimTag != m_tUIDesc.m_AnimTags.end())
			pHUD->Anim_Play(m_tUIDesc.szLayerTag, m_tUIDesc.szUITag, AnimTag->first);

		m_bRushActiveOn = false;

		Safe_Release(pHUD);
	}
	else if (m_tRushInfo.iPrevSkillState == ENUM_CLASS(SKILL_STATE::ACTIVE)
		&& m_tRushInfo.iSkillState == ENUM_CLASS(SKILL_STATE::USE)
		&& m_bPlayingAnim == true)
	{
		CUIHUD* pHUD = dynamic_cast<CUIHUD*>(m_pGameInstance->GetCurrentLevelHUD());
		pHUD->Anim_Stop(m_tUIDesc.szLayerTag, m_tUIDesc.szUITag);

		auto AnimTag = m_tUIDesc.m_AnimTags.find(TEXT("Rush_Use"));
		if (AnimTag != m_tUIDesc.m_AnimTags.end())
			pHUD->Anim_Play(m_tUIDesc.szLayerTag, m_tUIDesc.szUITag, AnimTag->first);

		Safe_Release(pHUD);
	}
}

HRESULT CUIRushSlot::Render()
{
	__super::Render();

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(UI_SHADER_PASS::RUSH_SLOT))))
		return E_FAIL;

	if (FAILED(m_pVIBaseBufferCom->Bind_Resources()))
		return E_FAIL;

	if (FAILED(m_pVIBaseBufferCom->Render()))
		return E_FAIL;

	if (m_tRushInfo.iSkillState != ENUM_CLASS(SKILL_STATE::DEFAULT))
	{
		if (FAILED(Render_Glow()))
			return E_FAIL;
	}

#ifdef _DEBUG
	__super::Render_Debug_Rect();
#endif

	return S_OK;
}

HRESULT CUIRushSlot::Ready_Components()
{
	__super::Ready_Components();

	/* Com_VIBuffer */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect_Instance"),
		TEXT("Com_VIBaseBuffer"), reinterpret_cast<CComponent**>(&m_pVIBaseBufferCom))))
		return E_FAIL;

	/* Com_VIBuffer */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect_Instance"),
		TEXT("Com_VIGlowBuffer"), reinterpret_cast<CComponent**>(&m_pVIGlowBufferCom))))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_SkillFrame_Shadow"),
		TEXT("Com_ShadowTexture"), reinterpret_cast<CComponent**>(&m_pShadowTextureCom))))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Rush"),
		TEXT("Com_Texture2"), reinterpret_cast<CComponent**>(&m_pTextureCom2))))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Rush_Frame"),
		TEXT("Com_FrameTexture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_On_Ring"),
		TEXT("Com_CoolTimeTexture"), reinterpret_cast<CComponent**>(&m_pCoolTimeTextureCom))))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Rush_Glow"),
		TEXT("Com_RushGlowTexture"), reinterpret_cast<CComponent**>(&m_pRushGlowTextureCom))))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_On_Ring"),
		TEXT("Com_GlowTexture"), reinterpret_cast<CComponent**>(&m_pGlowTextureCom))))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Skill_Focus_Glow"),
		TEXT("Com_GlowTexture2"), reinterpret_cast<CComponent**>(&m_pGlowTextureCom2))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUIRushSlot::Bind_ShaderResources()
{
	__super::Bind_ShaderResources();

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->GetIdentityMatrixPtr())))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;
	if (FAILED(m_pShadowTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture0", 0))) 
		return E_FAIL;
	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture1", 0)))
		return E_FAIL;
	if (FAILED(m_pTextureCom2->Bind_ShaderResource(m_pShaderCom, "g_Texture2", 0)))
		return E_FAIL;

	_bool bUseTintColor = false;
	_bool bUseCoolTime = false;

	if (m_tRushInfo.iSkillState == ENUM_CLASS(SKILL_STATE::DEFAULT))
	{
		bUseTintColor = true;
		bUseCoolTime = true;

		if (FAILED(m_pCoolTimeTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture3", 0)))
			return E_FAIL;
	}
	
	if (FAILED(m_pShaderCom->Bind_RawValue("g_bUseTintColor", &bUseTintColor, sizeof(_bool))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vTintColor", &m_tUIDesc.m_tUIShaderDesc.vTintColor, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_bUseCoolTime", &bUseCoolTime, sizeof(_bool))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fCoolAmount", &m_fCoolAmount, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUIRushSlot::Execute(const UI_EVENT_DESC& EventDesc)
{
	const _wstring& Type = EventDesc.szTypeTag;
	const _wstring& Arg = EventDesc.szArg;
	const _wstring& ActionTag = EventDesc.szActionTag;

	// 애니메이션
	if (Type == TEXT("PlayAnimEvent"))
	{
		CUIHUD* pHUD = dynamic_cast<CUIHUD*>(m_pGameInstance->GetCurrentLevelHUD());
		auto AnimTag = m_tUIDesc.m_AnimTags.find(ActionTag);

		if (AnimTag != m_tUIDesc.m_AnimTags.end())
			pHUD->Anim_Play(m_tUIDesc.szLayerTag, m_tUIDesc.szUITag, AnimTag->first);

		Safe_Release(pHUD);
	}
	if (Type == TEXT("ActionEvent"))
	{
	}

	return S_OK;
}

void CUIRushSlot::CallbackEvent(void* pArg)
{
	auto* arg = static_cast<UI_EVENT_ARG_DESC*>(pArg);
	if (!arg) return;

	auto* state = static_cast<UI_SKILL_INFO_DESC*>(arg->pData);
	
	m_tRushInfo = *state;

	/*CUIHUD* pHUD = dynamic_cast<CUIHUD*>(m_pGameInstance->GetCurrentLevelHUD());
	
	if (*state == SKILL_STATE::DEFAULT)
	{
		pHUD->Anim_Stop(m_tUIDesc.szLayerTag, m_tUIDesc.szUITag);
		Safe_Release(pHUD);
		return;
	}

	auto AnimTag = m_tUIDesc.m_AnimTags.find(arg->szActionTag);
	if (AnimTag != m_tUIDesc.m_AnimTags.end())
		pHUD->Anim_Play(m_tUIDesc.szLayerTag, m_tUIDesc.szUITag, AnimTag->first);

	Safe_Release(pHUD);*/
}

HRESULT CUIRushSlot::Render_Glow()
{
	if (FAILED(Bind_GlowShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(UI_SHADER_PASS::RUSH_SLOT_GLOW))))
		return E_FAIL;

	if (FAILED(m_pVIGlowBufferCom->Bind_Resources()))
		return E_FAIL;

	if (FAILED(m_pVIGlowBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUIRushSlot::Bind_GlowShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->GetIdentityMatrixPtr())))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;
	
	/*if(m_eRushState == SKILL_STATE::ACTIVE_ON)
		if (FAILED(m_pGlowTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture0", 0)))
			return E_FAIL;
	if (m_eRushState == SKILL_STATE::ACTIVE)
	{
		if (FAILED(m_pGlowTextureCom2->Bind_ShaderResource(m_pShaderCom, "g_Texture0", 0)))
			return E_FAIL;
		if (FAILED(m_pShadowTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture1", 1)))
			return E_FAIL;
	}*/

	if (FAILED(m_pGlowTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture0", 0)))
		return E_FAIL;
	if (FAILED(m_pShadowTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture1", 1)))
		return E_FAIL;

	if (m_bRushActiveOn)
	{
		if (FAILED(m_pRushGlowTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture2", 0)))
			return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_bRushActiveOn", &m_bRushActiveOn, sizeof(_bool))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_bUseGlow", &m_tUIDesc.m_tUIShaderDesc.bUseGlow, sizeof(_bool))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_GlowIntensity", &m_tUIDesc.m_tUIShaderDesc.fGlowIntensity, sizeof(_float))))
		return E_FAIL;
	/*if (FAILED(m_pGlowTextureCom2->Bind_ShaderResource(m_pGlowShaderCom, "g_GlowTexture2", 0)))
		return E_FAIL;*/

	return S_OK;
}

CUIRushSlot* CUIRushSlot::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUIRushSlot* pInstance = new CUIRushSlot(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUIRushSlot::Clone(void* pArg)
{
	CUIRushSlot* pInstance = new CUIRushSlot(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUIRushSlot");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUIRushSlot::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom2);
	Safe_Release(m_pCoolTimeTextureCom);
	Safe_Release(m_pShadowTextureCom);
	Safe_Release(m_pGlowTextureCom);
	Safe_Release(m_pGlowTextureCom2);
	Safe_Release(m_pRushGlowTextureCom);
	Safe_Release(m_pVIBaseBufferCom);
	Safe_Release(m_pVIGlowBufferCom);
}
