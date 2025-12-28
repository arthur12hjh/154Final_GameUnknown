#include "pch.h"
#include "Interaction_NonAnim.h"
#include "GameInstance.h"

CInteraction_NonAnim::CInteraction_NonAnim(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CInteraction{ pDevice, pContext }
{
}

CInteraction_NonAnim::CInteraction_NonAnim(const CInteraction_NonAnim& Prototype)
	: CInteraction{ Prototype }
{
}

HRESULT CInteraction_NonAnim::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CInteraction_NonAnim::Initialize(void* pArg)
{

	PROB_INTERACTION_DESC* pDesc = static_cast<PROB_INTERACTION_DESC*>(pArg);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (pDesc && pDesc->pComponentTag)
	{
		wcsncpy_s(m_ComponentTag, 256, pDesc->pComponentTag, _TRUNCATE);
	}

	if (FAILED(Ready_Components(m_ComponentTag)))
		return E_FAIL;

	m_iInteractionID = pDesc->iInteractionID;

	m_iObjectID = Object_Number(m_ComponentTag);


	return S_OK;
}

void CInteraction_NonAnim::Priority_Update(_float fTimeDelta)
{
}

void CInteraction_NonAnim::Update(_float fTimeDelta)
{
}

void CInteraction_NonAnim::Late_Update(_float fTimeDelta)
{
	SetCullingCollider(m_iObjectID);
	_matrix worldMatrix = XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr());
	m_pCullingCollider->UpdateColiision(worldMatrix);

	m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
	//m_pGameInstance->Add_RenderGroup(RENDER::OCCLUSION, this);

#ifdef _DEBUG
	m_pGameInstance->Add_DebugComponent(m_pCullingCollider);
#endif
}

HRESULT CInteraction_NonAnim::Render()
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

HRESULT CInteraction_NonAnim::Ready_Components(const _tchar* pComponentTag)
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

HRESULT CInteraction_NonAnim::Bind_ShaderResources()
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

void CInteraction_NonAnim::SetCullingCollider(_uint iObjectID)
{
	auto pCullingCollider = static_cast<COBBCollider*>(m_pCullingCollider);
	switch (iObjectID)
	{
	case 1: // VendingMachine_6A (1, 2, 1)
		pCullingCollider->SetCollision({ 0.f, 1.4f, 0.5f }, {}, { 1.f, 2.f, 1.f });
		break;
	case 2: // VendingMachine_7A (2, 3, 2)
		pCullingCollider->SetCollision({ 0.f, 2.1f, 0.f }, {}, { 2.f, 3.f, 2.f });
		break;
	case 3: // Camp_1I (1, 1, 1)
		pCullingCollider->SetCollision({ 0.f, 1.f, 0.f }, {}, { 1.f, 1.f, 1.f });
		break;
	case 4: // Corpse_1A (1, 1, 1.5)
		pCullingCollider->SetCollision({ 0.f, 1.f, -0.5f }, {}, { 1.f, 2.f, 1.5f });
		break;
	case 5: // Corpse_1B (1, 1, 2.2)
		pCullingCollider->SetCollision({ -0.2f, 0.f, 0.1f }, {}, { 1.f, 1.f, 2.2f });
		break;
	case 6: // Corpse_2A (2.2, 1, 1.5)
	case 7: // Corpse_2B (2.2, 1, 1.5)
		pCullingCollider->SetCollision({ 0.f, 0.f, 0.f }, {}, { 2.2f, 1.f, 1.5f });
		break;
	case 8: // Corpse_2C (1.5, 1.5, 1.5)
		pCullingCollider->SetCollision({ 0.f, 0.5f, 0.f }, {}, { 1.5f, 1.5f, 1.5f });
		break;
	case 9: // Corpse_3B (4, 3, 4)
		pCullingCollider->SetCollision({ 0.f, 2.1f, 0.5f }, {}, { 4.f, 3.f, 4.f });
		break;
	}
}

_uint CInteraction_NonAnim::Object_Number(const _tchar* pComponentTag)
{
	if (pComponentTag == nullptr)
		return 0;

	const _tchar* pLastUnderscore = wcsrchr(pComponentTag, L'_');
	if (pLastUnderscore == nullptr || *(pLastUnderscore + 1) == L'\0')
		return 0;

	const _tchar* pSuffix = pLastUnderscore + 1;

	// 1. VendingMachine 시리즈
	if (wcsstr(pComponentTag, TEXT("VendingMachine"))) {
		if (!wcscmp(pSuffix, TEXT("6A"))) return 1;
		if (!wcscmp(pSuffix, TEXT("7A"))) return 2;
	}
	// 2. Camp 시리즈
	else if (wcsstr(pComponentTag, TEXT("Camp"))) {
		if (!wcscmp(pSuffix, TEXT("1I"))) return 3;
	}
	// 3. Corpse 시리즈
	else if (wcsstr(pComponentTag, TEXT("Corpse"))) {
		if (!wcscmp(pSuffix, TEXT("1A"))) return 4;
		if (!wcscmp(pSuffix, TEXT("1B"))) return 5;
		if (!wcscmp(pSuffix, TEXT("2A"))) return 6;
		if (!wcscmp(pSuffix, TEXT("2B"))) return 7;
		if (!wcscmp(pSuffix, TEXT("2C"))) return 8;
		if (!wcscmp(pSuffix, TEXT("3A"))) return 9;
	}

	return 0;
}

CInteraction_NonAnim* CInteraction_NonAnim::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CInteraction_NonAnim* pInstance = new CInteraction_NonAnim(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CInteraction_NonAnim::Clone(void* pArg)
{
	CInteraction_NonAnim* pInstance = new CInteraction_NonAnim(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CInteraction_NonAnim");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CInteraction_NonAnim::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}
