#include "pch.h"
#include "Face_Scarlet.h"
#include "Body_Scarlet.h"

#include "Bone.h"
#include "Model.h"

#include "GameInstance.h"

#include "Scarlet.h"

CFace_Scarlet::CFace_Scarlet(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject{ pDevice, pContext }
{
}

CFace_Scarlet::CFace_Scarlet(const CFace_Scarlet& Prototype)
	: CPartObject{ Prototype }
{
}


HRESULT CFace_Scarlet::Mapping_Shader_Material(_uint iIdx)
{
	// idx 0 (Shader pass : 4)	
	if (strcmp(m_pModelCom->Get_MaterialName(m_pModelCom->Get_Mesh_MaterialIndex(iIdx)), "MI_CH_M_NA_961_Head") == 0)
	{
		if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(FACE_MATERIAL::MI_CH_M_NA_961_Head))))
			return E_FAIL;
	}
	// idx 1 (Shader pass : 5)	
	else if (strcmp(m_pModelCom->Get_MaterialName(m_pModelCom->Get_Mesh_MaterialIndex(iIdx)), "MI_CH_M_NA_961_Eyebrow") == 0)
	{
		if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(FACE_MATERIAL::MI_CH_M_NA_961_Eyebrow))))
			return E_FAIL;
	}
	// idx 2 (Shader pass : 6)	
	else if (strcmp(m_pModelCom->Get_MaterialName(m_pModelCom->Get_Mesh_MaterialIndex(iIdx)), "MI_CH_M_NA_961_Eyes") == 0)
	{
		if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(FACE_MATERIAL::MI_CH_M_NA_961_Eyes))))
			return E_FAIL;
	}
	// idx 3 (Shader pass : 7)	
	else if (strcmp(m_pModelCom->Get_MaterialName(m_pModelCom->Get_Mesh_MaterialIndex(iIdx)), "MI_CH_M_NA_961_Lens") == 0)
	{
		if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(FACE_MATERIAL::MI_CH_M_NA_961_Lens))))
			return E_FAIL;
	}
	// idx 4 (Shader pass : 8)	
	else if (strcmp(m_pModelCom->Get_MaterialName(m_pModelCom->Get_Mesh_MaterialIndex(iIdx)), "MI_CH_M_NA_961_Eyelashes") == 0)
	{
		if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(FACE_MATERIAL::MI_CH_M_NA_961_Eyelashes))))
			return E_FAIL;
	}
	// idx 5 (Shader pass : 9)	
	else if (strcmp(m_pModelCom->Get_MaterialName(m_pModelCom->Get_Mesh_MaterialIndex(iIdx)), "MI_CH_M_NA_961_Tearline") == 0)
	{
		if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(FACE_MATERIAL::MI_CH_M_NA_961_Tearline))))
			return E_FAIL;
	}
	// idx 6 (Shader pass : 10)	
	else if (strcmp(m_pModelCom->Get_MaterialName(m_pModelCom->Get_Mesh_MaterialIndex(iIdx)), "MI_CH_M_NA_961_Eyeshadow") == 0)
	{
		if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(FACE_MATERIAL::MI_CH_M_NA_961_Eyeshadow))))
			return E_FAIL;
	}
	// idx 7 (Shader pass : 11)	
	else if (strcmp(m_pModelCom->Get_MaterialName(m_pModelCom->Get_Mesh_MaterialIndex(iIdx)), "MI_CH_M_NA_961_EyeBlend") == 0)
	{
		if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(FACE_MATERIAL::MI_CH_M_NA_961_EyeBlend))))
			return E_FAIL;
	}
	// idx 8 (Shader pass : 12)	
	else if (strcmp(m_pModelCom->Get_MaterialName(m_pModelCom->Get_Mesh_MaterialIndex(iIdx)), "MI_CH_M_NA_961_NoseShadow") == 0)
	{
		if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(FACE_MATERIAL::MI_CH_M_NA_961_NoseShadow))))
			return E_FAIL;
	}
	// idx 9 (Shader pass : 13)	
	else if (strcmp(m_pModelCom->Get_MaterialName(m_pModelCom->Get_Mesh_MaterialIndex(iIdx)), "MI_CH_M_NA_961_Teeth") == 0)
	{
		if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(FACE_MATERIAL::MI_CH_M_NA_961_Teeth))))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CFace_Scarlet::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CFace_Scarlet::Initialize(void* pArg)
{
	FACE_SCARLET_DESC* pDesc = static_cast<FACE_SCARLET_DESC*>(pArg);

	m_pParentState = pDesc->pParentState;
	//m_pSocketMatrix = pDesc->pSocketMatrix;
	//strcpy_s(m_szBoneTag, pDesc->szBoneTag);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pModelCom->Bind_MaterialTag(TEXTURE_TYPE::DIFFUSE, "g_DiffuseTexture");
	m_pModelCom->Bind_MaterialTag(TEXTURE_TYPE::OPACITY, "g_OpacityTexture");
	m_pModelCom->Bind_MaterialTag(TEXTURE_TYPE::NORMAL, "g_NormalTexture");
	m_pModelCom->Bind_MaterialTag(TEXTURE_TYPE::EMISSIVE, "g_EmissiveTexture");
	m_pModelCom->Bind_MaterialTag(TEXTURE_TYPE::ORM, "g_ORMTexture");
	m_pModelCom->Bind_MaterialTag(TEXTURE_TYPE::ORSS, "g_ORSSTexture");

	Bind_BoneToPartBody(pDesc->pBodyPtr);

	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));
	//m_pTransformCom->Rotation(XMConvertToRadians(90.f), XMConvertToRadians(90.f), XMConvertToRadians(0.f));


	m_pModelCom->Set_MorphAnimationIndex(7, TRUE);
	//m_pModelCom->Set_MorphAnimationIndex(12, TRUE);

	return S_OK;
}

void CFace_Scarlet::Priority_Update(_float fTimeDelta)
{
	int a = 10;
	m_pModelCom->Set_MorphAnimationIndex(8, TRUE);
}

void CFace_Scarlet::Update(_float fTimeDelta)
{
	//if (*m_pParentState & CScarlet::STATE_ATTACK)
	//	m_pModelCom->Set_AnimationIndex(0, false);
	//
	//if (*m_pParentState & CScarlet::STATE_IDLE)
	//	m_pModelCom->Set_AnimationIndex(3);
	//
	//if (*m_pParentState & CScarlet::STATE_WALK)
	//	m_pModelCom->Set_AnimationIndex(4);	

	XMStoreFloat4x4(&m_CombinedWorldMatrix,
		XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()) * XMLoadFloat4x4(m_pParentTransformCom->Get_WorldMatrixPtr()));
	//XMStoreFloat4x4(&m_CombinedWorldMatrix, XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));

}

void CFace_Scarlet::Late_Update(_float fTimeDelta)
{
	//m_pGameInstance->Add_RenderGroup(RENDER::SHADOW, this);
	m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);

	m_pModelCom->Play_MorphAnimation(fTimeDelta);

#ifdef _DEBUG

#endif
}

HRESULT CFace_Scarlet::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{

		//if (FAILED(m_pModelCom->Bind_BoneSRV(i, m_pShaderCom, "g_BoneMatrixBuffer")))
		//	return E_FAIL;

		if (FAILED(m_pModelCom->Bind_ShapeKeys(i, m_pShaderCom, "g_ShapeKeyWeights")))
			return E_FAIL;

		if (FAILED(m_pBodyModelCom->Bind_BoneMatrixSRV(m_pShaderCom, "g_BoneMatrixBuffer")))
			return E_FAIL;

		if (FAILED(m_pBodyModelCom->Bind_PreBoneMatrixSRV(m_pShaderCom)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_GlobalOffsetMatrices(m_pShaderCom)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_AllMaterials(i, m_pShaderCom, 0)))
			return E_FAIL;

		//여기서 머테리얼 인덱스 따라 패스 구분.
		if (FAILED(Mapping_Shader_Material(i)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}




	return S_OK;
}

HRESULT CFace_Scarlet::Render_Shadow()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_Shadow_Resource_Cascade(m_pShaderCom, "g_ViewMatrix", D3DTS::VIEW)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_Shadow_Resource_Cascade(m_pShaderCom, "g_ProjMatrix", D3DTS::PROJ)))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{

		//if (FAILED(m_pModelCom->Bind_BoneSRV(i, m_pShaderCom, "g_BoneMatrixBuffer")))
		//	return E_FAIL;

		if (FAILED(m_pBodyModelCom->Bind_BoneMatrixSRV(m_pShaderCom, "g_BoneMatrixBuffer")))
			return E_FAIL;

		if (FAILED(m_pBodyModelCom->Bind_PreBoneMatrixSRV(m_pShaderCom)))
			return E_FAIL;

		if (FAILED(m_pBodyModelCom->Bind_GlobalOffsetMatrices(m_pShaderCom)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(1)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}
	return S_OK;
}

HRESULT CFace_Scarlet::Ready_Components()
{
	/* Com_Model */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::EDITOR), TEXT("Prototype_Component_Model_Scarlet_Face_Morph"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* Com_Shader */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::EDITOR), TEXT("Prototype_Component_Shader_Scarlet_Face"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CFace_Scarlet::Bind_ShaderResources()
{
	/*m_pShaderCom->Bind_Matrix("g_WorldMatrix", );*/
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
		return E_FAIL;

	return S_OK;
}

HRESULT CFace_Scarlet::Bind_BoneToPartBody(void* pArg)
{
	CBody_Scarlet* pBody = static_cast<CBody_Scarlet*>(pArg);

	m_pBodyModelCom = static_cast<CModel*>(pBody->Find_Component(TEXT("Com_Model")));



	return S_OK;
}

CFace_Scarlet* CFace_Scarlet::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CFace_Scarlet* pInstance = new CFace_Scarlet(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CFace_Scarlet");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CFace_Scarlet::Clone(void* pArg)
{
	CFace_Scarlet* pInstance = new CFace_Scarlet(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CFace_Scarlet");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFace_Scarlet::Free()
{
	__super::Free();

}
