#include "pch.h"
#include "Desert_Grass.h"
#include "GameInstance.h"

CDesert_Grass::CDesert_Grass(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CDesertObject{ pDevice, pContext }
{
}

CDesert_Grass::CDesert_Grass(const CDesert_Grass& Prototype)
	: CDesertObject{ Prototype }
{
}

HRESULT CDesert_Grass::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CDesert_Grass::Initialize(void* pArg)
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

	//m_iObjectID = Object_Number(m_ComponentTag);

	return S_OK;
}

void CDesert_Grass::Priority_Update(_float fTimeDelta)
{
}

void CDesert_Grass::Update(_float fTimeDelta)
{
	//SetCullingCollider(m_iObjectID);
	//_matrix worldMatrix = XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr());
	//m_pCullingCollider->UpdateColiision(worldMatrix);
}

void CDesert_Grass::Late_Update(_float fTimeDelta)
{
	//if (m_pGameInstance->isIn_WorldFrustum(m_pCullingCollider))
	m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);

#ifdef _DEBUG
	//m_pGameInstance->Add_DebugComponent(m_pCullingCollider);
#endif
}

HRESULT CDesert_Grass::Render()
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

HRESULT CDesert_Grass::Ready_Components(const _tchar* pComponentTag)
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

HRESULT CDesert_Grass::Bind_ShaderResources()
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

void CDesert_Grass::SetCullingCollider(_uint iObjectID)
{
	auto pCullingCollider = static_cast<COBBCollider*>(m_pCullingCollider);
	switch (iObjectID)
	{
	case 1:
		pCullingCollider->SetCollision({ 0.f, 2.0f, 0.f }, {}, { 3, 3, 32 });
		break;
	case 2: case 9: case 10: case 13: case 14: case 16:
	case 30: case 62: case 63:
		pCullingCollider->SetCollision({ 0.f, 1.33f, 0.f }, {}, { 3, 2, 2 });
		break;
	case 75:
		pCullingCollider->SetCollision({ 0.f, 0.67f, 0.f }, {}, { 6, 1, 6 });
		break;
	}

}

_uint CDesert_Grass::Object_Number(const _tchar* pComponentTag)
{
	if (pComponentTag == nullptr)
		return 0;

	const _tchar* pLastUnderscore = wcsrchr(pComponentTag, L'_');

	if (pLastUnderscore == nullptr || *(pLastUnderscore + 1) == L'\0')
		return 0;

	const _tchar* pSuffix = pLastUnderscore + 1;

	if (wcscmp(pSuffix, TEXT("1A")) == 0)
		return 1;
	else if (wcscmp(pSuffix, TEXT("2A")) == 0)
		return 2;

	return 0;
}


CDesert_Grass* CDesert_Grass::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDesert_Grass* pInstance = new CDesert_Grass(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CDesertObject* CDesert_Grass::Clone(void* pArg)
{
	CDesert_Grass* pInstance = new CDesert_Grass(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CDesert_Grass");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDesert_Grass::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}

