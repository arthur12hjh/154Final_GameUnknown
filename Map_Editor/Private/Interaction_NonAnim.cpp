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

	m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
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
