#include "pch.h"
#include "PonyTail_Player.h"
#include "Body_Player.h"

#include "Bone.h"
#include "Model.h"

#include "GameManager.h"
#include "GameInstance.h"

#include "Player.h"
#include "StringHelper.h"
#include "Texture.h" 

CPonyTail_Player::CPonyTail_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPlayer_Parts{ pDevice, pContext }
{
}

CPonyTail_Player::CPonyTail_Player(const CPonyTail_Player& Prototype)
	: CPlayer_Parts{ Prototype }
	, m_pGameManager { CGameManager::GetInstance() }
{
	Safe_AddRef(m_pGameManager);
}


HRESULT CPonyTail_Player::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPonyTail_Player::Initialize(void* pArg)
{
	PONYTAIL_PLAYER_DESC* pDesc = static_cast<PONYTAIL_PLAYER_DESC*>(pArg);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	Bind_BoneToPartBody(pDesc->pBodyPtr);

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_HairJoints()))
		return E_FAIL;

	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));

	m_pModelCom->Bind_MaterialTag(TEXTURE_TYPE::DIFFUSE, "g_DiffuseTexture");
	m_pModelCom->Bind_MaterialTag(TEXTURE_TYPE::OPACITY, "g_OpacityTexture");
	m_pModelCom->Bind_MaterialTag(TEXTURE_TYPE::NORMAL, "g_NormalTexture");
	//m_pModelCom->Bind_MaterialTag(TEXTURE_TYPE::EMISSIVE, "g_EmissiveTexture");
	m_pModelCom->Bind_MaterialTag(TEXTURE_TYPE::ORM, "g_ORMTexture");
	m_pModelCom->Bind_MaterialTag(TEXTURE_TYPE::ORSS, "g_ORSSTexture");


	//루트, 링크 데스크
	m_tRootDesc = static_cast<JOINT_CHAIN_DESC*>(m_pJointChain->Get_RootDesc());
	m_tLinkDesc = static_cast<JOINT_CHAIN_DESC*>(m_pJointChain->Get_JointDesc());

	m_pModelCom->Set_PreTransformMatrix(XMMatrixScaling(0.06, 0.03, 0.06) * 
		XMMatrixRotationY(XMConvertToRadians(270.f)));

	return S_OK;
}

void CPonyTail_Player::Priority_Update(_float fTimeDelta)
{

}

void CPonyTail_Player::Update(_float fTimeDelta)
{
}

void CPonyTail_Player::Late_Update(_float fTimeDelta)
{
	XMStoreFloat4x4(&m_CombinedWorldMatrix,
		XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()) * XMLoadFloat4x4(m_pParentTransformCom->Get_WorldMatrixPtr()));

	m_pHairRoot->Update_PxTransform(
		XMLoadFloat4x4(m_pHairRootBone) * XMLoadFloat4x4(&m_CombinedWorldMatrix), true);

	for (auto& Pair : m_HairRigidBodies)
	{
		Pair.second->Update_PxTransform(
			XMLoadFloat4x4(Pair.first) * XMLoadFloat4x4(&m_CombinedWorldMatrix), true); 
	}

	m_pMotionTrail->Update_Trail(fTimeDelta);

	if (m_bIsActive)
	{
		m_pGameInstance->Add_RenderGroup(RENDER::NONLIGHT, m_pMotionTrail);
		m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
		m_pGameInstance->Add_RenderGroup(RENDER::SHADOW, this);
	}
	
#ifdef _DEBUG
	m_pJointChain->Update(fTimeDelta);
#endif

	Sync_BonesByJoint();
}

HRESULT CPonyTail_Player::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pBodyModelCom->Bind_BoneMatrixSRV(m_pShaderCom, "g_BoneMatrixBuffer")))
			return E_FAIL;
		
		if (FAILED(m_pBodyModelCom->Bind_PreBoneMatrixSRV(m_pShaderCom)))
			return E_FAIL;
		
		if (FAILED(m_pBodyModelCom->Bind_GlobalOffsetMatrices(m_pShaderCom)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_AllMaterials(i, m_pShaderCom, 0)))
			return E_FAIL;

		//4번 인덱스가 머리, 5번 인덱스가 포니테일
		if (FAILED(m_pShaderCom->Begin(5)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CPonyTail_Player::Render_Shadow()
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

HRESULT CPonyTail_Player::Render_MotionBlur()
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
		if (FAILED(m_pModelCom->Bind_BoneSRV(i, m_pShaderCom, "g_BoneMatrixBuffer")))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(4)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

	return S_OK;
}

void CPonyTail_Player::SetActive(_bool bFlag)
{
	m_bIsActive = bFlag;

	m_pHairRoot->Set_Simulation(!bFlag);

	for (auto& iter : m_HairRigidBodies)
	{
		iter.second->Set_Simulation(!bFlag);
	}

	for (auto& iter : m_HairLinks)
	{
		iter.second->Set_Simulation(!bFlag);
	}
}

void CPonyTail_Player::Teleport_JointChains()
{
	m_pJointChain->Teleport_RigidBodies(XMLoadFloat4x4(m_pHairRootBone) *
		XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()) * XMLoadFloat4x4(m_pParentTransformCom->Get_WorldMatrixPtr()));
}

HRESULT CPonyTail_Player::Ready_Components()
{
	/* Com_Model */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_PonyTail_Eve"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* Com_Shader */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Shader_Eve_Hair"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* Com_JointChain */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_JointChain"),
		TEXT("Com_JointChain"), reinterpret_cast<CComponent**>(&m_pJointChain))))
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

	/* Com_Ponytail_Mask */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Eve_Ponytail_Mask"),
		TEXT("Eve_Ponytail_Mask"), reinterpret_cast<CComponent**>(&m_pPonytailMaskTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CPonyTail_Player::Ready_HairJoints()
{
	if (FAILED(Ready_RootHair()))
		return E_FAIL;

	if (FAILED(Ready_ChildHair()))
		return E_FAIL;

	if (FAILED(Ready_HairBoneMapping()))
		return E_FAIL;

	if (FAILED(Ready_HairRigidBodies()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPonyTail_Player::Ready_RootHair()
{
	// 본 매트릭스
	m_pHairRootBone = m_pBodyModelCom->Get_BoneMatrixPtr("Ab-TL-HairB01");
	const _float4x4* pBoneB02 = m_pBodyModelCom->Get_BoneMatrixPtr("Ab-TL-HairB02");

	// 이 오브젝트(또는 캐릭터) 월드
	_matrix OwnerWorld =
		XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()) *
		XMLoadFloat4x4(m_pParentTransformCom->Get_WorldMatrixPtr());

	// 본 월드
	_matrix B01World = XMLoadFloat4x4(m_pHairRootBone) * OwnerWorld;
	_matrix B02World = XMLoadFloat4x4(pBoneB02) * OwnerWorld;

	// 1) 루트(키네마틱 앵커) - 원점이 B01에 있어야 함
	CRigidBody::RIGIDBODY_DESC RootDesc;
	RootDesc.eRigidBodyShape = CRigidBody::RIGIDBODY_SHAPE::NONE;
	RootDesc.eRigidBodyType = CRigidBody::RIGIDBODY_TYPE::KINEMATIC;
	RootDesc.tUserData.szActorTag = TEXT("HairRoot");
	RootDesc.vSize = _float3(0.05f, 0.05f, 0.f);
	RootDesc.fMass = { 0.f };
	RootDesc.iCollisionGroup = PHYSX_CUSTOM_2;
	RootDesc.iCollisionMask = PHYSX_TERRAIN | PHYSX_DYNAMIC | PHYSX_CUSTOM_3;
	RootDesc.isSimulateSync = false;
	RootDesc.isQuery = false;

	XMStoreFloat4x4(&RootDesc.StartWorldMatrix, B01World);

	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_RigidBody"),
		TEXT("Com_RootBody"), reinterpret_cast<CComponent**>(&m_pHairRoot), &RootDesc)))
		return E_FAIL;

	m_pGameInstance->Add_RigidBody_ToPhysx(this, m_pHairRoot);

	CRigidBody* pHairLink = { nullptr };

	// 첫 노드(다이나믹 구) - 원점이 B02에 있어야 함
	CRigidBody::RIGIDBODY_DESC LinkDesc;
	LinkDesc.eRigidBodyShape = CRigidBody::RIGIDBODY_SHAPE::CAPSULE; 
	LinkDesc.eRigidBodyType = CRigidBody::RIGIDBODY_TYPE::DYNAMIC;
	LinkDesc.tUserData.szActorTag = TEXT("HairNode_01");
	LinkDesc.vMaterial = _float3(0.2f, 0.2f, 0.f);

	XMStoreFloat4x4(&LinkDesc.StartWorldMatrix, B02World);

	LinkDesc.vSize = _float3(0.05f, 0.05f, 0.f);
	LinkDesc.fMass = { 0.05f };

	LinkDesc.iCollisionGroup = PHYSX_CUSTOM_2;
	LinkDesc.iCollisionMask = PHYSX_TERRAIN | PHYSX_DYNAMIC | PHYSX_CUSTOM_3;
	LinkDesc.isSimulateSync = false;
	LinkDesc.isQuery = false;

	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_RigidBody"),
		TEXT("Com_LinkBody_01"), reinterpret_cast<CComponent**>(&pHairLink), &LinkDesc)))
		return E_FAIL;

	m_pGameInstance->Add_RigidBody_ToPhysx(this, pHairLink);

	// 체인 연결
	m_pJointChain->Set_Root(m_pHairRoot);
	m_pJointChain->Add_Joint(pHairLink);

	m_HairLinks.push_back(make_pair(TEXT("Ab-TL-HairB02"), pHairLink));

	return S_OK;
}

HRESULT CPonyTail_Player::Ready_HairBoneMapping()
{
	XMStoreFloat4x4(&m_CombinedWorldMatrix,
		XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()) * XMLoadFloat4x4(m_pParentTransformCom->Get_WorldMatrixPtr()));

	_matrix matOwnerWorld = XMLoadFloat4x4(&m_CombinedWorldMatrix);

	_vector vOwnerS, vOwnerR, vOwnerT;
	XMMatrixDecompose(&vOwnerS, &vOwnerR, &vOwnerT, matOwnerWorld);
	vOwnerR = XMQuaternionNormalize(vOwnerR);

	_matrix matOwnerNoScale = XMMatrixAffineTransformation(
		XMVectorSet(1.f, 1.f, 1.f, 0.f), XMVectorZero(), vOwnerR, vOwnerT);

	m_vecHairActorToBone.resize(m_HairLinks.size());
	m_vecHairBoneScale.resize(m_HairLinks.size());

	_uint iCount = 0;
	for (auto& Pair : m_HairLinks)
	{
		_wstring strBoneName = Pair.first;
		_char* pBoneName = new _char[strBoneName.length() + 1];
		CStringHelper::ConvertWideToUTF(strBoneName.c_str(), pBoneName);

		_matrix matBoneBase = XMLoadFloat4x4(m_pBodyModelCom->Get_BoneMatrixPtr(pBoneName));

		_vector vBoneS, vBoneR, vBoneT;
		XMMatrixDecompose(&vBoneS, &vBoneR, &vBoneT, matBoneBase);

		_float3 s;
		XMStoreFloat3(&s, vBoneS);
		m_vecHairBoneScale[iCount] = s;

		_matrix matBoneRestWorld = matBoneBase * matOwnerNoScale;
		PxTransform tBoneRestWorld(m_pGameInstance->Convert_Matrix_ToPxTransform(matBoneRestWorld));

		PxTransform tActorRestWorld = Pair.second->Get_PxTransform();
		PxTransform tShapeRestWorld = tActorRestWorld * Pair.second->Get_ShapeLocalPose();

		m_vecHairActorToBone[iCount] = tShapeRestWorld.getInverse() * tBoneRestWorld;

		Safe_Delete_Array(pBoneName);
		++iCount;
	}

	return S_OK;
}

HRESULT CPonyTail_Player::Ready_ChildHair()
{
	//B02 ~ B09 까지 콜라이더 생성

	for (_int i = 2; i < 9; i += m_iBoneJointCount)
	{
		CRigidBody* pHairLink = { nullptr };
		_wstring SourBone = TEXT("Ab-TL-HairB0") + to_wstring(i);
		_wstring DestBone = TEXT("Ab-TL-HairB0") + to_wstring(i + m_iBoneJointCount > 9 ? 9 : i + m_iBoneJointCount);
		
		_char* pSourBoneName = new _char[SourBone.length() + 1];
		_char* pDestBoneName = new _char[DestBone.length() + 1];

		// 본 매트릭스
		CStringHelper::ConvertWideToUTF(SourBone.c_str(), pSourBoneName);
		CStringHelper::ConvertWideToUTF(DestBone.c_str(), pDestBoneName);

		// i가 2라면, Ab-TL-HairB02, Ab-TL-HairB03
		const _float4x4* pBoneB01 = m_pBodyModelCom->Get_BoneMatrixPtr(pSourBoneName);
		const _float4x4* pBoneB02 = m_pBodyModelCom->Get_BoneMatrixPtr(pDestBoneName);

		// 이 오브젝트(또는 캐릭터) 월드
		_matrix OwnerWorld =
			XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()) *
			XMLoadFloat4x4(m_pParentTransformCom->Get_WorldMatrixPtr());

		// 본 월드
		_matrix B01World = XMLoadFloat4x4(pBoneB01) * OwnerWorld;
		_matrix B02World = XMLoadFloat4x4(pBoneB02) * OwnerWorld;

		CRigidBody::RIGIDBODY_DESC LinkDesc;
		LinkDesc.eRigidBodyShape = CRigidBody::RIGIDBODY_SHAPE::CAPSULE;   
		LinkDesc.eRigidBodyType = CRigidBody::RIGIDBODY_TYPE::DYNAMIC;
		LinkDesc.tUserData.szActorTag = TEXT("HairNode_01");
		LinkDesc.vMaterial = _float3(0.2f, 0.2f, 0.f);

		XMStoreFloat4x4(&LinkDesc.StartWorldMatrix, B02World);

		LinkDesc.vSize = _float3(0.1f, 0.1f, 0.f);
		LinkDesc.fMass = { 0.05f };

		////팔까지 충돌
		//if (i <= 4)
		//{
		//	LinkDesc.iCollisionGroup = PHYSX_CUSTOM_2;
		//	LinkDesc.iCollisionMask = PHYSX_TERRAIN | PHYSX_DYNAMIC | PHYSX_DEFAULT | PHYSX_CUSTOM_3 | PHYSX_CUSTOM_4;

		//}
		//팔 충돌 X
		if (i <= 6)
		{
			LinkDesc.iCollisionGroup = PHYSX_CUSTOM_2;
			LinkDesc.iCollisionMask = PHYSX_TERRAIN | PHYSX_DYNAMIC | PHYSX_CUSTOM_3;

		}
		// 하체 충돌도 안해
		else
		{

			LinkDesc.iCollisionGroup = PHYSX_CUSTOM_2;
			LinkDesc.iCollisionMask = PHYSX_TERRAIN | PHYSX_DYNAMIC ;
		}

		LinkDesc.isSimulateSync = false;
		LinkDesc.isQuery = false;

		if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_RigidBody"),
			TEXT("Com_LinkBody_0") + to_wstring(i), reinterpret_cast<CComponent**>(&pHairLink), &LinkDesc)))
			return E_FAIL;

		m_pGameInstance->Add_RigidBody_ToPhysx(this, pHairLink);

		// 체인 연결
		m_pJointChain->Add_Joint(pHairLink);
		// 
		m_HairLinks.push_back(make_pair(DestBone, pHairLink));

		Safe_Delete_Array(pSourBoneName);
		Safe_Delete_Array(pDestBoneName);
	}

	return S_OK;
}

HRESULT CPonyTail_Player::Ready_HairRigidBodies()
{
	CRigidBody* pRigidBody = { nullptr };
	_uint i = 0;

	CRigidBody::RIGIDBODY_DESC RigidBodyDesc;
	RigidBodyDesc.eRigidBodyShape = CRigidBody::RIGIDBODY_SHAPE::SPHERE;
	RigidBodyDesc.eRigidBodyType = CRigidBody::RIGIDBODY_TYPE::KINEMATIC;
	RigidBodyDesc.tUserData.szActorTag = TEXT("BodyCollider");
	RigidBodyDesc.vSize = _float3(0.4f, 0.f, 0.f);
	RigidBodyDesc.fMass = { 0.f };
	RigidBodyDesc.iCollisionGroup = PHYSX_CUSTOM_3;
	RigidBodyDesc.iCollisionMask = PHYSX_TERRAIN | PHYSX_DYNAMIC | PHYSX_DEFAULT | PHYSX_CUSTOM_3 | PHYSX_CUSTOM_2;
	RigidBodyDesc.isSimulateSync = false;
	RigidBodyDesc.isQuery = false;
	RigidBodyDesc.vMaterial = _float3(0.f, 0.f, 0.f);
//##################### HEAD
	const _float4x4* pBone = m_pBodyModelCom->Get_BoneMatrixPtr("Bip001-Head");
	_matrix OwnerWorld = XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()) * XMLoadFloat4x4(m_pParentTransformCom->Get_WorldMatrixPtr());
	_matrix BoneWorld = XMLoadFloat4x4(pBone) * OwnerWorld;
	XMStoreFloat4x4(&RigidBodyDesc.StartWorldMatrix, BoneWorld);

	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_RigidBody"),
		TEXT("Com_Rigid") + to_wstring(i), reinterpret_cast<CComponent**>(&pRigidBody), &RigidBodyDesc)))
		return E_FAIL;
	pRigidBody->Set_AngularDamping(0.5f);
	pRigidBody->Set_LinearDamping(0.06f);

	m_HairRigidBodies.push_back(make_pair(pBone, pRigidBody));
	m_pGameInstance->Add_RigidBody_ToPhysx(this, pRigidBody);
	//pRigidBody->Set_CCD(true);
	i++;
//##################### SPINE
	RigidBodyDesc.vSize = _float3(0.5f, 0.5f, 0.f);
	RigidBodyDesc.eRigidBodyShape = CRigidBody::RIGIDBODY_SHAPE::CAPSULE;

	pBone = m_pBodyModelCom->Get_BoneMatrixPtr("BodyLookAt");
	OwnerWorld = XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()) * XMLoadFloat4x4(m_pParentTransformCom->Get_WorldMatrixPtr());
	BoneWorld = XMLoadFloat4x4(pBone) * OwnerWorld;
	XMStoreFloat4x4(&RigidBodyDesc.StartWorldMatrix, BoneWorld);

	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_RigidBody"),
		TEXT("Com_Rigid") + to_wstring(i), reinterpret_cast<CComponent**>(&pRigidBody), &RigidBodyDesc)))
		return E_FAIL;
	pRigidBody->Set_AngularDamping(0.5f);
	pRigidBody->Set_LinearDamping(0.06f);

	m_HairRigidBodies.push_back(make_pair(pBone, pRigidBody));
	m_pGameInstance->Add_RigidBody_ToPhysx(this, pRigidBody);
	//pRigidBody->Set_CCD(true);
	i++;

	return S_OK;
}

HRESULT CPonyTail_Player::Bind_ShaderResources()
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
	//if (FAILED(m_pPonytailMaskTextureCom->Bind_ShaderResource(m_pShaderCom, "g_OpacityTexture", 0)))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CPonyTail_Player::Bind_BoneToPartBody(void* pArg)
{
	CBody_Player* pBody = static_cast<CBody_Player*>(pArg);

	m_pBodyModelCom = static_cast<CModel*>(pBody->Find_Component(TEXT("Com_Model")));


	return S_OK;
}

void CPonyTail_Player::Sync_BonesByJoint()
{
	_matrix matOwnerWorld = XMLoadFloat4x4(&m_CombinedWorldMatrix);

	_vector vOwnerS, vOwnerR, vOwnerT;
	XMMatrixDecompose(&vOwnerS, &vOwnerR, &vOwnerT, matOwnerWorld);

	_matrix matOwnerNoScale = XMMatrixAffineTransformation(
		XMVectorSet(1.f, 1.f, 1.f, 0.f), XMVectorZero(), XMQuaternionNormalize(vOwnerR), vOwnerT);
	_matrix matOwnerNoScaleInv = XMMatrixInverse(nullptr, matOwnerNoScale);

	_uint iCount = 0;
	for (auto& Pair : m_HairLinks)
	{
		_wstring strBoneName = Pair.first;
		_char* pBoneName = new _char[strBoneName.length() + 1];
		CStringHelper::ConvertWideToUTF(strBoneName.c_str(), pBoneName);

		PxTransform tActorNow = Pair.second->Get_PxTransform();
		PxTransform tShapeNow = tActorNow * Pair.second->Get_ShapeLocalPose();
		PxTransform tBoneWorld = tShapeNow * m_vecHairActorToBone[iCount];

		_matrix matBoneModelNoScale =
			m_pGameInstance->Convert_PxTransform_ToMatrix(tBoneWorld) * matOwnerNoScaleInv;

		_vector vS, vR, vT2;
		XMMatrixDecompose(&vS, &vR, &vT2, matBoneModelNoScale);

		_float3 sc = m_vecHairBoneScale[iCount];
		_vector vScale = XMVectorSet(sc.x, sc.y, sc.z, 0.f);

		_matrix matNew = XMMatrixAffineTransformation(vScale, XMVectorZero(), XMQuaternionNormalize(vR), vT2);

		m_pBodyModelCom->Override_CombinedTransformationMatrix(pBoneName, matNew);

		Safe_Delete_Array(pBoneName);
		++iCount;
	}
}

CPonyTail_Player* CPonyTail_Player::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPonyTail_Player* pInstance = new CPonyTail_Player(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CPonyTail_Player");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPonyTail_Player::Clone(void* pArg)
{
	CPonyTail_Player* pInstance = new CPonyTail_Player(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CPonyTail_Player");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPonyTail_Player::Free()
{
	__super::Free();

	Safe_Release(m_pGameManager); 
	Safe_Release(m_pHairRoot);
	Safe_Release(m_pJointChain);

	for (auto& Pair : m_HairLinks)
		Safe_Release(Pair.second);
	m_HairLinks.clear();

	for (auto& RigidBody : m_HairRigidBodies)
		Safe_Release(RigidBody.second);
	m_HairRigidBodies.clear();

	Safe_Release(m_pPonytailMaskTextureCom);
}
