#include "pch.h"
#include "Chair.h"
#include "GameInstance.h"

CChair::CChair(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CDesertObject{ pDevice, pContext }
{
}

CChair::CChair(const CChair& Prototype)
	: CDesertObject{ Prototype }
{
}

HRESULT CChair::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CChair::Initialize(void* pArg)
{

	DESERT_OBJECT_DESC* pDesc = static_cast<DESERT_OBJECT_DESC*>(pArg);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (pDesc && pDesc->pComponentTag)
	{
		wcsncpy_s(m_ComponentTag, 256, pDesc->pComponentTag, _TRUNCATE);
	}

	if (FAILED(Ready_Components(m_ComponentTag)))
		return E_FAIL;

	m_iObjectID = Object_Number(m_ComponentTag);

	return S_OK;
}

void CChair::Priority_Update(_float fTimeDelta)
{
}

void CChair::Update(_float fTimeDelta)
{
	//SetCullingCollider(m_iObjectID);
	//_matrix worldMatrix = XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr());
	//m_pCullingCollider->UpdateColiision(worldMatrix);
}

void CChair::Late_Update(_float fTimeDelta)
{
	//if (m_pGameInstance->isIn_WorldFrustum(m_pCullingCollider))
	m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);

#ifdef _DEBUG
	//m_pGameInstance->Add_DebugComponent(m_pCullingCollider);
#endif
}

HRESULT CChair::Render()
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

HRESULT CChair::Ready_Components(const _tchar* pComponentTag)
{
	/* Com_Model */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::DESERT), pComponentTag,
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* Com_Shader */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::DESERT), TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CChair::Bind_ShaderResources()
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

void CChair::SetCullingCollider(_uint iObjectID)
{
	auto pCullingCollider = static_cast<COBBCollider*>(m_pCullingCollider);
	switch (iObjectID)
	{
	case 1:
		pCullingCollider->SetCollision({ 0.f, 1.2f, 0.f }, {}, { 4, 2, 7 });
		break;
	case 2:
		pCullingCollider->SetCollision({ 0.f, 3.f, 3.5f }, {}, { 4, 5, 7 });
		break;
	case 3:
		pCullingCollider->SetCollision({ 0.f, 1.8f, 1.5f }, {}, { 10, 3, 3 });
		break;
	case 4:
		pCullingCollider->SetCollision({ 0.f, 1.2f, 1.5f }, {}, { 2, 2, 3 });
		break;
	case 5:
		pCullingCollider->SetCollision({ 0.f, 108.f, 22.5f }, {}, { 60, 180, 45 });
		break;
	case 6:
		pCullingCollider->SetCollision({ 0.f, 0.6f, 1.f }, {}, { 4, 1, 2 });
		break;
	}

}

_uint CChair::Object_Number(const _tchar* pComponentTag)
{
	if (pComponentTag == nullptr)
		return 0;

	const _tchar* pLastUnderscore = wcsrchr(pComponentTag, L'_');

	if (pLastUnderscore == nullptr || *(pLastUnderscore + 1) == L'\0')
		return 0;

	const _tchar* pSuffix = pLastUnderscore + 1;

	if (wcscmp(pSuffix, TEXT("A")) == 0)
		return 1;
	else if (wcscmp(pSuffix, TEXT("B")) == 0)
		return 2;
	else if (wcscmp(pSuffix, TEXT("C")) == 0)
		return 3;
	else if (wcscmp(pSuffix, TEXT("D")) == 0)
		return 4;
	else if (wcscmp(pSuffix, TEXT("E")) == 0)
		return 5;
	else if (wcscmp(pSuffix, TEXT("F")) == 0)
		return 6;
	else
		return 0;
}

CChair* CChair::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CChair* pInstance = new CChair(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CDesertObject* CChair::Clone(void* pArg)
{
	CChair* pInstance = new CChair(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CChair");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CChair::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}
