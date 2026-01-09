#include "pch.h"
#include "NaytibaFace.h"

#include "GameInstance.h"
#include "Character.h"

CNaytibaFace::CNaytibaFace(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) :
    CPartObject(pDevice, pContext)
{
}

CNaytibaFace::CNaytibaFace(const CNaytibaFace& Prototype) :
    CPartObject(Prototype)
{
}

HRESULT CNaytibaFace::Mapping_Shader_Material(_uint iIdx)
{
	// idx 0 (Shader pass : 4)	
	if (strcmp(m_pModelCom->Get_MaterialName(m_pModelCom->Get_Mesh_MaterialIndex(iIdx)), "MI_CH_M_NA_961_Head.001") == 0)
	{
		if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(FACE_MATERIAL::MI_CH_M_NA_961_Head))))
			return E_FAIL;
	}
	// idx 1 (Shader pass : 5)	
	else if (strcmp(m_pModelCom->Get_MaterialName(m_pModelCom->Get_Mesh_MaterialIndex(iIdx)), "MI_CH_M_NA_961_Eyebrow.001") == 0)
	{
		if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(FACE_MATERIAL::MI_CH_M_NA_961_Eyebrow))))
			return E_FAIL;
	}
	// idx 2 (Shader pass : 6)	
	else if (strcmp(m_pModelCom->Get_MaterialName(m_pModelCom->Get_Mesh_MaterialIndex(iIdx)), "MI_CH_M_NA_961_Eyes.001") == 0)
	{
		if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(FACE_MATERIAL::MI_CH_M_NA_961_Eyes))))
			return E_FAIL;
	}
	// idx 3 (Shader pass : 7)	
	else if (strcmp(m_pModelCom->Get_MaterialName(m_pModelCom->Get_Mesh_MaterialIndex(iIdx)), "MI_CH_M_NA_961_Lens.001") == 0)
	{
		if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(FACE_MATERIAL::MI_CH_M_NA_961_Lens))))
			return E_FAIL;
	}
	// idx 4 (Shader pass : 8)	
	else if (strcmp(m_pModelCom->Get_MaterialName(m_pModelCom->Get_Mesh_MaterialIndex(iIdx)), "MI_CH_M_NA_961_Eyelashes.001") == 0)
	{
		if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(FACE_MATERIAL::MI_CH_M_NA_961_Eyelashes))))
			return E_FAIL;
	}
	// idx 5 (Shader pass : 9)	
	else if (strcmp(m_pModelCom->Get_MaterialName(m_pModelCom->Get_Mesh_MaterialIndex(iIdx)), "MI_CH_M_NA_961_Tearline.001") == 0)
	{
		if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(FACE_MATERIAL::MI_CH_M_NA_961_Tearline))))
			return E_FAIL;
	}
	// idx 6 (Shader pass : 10)	
	else if (strcmp(m_pModelCom->Get_MaterialName(m_pModelCom->Get_Mesh_MaterialIndex(iIdx)), "MI_CH_M_NA_961_Eyeshadow.001") == 0)
	{
		if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(FACE_MATERIAL::MI_CH_M_NA_961_Eyeshadow))))
			return E_FAIL;
	}
	// idx 7 (Shader pass : 11)	
	else if (strcmp(m_pModelCom->Get_MaterialName(m_pModelCom->Get_Mesh_MaterialIndex(iIdx)), "MI_CH_M_NA_961_EyeBlend.001") == 0)
	{
		if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(FACE_MATERIAL::MI_CH_M_NA_961_EyeBlend))))
			return E_FAIL;
	}
	// idx 8 (Shader pass : 12)	
	else if (strcmp(m_pModelCom->Get_MaterialName(m_pModelCom->Get_Mesh_MaterialIndex(iIdx)), "MI_CH_M_NA_961_NoseShadow.001") == 0)
	{
		if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(FACE_MATERIAL::MI_CH_M_NA_961_NoseShadow))))
			return E_FAIL;
	}
	// idx 9 (Shader pass : 13)	
	else if (strcmp(m_pModelCom->Get_MaterialName(m_pModelCom->Get_Mesh_MaterialIndex(iIdx)), "MI_CH_M_NA_961_Teeth.001") == 0)
	{
		if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(FACE_MATERIAL::MI_CH_M_NA_961_Teeth))))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CNaytibaFace::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CNaytibaFace::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

	NAYTIBA_FACE_DESC* pDesc = static_cast<NAYTIBA_FACE_DESC*>(pArg);

	Bind_BoneToPartBody(pDesc->pBodyModelCom);

	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));

	m_pModelCom->Bind_MaterialTag(TEXTURE_TYPE::DIFFUSE, "g_DiffuseTexture");
	m_pModelCom->Bind_MaterialTag(TEXTURE_TYPE::OPACITY, "g_OpacityTexture");
	m_pModelCom->Bind_MaterialTag(TEXTURE_TYPE::NORMAL, "g_NormalTexture");
	m_pModelCom->Bind_MaterialTag(TEXTURE_TYPE::EMISSIVE, "g_EmissiveTexture");
	m_pModelCom->Bind_MaterialTag(TEXTURE_TYPE::ORM, "g_ORMTexture");
	m_pModelCom->Bind_MaterialTag(TEXTURE_TYPE::ORSS, "g_ORSSTexture");

    return S_OK;
}

void CNaytibaFace::Priority_Update(_float fTimeDelta)
{
}

void CNaytibaFace::Update(_float fTimeDelta)
{
}

void CNaytibaFace::Late_Update(_float fTimeDelta)
{
	XMStoreFloat4x4(&m_CombinedWorldMatrix,
		XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()) * XMLoadFloat4x4(m_pParentTransformCom->Get_WorldMatrixPtr()));

	if (m_bIsActive && VISIBILITY::VISIBLE == m_eVisibility)
	{
		m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
		m_pGameInstance->Add_RenderGroup(RENDER::SHADOW, this);
	}
}

HRESULT CNaytibaFace::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	//Shader_Eve_Face
	for (size_t i = 0; i < iNumMeshes; i++)
	{
		//if (FAILED(m_pSpecDetailTextureCom->Bind_ShaderResource(m_pShaderCom, "g_SpecDetailTexture", 0)))
		//	return E_FAIL;

		//if (FAILED(m_pSSSAOCom->Bind_ShaderResource(m_pShaderCom, "g_SSSAOTexture", 0)))
		//	return E_FAIL;

		if (FAILED(m_pBodyModelCom->Bind_BoneMatrixSRV(m_pShaderCom, "g_BoneMatrixBuffer")))
			return E_FAIL;

		if (FAILED(m_pBodyModelCom->Bind_PreBoneMatrixSRV(m_pShaderCom)))
			return E_FAIL;

		if (FAILED(m_pBodyModelCom->Bind_GlobalOffsetMatrices(m_pShaderCom)))
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

HRESULT CNaytibaFace::Render_Shadow()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_Shadow_Resource_Cascade(m_pShaderCom, "g_LightViewMatrix", D3DTS::VIEW)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_Shadow_Resource_Cascade(m_pShaderCom, "g_LightProjMatrix", D3DTS::PROJ)))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pBodyModelCom->Bind_BoneMatrixSRV(m_pShaderCom, "g_BoneMatrixBuffer")))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(1)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CNaytibaFace::Render_MotionBlur()
{
	/* 이전 프레임 월드매트릭스도 바인딩 */
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_PreWorldMatrix", &m_PreCombinedWorldMatrix)))
		return E_FAIL;

	/* 이전 뷰 매트릭스도 바인딩 */
	if (FAILED(m_pShaderCom->Bind_Matrix("g_PreViewMatrix", m_pGameInstance->Get_PreTransform_Float4x4(D3DTS::VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pBodyModelCom->Bind_BoneMatrixSRV(m_pShaderCom, "g_BoneMatrixBuffer")))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Begin(4)))
			return E_FAIL;
		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

	return S_OK;
}


HRESULT CNaytibaFace::Ready_Components()
{
	/* Com_Model */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_Face_Scarlet"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* Com_Shader */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Shader_Scarlet_Face"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

    return S_OK;
}

HRESULT CNaytibaFace::Bind_ShaderResources()
{
	_bool bFlag = static_cast<CCharacter*>(m_pParent)->Get_DepthMaskingB();
	if (FAILED(m_pShaderCom->Bind_RawValue("g_IsMaskingDepthB", &bFlag, sizeof(_bool))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_IsMaskingDepthW", &bFlag, sizeof(_bool))))
		return E_FAIL;

	if (nullptr == m_pShaderCom)
		return E_FAIL;

	/*m_pShaderCom->Bind_Matrix("g_WorldMatrix", );*/
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
		return E_FAIL;

	return S_OK;
}

HRESULT CNaytibaFace::Bind_BoneToPartBody(CModel* pArg)
{
	m_pBodyModelCom = pArg;

	return S_OK;
}

CNaytibaFace* CNaytibaFace::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CNaytibaFace* pInstance = new CNaytibaFace(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : NaytibaFace");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CNaytibaFace::Clone(void* pArg)
{
	CNaytibaFace* pInstance = new CNaytibaFace(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : NaytibaFace");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CNaytibaFace::Free()
{
	__super::Free();
}
