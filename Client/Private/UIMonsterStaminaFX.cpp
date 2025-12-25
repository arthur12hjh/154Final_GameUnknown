#include "pch.h"
#include "UIMonsterStaminaFX.h"

#include "GameInstance.h"
#include "UIHUD.h"

#include "UIMonsterStamina.h"

CUIMonsterStaminaFX::CUIMonsterStaminaFX(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIBase{ pDevice, pContext }
{
}

CUIMonsterStaminaFX::CUIMonsterStaminaFX(const CUIMonsterStaminaFX& Prototype) 
	: CUIBase{ Prototype }
{
}

HRESULT CUIMonsterStaminaFX::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUIMonsterStaminaFX::Initialize(void* pArg)
{	
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CUIMonsterStaminaFX::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CUIMonsterStaminaFX::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

	
}

void CUIMonsterStaminaFX::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	if (static_cast<CUIMonsterStamina*>(m_pParent))
		m_iCurrentStamina = static_cast<CUIMonsterStamina*>(m_pParent)->Get_CurrentStamina();

	auto pStamina = static_cast<CUIMonsterStamina*>(m_pParent);
	if (!pStamina) return;

	_float fill = pStamina->Get_CurrentFill() - ((2.f / pStamina->Get_UIBase_Desc().fSizeX) * 2.f);
	_float width = pStamina->Get_UIBase_Desc().fSizeX;

	// pivot = center 기준 Fill 이동 공식
	_float posX = (width * (fill - 0.5f));

	// 부모 기준 위치
	_vector vParentPos = pStamina->GetTransform()->Get_State(STATE::POSITION);

	// Glow 중심 위치 = Parent 중심 + posX
	_vector vGlowPos = XMVectorSet(
		XMVectorGetX(vParentPos) + posX,
		XMVectorGetY(vParentPos),
		XMVectorGetZ(vParentPos),
		1.f
	);

	m_pTransformCom->Set_State(STATE::POSITION, vGlowPos);

	if (m_iCurrentStamina != m_iPrevStamina && m_iCurrentStamina < m_iPrevStamina)
	{
		CUIHUD* pHUD = dynamic_cast<CUIHUD*>(m_pGameInstance->GetCurrentLevelHUD());

		auto AnimTag = m_tUIDesc.m_AnimTags.find(TEXT("Stamina_FX_") + to_wstring(m_iCloneIdx));
		if (AnimTag != m_tUIDesc.m_AnimTags.end())
			pHUD->Anim_Play(m_tUIDesc.szLayerTag, m_tUIDesc.szUITag, AnimTag->first);

		Safe_Release(pHUD);
	}

	m_iPrevStamina = m_iCurrentStamina;
}

HRESULT CUIMonsterStaminaFX::Render()
{
	__super::Render();

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(UI_SHADER_PASS::STAMINA_FX))))
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

HRESULT CUIMonsterStaminaFX::Ready_Components()
{
	__super::Ready_Components();

	/* Com_VIBuffer */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect_Instance"),
		TEXT("Com_VIBaseBuffer"), reinterpret_cast<CComponent**>(&m_pVIBaseBufferCom))))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Boss_StaminaFX"),
		TEXT("Com_Texture_UI_Boss_Stamina_FX"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Boss_Stamina"),
		TEXT("Com_Texture_UI_Boss_Stamina"), reinterpret_cast<CComponent**>(&m_pBaseTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUIMonsterStaminaFX::Bind_ShaderResources()
{
	if (FAILED(__super::Initialize_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->GetIdentityMatrixPtr())))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;
	if (FAILED(m_pBaseTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture2", 1)))
		return E_FAIL;
	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture0", 0)))
		return E_FAIL;
	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture1", 1)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_Alpha", &m_tUIDesc.fAlpha, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_bUseGlow", &m_tUIDesc.m_tUIShaderDesc.bUseGlow, sizeof(_bool))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_GlowIntensity", &m_tUIDesc.m_tUIShaderDesc.fGlowIntensity, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_bUseScale", &m_tUIDesc.m_tUIShaderDesc.bUseScale, sizeof(_bool))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fScale", &m_tUIDesc.m_tUIShaderDesc.fScale, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUIMonsterStaminaFX::Execute(const UI_EVENT_DESC& EventDesc)
{
	return S_OK;
}

void CUIMonsterStaminaFX::CallbackEvent(void* pArg)
{
	auto* arg = static_cast<UI_EVENT_ARG_DESC*>(pArg);
	if (!arg) return;

	CUIHUD* pHUD = dynamic_cast<CUIHUD*>(m_pGameInstance->GetCurrentLevelHUD());

	auto AnimTag = m_tUIDesc.m_AnimTags.find(arg->szActionTag);
	if (AnimTag != m_tUIDesc.m_AnimTags.end())
		pHUD->Anim_Play(m_tUIDesc.szLayerTag, m_tUIDesc.szUITag, AnimTag->first);

	Safe_Release(pHUD);
}

CUIMonsterStaminaFX* CUIMonsterStaminaFX::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUIMonsterStaminaFX* pInstance = new CUIMonsterStaminaFX(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUIMonsterStaminaFX::Clone(void* pArg)
{
	CUIMonsterStaminaFX* pInstance = new CUIMonsterStaminaFX(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUIMonsterStaminaFX");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUIMonsterStaminaFX::Free()
{
	__super::Free();

	Safe_Release(m_pVIBaseBufferCom);
	Safe_Release(m_pBaseTextureCom);
}
