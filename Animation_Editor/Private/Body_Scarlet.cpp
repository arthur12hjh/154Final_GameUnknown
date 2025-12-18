#include "pch.h"
#include "Body_Scarlet.h"

#include "GameInstance.h"

#include "Scarlet.h"

CBody_Scarlet::CBody_Scarlet(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject{ pDevice, pContext }
{
}

CBody_Scarlet::CBody_Scarlet(const CBody_Scarlet& Prototype)
	: CPartObject{ Prototype }
{
}

const _float4x4* CBody_Scarlet::Get_BoneMatrixPtr(const _char* pBoneName)
{
	return m_pModelCom->Get_BoneMatrixPtr(pBoneName);
}

_bool CBody_Scarlet::isFinish_Att()
{
	if (*m_pParentState & CScarlet::STATE_ATTACK)
		return m_isAnimFinish;

	return false;
}

HRESULT CBody_Scarlet::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBody_Scarlet::Initialize(void* pArg)
{
	BODY_SCARLET_DESC* pDesc = static_cast<BODY_SCARLET_DESC*>(pArg);

	m_pParentState = pDesc->pParentState;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_iAnimationIndex = 0;
	m_pModelCom->Set_AnimationIndex(m_iAnimationIndex);

	m_pModelCom->Bind_MaterialTag(TEXTURE_TYPE::DIFFUSE, "g_DiffuseTexture");
	m_pModelCom->Bind_MaterialTag(TEXTURE_TYPE::NORMAL, "g_NormalTexture");
	m_pModelCom->Bind_MaterialTag(TEXTURE_TYPE::EMISSIVE, "g_EmissiveTexture");
	m_pModelCom->Bind_MaterialTag(TEXTURE_TYPE::ORM, "g_ORMTexture");

	return S_OK;
}

void CBody_Scarlet::Priority_Update(_float fTimeDelta)
{
	int a = 10;
}

void CBody_Scarlet::Update(_float fTimeDelta)
{
	//if (*m_pParentState & CScarlet::STATE_ATTACK)
	//	m_pModelCom->Set_AnimationIndex(0, false);
	//
	//if (*m_pParentState & CScarlet::STATE_IDLE)
	//	m_pModelCom->Set_AnimationIndex(3);
	//
	//if (*m_pParentState & CScarlet::STATE_WALK)
	//	m_pModelCom->Set_AnimationIndex(4);

	XMStoreFloat4x4(&m_CombinedWorldMatrix,
		XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()) * XMLoadFloat4x4(m_pParentTransformCom->Get_WorldMatrixPtr()));

}

void CBody_Scarlet::Late_Update(_float fTimeDelta)
{
	//m_pGameInstance->Add_RenderGroup(RENDER::SHADOW, this);
	m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);

#ifdef _DEBUG

#endif
}

HRESULT CBody_Scarlet::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_BoneSRV(i, m_pShaderCom, "g_BoneMatrixBuffer")))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_AllMaterials(i, m_pShaderCom, 0)))
			return E_FAIL;


		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}




	return S_OK;
}

HRESULT CBody_Scarlet::Render_Shadow()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_Shadow_Resource(m_pShaderCom, "g_ViewMatrix", D3DTS::VIEW)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_Shadow_Resource(m_pShaderCom, "g_ProjMatrix", D3DTS::PROJ)))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_BoneSRV(i, m_pShaderCom, "g_BoneMatrixBuffer")))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(1)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}
	return S_OK;
}

HRESULT CBody_Scarlet::Ready_Components()
{
	/* Com_Model */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::EDITOR), TEXT("Prototype_Component_Model_Scarlet_Body"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* Com_Shader */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::EDITOR), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	m_pModelCom->Bind_MaterialTag(TEXTURE_TYPE::DIFFUSE, "g_DiffuseTexture");
	m_pModelCom->Bind_MaterialTag(TEXTURE_TYPE::NORMAL, "g_NormalTexture");
	m_pModelCom->Bind_MaterialTag(TEXTURE_TYPE::EMISSIVE, "g_EmissiveTexture");

	return S_OK;
}

HRESULT CBody_Scarlet::Bind_ShaderResources()
{
	/*m_pShaderCom->Bind_Matrix("g_WorldMatrix", );*/
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
		return E_FAIL;

	return S_OK;
}

CBody_Scarlet* CBody_Scarlet::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBody_Scarlet* pInstance = new CBody_Scarlet(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBody_Scarlet");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBody_Scarlet::Clone(void* pArg)
{
	CBody_Scarlet* pInstance = new CBody_Scarlet(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBody_Scarlet");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBody_Scarlet::Free()
{
	__super::Free();
}
