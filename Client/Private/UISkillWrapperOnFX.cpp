#include "pch.h"
#include "UISkillWrapperOnFX.h"

#include "GameInstance.h"
#include "UIHUD.h"

CUISkillWrapperOnFX::CUISkillWrapperOnFX(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIBase{ pDevice, pContext }
{
}

CUISkillWrapperOnFX::CUISkillWrapperOnFX(const CUISkillWrapperOnFX& Prototype) 
	: CUIBase{ Prototype }
{
}

HRESULT CUISkillWrapperOnFX::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUISkillWrapperOnFX::Initialize(void* pArg)
{	
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CUISkillWrapperOnFX::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);

	if (m_tUIDesc.fAlpha <= 0.f)
		m_eVisibility = VISIBILITY::HIDDEN;
	else
		m_eVisibility = VISIBILITY::VISIBLE;
}

void CUISkillWrapperOnFX::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void CUISkillWrapperOnFX::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

}

HRESULT CUISkillWrapperOnFX::Render()
{
	__super::Render();

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(UI_SHADER_PASS::SKILL_WRAPPER_ON_FX))))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Resources()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

#ifdef _DEBUG
	__super::Render_Debug_Rect();
#endif

	return S_OK;
}

HRESULT CUISkillWrapperOnFX::Ready_Components()
{
	__super::Ready_Components();

	/* Com_VIBuffer */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_SkillWrapper_On_Fx"),
		TEXT("Com_Texture_FX"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUISkillWrapperOnFX::Bind_ShaderResources()
{
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

	return S_OK;
}

HRESULT CUISkillWrapperOnFX::Execute(const UI_EVENT_DESC& EventDesc)
{
	return S_OK;
}

void CUISkillWrapperOnFX::CallbackEvent(void* pArg)
{
	auto* arg = static_cast<UI_EVENT_ARG_DESC*>(pArg);
	if (!arg) return;

	CUIHUD* pHUD = dynamic_cast<CUIHUD*>(m_pGameInstance->GetCurrentLevelHUD());

	auto AnimTag = m_tUIDesc.m_AnimTags.find(arg->szActionTag);
	if (AnimTag != m_tUIDesc.m_AnimTags.end())
		pHUD->Anim_Play(m_tUIDesc.szLayerTag, m_tUIDesc.szUITag, AnimTag->first);
	

	Safe_Release(pHUD);
}


CUISkillWrapperOnFX* CUISkillWrapperOnFX::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUISkillWrapperOnFX* pInstance = new CUISkillWrapperOnFX(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUISkillWrapperOnFX::Clone(void* pArg)
{
	CUISkillWrapperOnFX* pInstance = new CUISkillWrapperOnFX(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUISkillWrapperOnFX");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUISkillWrapperOnFX::Free()
{
	__super::Free();
}
