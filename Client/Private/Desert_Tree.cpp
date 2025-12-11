#include "pch.h"
#include "Desert_Tree.h"
#include "GameInstance.h"

CDesert_Tree::CDesert_Tree(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CActor{ pDevice, pContext }
{
}

CDesert_Tree::CDesert_Tree(const CDesert_Tree& Prototype)
	: CActor{ Prototype }
{
}

HRESULT CDesert_Tree::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CDesert_Tree::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	ACTOR_DESC* pDesc = static_cast<ACTOR_DESC*>(pArg);
	if (FAILED(Ready_Components(pDesc->szVIBuffer_PrototypeName)))
		return E_FAIL;

	return S_OK;
}

void CDesert_Tree::Priority_Update(_float fTimeDelta)
{
}

void CDesert_Tree::Update(_float fTimeDelta)
{
}

void CDesert_Tree::Late_Update(_float fTimeDelta)
{

	m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
}

HRESULT CDesert_Tree::Render()
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
		if (FAILED(m_pModelCom->Bind_Material(i, m_pShaderCom, "g_ORMTexture", aiTextureType_METALNESS, 0)))
			return E_FAIL;


		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;


		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CDesert_Tree::Ready_Components(const _tchar* pComponentTag)
{
	/* Com_Model */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), pComponentTag,
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* Com_Shader */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CDesert_Tree::Bind_ShaderResources()
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

CDesert_Tree* CDesert_Tree::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDesert_Tree* pInstance = new CDesert_Tree(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CDesert_Tree::Clone(void* pArg)
{
	CDesert_Tree* pInstance = new CDesert_Tree(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CDesert_Tree");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDesert_Tree::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}
