#include "pch.h"
#include "UISkillWrapperLineFX.h"

#include "GameInstance.h"
#include "UIHUD.h"

CUISkillWrapperLineFX::CUISkillWrapperLineFX(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIBase{ pDevice, pContext }
{
}

CUISkillWrapperLineFX::CUISkillWrapperLineFX(const CUISkillWrapperLineFX& Prototype) 
	: CUIBase{ Prototype }
{
}

HRESULT CUISkillWrapperLineFX::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUISkillWrapperLineFX::Initialize(void* pArg)
{	
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CUISkillWrapperLineFX::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CUISkillWrapperLineFX::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void CUISkillWrapperLineFX::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

}

HRESULT CUISkillWrapperLineFX::Render()
{
	__super::Render();

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(UI_SHADER_PASS::SKILL_WRAPPER_ON_LINE))))
		return E_FAIL;

	if (FAILED(m_pVIBaseBufferCom->Bind_Resources()))
		return E_FAIL;

	if (FAILED(m_pVIBaseBufferCom->Render()))
		return E_FAIL;

#ifdef _DEBUG
	__super::Render_Debug_Rect();
#endif

	return S_OK;
}

HRESULT CUISkillWrapperLineFX::Ready_Components()
{
	__super::Ready_Components();

	/* Com_VIBuffer */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect_Instance"),
		TEXT("Com_VIBaseBuffer"), reinterpret_cast<CComponent**>(&m_pVIBaseBufferCom))))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Skill_Line_Fx"),
		TEXT("Com_Texture_FX"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUISkillWrapperLineFX::Bind_ShaderResources()
{
	/*if(FAILED(__super::Bind_ShaderResources()))
		return E_FAIL;*/

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->GetIdentityMatrixPtr())))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;
	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture0", 0)))
		return E_FAIL;
	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture1", 1)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_GlowIntensity", &m_tUIDesc.m_tUIShaderDesc.fGlowIntensity, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_Alpha", &m_tUIDesc.fAlpha, sizeof(_float))))
		return E_FAIL;

	/*if (FAILED(m_pShaderCom->Bind_RawValue("g_bUseGlow", &m_tUIDesc.m_tUIShaderDesc.bUseGlow, sizeof(_bool))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_GlowIntensity", &m_tUIDesc.m_tUIShaderDesc.fGlowIntensity, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_GlowSpread", &m_tUIDesc.m_tUIShaderDesc.fGlowSpread, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_bUseTintColor", &m_tUIDesc.m_tUIShaderDesc.bUseTintColor, sizeof(_bool))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vTintColor", &m_tUIDesc.m_tUIShaderDesc.vTintColor, sizeof(_float4))))
		return E_FAIL;*/

	return S_OK;
}

HRESULT CUISkillWrapperLineFX::Execute(const UI_EVENT_DESC& EventDesc)
{
	return S_OK;
}

void CUISkillWrapperLineFX::CallbackEvent(void* pArg)
{
	auto* arg = static_cast<UI_EVENT_ARG_DESC*>(pArg);
	if (!arg) return;

	CUIHUD* pHUD = dynamic_cast<CUIHUD*>(m_pGameInstance->GetCurrentLevelHUD());
		
	auto AnimTag = m_tUIDesc.m_AnimTags.find(arg->szActionTag);
	if (AnimTag != m_tUIDesc.m_AnimTags.end())
		pHUD->Anim_Play(m_tUIDesc.szLayerTag, m_tUIDesc.szUITag, AnimTag->first);

	Safe_Release(pHUD);
}


CUISkillWrapperLineFX* CUISkillWrapperLineFX::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUISkillWrapperLineFX* pInstance = new CUISkillWrapperLineFX(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUISkillWrapperLineFX::Clone(void* pArg)
{
	CUISkillWrapperLineFX* pInstance = new CUISkillWrapperLineFX(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUISkillWrapperLineFX");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUISkillWrapperLineFX::Free()
{
	__super::Free();

	Safe_Release(m_pVIBaseBufferCom);
}
