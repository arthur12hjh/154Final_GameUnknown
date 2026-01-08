#include "pch.h"
#include "CherryBlossom4.h"
#include "GameInstance.h"

CCherryBlossom4::CCherryBlossom4(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CStaticMap{ pDevice, pContext }
{
}

CCherryBlossom4::CCherryBlossom4(const CCherryBlossom4& Prototype)
	: CStaticMap{ Prototype }
{
}

HRESULT CCherryBlossom4::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCherryBlossom4::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CCherryBlossom4::Priority_Update(_float fTimeDelta)
{
}

void CCherryBlossom4::Update(_float fTimeDelta)
{
	m_pColliderCom->UpdateColiision(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));

	//m_fTimeAcc += fTimeDelta;

}

void CCherryBlossom4::Late_Update(_float fTimeDelta)
{
	if (true == m_pGameInstance->isIn_WorldFrustum(m_pColliderCom))
	{
		m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
	}

#ifdef _DEBUG
	m_pGameInstance->Add_DebugComponent(m_pColliderCom);
#endif
}

HRESULT CCherryBlossom4::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fTime", &m_fTimeAcc, sizeof(_float))))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{

		if (FAILED(m_pModelCom->Bind_Material(i, m_pShaderCom, "g_DiffuseTexture", aiTextureType_DIFFUSE, 0)))
			return E_FAIL;
		if (FAILED(m_pModelCom->Bind_Material(i, m_pShaderCom, "g_NormalTexture", aiTextureType_NORMALS, 0)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(9)))
			return E_FAIL;


		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CCherryBlossom4::Ready_Components()
{
	/* Com_Model */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::LEVEL_PROB), TEXT("Prototype_Component_Model_CherryBlossom4"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* Com_Shader */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::LEVEL_PROB), TEXT("Prototype_Component_Texture_BlossomMask"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* Com_Collider_OBB */
	COBBCollider::OBB_COLLIDER_DESC		OBBDesc{};
	OBBDesc.vSize = _float3(10.f, 12.f, 13.f);
	OBBDesc.vCenter = _float3(7.5f, 8, 1.f);

	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider_OBB"), reinterpret_cast<CComponent**>(&m_pColliderCom), &OBBDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CCherryBlossom4::Bind_ShaderResources()
{
	/*m_pShaderCom->Bind_Matrix("g_WorldMatrix", );*/
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResources(m_pShaderCom, "g_MaskTexture")))
		return E_FAIL;

	return S_OK;
}

CCherryBlossom4* CCherryBlossom4::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCherryBlossom4* pInstance = new CCherryBlossom4(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CStaticMap* CCherryBlossom4::Clone(void* pArg)
{
	CCherryBlossom4* pInstance = new CCherryBlossom4(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCherryBlossom4");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCherryBlossom4::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
}
