#include "pch.h"
#include "CanBox.h"
#include "GameInstance.h"

CCanBox::CCanBox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CInteraction{ pDevice, pContext }
{
}

CCanBox::CCanBox(const CCanBox& Prototype)
	: CInteraction{ Prototype }
{
}

HRESULT CCanBox::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCanBox::Initialize(void* pArg)
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
	m_eCurState = OPEN;
	m_pModelCom->Set_AnimationIndex(m_eCurState);

	return S_OK;
}

void CCanBox::Priority_Update(_float fTimeDelta)
{
}

void CCanBox::Update(_float fTimeDelta)
{
	if (m_eCurState != m_ePrevState)
	{

		switch (m_eCurState)
		{
		case OPEN:
			m_pModelCom->Set_AnimationIndex(OPEN, false, 0.f);
			break;
		}

		m_ePrevState = m_eCurState;
	}

	m_pModelCom->Play_Animation(fTimeDelta);

	//m_pColliderCom->UpdateColiision(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));
}

void CCanBox::Late_Update(_float fTimeDelta)
{

	m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
}

HRESULT CCanBox::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_BoneSRV(i, m_pShaderCom, "g_BoneMatrixBuffer")))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_Material(i, m_pShaderCom, "g_DiffuseTexture", aiTextureType_DIFFUSE, 0)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_Material(i, m_pShaderCom, "g_ORMTexture", aiTextureType_METALNESS, 0)))
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

HRESULT CCanBox::Ready_Components(const _tchar* pComponentTag)
{
	/* Com_Model */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::DESERT), pComponentTag,
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* Com_Shader */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::DESERT), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;


	return S_OK;
}

HRESULT CCanBox::Bind_ShaderResources()
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

CCanBox* CCanBox::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCanBox* pInstance = new CCanBox(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCanBox::Clone(void* pArg)
{
	CCanBox* pInstance = new CCanBox(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCanBox");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCanBox::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}
