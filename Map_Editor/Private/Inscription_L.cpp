#include "pch.h"
#include "Inscription_L.h"
#include "GameInstance.h"

CInscription_L::CInscription_L(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CStaticMap{ pDevice, pContext }
{
}

CInscription_L::CInscription_L(const CInscription_L& Prototype)
	: CStaticMap{ Prototype }
{
}

HRESULT CInscription_L::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CInscription_L::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CInscription_L::Priority_Update(_float fTimeDelta)
{
}

void CInscription_L::Update(_float fTimeDelta)
{
	m_pColliderCom->UpdateColiision(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));

}

void CInscription_L::Late_Update(_float fTimeDelta)
{

	/*if (true == m_pGameInstance->isIn_WorldFrustum(m_pColliderCom))
	{
	}*/
		m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);

}

HRESULT CInscription_L::Render()
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
		if (FAILED(m_pModelCom->Bind_Material(i, m_pShaderCom, "g_EmissiveTexture", aiTextureType_EMISSIVE, 0)))
			return E_FAIL;


		if (FAILED(m_pShaderCom->Begin(3)))
			return E_FAIL;


		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CInscription_L::Ready_Components()
{
	/* Com_Model */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_Component_Model_Inscription_L"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* Com_Shader */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	COBBCollider::OBB_COLLIDER_DESC		OBBDesc{};

	OBBDesc.vSize = _float3(3.f, 10.f, 3.f);
	OBBDesc.vCenter = _float3(0.f, OBBDesc.vSize.y, 0.f);
	OBBDesc.vAngles = _float3(0.f, 0.f/*XMConvertToRadians(45.0f)*/, 0.f);
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider_OBB"), reinterpret_cast<CComponent**>(&m_pColliderCom), &OBBDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CInscription_L::Bind_ShaderResources()
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

CInscription_L* CInscription_L::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CInscription_L* pInstance = new CInscription_L(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CStaticMap* CInscription_L::Clone(void* pArg)
{
	CInscription_L* pInstance = new CInscription_L(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CInscription_L");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CInscription_L::Free()
{
	__super::Free();
}
