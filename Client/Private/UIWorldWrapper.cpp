#include "pch.h"
#include "UIWorldWrapper.h"

#include "GameInstance.h"
#include "GameManager.h"
#include "Interaction_Component.h"

CUIWorldWrapper::CUIWorldWrapper(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIBase{ pDevice, pContext }
{
}

CUIWorldWrapper::CUIWorldWrapper(const CUIWorldWrapper& Prototype) 
	: CUIBase{ Prototype }
{
}

HRESULT CUIWorldWrapper::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUIWorldWrapper::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CUIWorldWrapper::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CUIWorldWrapper::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void CUIWorldWrapper::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
}

HRESULT CUIWorldWrapper::Render()
{
	__super::Render();

	/*if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(UI_SHADER_PASS::UI))))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Resources()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;*/

#ifdef _DEBUG
	__super::Render_Debug_Rect();
#endif

	return S_OK;
}

HRESULT CUIWorldWrapper::Ready_Components()
{
	__super::Ready_Components();

	///* Com_VIBaseBuffer */
	//if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
	//	TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
	//	return E_FAIL;

	///* Com_Texture_HP */
	//if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Btn_Empty"),
	//	TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CUIWorldWrapper::Bind_ShaderResources()
{
	/*if (FAILED(__super::Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->GetIdentityMatrixPtr())))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;
	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;*/

	return S_OK;
}

HRESULT CUIWorldWrapper::Execute(const UI_EVENT_DESC& EventDesc)
{
	return S_OK;
}

void CUIWorldWrapper::CallbackEvent(void* pArg)
{
}

CUIWorldWrapper* CUIWorldWrapper::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUIWorldWrapper* pInstance = new CUIWorldWrapper(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUIWorldWrapper::Clone(void* pArg)
{
	CUIWorldWrapper* pInstance = new CUIWorldWrapper(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUIWorldWrapper");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUIWorldWrapper::Free()
{
	__super::Free();

	Safe_Release(m_pInteractionCom);
}
