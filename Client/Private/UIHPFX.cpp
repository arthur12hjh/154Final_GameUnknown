#include "pch.h"
#include "UIHPFX.h"

#include "GameInstance.h"

#include "UIHPBar.h"

CUIHPFX::CUIHPFX(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIBase{ pDevice, pContext }
{
}

CUIHPFX::CUIHPFX(const CUIHPFX& Prototype) 
	: CUIBase{ Prototype }
{
}

HRESULT CUIHPFX::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUIHPFX::Initialize(void* pArg)
{	
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CUIHPFX::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CUIHPFX::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

	//float width = m_tUIDesc.fSizeX;

	//// Glow의 기본 위치(start)
	//float startX = -width * 0.5f;

	//// 이동할 X값
	//float posX = startX + (width * 0.5f) * dynamic_cast<CUIHPBar*>(m_pParent)->Get_CurrentFill();

	////m_tUIDesc.fOffsetX *= posX;

	//m_pTransformCom->Set_State((STATE::POSITION), XMVectorSet(
	//	XMVectorGetX(m_pTransformCom->Get_State(STATE::POSITION)) + (posX + (width * -0.5f)),
	//	XMVectorGetY(m_pTransformCom->Get_State(STATE::POSITION)),
	//	XMVectorGetZ(m_pTransformCom->Get_State(STATE::POSITION)),
	//	0.f
	//));
}

void CUIHPFX::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

}

HRESULT CUIHPFX::Render()
{
	__super::Render();

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(3)))
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

HRESULT CUIHPFX::Ready_Components()
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

HRESULT CUIHPFX::Bind_ShaderResources()
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

HRESULT CUIHPFX::Execute(const UI_EVENT_DESC& EventDesc)
{
	return S_OK;
}

void CUIHPFX::CallbackEvent(void* pArg)
{
}


CUIHPFX* CUIHPFX::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUIHPFX* pInstance = new CUIHPFX(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUIHPFX::Clone(void* pArg)
{
	CUIHPFX* pInstance = new CUIHPFX(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUIHPFX");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUIHPFX::Free()
{
	__super::Free();
}
