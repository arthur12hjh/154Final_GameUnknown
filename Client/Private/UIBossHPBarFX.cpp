#include "pch.h"
#include "UIBossHPBarFX.h"

#include "GameInstance.h"

#include "UIBossHPBar.h"

CUIBossHPBarFX::CUIBossHPBarFX(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIBase{ pDevice, pContext }
{
}

CUIBossHPBarFX::CUIBossHPBarFX(const CUIBossHPBarFX& Prototype) 
	: CUIBase{ Prototype }
{
}

HRESULT CUIBossHPBarFX::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUIBossHPBarFX::Initialize(void* pArg)
{	
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CUIBossHPBarFX::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CUIBossHPBarFX::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

	auto pHPBar = dynamic_cast<CUIBossHPBar*>(m_pParent);
	if (!pHPBar) return;

	_float fill = pHPBar->Get_CurrentFill();
	_float width = pHPBar->Get_UIBase_Desc().fSizeX;

	// pivot = center 기준 Fill 이동 공식
	_float posX = (width * (fill - 0.5f));

	// 부모 기준 위치
	_vector vParentPos = pHPBar->GetTransform()->Get_State(STATE::POSITION);

	// Glow 중심 위치 = Parent 중심 + posX
	_vector vGlowPos = XMVectorSet(
		XMVectorGetX(vParentPos) + posX,
		XMVectorGetY(vParentPos),
		XMVectorGetZ(vParentPos),
		1.f
	);

	m_pTransformCom->Set_State(STATE::POSITION, vGlowPos);
}

void CUIBossHPBarFX::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

}

HRESULT CUIBossHPBarFX::Render()
{
	__super::Render();

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(UI_SHADER_PASS::GLOWFX))))
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

HRESULT CUIBossHPBarFX::Ready_Components()
{
	__super::Ready_Components();

	/* Com_VIBuffer */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Player_Hp_FX"),
		TEXT("Com_Texture_FX"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUIBossHPBarFX::Bind_ShaderResources()
{
	/*if(FAILED(__super::Bind_ShaderResources()))
		return E_FAIL;*/

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->GetIdentityMatrixPtr())))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;
	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_bUseGlow", &m_tUIDesc.m_tUIShaderDesc.bUseGlow, sizeof(_bool))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_GlowIntensity", &m_tUIDesc.m_tUIShaderDesc.fGlowIntensity, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_GlowSpread", &m_tUIDesc.m_tUIShaderDesc.fGlowSpread, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_bUseTintColor", &m_tUIDesc.m_tUIShaderDesc.bUseTintColor, sizeof(_bool))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vTintColor", &m_tUIDesc.m_tUIShaderDesc.vTintColor, sizeof(_float4))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUIBossHPBarFX::Execute(const UI_EVENT_DESC& EventDesc)
{
	return S_OK;
}

void CUIBossHPBarFX::CallbackEvent(void* pArg)
{
}


CUIBossHPBarFX* CUIBossHPBarFX::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUIBossHPBarFX* pInstance = new CUIBossHPBarFX(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUIBossHPBarFX::Clone(void* pArg)
{
	CUIBossHPBarFX* pInstance = new CUIBossHPBarFX(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUIBossHPBarFX");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUIBossHPBarFX::Free()
{
	__super::Free();
}
