#include "pch.h"
#include "Giwajip2.h"
#include "GameInstance.h"

CGiwajip2::CGiwajip2(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CStaticMap{ pDevice, pContext }
{
}

CGiwajip2::CGiwajip2(const CGiwajip2& Prototype)
	: CStaticMap{ Prototype }
{
}

HRESULT CGiwajip2::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CGiwajip2::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CGiwajip2::Priority_Update(_float fTimeDelta)
{
}

void CGiwajip2::Update(_float fTimeDelta)
{
	m_pColliderCom->UpdateColiision(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));
}

void CGiwajip2::Late_Update(_float fTimeDelta)
{
	if (true == m_pGameInstance->isIn_WorldFrustum(m_pColliderCom))
	{
		m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
	}
}

HRESULT CGiwajip2::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;


	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{

		if (FAILED(m_pModelCom->Bind_Material(i, m_pShaderCom, "g_DiffuseTexture", aiTextureType_DIFFUSE, 0)))
			return E_FAIL;
		if (FAILED(m_pModelCom->Bind_Material(i, m_pShaderCom, "g_NormalTexture", aiTextureType_NORMALS, 0)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;


		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CGiwajip2::Ready_Components()
{
	/* Com_Model */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_Giwajip2"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* Com_Shader */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* Com_Collider_Sphere */
	CSphereCollider::SPHERE_COLLIDER_DESC		SphereDesc{};

	SphereDesc.fRadius = 10.f;
	SphereDesc.vCenter = _float3(0.f, SphereDesc.fRadius, 0.f);

	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider_Sphere"), reinterpret_cast<CComponent**>(&m_pColliderCom), &SphereDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CGiwajip2::Bind_ShaderResources()
{
	/*m_pShaderCom->Bind_Matrix("g_WorldMatrix", );*/
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
		return E_FAIL;

	return S_OK;
}

CGiwajip2* CGiwajip2::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CGiwajip2* pInstance = new CGiwajip2(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CStaticMap* CGiwajip2::Clone(void* pArg)
{
	CGiwajip2* pInstance = new CGiwajip2(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CGiwajip2");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CGiwajip2::Free()
{
	__super::Free();

}
