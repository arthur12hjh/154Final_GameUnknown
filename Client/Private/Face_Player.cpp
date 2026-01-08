#include "pch.h"
#include "Face_Player.h"
#include "Body_Player.h"

#include "Bone.h"
#include "Model.h"

#include "GameInstance.h"
#include "GameManager.h"
#include "Player.h"

CFace_Player::CFace_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPlayer_Parts{ pDevice, pContext }
{
}

CFace_Player::CFace_Player(const CFace_Player& Prototype)
	: CPlayer_Parts{ Prototype }
{
}

HRESULT CFace_Player::Mapping_Shader_Material(_uint iIdx)
{
	// idx 0 (Shader pass : 4)	
	if (strcmp(m_pModelCom->Get_MaterialName(m_pModelCom->Get_Mesh_MaterialIndex(iIdx)), "MA_MouthInner_Inst") == 0)
	{
		if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(FACE_MATERIAL::MA_MouthInner_Inst))))
			return E_FAIL;
	}
	// idx 1 (Shader pass : 5)	
	else if (strcmp(m_pModelCom->Get_MaterialName(m_pModelCom->Get_Mesh_MaterialIndex(iIdx)), "M_MikeEyeBlend_Inst") == 0)
	{
		if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(FACE_MATERIAL::M_MikeEyeBlend_Inst))))
			return E_FAIL;
	}
	// idx 2 (Shader pass : 6)	
	else if (strcmp(m_pModelCom->Get_MaterialName(m_pModelCom->Get_Mesh_MaterialIndex(iIdx)), "M_lacrimal_fluid") == 0)
	{
		if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(FACE_MATERIAL::M_lacrimal_fluid))))
			return E_FAIL;
	}
	// idx 3 (Shader pass : 7)	
	else if (strcmp(m_pModelCom->Get_MaterialName(m_pModelCom->Get_Mesh_MaterialIndex(iIdx)), "MI_EVE_Head_V02") == 0)
	{
		if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(FACE_MATERIAL::MI_EVE_Head_V02))))
			return E_FAIL;
	}
	// idx 4 (Shader pass : 8)	
	else if (strcmp(m_pModelCom->Get_MaterialName(m_pModelCom->Get_Mesh_MaterialIndex(iIdx)), "MI_EyeRefractive1") == 0)
	{
		if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(FACE_MATERIAL::MI_EyeRefractive1))))
			return E_FAIL;
	}
	// idx 5 (Shader pass : 9)	
	else if (strcmp(m_pModelCom->Get_MaterialName(m_pModelCom->Get_Mesh_MaterialIndex(iIdx)), "MI_EVE_Eyeshadow_Occlusion") == 0)
	{
		if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(FACE_MATERIAL::MI_EVE_Eyeshadow_Occlusion))))
			return E_FAIL;
	}
	// idx 6 (Shader pass : 10)	
	else if (strcmp(m_pModelCom->Get_MaterialName(m_pModelCom->Get_Mesh_MaterialIndex(iIdx)), "NewMaterial") == 0)
	{
		if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(FACE_MATERIAL::NewMaterial))))
			return E_FAIL;
	}
	// idx 7 (Shader pass : 11)	
	else if (strcmp(m_pModelCom->Get_MaterialName(m_pModelCom->Get_Mesh_MaterialIndex(iIdx)), "MI_EyeBrow1") == 0)
	{
		if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(FACE_MATERIAL::MI_EyeBrow1))))
			return E_FAIL;
	}
	// idx 8 (Shader pass : 12)	
	else if (strcmp(m_pModelCom->Get_MaterialName(m_pModelCom->Get_Mesh_MaterialIndex(iIdx)), "EyeLight_Inst") == 0)
	{
		if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(FACE_MATERIAL::EyeLight_Inst))))
			return E_FAIL;
	}
	// idx 9 (Shader pass : 13)	
	else if (strcmp(m_pModelCom->Get_MaterialName(m_pModelCom->Get_Mesh_MaterialIndex(iIdx)), "MI_Teeth") == 0)
	{
		if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(FACE_MATERIAL::MI_Teeth))))
			return E_FAIL;
	}
	// idx 10 (Shader pass : 14)	
	else if (strcmp(m_pModelCom->Get_MaterialName(m_pModelCom->Get_Mesh_MaterialIndex(iIdx)), "MA_TeethOcculusion_Inst1") == 0)
	{
		if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(FACE_MATERIAL::MA_TeethOcculusion_Inst1))))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CFace_Player::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CFace_Player::Initialize(void* pArg)
{
	FACE_PLAYER_DESC* pDesc = static_cast<FACE_PLAYER_DESC*>(pArg);

	//m_pSocketMatrix = pDesc->pSocketMatrix;
	//strcpy_s(m_szBoneTag, pDesc->szBoneTag);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	Bind_BoneToPartBody(pDesc->pBodyPtr);

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));

	m_pModelCom->Bind_MaterialTag(TEXTURE_TYPE::DIFFUSE, "g_DiffuseTexture");
	m_pModelCom->Bind_MaterialTag(TEXTURE_TYPE::OPACITY, "g_OpacityTexture");
	m_pModelCom->Bind_MaterialTag(TEXTURE_TYPE::NORMAL, "g_NormalTexture");
	//m_pModelCom->Bind_MaterialTag(TEXTURE_TYPE::EMISSIVE, "g_EmissiveTexture");
	m_pModelCom->Bind_MaterialTag(TEXTURE_TYPE::ORM, "g_ORMTexture");
	m_pModelCom->Bind_MaterialTag(TEXTURE_TYPE::ORSS, "g_ORSSTexture");

	return S_OK;
}

void CFace_Player::Priority_Update(_float fTimeDelta)
{

}

void CFace_Player::Update(_float fTimeDelta)
{

}

void CFace_Player::Late_Update(_float fTimeDelta)
{
	XMStoreFloat4x4(&m_CombinedWorldMatrix,
		XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()) * XMLoadFloat4x4(m_pParentTransformCom->Get_WorldMatrixPtr()));

	m_pMotionTrail->Update_Trail(fTimeDelta);
	if (m_bIsActive)
	{
		m_pGameInstance->Add_RenderGroup(RENDER::NONLIGHT, m_pMotionTrail);
		m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
		m_pGameInstance->Add_RenderGroup(RENDER::SHADOW, this);
	}
	//m_pRigidBody->Update_PxTransform(XMLoadFloat4x4(&m_CombinedWorldMatrix), true);

	//m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
#ifdef _DEBUG

#endif
}

HRESULT CFace_Player::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	//Shader_Eve_Face
	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pSpecDetailTextureCom->Bind_ShaderResource(m_pShaderCom, "g_SpecDetailTexture", 0)))
			return E_FAIL;

		if (FAILED(m_pSSSAOCom->Bind_ShaderResource(m_pShaderCom, "g_SSSAOTexture", 0)))
			return E_FAIL;

		if (FAILED(m_pBodyModelCom->Bind_BoneMatrixSRV(m_pShaderCom, "g_BoneMatrixBuffer")))
			return E_FAIL;
		
		if (FAILED(m_pBodyModelCom->Bind_PreBoneMatrixSRV(m_pShaderCom)))
			return E_FAIL;
		
		if (FAILED(m_pBodyModelCom->Bind_GlobalOffsetMatrices(m_pShaderCom)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_AllMaterials(i, m_pShaderCom, 0)))
			return E_FAIL;

		//여기서 머테리얼 인덱스 따라 패스 구분.
		if(FAILED(Mapping_Shader_Material(i)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}


	return S_OK;
}

HRESULT CFace_Player::Render_Shadow()
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

HRESULT CFace_Player::Render_MotionBlur()
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

HRESULT CFace_Player::Ready_Components()
{
	/* Com_Model */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_Face_Eve"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* Com_MotionTrail */
	CMotionTrailComponent::MOTION_TRAIL_COMPONENT_DESC MotionTrailCom = {};
	MotionTrailCom.pModel = m_pModelCom;
	MotionTrailCom.pPreBoneModel = m_pBodyModelCom;
	MotionTrailCom.pTransform = &m_CombinedWorldMatrix;
	MotionTrailCom.fUpdateTime = 0.2f;
	MotionTrailCom.fLifeTime = 0.45f;

	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_MotionTrail"),
		TEXT("Com_MotionTrail"), reinterpret_cast<CComponent**>(&m_pMotionTrail), &MotionTrailCom)))
		return E_FAIL;

	m_pMotionTrail->SetMotionTrailColor({ 0.f , 1.f, 0.f, 1.f });
	m_pMotionTrail->SetRimLight(0.1f, 0.7f);

	/* Com_Shader */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Shader_Eve_Face"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* Com_SpecDetail */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Eve_Head_SSSAO"),
		TEXT("Com_SpecDetail"), reinterpret_cast<CComponent**>(&m_pSSSAOCom))))
		return E_FAIL;

	/* Com_SSSAO */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Eve_Head_SpecDetail"),
		TEXT("Com_SSSAO"), reinterpret_cast<CComponent**>(&m_pSpecDetailTextureCom))))
		return E_FAIL;

	//PxUserData tUserData;
	//tUserData.szActorTag = TEXT("Face");

	////리지드 바디 Desc 세팅. 머테리얼이랑 Mass, userdata, shape, type 부분 위주로 살펴보세요.
	//CRigidBody::RIGIDBODY_DESC RigidBodyDesc;
	//// 콜라이더 모양
	//RigidBodyDesc.eRigidBodyShape = CRigidBody::RIGIDBODY_SHAPE::CAPSULE;

	//// 충돌처리를 할지말지 
	//// DYNAMIC : 충돌 
	//// KINEMATIC : 충돌 X
	//RigidBodyDesc.eRigidBodyType = CRigidBody::RIGIDBODY_TYPE::KINEMATIC;

	//RigidBodyDesc.StartWorldMatrix = *m_pTransformCom->Get_WorldMatrixPtr();
	//RigidBodyDesc.tUserData = tUserData;
	//RigidBodyDesc.vMaterial = _float3(0.5f, 0.5f, 0.3f);
	//RigidBodyDesc.vSize = m_pTransformCom->Get_Scale();
	//RigidBodyDesc.fMass = { 0.3f };
	//RigidBodyDesc.isQuery = { false };
	///* Com_RigidBody */
	//if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_RigidBody"),
	//	TEXT("Com_RigidBody"), reinterpret_cast<CComponent**>(&m_pRigidBody), &RigidBodyDesc)))
	//	return E_FAIL;

	// 리지드 바디 세팅 끝났으면 Physx 매니저에 집어넣는 과정도 있어야돼요.
	// 없으면 충돌 안됨
	m_pGameInstance->Add_RigidBody_ToPhysx(this, m_pRigidBody);

	return S_OK;
}

HRESULT CFace_Player::Bind_ShaderResources()
{
	CPlayer* pPlayer = m_pGameManager->GetGameCharacter();
	Safe_Release(pPlayer);
	_bool bFlag = static_cast<CCharacter*>(pPlayer)->Get_DepthMaskingB();

	if (FAILED(m_pShaderCom->Bind_RawValue("g_IsMaskingDepthB", &bFlag, sizeof(_bool))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_IsMaskingDepthW", &bFlag, sizeof(_bool))))
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

HRESULT CFace_Player::Bind_BoneToPartBody(void* pArg)
{
	CBody_Player* pBody = static_cast<CBody_Player*>(pArg);

	m_pBodyModelCom = static_cast<CModel*>(pBody->Find_Component(TEXT("Com_Model")));




	return S_OK;
}

CFace_Player* CFace_Player::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CFace_Player* pInstance = new CFace_Player(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CFace_Player");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CFace_Player::Clone(void* pArg)
{
	CFace_Player* pInstance = new CFace_Player(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CFace_Player");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFace_Player::Free()
{
	__super::Free();

	Safe_Release(m_pSpecDetailTextureCom);

	Safe_Release(m_pSSSAOCom);
	Safe_Release(m_pRigidBody);
}