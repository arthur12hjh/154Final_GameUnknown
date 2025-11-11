#include "pch.h"
#include "UIButton.h"

#include "GameInstance.h"

CUIButton::CUIButton(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIBase{ pDevice, pContext }
{
}

CUIButton::CUIButton(const CUIButton& Prototype) 
	: CUIBase{ Prototype }
{
}

HRESULT CUIButton::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUIButton::Initialize(void* pArg)
{
	//CUIObject::UIOBJECT_DESC	Desc = *static_cast<CUIObject::UIOBJECT_DESC*>(pArg);

	/*Desc.fX = g_iWinSizeX >> 1;
	Desc.fY = g_iWinSizeY >> 1;
	Desc.fSizeX = 100.f;
	Desc.fSizeY = 100.f;*/
	
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CUIButton::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CUIButton::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void CUIButton::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
}

HRESULT CUIButton::Render()
{
	if (m_tUIDesc.Get_UI_Texture_Desc())
	{
		if (FAILED(Bind_ShaderResources()))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(m_tUIDesc.Get_UI_Texture_Desc()->iPass)))
			return E_FAIL;

		if (FAILED(m_pVIBufferCom->Bind_Resources()))
			return E_FAIL;

		if (FAILED(m_pVIBufferCom->Render()))
			return E_FAIL;
	}

	__super::Render();

	return S_OK;
}

HRESULT CUIButton::Ready_Components()
{
	/* Com_VIBuffer */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	///* Com_Texture */
	//if (FAILED(__super::Add_Component(m_tUIDesc.iLevel, m_tUIDesc.szTextureComTag,
	//	TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
	//	return E_FAIL;

	/* Com_Shader */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUIButton::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->GetIdentityMatrixPtr())))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;
	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_tUIDesc.Get_UI_Texture_Desc()->iTextureIndex)))
		return E_FAIL;

	return S_OK;
}

CUIButton* CUIButton::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUIButton* pInstance = new CUIButton(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUIButton::Clone(void* pArg)
{
	CUIButton* pInstance = new CUIButton(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUIButton");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUIButton::Free()
{
	__super::Free();
}
