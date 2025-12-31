#include "pch.h"
#include "DisplayBox.h"

#include "GameInstance.h"
#include "GameManager.h"
#include "InteractionBinder.h"

#include "UIHUD.h"
#include "UIScript.h"
#include "UIActionEvent.h"

CDisplayBox::CDisplayBox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CProb_Interaction{ pDevice, pContext }
{
}

CDisplayBox::CDisplayBox(const CDisplayBox& Prototype)
	: CProb_Interaction{ Prototype }
{
}

HRESULT CDisplayBox::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CDisplayBox::Initialize(void* pArg)
{
	PROB_INTERACTION_DESC* pDesc = static_cast<PROB_INTERACTION_DESC*>(pArg);
	static_cast<PROB_INTERACTION_DESC*>(pArg)->iInteractionID = 9;
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pDesc->szVIBuffer_PrototypeName)))
		return E_FAIL;

	/*if (FAILED(Ready_Col(pDesc->szVIBuffer_PrototypeName)))
		return E_FAIL;*/

	//m_pInteractionCom->Set_InterDesc(m_pGameManager->Find_InteractionData(pDesc->iInteractionID));
	//m_pInteractionCom->Set_InterState(INTERACTION_STATE::DEFAULT);
	m_eCurState = BOX_STATE::IDLE;
	m_pModelCom->Set_AnimationIndex(0, false);
	m_pCullingCollider->UpdateColiision(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));

	return S_OK;
}

void CDisplayBox::Priority_Update(_float fTimeDelta)
{
}

void CDisplayBox::Update(_float fTimeDelta)
{
	//if (m_pGameInstance->isIn_DistanceFrustum(m_pTransformCom->Get_State(STATE::POSITION), 150.f))
	//{
	//	if (m_pModelCom->Play_Animation(fTimeDelta))
	//	{
	//		switch (m_eCurState)
	//		{
	//		case BOX_STATE::OPEN:
	//		{
	//			m_pModelCom->Set_AnimationIndex(0, false);
	//			m_eCurState = BOX_STATE::IDLE;
	//			//m_pRigidBody->Set_Simulation(true);
	//		}
	//		break;
	//		case BOX_STATE::IDLE:
	//		{
	//			m_pModelCom->Set_AnimationIndex(1, false);
	//			m_eCurState = BOX_STATE::OPEN;
	//			//m_pRigidBody->Set_Simulation(false);
	//		}
	//		break;
	//		}

	//		m_pInteractionCom->Set_InterState(INTERACTION_STATE::DEFAULT);
	//	}
	//	
	//}
	m_pModelCom->Play_Animation(fTimeDelta);

}

void CDisplayBox::Late_Update(_float fTimeDelta)
{
	if (m_pGameInstance->isIn_WorldFrustum(m_pCullingCollider))
	{
		/*if (INTERACTION_STATE::ACTIVE > m_pInteractionCom->Get_InterState())
			m_pInteractionCom->Update_Com(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));*/
#ifdef _DEBUG
		m_pGameInstance->Add_DebugComponent(m_pCullingCollider);
#endif
		m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
		m_pGameInstance->Add_RenderGroup(RENDER::OCCLUSION, this);
	}
}

HRESULT CDisplayBox::Render()
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

HRESULT CDisplayBox::Ready_Components(const _tchar* pComponentTag)
{
	auto pCullingCollider = static_cast<COBBCollider*>(m_pCullingCollider);
	pCullingCollider->SetCollision({ 0.f, 1.5f, 0.f }, {}, { 2.f, 2.f, 2.f });

	_float3 Com_Size = pCullingCollider->GetOrizinBounding().Extents;

	/* Com_Model */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::LEVEL_PROB), pComponentTag,
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	///* Com_Interaction */
	//CInteraction_Component::INTERACTION_DESC InteractionDesc = {};
	//InteractionDesc.vSize = Com_Size;
	//InteractionDesc.BeginCallBackFunc = [&]() { this->Begin_OverlapCallBack(); };
	//InteractionDesc.EndCallBackFunc = [&]() { this->End_OverlapCallBack(); };
	//InteractionDesc.InteractionEvent = [&](_float fTimeDelta, CGameObject* pActionObject) { Excute_CallBack(fTimeDelta, pActionObject); };

	//if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_InteractionBinder"),
	//	TEXT("Com_Interaction"), reinterpret_cast<CComponent**>(&m_pInteractionCom), &InteractionDesc)))
	//	return E_FAIL;

	//m_pInteractionCom->SetInteractionHitType(HIT_TYPE::INTERACTION);
	//m_pInteractionCom->ADD_InteractionIgnoreObject(HIT_TYPE::MONSTER);

	/* Com_Shader */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CDisplayBox::Bind_ShaderResources()
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

CDisplayBox* CDisplayBox::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDisplayBox* pInstance = new CDisplayBox(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CDisplayBox::Clone(void* pArg)
{
	CDisplayBox* pInstance = new CDisplayBox(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CDisplayBox");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDisplayBox::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
}
