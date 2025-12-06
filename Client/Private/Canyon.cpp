#include "pch.h"
#include "Canyon.h"
#include "GameInstance.h"

CCanyon::CCanyon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CActor{ pDevice, pContext }
{
}

CCanyon::CCanyon(const CCanyon& Prototype)
	: CActor{ Prototype }
{
}

HRESULT CCanyon::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCanyon::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	ACTOR_DESC* pDesc = static_cast<ACTOR_DESC*>(pArg);
	if (FAILED(Ready_Components(pDesc->szVIBuffer_PrototypeName)))
		return E_FAIL;

	return S_OK;
}

void CCanyon::Priority_Update(_float fTimeDelta)
{
}

void CCanyon::Update(_float fTimeDelta)
{
	_matrix worldMatrix = XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr());

	m_pColliderCom->UpdateColiision(worldMatrix);
}

void CCanyon::Late_Update(_float fTimeDelta)
{
	if(m_pGameInstance->isIn_WorldFrustum(m_pCullingCollider))
		m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);

#ifdef _DEBUG
	m_pGameInstance->Add_DebugComponent(m_pColliderCom);
#endif
}

HRESULT CCanyon::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pInstanceModelCom->GetModelNumMeshes();
	for (_uint i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pInstanceModelCom->Bind_MatrialTexture(m_pShaderCom, i, "g_DiffuseTexture", aiTextureType_DIFFUSE, 0)))
			return E_FAIL;

		if (FAILED(m_pInstanceModelCom->Bind_MatrialTexture(m_pShaderCom, i, "g_NormalTexture", aiTextureType_NORMALS, 0)))
			return E_FAIL;

		if (FAILED(m_pInstanceModelCom->Bind_MatrialTexture(m_pShaderCom, i, "g_ORMTexture", aiTextureType_METALNESS, 0)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		if (FAILED(m_pInstanceModelCom->Render(i)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CCanyon::Ready_Components(const _tchar* pComponentTag)
{
	/* Com_Model */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), pComponentTag,
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pInstanceModelCom))))
		return E_FAIL;

	/* Com_Shader */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Shader_Instance_Model"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	

	return S_OK;
}

HRESULT CCanyon::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
		return E_FAIL;

	return S_OK;
}

CCanyon* CCanyon::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCanyon* pInstance = new CCanyon(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCanyon::Clone(void* pArg)
{
	CCanyon* pInstance = new CCanyon(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCanyon");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCanyon::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pInstanceModelCom);
	Safe_Release(m_pShaderCom);
}
