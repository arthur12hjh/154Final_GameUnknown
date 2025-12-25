#include "pch.h"
#include "CM_Rock7.h"
#include "GameInstance.h"

CCM_Rock7::CCM_Rock7(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CStaticMap{ pDevice, pContext }
{
}

CCM_Rock7::CCM_Rock7(const CCM_Rock7& Prototype)
	: CStaticMap{ Prototype }
{
}

HRESULT CCM_Rock7::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCM_Rock7::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CCM_Rock7::Priority_Update(_float fTimeDelta)
{
}

void CCM_Rock7::Update(_float fTimeDelta)
{
	m_pColliderCom->UpdateColiision(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));

}

void CCM_Rock7::Late_Update(_float fTimeDelta)
{
	/*if (true == m_pGameInstance->isIn_WorldFrustum(m_pColliderCom))
	{
	}*/
		m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
}

HRESULT CCM_Rock7::Render()
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

HRESULT CCM_Rock7::Ready_Components()
{
	/* Com_Model */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_Component_Model_CM_Rock7"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* Com_Shader */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* Com_Collider_Sphere */
	CSphereCollider::SPHERE_COLLIDER_DESC		SphereDesc{};

	SphereDesc.fRadius = 5.f;
	SphereDesc.vCenter = _float3(0.f, SphereDesc.fRadius * 0.7f, 0.f);

	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider_Sphere"), reinterpret_cast<CComponent**>(&m_pColliderCom), &SphereDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CCM_Rock7::Bind_ShaderResources()
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

CCM_Rock7* CCM_Rock7::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCM_Rock7* pInstance = new CCM_Rock7(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CStaticMap* CCM_Rock7::Clone(void* pArg)
{
	CCM_Rock7* pInstance = new CCM_Rock7(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCM_Rock7");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCM_Rock7::Free()
{
	__super::Free();
}
