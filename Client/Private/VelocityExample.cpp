#include "pch.h"
#include "VelocityExample.h"

#include "GameInstance.h"

CVelocityExample::CVelocityExample(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject { pDevice, pContext }
{
}

CVelocityExample::CVelocityExample(const CVelocityExample& Prototype)
    : CGameObject { Prototype }
{
}

HRESULT CVelocityExample::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CVelocityExample::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CVelocityExample::Priority_Update(_float fTimeDelta)
{
}

void CVelocityExample::Update(_float fTimeDelta)
{
}

void CVelocityExample::Late_Update(_float fTimeDelta)
{
	//m_pGameInstance->Add_RenderGroup(RENDER::NONLIGHT, this);
	m_pGameInstance->Add_RenderGroup(RENDER::MOTIONBLUR, this);
}

HRESULT CVelocityExample::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Resources()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CVelocityExample::Render_MotionBlur()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(4)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Resources()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CVelocityExample::Ready_Components()
{
	/* Com_VIBuffer */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Snow"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* Com_Shader */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;


    return S_OK;
}

HRESULT CVelocityExample::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", static_cast<_uint>(0))))
		return E_FAIL;

	return S_OK;
}

CVelocityExample* CVelocityExample::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CVelocityExample* pInstance = new CVelocityExample(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Create Failed : Velcoity Example");
	}
    return pInstance;
}

CGameObject* CVelocityExample::Clone(void* pArg)
{
	CVelocityExample* pInstance = new CVelocityExample(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Clone Failed : Velcoity Example");
	}
	return pInstance;
}

void CVelocityExample::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
}
