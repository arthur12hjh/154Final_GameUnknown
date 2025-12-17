#include "pch.h"
#include "PonyTail_Player.h"
#include "Body_Player.h"

#include "Bone.h"
#include "Model.h"

#include "GameManager.h"
#include "GameInstance.h"

#include "Player.h"
#include "StringHelper.h"

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


	return S_OK;
}

void CPonyTail_Player::Priority_Update(_float fTimeDelta)
{

}

void CPonyTail_Player::Update(_float fTimeDelta)
{
	//typedef struct tagJointChainDesc {
	//	_float fAngularDampimg;
	//	_float fLinearDamping;
	//	_float fMaxAngularVelocity;
	//	_float fMaxDepenetrationVelocity;
	//} JOINT_CHAIN_DESC;
	//플래그 제어용 테스트 디버거. 곧 지울게요
//#ifdef _DEBUG
//	ImGui::Begin("PONYTAIL_DESC");
//
//	ImGui::DragFloat("Root Hair AngularDamping", &m_tRootDesc->fAngularDampimg, 0.3f, 0.1f, 10.f);
//	ImGui::DragFloat("Root Hair LinearDamping", &m_tRootDesc->fLinearDamping, 0.01f, 0.01f, 1.f);
//	//ImGui::DragFloat("Root Hair MaxAngularVelocity", &m_tRootDesc->fMaxAngularVelocity, 0.01f, 0.f, 1.f);
//	//ImGui::DragFloat("Root Hair DepenetrationVelocity", &m_tRootDesc->fMaxDepenetrationVelocity, 0.01f, 0.f, 1.f);
//
//	ImGui::DragFloat("Link Hair AngularDamping", &m_tLinkDesc->fAngularDampimg, 0.01f, 0.f, 1.f);
//	ImGui::DragFloat("Link Hair LinearDamping", &m_tLinkDesc->fLinearDamping, 0.01f, 0.f, 1.f);
//	//ImGui::DragFloat("LinkLink Hair MaxAngularVelocity", &m_tLinkDesc->fMaxAngularVelocity, 0.01f, 0.f, 1.f);
//	//ImGui::DragFloat("Link Hair DepenetrationVelocity", &m_tLinkDesc->fMaxDepenetrationVelocity, 0.01f, 0.f, 1.f);
//
//
//	ImGui::End();
//	
//	
//	m_pJointChain->Update(fTimeDelta);
//#endif
}

void CPonyTail_Player::Late_Update(_float fTimeDelta)
{
	XMStoreFloat4x4(&m_CombinedWorldMatrix,
		XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()) * XMLoadFloat4x4(m_pParentTransformCom->Get_WorldMatrixPtr()));

	m_pHairRoot->Update_PxTransform(
		XMLoadFloat4x4(m_pHairRootBone) * XMLoadFloat4x4(&m_CombinedWorldMatrix), true);

	m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
	//m_pGameInstance->Add_RenderGroup(RENDER::SHADOW, this);
	
#ifdef _DEBUG

#endif
}

HRESULT CPonyTail_Player::Render()
{
	Sync_BonesByJoint();

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

	if (FAILED(m_pGameInstance->Bind_Shadow_Resource(m_pShaderCom, "g_ViewMatrix", D3DTS::VIEW)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_Shadow_Resource(m_pShaderCom, "g_ProjMatrix", D3DTS::PROJ)))
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

	return S_OK;
}

HRESULT CPonyTail_Player::Ready_HairJoints()
{
	if (FAILED(Ready_RootHair()))
		return E_FAIL;

	if (FAILED(Ready_ChildHair()))
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
	RootDesc.vSize = _float3(0.f, 0.f, 0.f);
	RootDesc.fMass = { 0.f };
	RootDesc.iCollisionGroup = PHYSX_CUSTOM_3;
	RootDesc.iCollisionMask = PHYSX_TERRAIN | PHYSX_DYNAMIC | PHYSX_DEFAULT;
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
	LinkDesc.eRigidBodyShape = CRigidBody::RIGIDBODY_SHAPE::SPHERE;   // 핵심: SPHERE
	LinkDesc.eRigidBodyType = CRigidBody::RIGIDBODY_TYPE::DYNAMIC;
	LinkDesc.tUserData.szActorTag = TEXT("HairNode_01");
	LinkDesc.vMaterial = _float3(0.5f, 0.5f, 0.3f);

	XMStoreFloat4x4(&LinkDesc.StartWorldMatrix, B02World);

	LinkDesc.vSize = _float3(0.1f, 0.f, 0.f); // SPHERE는 x만 반지름으로 씀
	LinkDesc.fMass = { 0.05f };

	LinkDesc.iCollisionGroup = PHYSX_CUSTOM_3;
	LinkDesc.iCollisionMask = PHYSX_TERRAIN | PHYSX_DYNAMIC | PHYSX_DEFAULT;
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

		// 첫 노드(다이나믹 구) - 원점이 B02에 있어야 함
		CRigidBody::RIGIDBODY_DESC LinkDesc;
		LinkDesc.eRigidBodyShape = CRigidBody::RIGIDBODY_SHAPE::SPHERE;   // 핵심: SPHERE
		LinkDesc.eRigidBodyType = CRigidBody::RIGIDBODY_TYPE::DYNAMIC;
		LinkDesc.tUserData.szActorTag = TEXT("HairNode_01");
		LinkDesc.vMaterial = _float3(0.5f, 0.5f, 0.3f);

		XMStoreFloat4x4(&LinkDesc.StartWorldMatrix, B02World);

		LinkDesc.vSize = _float3(0.1f, 0.f, 0.f); // SPHERE는 x만 반지름으로 씀
		LinkDesc.fMass = { 0.05f };

		LinkDesc.iCollisionGroup = PHYSX_CUSTOM_3;
		LinkDesc.iCollisionMask = PHYSX_TERRAIN | PHYSX_DYNAMIC | PHYSX_DEFAULT;
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

HRESULT CPonyTail_Player::Bind_ShaderResources()
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

HRESULT CPonyTail_Player::Bind_BoneToPartBody(void* pArg)
{
	CBody_Player* pBody = static_cast<CBody_Player*>(pArg);

	m_pBodyModelCom = static_cast<CModel*>(pBody->Find_Component(TEXT("Com_Model")));


	return S_OK;
}

void CPonyTail_Player::Sync_BonesByJoint()
{
	//01은 루트.
	// m_pHairLink에서 가져와서 Ab-TL-HairB02부턴 리지드바디 세팅
	// TEXT("Ab-TL-HairB0")
	// 해봐야 1대1 대응, 아니면 중간 본 자르는 정도니까.. 
	// 아직 럴프는 안들어갔는데 일단 ㄱㄱ..
	for (auto& Pair : m_HairLinks)
	{	
		_wstring strBoneName = Pair.first;
		_char* pBoneName = new _char[strBoneName.length() + 1];
		CStringHelper::ConvertWideToUTF(strBoneName.c_str(), pBoneName);

		//본매핑콱시팔바로피직스정상화역시정상화는대재훈
		m_pBodyModelCom->Override_CombinedTransformationMatrix(pBoneName,
			m_pGameInstance->Convert_PxTransform_ToMatrix(Pair.second->Get_PxTransform()) * XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_CombinedWorldMatrix)));

		Safe_Delete_Array(pBoneName);
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

}
