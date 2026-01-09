#include "pch.h"
#include "CherryBlossom2.h"
#include "GameInstance.h"

CCherryBlossom2::CCherryBlossom2(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CStaticMap{ pDevice, pContext }
{
}

CCherryBlossom2::CCherryBlossom2(const CCherryBlossom2& Prototype)
	: CStaticMap{ Prototype }
{
}

HRESULT CCherryBlossom2::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCherryBlossom2::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CCherryBlossom2::Priority_Update(_float fTimeDelta)
{
}

void CCherryBlossom2::Update(_float fTimeDelta)
{
	m_pColliderCom->UpdateColiision(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));

	m_fTimeAcc += fTimeDelta;
}

void CCherryBlossom2::Late_Update(_float fTimeDelta)
{
	if (true == m_pGameInstance->isIn_WorldFrustum(m_pColliderCom))
	{
		m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
	}

#ifdef _DEBUG
	m_pGameInstance->Add_DebugComponent(m_pColliderCom);
#endif
}

HRESULT CCherryBlossom2::Render()
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
		if (FAILED(m_pModelCom->Bind_Material(i, m_pShaderCom, "g_ORMTexture", aiTextureType_METALNESS, 0)))
			return E_FAIL;
		if (FAILED(m_pModelCom->Bind_Material(i, m_pShaderCom, "g_EmissiveTexture", aiTextureType_EMISSIVE, 0)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(9)))
			return E_FAIL;


		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CCherryBlossom2::Ready_Components()
{
	/* Com_Model */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::LEVEL_PROB), TEXT("Prototype_Component_Model_CherryBlossom2"),
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
	OBBDesc.vSize = _float3(15.f, 12.f, 15.f);
	OBBDesc.vCenter = _float3(3.f, 9.f, 0.f);

	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider_OBB"), reinterpret_cast<CComponent**>(&m_pColliderCom), &OBBDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CCherryBlossom2::Bind_ShaderResources()
{
	_bool bFlag = { false };
	if (FAILED(m_pShaderCom->Bind_RawValue("g_IsMaskingDepthB", &bFlag, sizeof(_bool))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_IsMaskingDepthW", &bFlag, sizeof(_bool))))
		return E_FAIL;

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

CCherryBlossom2* CCherryBlossom2::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCherryBlossom2* pInstance = new CCherryBlossom2(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CStaticMap* CCherryBlossom2::Clone(void* pArg)
{
	CCherryBlossom2* pInstance = new CCherryBlossom2(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCherryBlossom2");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCherryBlossom2::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
}
