#include "pch.h"
#include "RepairConsole.h"
#include "GameInstance.h"

CRepairConsole::CRepairConsole(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CProb_Interaction{ pDevice, pContext }
{
}

CRepairConsole::CRepairConsole(const CRepairConsole& Prototype)
	: CProb_Interaction{ Prototype }
{
}

HRESULT CRepairConsole::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CRepairConsole::Initialize(void* pArg)
{

	PROB_INTERACTION_DESC* pDesc = static_cast<PROB_INTERACTION_DESC*>(pArg);
	static_cast<PROB_INTERACTION_DESC*>(pArg)->iInteractionID = 999;
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pDesc->szVIBuffer_PrototypeName)))
		return E_FAIL;

	m_eCurState = OPEN;
	m_pModelCom->Set_AnimationIndex(m_eCurState);

	_matrix worldMatrix = XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr());
	m_pCullingCollider->UpdateColiision(worldMatrix);

	return S_OK;
}

void CRepairConsole::Priority_Update(_float fTimeDelta)
{
}

void CRepairConsole::Update(_float fTimeDelta)
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

	//m_pModelCom->Play_Animation(fTimeDelta);

	//m_pColliderCom->UpdateColiision(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));
}

void CRepairConsole::Late_Update(_float fTimeDelta)
{

	if (!m_pGameInstance->isIn_WorldFrustum(m_pCullingCollider))
	{
		return;
	}

	m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
	m_pGameInstance->Add_RenderGroup(RENDER::OCCLUSION, this);

#ifdef _DEBUG
	m_pGameInstance->Add_DebugComponent(m_pCullingCollider);
#endif

}

HRESULT CRepairConsole::Render()
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

HRESULT CRepairConsole::Ready_Components(const _tchar* pComponentTag)
{
	/* Com_Model */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::LEVEL_PROB), pComponentTag,
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* Com_Shader */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	auto pCullingCollider = static_cast<COBBCollider*>(m_pCullingCollider);
	pCullingCollider->SetCollision({ 0.f, 1.5f, 0.f }, {}, { 1.5f, 1.5f, 1.5f });

	return S_OK;
}

HRESULT CRepairConsole::Bind_ShaderResources()
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

HRESULT CRepairConsole::Begin_OverlapCallBack()
{
	return S_OK;
}

HRESULT CRepairConsole::End_OverlapCallBack()
{
	return S_OK;
}

void CRepairConsole::Excute_CallBack(_float fTimeDelta, CGameObject* pActionObject)
{
}

CRepairConsole* CRepairConsole::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRepairConsole* pInstance = new CRepairConsole(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CRepairConsole::Clone(void* pArg)
{
	CRepairConsole* pInstance = new CRepairConsole(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CRepairConsole");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRepairConsole::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
}
