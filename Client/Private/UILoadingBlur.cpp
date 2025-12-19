#include "pch.h"
#include "UILoadingBlur.h"

#include "GameInstance.h"
#include "GameManager.h"

CUILoadingBlur::CUILoadingBlur(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIBase{ pDevice, pContext }
{
}

CUILoadingBlur::CUILoadingBlur(const CUILoadingBlur& Prototype) 
	: CUIBase{ Prototype }
{
}

HRESULT CUILoadingBlur::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUILoadingBlur::Initialize(void* pArg)
{	
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CUILoadingBlur::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CUILoadingBlur::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void CUILoadingBlur::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
}

HRESULT CUILoadingBlur::Render()
{
	__super::Render();

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(UI_SHADER_PASS::LOADING_BLUR))))
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

HRESULT CUILoadingBlur::Ready_Components()
{
	__super::Ready_Components();

	/* Com_VIBuffer */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* Com_Texture_UI_Potion */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::LOADING), TEXT("Prototype_Component_UI_Texture_LoadingBG"),
		TEXT("Com_Texture_UI_Loading_FX"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUILoadingBlur::Bind_ShaderResources()
{
	//__super::Bind_ShaderResources();

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->GetIdentityMatrixPtr())))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;
	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture0", 1)))
		return E_FAIL;
	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture1", 0)))
		return E_FAIL;

	if(FAILED(m_pShaderCom->Bind_RawValue("g_bUseGlow", &m_tUIDesc.m_tUIShaderDesc.bUseGlow, sizeof(_bool))))
		return E_FAIL;
	if(FAILED(m_pShaderCom->Bind_RawValue("g_GlowIntensity", &m_tUIDesc.m_tUIShaderDesc.fGlowIntensity, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUILoadingBlur::Execute(const UI_EVENT_DESC& EventDesc)
{
	return S_OK;
}

void CUILoadingBlur::CallbackEvent(void* pArg)
{
}

CUILoadingBlur* CUILoadingBlur::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUILoadingBlur* pInstance = new CUILoadingBlur(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUILoadingBlur::Clone(void* pArg)
{
	CUILoadingBlur* pInstance = new CUILoadingBlur(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUILoadingBlur");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUILoadingBlur::Free()
{
	__super::Free();
}
