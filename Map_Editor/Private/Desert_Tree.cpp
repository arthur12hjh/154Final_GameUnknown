#include "pch.h"
#include "Desert_Tree.h"
#include "GameInstance.h"

CDesert_Tree::CDesert_Tree(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CDesertObject{ pDevice, pContext }
{
}

CDesert_Tree::CDesert_Tree(const CDesert_Tree& Prototype)
	: CDesertObject{ Prototype }
{
}

HRESULT CDesert_Tree::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CDesert_Tree::Initialize(void* pArg)
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

void CDesert_Tree::Priority_Update(_float fTimeDelta)
{
}

void CDesert_Tree::Update(_float fTimeDelta)
{
	SetCullingCollider(m_iObjectID);
	_matrix worldMatrix = XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr());
	m_pCullingCollider->UpdateColiision(worldMatrix);
}

void CDesert_Tree::Late_Update(_float fTimeDelta)
{
	//if (m_pGameInstance->isIn_WorldFrustum(m_pCullingCollider))
		m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);

#ifdef _DEBUG
	m_pGameInstance->Add_DebugComponent(m_pCullingCollider);
#endif
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


		if (FAILED(m_pShaderCom->Begin(7)))
			return E_FAIL;


		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CDesert_Tree::Ready_Components(const _tchar* pComponentTag)
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

void CDesert_Tree::SetCullingCollider(_uint iObjectID)
{
	auto pCullingCollider = static_cast<COBBCollider*>(m_pCullingCollider);
	switch (iObjectID)
	{
	case 1: pCullingCollider->SetCollision({ 0.f, 15.f, 0.f }, {}, { 9.f, 16.f, 9.f }); break;
	case 2: pCullingCollider->SetCollision({ 0.f, 85.0f, 0.f }, {}, { 80.f, 90.f, 80.f }); break;
	case 3: pCullingCollider->SetCollision({ 0.f, 2.8f, 0.f }, {}, { 3.f, 4.f, 2.f }); break;
	case 4: pCullingCollider->SetCollision({ -8.f, 2.8f, -4.f }, {}, { 20.f, 4.f, 8.f }); break;
	case 5:  // Tree_5B
		pCullingCollider->SetCollision({ 5.f, 17.0f, 0.f }, {}, { 10.f, 20.f, 10.f }); break;
	case 6:  // Tree_5C
		pCullingCollider->SetCollision({ 0.f, 7.f, 0.f }, {}, { 10.f, 8.f, 10.f }); break;
	case 7:  // Tree_6A
	case 8:  // Tree_6B
		pCullingCollider->SetCollision({ 0.f, 9.0f, 0.f }, {}, { 10.f, 10.f, 10.f }); break;
	case 9:  // Tree_7A
		pCullingCollider->SetCollision({ 0.f, 11.f, 0.f }, {}, { 10.f, 12.f, 10.f }); break;
	case 10: // Tree_7B
		pCullingCollider->SetCollision({ 0.f, 13.f, -8.f }, {}, { 10.f, 15.f, 10.f }); break;
	case 11: // Tree_8A
	case 12: // Tree_8Aa
		pCullingCollider->SetCollision({ 0.f, 4.9f, 0.f }, {}, { 8.f, 7.f, 8.f }); break;
	case 13: // Tree_8B
	case 14: // Tree_8Ba
		pCullingCollider->SetCollision({ 0.f, 4.9f, 0.f }, {}, { 9.f, 7.f, 9.f }); break;
	case 15: // Tree_10A
		pCullingCollider->SetCollision({ 1.5f, 1.8f, 0.f }, {}, { 5.f, 2.f, 3.f }); break;
	case 16: // Tree_11B
		pCullingCollider->SetCollision({ 0.f, 7.0f, 0.f }, {}, { 10.f, 10.f, 10.f }); break;
	case 17: // Tree_15A
		pCullingCollider->SetCollision({ 0.f, 1.4f, 0.f }, {}, { 2.f, 2.f, 2.f }); break;
	case 18: // Tree_15B
		pCullingCollider->SetCollision({ 0.f, 0.f, 0.f }, {}, { 1.f, 1.f, 1.f }); break;
	case 19: // Tree_16A
		pCullingCollider->SetCollision({ 0.f, 1.4f, 0.f }, {}, { 1.f, 2.f, 1.f }); break;
	case 20: // Tree_17A
		pCullingCollider->SetCollision({ 0.f, 2.5f, 0.f }, {}, { 3.f, 3.f, 2.f }); break;
	case 21: // Tree_18A
		pCullingCollider->SetCollision({ 1.f, 4.f, 0.f }, {}, { 4.f, 5.f, 2.f }); break;
	case 22: // Tree_19A
	case 23: // Tree_20A
		pCullingCollider->SetCollision({ 0.f, 0.f, 0.f }, {}, { 1.f, 1.f, 1.f }); break;
	case 24: // Tree_21A
		pCullingCollider->SetCollision({ 0.f, 1.4f, 1.f }, {}, { 4.f, 2.f, 5.f }); break;
	case 25: // Tree_23A
		pCullingCollider->SetCollision({ 0.f, 0.7f, 0.f }, {}, { 3.f, 1.f, 3.f }); break;
	case 26: // Tree_25A
		pCullingCollider->SetCollision({ 0.f, 0.f, 0.f }, {}, { 2.f, 1.f, 2.f }); break;
	case 27: // Tree_26A
	case 28: // Tree_27A
	case 29: // Tree_29A
	case 30: // Tree_30A
		pCullingCollider->SetCollision({ 0.f, 0.f, 0.f }, {}, { 1.f, 1.f, 1.f }); break;
	case 31: // Tree_31A
		pCullingCollider->SetCollision({ 0.f, 1.7f, 0.f }, {}, { 1.5f, 2.5f, 1.f }); break;
	case 32: // Tree_32A
		pCullingCollider->SetCollision({ 0.f, 1.4f, 0.f }, {}, { 1.f, 2.f, 1.f }); break;
	case 33: // Tree_33A
		pCullingCollider->SetCollision({ 0.f, 0.f, 0.f }, {}, { 1.f, 1.f, 1.5f }); break;
	case 34: // Tree_34A
		pCullingCollider->SetCollision({ 0.f, 0.f, 0.f }, {}, { 1.f, 1.f, 1.f }); break;
	case 35: // Tree_42A
		pCullingCollider->SetCollision({ 0.f, 5.f, 0.f }, {}, { 4.f, 6.f, 4.f }); break;
	case 36: // Tree_43A
		pCullingCollider->SetCollision({ 0.f, 7.f, 0.f }, {}, { 8.f, 8.f, 8.f }); break;
	case 37: // Tree_43B
		pCullingCollider->SetCollision({ 0.f, 6.5f, 0.f }, {}, { 5.f, 8.f, 5.f }); break;
	case 38: // Tree_44A
		pCullingCollider->SetCollision({ 0.f, 5.f, 0.f }, {}, { 5.f, 6.f, 5.f }); break;
	case 39: // Tree_44B
		pCullingCollider->SetCollision({ -4.f, 2.1f, -5.f }, {}, { 5.f, 3.f, 8.f }); break;
	case 40: // Tree_44D
		pCullingCollider->SetCollision({ -1.f, 1.f, -4.f }, {}, { 4.f, 2.f, 8.f }); break;
	case 41: // Tree_44F
		pCullingCollider->SetCollision({ -2.f, 1.4f, -3.f }, {}, { 4.f, 2.f, 7.f }); break;
	case 42: // Tree_45A
		pCullingCollider->SetCollision({ 0.f, 4.9f, 0.f }, {}, { 4.f, 7.f, 4.f }); break;
	case 43: // Tree_45B
		pCullingCollider->SetCollision({ 0.f, 4.f, 0.f }, {}, { 7.f, 7.f, 7.f }); break;
	case 44: // Tree_45C
		pCullingCollider->SetCollision({ 0.f, 1.f, 0.f }, {}, { 4.f, 4.f, 4.f }); break;
	}

}

_uint CDesert_Tree::Object_Number(const _tchar* pComponentTag)
{
	if (pComponentTag == nullptr)
		return 0;

	const _tchar* pLastUnderscore = wcsrchr(pComponentTag, L'_');
	if (pLastUnderscore == nullptr || *(pLastUnderscore + 1) == L'\0')
		return 0;

	const _tchar* pSuffix = pLastUnderscore + 1;

	if (!wcscmp(pSuffix, TEXT("1A"))) return 1;
	if (!wcscmp(pSuffix, TEXT("2A"))) return 2;
	if (!wcscmp(pSuffix, TEXT("3A"))) return 3;
	if (!wcscmp(pSuffix, TEXT("4A"))) return 4;
	if (!wcscmp(pSuffix, TEXT("5B"))) return 5;
	if (!wcscmp(pSuffix, TEXT("5C"))) return 6;
	if (!wcscmp(pSuffix, TEXT("6A"))) return 7;
	if (!wcscmp(pSuffix, TEXT("6B"))) return 8;
	if (!wcscmp(pSuffix, TEXT("7A"))) return 9;
	if (!wcscmp(pSuffix, TEXT("7B"))) return 10;
	if (!wcscmp(pSuffix, TEXT("8A"))) return 11;
	if (!wcscmp(pSuffix, TEXT("8Aa"))) return 12;
	if (!wcscmp(pSuffix, TEXT("8B"))) return 13;
	if (!wcscmp(pSuffix, TEXT("8Ba"))) return 14;
	if (!wcscmp(pSuffix, TEXT("10A"))) return 15;
	if (!wcscmp(pSuffix, TEXT("11B"))) return 16;
	if (!wcscmp(pSuffix, TEXT("15A"))) return 17;
	if (!wcscmp(pSuffix, TEXT("15B"))) return 18;
	if (!wcscmp(pSuffix, TEXT("16A"))) return 19;
	if (!wcscmp(pSuffix, TEXT("17A"))) return 20;
	if (!wcscmp(pSuffix, TEXT("18A"))) return 21;
	if (!wcscmp(pSuffix, TEXT("19A"))) return 22;
	if (!wcscmp(pSuffix, TEXT("20A"))) return 23;
	if (!wcscmp(pSuffix, TEXT("21A"))) return 24;
	if (!wcscmp(pSuffix, TEXT("23A"))) return 25;
	if (!wcscmp(pSuffix, TEXT("25A"))) return 26;
	if (!wcscmp(pSuffix, TEXT("26A"))) return 27;
	if (!wcscmp(pSuffix, TEXT("27A"))) return 28;
	if (!wcscmp(pSuffix, TEXT("29A"))) return 29;
	if (!wcscmp(pSuffix, TEXT("30A"))) return 30;
	if (!wcscmp(pSuffix, TEXT("31A"))) return 31;
	if (!wcscmp(pSuffix, TEXT("32A"))) return 32;
	if (!wcscmp(pSuffix, TEXT("33A"))) return 33;
	if (!wcscmp(pSuffix, TEXT("34A"))) return 34;
	if (!wcscmp(pSuffix, TEXT("42A"))) return 35;
	if (!wcscmp(pSuffix, TEXT("43A"))) return 36;
	if (!wcscmp(pSuffix, TEXT("43B"))) return 37;
	if (!wcscmp(pSuffix, TEXT("44A"))) return 38;
	if (!wcscmp(pSuffix, TEXT("44B"))) return 39;
	if (!wcscmp(pSuffix, TEXT("44D"))) return 40;
	if (!wcscmp(pSuffix, TEXT("44F"))) return 41;
	if (!wcscmp(pSuffix, TEXT("45A"))) return 42;
	if (!wcscmp(pSuffix, TEXT("45B"))) return 43;
	if (!wcscmp(pSuffix, TEXT("45C"))) return 44;

	return 0;
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

CDesertObject* CDesert_Tree::Clone(void* pArg)
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
