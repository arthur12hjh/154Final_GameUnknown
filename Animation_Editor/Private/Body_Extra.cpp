#include "pch.h"
#include "Body_Extra.h"

#include "GameInstance.h"

#include "Character.h"

CBody_Extra::CBody_Extra(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject{ pDevice, pContext }
{
}

CBody_Extra::CBody_Extra(const CBody_Extra& Prototype)
	: CPartObject{ Prototype }
{
}

const _float4x4* CBody_Extra::Get_BoneMatrixPtr(const _char* pBoneName)
{
	return m_pModelCom->Get_BoneMatrixPtr(pBoneName);
}

HRESULT CBody_Extra::Mapping_Shader_Material(_uint iIdx)
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

_bool CBody_Extra::isFinish_Att()
{
	return false;
}

HRESULT CBody_Extra::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBody_Extra::Initialize(void* pArg)
{
	BODY_EXTRA_DESC* pDesc = static_cast<BODY_EXTRA_DESC*>(pArg);

	m_szModelTag = pDesc->szModelTag;

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

	m_iAnimationIndex = 0;
	m_pModelCom->Set_AnimationIndex(m_iAnimationIndex);

	return S_OK;
}

void CBody_Extra::Priority_Update(_float fTimeDelta)
{
	int a = 10;
}

void CBody_Extra::Update(_float fTimeDelta)
{
	//if (*m_pParentState & CCharacter::STATE_ATTACK)
	//	m_pModelCom->Set_AnimationIndex(0, false);
	//
	//if (*m_pParentState & CCharacter::STATE_IDLE)
	//	m_pModelCom->Set_AnimationIndex(3);
	//
	//if (*m_pParentState & CCharacter::STATE_WALK)
	//	m_pModelCom->Set_AnimationIndex(4);

	XMStoreFloat4x4(&m_CombinedWorldMatrix,
		XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()) * XMLoadFloat4x4(m_pParentTransformCom->Get_WorldMatrixPtr()));

}

void CBody_Extra::Late_Update(_float fTimeDelta)
{
	//m_pGameInstance->Add_RenderGroup(RENDER::SHADOW, this);
	m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);

#ifdef _DEBUG

#endif
}

HRESULT CBody_Extra::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();


	if (m_szModelTag != TEXT("Prototype_Component_Model_Scarlet_Face_Morph"))
	{
		for (size_t i = 0; i < iNumMeshes; i++)
		{
			if (FAILED(m_pModelCom->Bind_BoneSRV(i, m_pShaderCom, "g_BoneMatrixBuffer")))
				return E_FAIL;

			if (FAILED(m_pModelCom->Bind_AllMaterials(i, m_pShaderCom, 0)))
				return E_FAIL;

			if (FAILED(m_pShaderCom->Begin(0)))
				return E_FAIL;

			if (FAILED(m_pModelCom->Render(i)))
				return E_FAIL;
		}
	}
	else
	{
		for (size_t i = 0; i < iNumMeshes; i++)
		{
			if (FAILED(m_pModelCom->Bind_BoneSRV(i, m_pShaderCom, "g_BoneMatrixBuffer")))
				return E_FAIL;

			if (FAILED(m_pModelCom->Bind_AllMaterials(i, m_pShaderCom, 0)))
				return E_FAIL;

			//여기서 머테리얼 인덱스 따라 패스 구분.
			if (FAILED(Mapping_Shader_Material(i)))
				return E_FAIL;

			if (FAILED(m_pModelCom->Render(i)))
				return E_FAIL;
		}
	}




	return S_OK;
}

HRESULT CBody_Extra::Render_Shadow()
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
		if (FAILED(m_pModelCom->Bind_BoneSRV(i, m_pShaderCom, "g_BoneMatrixBuffer")))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(1)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}
	return S_OK;
}

HRESULT CBody_Extra::Ready_Components()
{
	/* Com_Model */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::EDITOR), m_szModelTag,
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	if (m_szModelTag == TEXT("Prototype_Component_Model_Scarlet_Face_Morph")) 
	{
		/* Com_Shader */
		if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::EDITOR), TEXT("Prototype_Component_Shader_Scarlet_Face"),
			TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
			return E_FAIL;
	}
	else
	{
		/* Com_Shader */
		if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::EDITOR), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
			TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CBody_Extra::Bind_ShaderResources()
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

CBody_Extra* CBody_Extra::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBody_Extra* pInstance = new CBody_Extra(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBody_Extra");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBody_Extra::Clone(void* pArg)
{
	CBody_Extra* pInstance = new CBody_Extra(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBody_Extra");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBody_Extra::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}
