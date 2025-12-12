#include "pch.h"
#include "UISkillSlot.h"

#include "GameInstance.h"
#include "GameManager.h"
#include "Player.h"

#include "UIHUD.h"
#include "UISkillWrapper.h"

CUISkillSlot::CUISkillSlot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIBase{ pDevice, pContext }
{
}

CUISkillSlot::CUISkillSlot(const CUISkillSlot& Prototype) 
	: CUIBase{ Prototype }
{
}

HRESULT CUISkillSlot::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUISkillSlot::Initialize(void* pArg)
{	
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	// 임시 초기화
	if(m_tUIDesc.szUITag == TEXT("SkillSlot_0"))
		m_tSkillInfo.iSkillIndex = 0;
	else if (m_tUIDesc.szUITag == TEXT("SkillSlot_1"))
		m_tSkillInfo.iSkillIndex = 1;
	else if (m_tUIDesc.szUITag == TEXT("SkillSlot_2"))
		m_tSkillInfo.iSkillIndex = 2;
	else if (m_tUIDesc.szUITag == TEXT("SkillSlot_3"))
		m_tSkillInfo.iSkillIndex = 3;

	m_tSkillInfo.iSkillState = ENUM_CLASS(SKILL_STATE::DEFAULT);

	return S_OK;
}

void CUISkillSlot::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CUISkillSlot::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);	

#ifdef _DEBUG
#endif
}

void CUISkillSlot::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	if (m_tSkillInfo.iPrevSkillState == ENUM_CLASS(SKILL_STATE::DEFAULT)
		&& m_tSkillInfo.iSkillState == ENUM_CLASS(SKILL_STATE::DEFAULT)
		&& m_bPlayingAnim == false)
	{
		CUIHUD* pHUD = dynamic_cast<CUIHUD*>(m_pGameInstance->GetCurrentLevelHUD());
		pHUD->Anim_Stop(m_tUIDesc.szLayerTag, m_tUIDesc.szUITag);
		Safe_Release(pHUD);
	}
	else if(m_tSkillInfo.iPrevSkillState == ENUM_CLASS(SKILL_STATE::DEFAULT)
		&& m_tSkillInfo.iSkillState == ENUM_CLASS(SKILL_STATE::ACTIVE_ON)
		&& m_bPlayingAnim == false)
	{
		CUIHUD* pHUD = dynamic_cast<CUIHUD*>(m_pGameInstance->GetCurrentLevelHUD());

		pHUD->Anim_Stop(m_tUIDesc.szLayerTag, m_tUIDesc.szUITag);

		auto AnimTag = m_tUIDesc.m_AnimTags.find(TEXT("Skill_") + to_wstring(m_tSkillInfo.iSkillIndex) + TEXT("_ActiveOn"));
		if (AnimTag != m_tUIDesc.m_AnimTags.end())
			pHUD->Anim_Play(m_tUIDesc.szLayerTag, m_tUIDesc.szUITag, AnimTag->first);

		Safe_Release(pHUD);
	}
	else if((m_tSkillInfo.iPrevSkillState == ENUM_CLASS(SKILL_STATE::ACTIVE_ON)
			|| m_tSkillInfo.iPrevSkillState == ENUM_CLASS(SKILL_STATE::USE))
		&& m_tSkillInfo.iSkillState == ENUM_CLASS(SKILL_STATE::ACTIVE)
		&& m_bPlayingAnim == false)
	{
		CUIHUD* pHUD = dynamic_cast<CUIHUD*>(m_pGameInstance->GetCurrentLevelHUD());

		auto AnimTag = m_tUIDesc.m_AnimTags.find(TEXT("Skill_") + to_wstring(m_tSkillInfo.iSkillIndex) + TEXT("_Active"));
		if (AnimTag != m_tUIDesc.m_AnimTags.end())
			pHUD->Anim_Play(m_tUIDesc.szLayerTag, m_tUIDesc.szUITag, AnimTag->first);

		Safe_Release(pHUD);
	}
	else if(m_tSkillInfo.iPrevSkillState == ENUM_CLASS(SKILL_STATE::ACTIVE)
		&& m_tSkillInfo.iSkillState == ENUM_CLASS(SKILL_STATE::USE)
		&& m_bPlayingAnim == true)
	{
		CUIHUD* pHUD = dynamic_cast<CUIHUD*>(m_pGameInstance->GetCurrentLevelHUD());
		pHUD->Anim_Stop(m_tUIDesc.szLayerTag, m_tUIDesc.szUITag);

		auto AnimTag = m_tUIDesc.m_AnimTags.find(TEXT("Skill_") + to_wstring(m_tSkillInfo.iSkillIndex) + TEXT("_Use"));
		if (AnimTag != m_tUIDesc.m_AnimTags.end())
			pHUD->Anim_Play(m_tUIDesc.szLayerTag, m_tUIDesc.szUITag, AnimTag->first);

		Safe_Release(pHUD);
	}
}

HRESULT CUISkillSlot::Render()
{
	__super::Render();

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(UI_SHADER_PASS::SKILL_SLOT))))
		return E_FAIL;

	if (FAILED(m_pVIBaseBufferCom->Bind_Resources()))
		return E_FAIL;

	if (FAILED(m_pVIBaseBufferCom->Render()))
		return E_FAIL;

	if (m_tSkillInfo.iSkillState != ENUM_CLASS(SKILL_STATE::DEFAULT))
	{
		if (FAILED(Render_Glow()))
			return E_FAIL;
	}
	else
	{
		m_tUIDesc.m_tUIShaderDesc = m_tOriginUIDesc.m_tUIShaderDesc;
	}

#ifdef _DEBUG
	__super::Render_Debug_Rect();
#endif

	return S_OK;
}

HRESULT CUISkillSlot::Ready_Components()
{
	__super::Ready_Components();

	/* Com_VIBuffer */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect_Instance"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBaseBufferCom))))
		return E_FAIL;

	/* Com_VIBuffer */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect_Instance"),
		TEXT("Com_VIBuffer_Glow"), reinterpret_cast<CComponent**>(&m_pVIGlowBufferCom))))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_SkillFrame_Shadow"),
		TEXT("Com_ShadowTexture"), reinterpret_cast<CComponent**>(&m_pShadowTextureCom))))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_SkillFrame"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_On_Ring"),
		TEXT("Com_GlowTexture"), reinterpret_cast<CComponent**>(&m_pGlowTextureCom))))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Skill_Focus_Glow"),
		TEXT("Com_GlowTexture2"), reinterpret_cast<CComponent**>(&m_pGlowTextureCom2))))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Skills"),
		TEXT("Com_Texture2"), reinterpret_cast<CComponent**>(&m_pTextureCom2))))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Skill_Slot_Covor"),
		TEXT("Com_Texture_UI_Skill_Slot_Covor"), reinterpret_cast<CComponent**>(&m_pCoverTextureCom))))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Number"),
		TEXT("Com_Texture_Cost"), reinterpret_cast<CComponent**>(&m_pCostTextureCom))))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Beta_Cost_Deco"),
		TEXT("Com_Texture_CostDeco"), reinterpret_cast<CComponent**>(&m_pCostDecoTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUISkillSlot::Bind_ShaderResources()
{
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

	_bool bSkillUseable = false;

	/*if (FAILED(m_pShaderCom->Bind_SRV("g_Texture2", nullptr)))
		return E_FAIL;*/

	if (static_cast<CUISkillWrapper*>(m_pParent)->Get_SkillID(m_tSkillInfo.iSkillIndex) != 0)
	{
		bSkillUseable = true;
		_uint iGauge = m_pGameManager->Find_BetaSkillData(static_cast<CUISkillWrapper*>(m_pParent)->Get_SkillID(m_tSkillInfo.iSkillIndex))->iRequiredBetaGauge;

		switch (static_cast<CUISkillWrapper*>(m_pParent)->Get_SkillID(m_tSkillInfo.iSkillIndex))
		{
		case 1004:
		{
			if (FAILED(m_pTextureCom2->Bind_ShaderResource(m_pShaderCom, "g_Texture2", 0))) // 스킬 텍스쳐 : 플레이어한테서 받아야함
				return E_FAIL;
			break;
		}
		case 1005:
		{
			if (FAILED(m_pTextureCom2->Bind_ShaderResource(m_pShaderCom, "g_Texture2", 1))) // 스킬 텍스쳐 : 플레이어한테서 받아야함
				return E_FAIL;
			break;
		}
		}

		if (FAILED(m_pCostTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture4", (iGauge / 4)))) // 스킬 텍스쳐 : 플레이어한테서 받아야함
			return E_FAIL;
		if (FAILED(m_pCostDecoTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture5", 0))) // 스킬 텍스쳐 : 플레이어한테서 받아야함
			return E_FAIL;
	}

	_bool bUseTintColor = false;
	_bool bUseCoverTexture = false;

	if (m_tSkillInfo.iSkillState == ENUM_CLASS(SKILL_STATE::DEFAULT))
	{
		bUseTintColor = true;
		bUseCoverTexture = false;
		if (FAILED(m_pShaderCom->Bind_SRV("g_Texture3", nullptr)))
			return E_FAIL;
	}
	if ((m_tSkillInfo.iPrevSkillState == ENUM_CLASS(SKILL_STATE::ACTIVE_ON)
		|| m_tSkillInfo.iPrevSkillState == ENUM_CLASS(SKILL_STATE::USE))
		&& m_tSkillInfo.iSkillState == ENUM_CLASS(SKILL_STATE::ACTIVE)
		&& m_bPlayingAnim == false)
	{
		bUseCoverTexture = false;
		if (FAILED(m_pShaderCom->Bind_SRV("g_Texture3", nullptr)))
			return E_FAIL;
	}

	if (m_tSkillInfo.iPrevSkillState == ENUM_CLASS(SKILL_STATE::ACTIVE)
		&& m_tSkillInfo.iSkillState == ENUM_CLASS(SKILL_STATE::USE)
		&& m_bPlayingAnim == true)
	{
		bUseCoverTexture = true;

		if (FAILED(m_pCoverTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture3", 0)))
			return E_FAIL;
	}

	
	if (FAILED(m_pShaderCom->Bind_RawValue("g_Alpha", &m_tUIDesc.fAlpha, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_UseCover", &bUseCoverTexture, sizeof(_bool))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_isUseable", &bSkillUseable, sizeof(_bool))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_bUseTintColor", &bUseTintColor, sizeof(_bool))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vTintColor", &m_tUIDesc.m_tUIShaderDesc.vTintColor, sizeof(_float4))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUISkillSlot::Execute(const UI_EVENT_DESC& EventDesc)
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

	return S_OK;
}

void CUISkillSlot::CallbackEvent(void* pArg)
{
	auto* arg = static_cast<UI_EVENT_ARG_DESC*>(pArg);
	if (!arg) return;

	auto* pDesc = static_cast<UI_SKILL_INFO_DESC*>(arg->pData);
	
	m_tSkillInfo = *pDesc;
}

HRESULT CUISkillSlot::Render_Glow()
{
	if (FAILED(Bind_GlowShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(UI_SHADER_PASS::SKILL_SLOT_GLOW))))
		return E_FAIL;

	if (FAILED(m_pVIGlowBufferCom->Bind_Resources()))
		return E_FAIL;

	if (FAILED(m_pVIGlowBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUISkillSlot::Bind_GlowShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->GetIdentityMatrixPtr())))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;
	if (FAILED(m_pGlowTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture0", 0)))
		return E_FAIL;
	if (FAILED(m_pShadowTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture1", 1)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_bUseGlow", &m_tUIDesc.m_tUIShaderDesc.bUseGlow, sizeof(_bool))))
		return E_FAIL;

	//m_tUIDesc.m_tUIShaderDesc.fGlowIntensity = 1.f;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_GlowIntensity", &m_tUIDesc.m_tUIShaderDesc.fGlowIntensity, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

CUISkillSlot* CUISkillSlot::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUISkillSlot* pInstance = new CUISkillSlot(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUISkillSlot::Clone(void* pArg)
{
	CUISkillSlot* pInstance = new CUISkillSlot(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUISkillSlot");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUISkillSlot::Free()
{
	__super::Free();

	Safe_Release(m_pCoverTextureCom);
	Safe_Release(m_pTextureCom2);
	Safe_Release(m_pShadowTextureCom);
	Safe_Release(m_pGlowTextureCom);
	Safe_Release(m_pGlowTextureCom2);
	Safe_Release(m_pCostTextureCom);
	Safe_Release(m_pCostDecoTextureCom);
	Safe_Release(m_pVIBaseBufferCom);
	Safe_Release(m_pVIGlowBufferCom);
}
