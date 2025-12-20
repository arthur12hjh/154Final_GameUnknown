#include "pch.h"
#include "Desert_Architecture.h"
#include "GameInstance.h"

CDesert_Architecture::CDesert_Architecture(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CDesertObject{ pDevice, pContext }
{
}

CDesert_Architecture::CDesert_Architecture(const CDesert_Architecture& Prototype)
	: CDesertObject{ Prototype }
{
}

HRESULT CDesert_Architecture::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CDesert_Architecture::Initialize(void* pArg)
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

void CDesert_Architecture::Priority_Update(_float fTimeDelta)
{
}

void CDesert_Architecture::Update(_float fTimeDelta)
{
	SetCullingCollider(m_iObjectID);
	_matrix worldMatrix = XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr());
	m_pCullingCollider->UpdateColiision(worldMatrix);
}

void CDesert_Architecture::Late_Update(_float fTimeDelta)
{
	//if (m_pGameInstance->isIn_WorldFrustum(m_pCullingCollider))
		m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);

#ifdef _DEBUG
	m_pGameInstance->Add_DebugComponent(m_pCullingCollider);
#endif
}

HRESULT CDesert_Architecture::Render()
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

HRESULT CDesert_Architecture::Ready_Components(const _tchar* pComponentTag)
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

HRESULT CDesert_Architecture::Bind_ShaderResources()
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

void CDesert_Architecture::SetCullingCollider(_uint iObjectID)
{
	auto pCullingCollider = static_cast<COBBCollider*>(m_pCullingCollider);
	switch (iObjectID)
	{
	case 1: // Ruin_7A (20, 30, 35)
		pCullingCollider->SetCollision({ -5.f, 21.f, -15.f }, {}, { 20.f, 30.f, 35.f });
		break;
	case 2: // Ruin_7B (30, 15, 25)
		pCullingCollider->SetCollision({ -20.f, 10.5f, -10.f }, {}, { 30.f, 15.f, 25.f });
		break;
	case 3: // Ruin_7C (12, 25, 12)
		pCullingCollider->SetCollision({ 0.f, 23.f, 0.f }, {}, { 12.f, 25.f, 12.f });
		break;
	case 4: // Ruin_7D (18, 8, 5)
		pCullingCollider->SetCollision({ 0.f, 4.f, 0.f }, {}, { 18.f, 8.f, 5.f });
		break;
	case 5: // Ruin_7E (4, 6, 4)
		pCullingCollider->SetCollision({ 0.f, 4.2f, 0.f }, {}, { 4.f, 6.f, 4.f });
		break;
	case 6: // Bridge_4A (10, 65, 30)
		pCullingCollider->SetCollision({ 0.f, 40.f, 0.f }, {}, { 10.f, 65.f, 30.f });
		break;
	case 7: // Bridge_4B (50, 65, 30)
		pCullingCollider->SetCollision({ 30.f, 45.5f, 0.f }, {}, { 40.f, 65.f, 30.f });
		break;
	case 8: // Bridge_4D (30, 80, 30)
		pCullingCollider->SetCollision({ 0.f, 45.f, 0.f }, {}, { 30.f, 80.f, 30.f });
		break;
	case 9: // Bridge_4L (40, 120, 150)
		pCullingCollider->SetCollision({ -10.f, -90.f, 100.f }, {}, { 40.f, 120.f, 150.f });
		break;
	case 10: // Bridge_4M (70, 70, 80)
		pCullingCollider->SetCollision({ 0.f, 30.f, -100.f }, {}, { 70.f, 80.f, 80.f });
		break;
	case 11: // Bridge_4N (30, 10, 5)
		pCullingCollider->SetCollision({ -35.f, 40.f, -70.f }, {}, { 30.f, 10.f, 5.f });
		break;
	case 12: // Bridge_5 (50, 60, 50)
		pCullingCollider->SetCollision({ 0.f, -10.f, 0.f }, {}, { 50.f, 60.f, 50.f });
		break;
	case 13: // Bridge_6 (60, 110, 100)
		pCullingCollider->SetCollision({ 0.f, 65.f, 0.f }, {}, { 60.f, 110.f, 100.f });
		break;
	case 14: // Bridge_14A (6, 8, 25)
		pCullingCollider->SetCollision({ 0.f, 5.6f, 0.f }, {}, { 6.f, 8.f, 25.f });
		break;
	case 15: // Bridge_14B (6, 5, 20)
		pCullingCollider->SetCollision({ 0.f, 3.5f, 0.f }, {}, { 6.f, 5.f, 20.f });
		break;
	}

}

_uint CDesert_Architecture::Object_Number(const _tchar* pComponentTag)
{
	if (pComponentTag == nullptr)
		return 0;

	const _tchar* pLastUnderscore = wcsrchr(pComponentTag, L'_');
	if (pLastUnderscore == nullptr || *(pLastUnderscore + 1) == L'\0')
		return 0;

	const _tchar* pSuffix = pLastUnderscore + 1;

	// 1. Ruin 시리즈 (1 ~ 5)
	if (wcsstr(pComponentTag, TEXT("Ruin"))) {
		if (!wcscmp(pSuffix, TEXT("7A"))) return 1;
		if (!wcscmp(pSuffix, TEXT("7B"))) return 2;
		if (!wcscmp(pSuffix, TEXT("7C"))) return 3;
		if (!wcscmp(pSuffix, TEXT("7D"))) return 4;
		if (!wcscmp(pSuffix, TEXT("7E"))) return 5;
	}
	// 2. Bridge 시리즈 (6 ~ 15)
	else if (wcsstr(pComponentTag, TEXT("Bridge"))) {
		if (!wcscmp(pSuffix, TEXT("4A"))) return 6;
		if (!wcscmp(pSuffix, TEXT("4B"))) return 7;
		if (!wcscmp(pSuffix, TEXT("4D"))) return 8;
		if (!wcscmp(pSuffix, TEXT("4L"))) return 9;
		if (!wcscmp(pSuffix, TEXT("4M"))) return 10;
		if (!wcscmp(pSuffix, TEXT("4N"))) return 11;
		if (!wcscmp(pSuffix, TEXT("5")))  return 12;
		if (!wcscmp(pSuffix, TEXT("6")))  return 13;
		if (!wcscmp(pSuffix, TEXT("14A"))) return 14;
		if (!wcscmp(pSuffix, TEXT("14B"))) return 15;
	}

	return 0;
}

CDesert_Architecture* CDesert_Architecture::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDesert_Architecture* pInstance = new CDesert_Architecture(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CDesertObject* CDesert_Architecture::Clone(void* pArg)
{
	CDesert_Architecture* pInstance = new CDesert_Architecture(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CDesert_Architecture");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDesert_Architecture::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}
