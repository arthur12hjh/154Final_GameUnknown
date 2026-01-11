#include "pch.h"
#include "Body_Player.h"

#include "GameInstance.h"

#include "Effect.h"
#include "GameManager.h"
#include "Nayitba.h"
#include "StringHelper.h"
#include "Player.h"

CBody_Player::CBody_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPlayer_Parts{ pDevice, pContext }
{
}

CBody_Player::CBody_Player(const CBody_Player& Prototype) 
	: CPlayer_Parts{ Prototype }
{
}

_bool CBody_Player::isFinish_Att()
{
	return false;
}

void CBody_Player::Active_SFX(const _wstring& strObjectTag, const ANIM_NOTIFY& NotifyReference)
{
	if (strObjectTag == TEXT("Play_Effect"))
	{

	}
	else if (strObjectTag == TEXT("Hand_Blood"))
	{
		m_pBlood->Play();
	}
	else if (strObjectTag == TEXT("Hand_Blood_End"))
	{
		m_pBlood->Stop();
	}
}

void CBody_Player::Activate_PartObject_Collider(const _wstring& strColliderTag, const ANIM_NOTIFY& NotifyRef)
{
	auto pComponents = Find_Component(strColliderTag);
	if (nullptr == pComponents)
		return;

	m_bIsEnableCollider = NotifyRef.iNumData01;
	if (false == m_bIsEnableCollider)
	{
		static_cast<CCollider*>(pComponents)->ResetCollision();
	}
	//else
	//	m_pGameInstance->GamePauseDurationTime(1, 0.01f, 10.f);
}

HRESULT CBody_Player::Mapping_Shader_Material(_uint iIdx)
{
	if (strcmp(m_pModelCom->Get_MaterialName(m_pModelCom->Get_Mesh_MaterialIndex(iIdx)), "MI_Basebody_V02_F1") == 0)
	{
		if (FAILED(m_pShaderCom->Begin(6)))
			return E_FAIL;
	}

	else 
	{
		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CBody_Player::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBody_Player::Initialize(void* pArg)
{	
	BODY_PLAYER_DESC* pDesc = static_cast<BODY_PLAYER_DESC*>(pArg);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pModelCom->Set_AnimationIndex(1);

	if (FAILED(Ready_VerticalJoints()))
		return E_FAIL;

	if (FAILED(Ready_HorizontalJoints()))
		return E_FAIL; 

	if (FAILED(Ready_SkirtBoneOrigin()))
		return E_FAIL;

	if(FAILED(Ready_ThighRigidBodies()))
		return E_FAIL;

	if (FAILED(Ready_UpperRigidBodies()))
		return E_FAIL;

	return S_OK;
}

void CBody_Player::Priority_Update(_float fTimeDelta)
{
	Update_PreCombinedMatrix();
}

void CBody_Player::Update(_float fTimeDelta)
{ 
	// FSM쪽에서 애니 재생.
	// m_isAnimFinish = m_pModelCom->Play_Animation(fTimeDelta);
	if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_M))
		m_pMotionTrail->EnableMotionTrail(true);

	if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_N))
		m_pMotionTrail->EnableMotionTrail(false);



	if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_K))
		m_pBlood->Play();
	if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_L))
		m_pBlood->Stop();

	m_pBlood->Update(fTimeDelta);
}

void CBody_Player::Late_Update(_float fTimeDelta)
{
	XMStoreFloat4x4(&m_CombinedWorldMatrix,
		XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()) * XMLoadFloat4x4(m_pParentTransformCom->Get_WorldMatrixPtr()));

	_matrix vResult = XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()) *
		XMLoadFloat4x4(m_pModelCom->Get_BoneMatrixPtr("Ab-R-Calf-Tw1")) * XMLoadFloat4x4(m_pParentTransformCom->Get_WorldMatrixPtr());

	m_pBlood->Late_Update(fTimeDelta);
	m_pColliderCom->UpdateColiision(vResult);

	if (m_bIsEnableCollider)
	{
		m_pGameInstance->ADD_Collider(m_pColliderCom);
	}

	for (auto& Pair : m_ThighRigidBodies)
	{
		Pair.second->Update_PxTransform(
			XMLoadFloat4x4(Pair.first) * XMLoadFloat4x4(&m_CombinedWorldMatrix), true);
	}
#ifdef _DEBUG
	m_pGameInstance->Add_DebugComponent(m_pColliderCom);
#endif
	//조인트 업데이트용. 신경 안써도됨
	for (auto& Pair : m_RootRigidBodies)
	{
		Pair.second->Update_PxTransform(
			XMLoadFloat4x4(Pair.first) * XMLoadFloat4x4(&m_CombinedWorldMatrix), true);
	}

	_float fDist = XMVectorGetX(XMVector3Length(m_pParentTransformCom->Get_State(STATE::POSITION) - XMLoadFloat4(m_pGameInstance->Get_CamPosition())));
	
	m_pMotionTrail->Update_Trail(fTimeDelta);
	if (m_bIsActive)
	{
		m_pGameInstance->Add_RenderGroup(RENDER::NONLIGHT, m_pMotionTrail);
		m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
		m_pGameInstance->Add_RenderGroup(RENDER::SHADOW, this);
	}

	if (fDist < 100.f)
	{
		CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pParent);
		if (pPlayer)
		{
			m_pGameInstance->Add_RenderGroup(RENDER::MOTIONBLUR, this);
		}
	}
}

HRESULT CBody_Player::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(i, m_pShaderCom, "g_DiffuseTexture", aiTextureType_DIFFUSE, 0)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_Material(i, m_pShaderCom, "g_ORMTexture", aiTextureType_METALNESS, 0)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_Material(i, m_pShaderCom, "g_ORSSTexture", aiTextureType_CLEARCOAT, 0)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_Material(i, m_pShaderCom, "g_NormalTexture", aiTextureType_NORMALS, 0)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_BoneSRV(i, m_pShaderCom, "g_BoneMatrixBuffer")))
			return E_FAIL;

		//rimlight 버전
		if (FAILED(Mapping_Shader_Material(i)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CBody_Player::Render_Shadow()
{
	Sync_BonesByJoint();

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_Shadow_Resource_Cascade(m_pShaderCom, "g_LightViewMatrix", D3DTS::VIEW)))
		return E_FAIL;	
	
	if (FAILED(m_pGameInstance->Bind_Shadow_Resource_Cascade(m_pShaderCom, "g_LightProjMatrix", D3DTS::PROJ)))
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

HRESULT CBody_Player::Render_MotionBlur()
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

HRESULT CBody_Player::Ready_Components()
{
	/* Com_Model */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_Eve_Body_24_TypeB"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;
	
	/* Com_MotionTrail */
	CMotionTrailComponent::MOTION_TRAIL_COMPONENT_DESC MotionTrailCom = {};
	MotionTrailCom.pModel = m_pModelCom;
	MotionTrailCom.pPreBoneModel = m_pModelCom;
	MotionTrailCom.pTransform = &m_CombinedWorldMatrix;
	MotionTrailCom.fUpdateTime = 0.2f;
	MotionTrailCom.fLifeTime = 0.45f;

	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_MotionTrail"),
		TEXT("Com_MotionTrail"), reinterpret_cast<CComponent**>(&m_pMotionTrail), &MotionTrailCom)))
		return E_FAIL;

	/* Com_Shader */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	COBBCollider::OBB_COLLIDER_DESC		OBBDesc{};
	OBBDesc.vSize = _float3(0.3f, 0.7f, 0.3f);
	OBBDesc.vCenter = _float3(0.f, 0.15f, 0.f);
	OBBDesc.vAngles = _float3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider_OBB"), reinterpret_cast<CComponent**>(&m_pColliderCom), &OBBDesc)))
		return E_FAIL;

	m_pColliderCom->BindBeginOverlapEvent([&](_float3 vHitPoint, _float3 vHitDir, CGameObject* pHitActor) { Begin_OverlapEvent(vHitPoint, vHitDir, pHitActor); });

	m_pColliderCom->SetColliderHitType(HIT_TYPE::PLAYER);
	m_pColliderCom->ADD_IgnoreObjectType(HIT_TYPE::SENCE);
	m_pColliderCom->ADD_IgnoreObjectType(HIT_TYPE::PLAYER);
	m_pColliderCom->ADD_IgnoreObjectType(HIT_TYPE::INTERACTION);

	CEffect::EFFECT_TRANSFORM_DESC desc;

	desc.pWorldMatrix = &m_CombinedWorldMatrix;
	desc.pRootMatrix = m_pModelCom->Get_BoneMatrixPtr("Bip001-L-Hand");
	desc.vPos = XMVectorSet(0, 0, -0.1f, 1);
	desc.fRot = _float3(0, 0, 0);
	desc.fSize = 0.06f;

	m_pBlood = static_cast<CEffect*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Effect_Monster_Club"), &desc));
	m_pBlood->Play();
	m_pBlood->Stop();

	return S_OK;
}

HRESULT CBody_Player::Bind_ShaderResources()
{
	CPlayer* pPlayer = m_pGameManager->GetGameCharacter(); 
	Safe_Release(pPlayer);
	_bool bFlag = static_cast<CCharacter*>(pPlayer)->Get_DepthMaskingB();

	if (FAILED(m_pShaderCom->Bind_RawValue("g_IsMaskingDepthB", &bFlag, sizeof(_bool))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_IsMaskingDepthW", &bFlag, sizeof(_bool))))
		return E_FAIL;

	/*m_pShaderCom->Bind_Matrix("g_WorldMatrix", );*/
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
		return E_FAIL;	
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
		return E_FAIL;

	return S_OK;
}

void CBody_Player::Begin_OverlapEvent(_float3 vHitPoint, _float3 vHitDir, CGameObject* pHitActor)
{
	CNaytiba* pNaytiba = dynamic_cast<CNaytiba*>(pHitActor);
	if (pNaytiba)
	{
		DEFAULT_DAMAGE_DESC pDamageDesc = {};
		pDamageDesc.pAttacker = m_pParent;
		pDamageDesc.vHitPoint = vHitPoint;
		pDamageDesc.vHitDir = vHitDir;

		_uint iSkillID = static_cast<CPlayer*>(m_pParent)->GetSkillDataID();
		if (-1 == iSkillID)
			return;

		pDamageDesc.pSkillData = m_pGameManager->Find_SkillData(iSkillID);
		pNaytiba->Damaged(&pDamageDesc);
	}


}

HRESULT CBody_Player::Ready_VerticalJoints()
{
	//1번 인덱스부터, 10번까지 하자. 읽기 편하게..
	for (_uint i = 1; i <= 10; ++i)
	{
		vector<_wstring> strBoneNames = {};
		CJointChain* pVerticalJointChain = { nullptr };
#pragma region BoneNameStore
		switch (i)
		{
		case 1:
			strBoneNames.push_back(TEXT("Ab-R-SkirtD-01"));
			strBoneNames.push_back(TEXT("Ab-R-SkirtD-02"));
			strBoneNames.push_back(TEXT("Ab-R-SkirtD-03"));
			break;
		case 2:
			strBoneNames.push_back(TEXT("Ab-L-SkirtD-01"));
			strBoneNames.push_back(TEXT("Ab-L-SkirtD-02"));
			strBoneNames.push_back(TEXT("Ab-L-SkirtD-03"));
			break;
		case 3:
			strBoneNames.push_back(TEXT("Ab-L-SkirtO-01"));
			strBoneNames.push_back(TEXT("Ab-L-SkirtO-02"));
			strBoneNames.push_back(TEXT("Ab-L-SkirtO-03"));
			break;
		case 4:
			strBoneNames.push_back(TEXT("Ab-L-SkirtE-01"));
			strBoneNames.push_back(TEXT("Ab-L-SkirtE-02"));
			strBoneNames.push_back(TEXT("Ab-L-SkirtE-03"));
			break;
#pragma region 4Bones
		case 5:
			strBoneNames.push_back(TEXT("Ab-L-SkirtS-01"));
			strBoneNames.push_back(TEXT("Ab-L-SkirtS-02"));
			strBoneNames.push_back(TEXT("Ab-L-SkirtS-03"));
			strBoneNames.push_back(TEXT("Ab-L-SkirtS-04"));
			break;
		case 6:
			strBoneNames.push_back(TEXT("Ab-L-SkirtQ-01"));
			strBoneNames.push_back(TEXT("Ab-L-SkirtQ-02"));
			strBoneNames.push_back(TEXT("Ab-L-SkirtQ-03"));
			strBoneNames.push_back(TEXT("Ab-L-SkirtQ-04"));
			break;
		case 7:
			strBoneNames.push_back(TEXT("Ab-R-SkirtQ-01"));
			strBoneNames.push_back(TEXT("Ab-R-SkirtQ-02"));
			strBoneNames.push_back(TEXT("Ab-R-SkirtQ-03"));
			strBoneNames.push_back(TEXT("Ab-R-SkirtQ-04"));
			break;
		case 8:
			strBoneNames.push_back(TEXT("Ab-R-SkirtS-01"));
			strBoneNames.push_back(TEXT("Ab-R-SkirtS-02"));
			strBoneNames.push_back(TEXT("Ab-R-SkirtS-03"));
			strBoneNames.push_back(TEXT("Ab-R-SkirtS-04"));
			break;
#pragma endregion
		case 9:
			strBoneNames.push_back(TEXT("Ab-R-SkirtE-01"));
			strBoneNames.push_back(TEXT("Ab-R-SkirtE-02"));
			strBoneNames.push_back(TEXT("Ab-R-SkirtE-03"));
			break;
		case 10:
			strBoneNames.push_back(TEXT("Ab-R-SkirtO-01"));
			strBoneNames.push_back(TEXT("Ab-R-SkirtO-02"));
			strBoneNames.push_back(TEXT("Ab-R-SkirtO-03"));
			break;
		default:
			break;
		}
#pragma endregion
		//m_RootBoneMatrices
		for (_uint iVerticalCount = 0; iVerticalCount < strBoneNames.size(); ++iVerticalCount)
		{
			CRigidBody* pRigidBody = { nullptr };
#pragma region RootBoneRigidBody
			//만약 루트본을 세팅하는 과정이라면,
			if(0 == iVerticalCount)
			{
				_matrix pRootBoneMatrix = {};
				_char* pRootBoneName = new _char[strBoneNames[iVerticalCount].length() + 1];

				CStringHelper::ConvertWideToUTF(strBoneNames[iVerticalCount].c_str(), pRootBoneName);
				const _float4x4* pRootBone = m_pModelCom->Get_BoneMatrixPtr(pRootBoneName);
				// 루트 본 저장해두고.
				pRootBoneMatrix = XMLoadFloat4x4(pRootBone);

				// 이 오브젝트(또는 캐릭터) 월드
				_matrix OwnerWorld = XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()) *
					XMLoadFloat4x4(m_pParentTransformCom->Get_WorldMatrixPtr());
				
				// 데스크 세팅해주고.
				CRigidBody::RIGIDBODY_DESC RootDesc;
				RootDesc.eRigidBodyShape = CRigidBody::RIGIDBODY_SHAPE::NONE;
				RootDesc.eRigidBodyType = CRigidBody::RIGIDBODY_TYPE::KINEMATIC;
				// SkirtBoneRoot 0 , 1, ... 10
				RootDesc.tUserData.szActorTag = TEXT("SkirtBoneRoot") + to_wstring(i);
				RootDesc.vSize = _float3(0.01f, 0.01f, 0.f);
				RootDesc.fMass = { 1.f };
				RootDesc.iCollisionGroup = PHYSX_CUSTOM_4;
				RootDesc.iCollisionMask = PHYSX_DYNAMIC;
				RootDesc.isSimulateSync = false;
				RootDesc.isQuery = false;
				XMStoreFloat4x4(&RootDesc.StartWorldMatrix, pRootBoneMatrix * OwnerWorld);

				// 태그 이름으로 넣어준다.
				if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_RigidBody"),
					RootDesc.tUserData.szActorTag, reinterpret_cast<CComponent**>(&pRigidBody), &RootDesc)))
					return E_FAIL;

				// 마찬가지로 태그 이름으로 넣어준다.
				// 루트 리지드 바디는 m_RootRigidBodies에 저장한다. 자식 리지드바디랑 저장 방법이 다르니 조심할 것 
				m_RootRigidBodies.push_back(make_pair(pRootBone, pRigidBody));
				Safe_Delete_Array(pRootBoneName);

				// 이름으로 i번쨰 조인트 체인임을 명시해준다. 
				if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_JointChain"),
					TEXT("Com_JointChain") + to_wstring(i), reinterpret_cast<CComponent**>(&pVerticalJointChain))))
					return E_FAIL;

				m_VerticalJointChains.push_back(pVerticalJointChain);

				pVerticalJointChain->Set_Root(pRigidBody);
				m_pGameInstance->Add_RigidBody_ToPhysx(this, pRigidBody);
			}
#pragma endregion

		
#pragma region ChildBoneRigidBodies
			else
			{
				_matrix pChildBoneMatrix = {};
				_char* pChildBoneName = new _char[strBoneNames[iVerticalCount].length() + 1];

				CStringHelper::ConvertWideToUTF(strBoneNames[iVerticalCount].c_str(), pChildBoneName);

				pChildBoneMatrix = XMLoadFloat4x4(m_pModelCom->Get_BoneMatrixPtr(pChildBoneName));

				// 이 오브젝트(또는 캐릭터) 월드
				_matrix OwnerWorld = XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()) *
					XMLoadFloat4x4(m_pParentTransformCom->Get_WorldMatrixPtr());

				// 첫 노드(다이나믹 구) - 원점이 B02에 있어야 함
				CRigidBody::RIGIDBODY_DESC ChildDesc;
				ChildDesc.eRigidBodyShape = CRigidBody::RIGIDBODY_SHAPE::CAPSULE;
				ChildDesc.eRigidBodyType = CRigidBody::RIGIDBODY_TYPE::DYNAMIC;
				//actor tag로 i번째 인덱스의 iVerticalCount번쨰 자식임을 명시한다.
				ChildDesc.tUserData.szActorTag = TEXT("SkirtBoneChild") + to_wstring(i) + to_wstring(iVerticalCount);
				ChildDesc.vMaterial = _float3(0.2f, 0.2f, 0.f);
				ChildDesc.vSize = _float3(0.01f, 0.01f, 0.f);
				ChildDesc.fMass = { 1.f };
				ChildDesc.iCollisionGroup = PHYSX_CUSTOM_4;
				ChildDesc.iCollisionMask =  PHYSX_DYNAMIC | PHYSX_CUSTOM_3;
				ChildDesc.isSimulateSync = false;
				ChildDesc.isQuery = false;
				XMStoreFloat4x4(&ChildDesc.StartWorldMatrix, pChildBoneMatrix * OwnerWorld);

				// 태그 이름으로 넣어준다.
				if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_RigidBody"),
					ChildDesc.tUserData.szActorTag, reinterpret_cast<CComponent**>(&pRigidBody), &ChildDesc)))
					return E_FAIL;

				// 마찬가지로 태그 이름으로 넣어준다.
				// 부모랑은 다르게 m_SkirtRigidBodies 에 보관하며, 키도 본 이름으로 세팅한다.
				m_SkirtRigidBodies.push_back(make_pair(strBoneNames[iVerticalCount], pRigidBody));

				Safe_Delete_Array(pChildBoneName);

				pVerticalJointChain->Add_Joint(pRigidBody);
				m_pGameInstance->Add_RigidBody_ToPhysx(this, pRigidBody);
				pRigidBody->Set_ContactOffset(1.5f);
			}

#pragma endregion
		}
		strBoneNames.clear();
	}

	return S_OK;
}

HRESULT CBody_Player::Ready_HorizontalJoints()
{
	return S_OK;
}

//현재 트랜스폼의 매트릭스를 기준으로 한 본의 오리진 행렬을 미리 연산해둔다.
// 이게 왜필요함 ? << 본과 리지드바디 간의 축이 맞지 않아서 그대로 떄려박으면 수틀리는 경우가 많음.
// (좌표계도 뒤집어져있고)
HRESULT CBody_Player::Ready_SkirtBoneOrigin()
{
	//컴바인드 매트릭스 1회 연산 (일단 위치 잡아야하니까.)
	XMStoreFloat4x4(&m_CombinedWorldMatrix,
		XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()) * XMLoadFloat4x4(m_pParentTransformCom->Get_WorldMatrixPtr()));

	_matrix matOwnerWorld = XMLoadFloat4x4(&m_CombinedWorldMatrix);

	//매트릭스를 분해한다.
	_vector vOwnerS, vOwnerR, vOwnerT;
	XMMatrixDecompose(&vOwnerS, &vOwnerR, &vOwnerT, matOwnerWorld);

	//스케일은 제거해서 따로 보관하고, 로테이션이랑 트랜스레이션만 가져온다.
	_matrix matOwnerNoScale = XMMatrixAffineTransformation(XMVectorSet(1.f, 1.f, 1.f, 0.f), XMVectorZero(),
		vOwnerR, vOwnerT);

	for (auto& Pair : m_SkirtRigidBodies)
	{
		_wstring strBoneName = Pair.first;
		_char* pBoneName = new _char[strBoneName.length() + 1];
		CStringHelper::ConvertWideToUTF(strBoneName.c_str(), pBoneName);

		_matrix matBoneBase = XMLoadFloat4x4(m_pModelCom->Get_BoneMatrixPtr(pBoneName));

		_vector vBoneScale, vBoneRotation, vBoneTranslation;
		_float3 vScale = {};
		XMMatrixDecompose(&vBoneScale, &vBoneRotation, &vBoneTranslation, matBoneBase);
		XMStoreFloat3(&vScale, vBoneScale);

		m_SkirtBoneScales.push_back(make_pair(strBoneName, vScale));

		//본의 초기 월드 위치를 구해낸다.
		_matrix matBoneRestWorld = matBoneBase * matOwnerNoScale;
		PxTransform tBoneRestWorld(m_pGameInstance->Convert_Matrix_ToPxTransform(matBoneRestWorld));

		//리지드 바디의 초기 월드 위치를 구해낸다.
		PxTransform tActorRestWorld = Pair.second->Get_PxTransform();
		PxTransform tShapeRestWorld = tActorRestWorld * Pair.second->Get_ShapeLocalPose();

		//본의 초기 월드 위치를, 리지드 바디의 로컬 좌표계로 끌어들인뒤, SkirtActorToBones에 저장해둔다.
		m_SkirtActorToBones.push_back(make_pair(strBoneName, tShapeRestWorld.getInverse() * tBoneRestWorld));

		Safe_Delete_Array(pBoneName);
	}

	return S_OK;
}

HRESULT CBody_Player::Ready_ThighRigidBodies()
{
	CRigidBody::RIGIDBODY_DESC RigidBodyDesc;
	RigidBodyDesc.eRigidBodyShape = CRigidBody::RIGIDBODY_SHAPE::CAPSULE;
	RigidBodyDesc.eRigidBodyType = CRigidBody::RIGIDBODY_TYPE::KINEMATIC;
	RigidBodyDesc.tUserData.szActorTag = TEXT("BodyCollider");
	RigidBodyDesc.vSize = _float3(0.24f, 0.4f, 0.f);
	RigidBodyDesc.fMass = { 0.f };
	RigidBodyDesc.iCollisionGroup = PHYSX_CUSTOM_3;
	RigidBodyDesc.iCollisionMask = PHYSX_TERRAIN | PHYSX_DYNAMIC | PHYSX_DEFAULT | PHYSX_CUSTOM_4;
	RigidBodyDesc.isSimulateSync = false;
	RigidBodyDesc.isQuery = false;
	RigidBodyDesc.vMaterial = _float3(0.2f, 0.2f, 0.f);

	//##################### R-Thigh
	CRigidBody* pRigidBody = { nullptr };
	const _float4x4* pBone = m_pModelCom->Get_BoneMatrixPtr("Ab-R-Thigh-Tw1");
	_matrix OwnerWorld = XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()) * XMLoadFloat4x4(m_pParentTransformCom->Get_WorldMatrixPtr());
	_matrix BoneWorld = XMLoadFloat4x4(pBone) * OwnerWorld;
	XMStoreFloat4x4(&RigidBodyDesc.StartWorldMatrix, BoneWorld);

	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_RigidBody"),
		TEXT("Com_RigidBody_Ab-R-Thigh-Tw1"), reinterpret_cast<CComponent**>(&pRigidBody), &RigidBodyDesc)))
		return E_FAIL;

	pRigidBody->Set_AngularDamping(0.01f);
	pRigidBody->Set_LinearDamping(0.06f);

	m_ThighRigidBodies.push_back(make_pair(pBone, pRigidBody));
	m_pGameInstance->Add_RigidBody_ToPhysx(this, pRigidBody);

	//##################### L-Thigh
	pBone = m_pModelCom->Get_BoneMatrixPtr("Ab-L-Thigh-Tw1");
	OwnerWorld = XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()) * XMLoadFloat4x4(m_pParentTransformCom->Get_WorldMatrixPtr());
	BoneWorld = XMLoadFloat4x4(pBone) * OwnerWorld;
	XMStoreFloat4x4(&RigidBodyDesc.StartWorldMatrix, BoneWorld);

	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_RigidBody"),
		TEXT("Com_RigidBody_Ab-L-Thigh-Tw1"), reinterpret_cast<CComponent**>(&pRigidBody), &RigidBodyDesc)))
		return E_FAIL;

	pRigidBody->Set_AngularDamping(0.01f);
	pRigidBody->Set_LinearDamping(0.06f);

	m_ThighRigidBodies.push_back(make_pair(pBone, pRigidBody));
	m_pGameInstance->Add_RigidBody_ToPhysx(this, pRigidBody);


	//############################## 하반신
	RigidBodyDesc.vSize = _float3(0.55f, 0.f, 0.f);

	pBone = m_pModelCom->Get_BoneMatrixPtr("SC_PhotoMode_Hide");
	OwnerWorld = XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()) * XMLoadFloat4x4(m_pParentTransformCom->Get_WorldMatrixPtr());
	BoneWorld = XMLoadFloat4x4(pBone) * OwnerWorld;
	XMStoreFloat4x4(&RigidBodyDesc.StartWorldMatrix, BoneWorld);

	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_RigidBody"),
		TEXT("Com_RigidBody_SC_PhotoMode_Hide"), reinterpret_cast<CComponent**>(&pRigidBody), &RigidBodyDesc)))
		return E_FAIL;

	pRigidBody->Set_AngularDamping(0.01f);
	pRigidBody->Set_LinearDamping(0.06f);
	pRigidBody->Set_LocalPos(_float3(0.f, 0.f, 0.1f));

	m_ThighRigidBodies.push_back(make_pair(pBone, pRigidBody));
	m_pGameInstance->Add_RigidBody_ToPhysx(this, pRigidBody);
	//pRigidBody->Set_CCD(true);


	return S_OK;
}

HRESULT CBody_Player::Ready_UpperRigidBodies()
{
	CRigidBody::RIGIDBODY_DESC RigidBodyDesc;
	RigidBodyDesc.eRigidBodyShape = CRigidBody::RIGIDBODY_SHAPE::CAPSULE;
	RigidBodyDesc.eRigidBodyType = CRigidBody::RIGIDBODY_TYPE::KINEMATIC;
	RigidBodyDesc.tUserData.szActorTag = TEXT("BodyCollider");
	RigidBodyDesc.fMass = { 0.f };
	RigidBodyDesc.iCollisionGroup = PHYSX_CUSTOM_3;
	RigidBodyDesc.iCollisionMask = PHYSX_TERRAIN | PHYSX_DYNAMIC | PHYSX_DEFAULT | PHYSX_CUSTOM_2 | PHYSX_CUSTOM_3;
	RigidBodyDesc.isSimulateSync = false;
	RigidBodyDesc.isQuery = false;
	RigidBodyDesc.vMaterial = _float3(0.2f, 0.2f, 0.f);

	for (_uint i = 0; i < 2; ++i)
	{
		const _float4x4* pBone = {};
		_wstring strBoneName = TEXT("Com_RigidBody");
		switch (i)
		{
		case 0:
			RigidBodyDesc.vSize = _float3(0.15f, 0.2f, 0.f);
			pBone = m_pModelCom->Get_BoneMatrixPtr("Ab-R-UpperArm-Tw1");
			strBoneName += TEXT("Ab-R-UpperArm-Tw1");
			break;
		case 1:
			RigidBodyDesc.vSize = _float3(0.15f, 0.2f, 0.f);
			pBone = m_pModelCom->Get_BoneMatrixPtr("Ab-L-UpperArm-Tw1");
			strBoneName += TEXT("Ab-L-UpperArm-Tw1");
			break;
		case 2:
			RigidBodyDesc.vSize = _float3(0.2f, 0.4f, 0.f);
			pBone = m_pModelCom->Get_BoneMatrixPtr("Ab-R-Forearm-Tw1");
			strBoneName += TEXT("Ab-R-Forearm-Tw1");
			break;
		case 3:
			RigidBodyDesc.vSize = _float3(0.2f, 0.4f, 0.f);
			pBone = m_pModelCom->Get_BoneMatrixPtr("Ab-L-Forearm-Tw1");
			strBoneName += TEXT("Ab-L-Forearm-Tw1");
			break;
		}

		CRigidBody* pRigidBody = { nullptr };
		_matrix OwnerWorld = XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()) * XMLoadFloat4x4(m_pParentTransformCom->Get_WorldMatrixPtr());
		_matrix BoneWorld = XMLoadFloat4x4(pBone) * OwnerWorld;

		XMStoreFloat4x4(&RigidBodyDesc.StartWorldMatrix, BoneWorld);

		if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_RigidBody"),
			strBoneName, reinterpret_cast<CComponent**>(&pRigidBody), &RigidBodyDesc)))
			return E_FAIL;

		pRigidBody->Set_AngularDamping(0.01f);
		pRigidBody->Set_LinearDamping(0.06f);

		m_ThighRigidBodies.push_back(make_pair(pBone, pRigidBody));
		m_pGameInstance->Add_RigidBody_ToPhysx(this, pRigidBody);
	}

	return S_OK;
}

void CBody_Player::Sync_BonesByJoint()
{
	// 컴바인드 매트릭스의 SRT를 가져온다.
	_matrix matOwnerWorld = XMLoadFloat4x4(&m_CombinedWorldMatrix);
	_vector vOwnerS, vOwnerR, vOwnerT;
	XMMatrixDecompose(&vOwnerS, &vOwnerR, &vOwnerT, matOwnerWorld);

	// 스케일을 제거한 뒤 역행렬로 만들어서 보관한다.
	_matrix matOwnerNoScale = XMMatrixAffineTransformation(
		XMVectorSet(1.f, 1.f, 1.f, 0.f), XMVectorZero(), XMQuaternionNormalize(vOwnerR), vOwnerT);
	_matrix matOwnerNoScaleInv = XMMatrixInverse(nullptr, matOwnerNoScale);

	_uint iCount = 0;
	for (auto& Pair : m_SkirtRigidBodies)
	{
		_wstring strBoneName = Pair.first;
		_char* pBoneName = new _char[strBoneName.length() + 1];
		CStringHelper::ConvertWideToUTF(strBoneName.c_str(), pBoneName);

		//현재 리지드바디의 월드 위치를 가져온다.
		PxTransform tActorNow = Pair.second->Get_PxTransform();
		PxTransform tShapeNow = tActorNow * Pair.second->Get_ShapeLocalPose();

		// 리지드 바디의 로컬 좌표계로 끌어들였던, 본의 초기 월드 위치를
		// 리지드 바디의 현재 월드 행렬로 끌어올린다.
		PxTransform tBoneWorld = tShapeNow * m_SkirtActorToBones[iCount].second;

		// 월드까지 왔지만 스케일은 없는 상태임.
		_matrix matBoneModelNoScale = m_pGameInstance->Convert_PxTransform_ToMatrix(tBoneWorld) * matOwnerNoScaleInv;

		// 본의 월드를 다시 분리해서, 
		_vector vS, vR, vT;
		XMMatrixDecompose(&vS, &vR, &vT, matBoneModelNoScale);

		// 미리 빼놨던 스케일을 가져온 뒤
		_float3 vScaleFloat = m_SkirtBoneScales[iCount].second;
		_vector vScale = XMVectorSet(vScaleFloat.x, vScaleFloat.y, vScaleFloat.z, 0.f);

		// 합성한다.
		_matrix matNew = XMMatrixAffineTransformation(vScale, XMVectorZero(), XMQuaternionNormalize(vR), vT);

		//본의 월드행렬을 덮어 씌운다.
		m_pModelCom->Override_CombinedTransformationMatrix(pBoneName, matNew);

		Safe_Delete_Array(pBoneName);
		iCount++;
	}

}

CBody_Player* CBody_Player::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBody_Player* pInstance = new CBody_Player(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBody_Player::Clone(void* pArg)
{
	CBody_Player* pInstance = new CBody_Player(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBody_Player");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBody_Player::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pBlood);
	for (auto& iter : m_HorizontalJointChains)
	{
		Safe_Release(iter);
	}
	m_HorizontalJointChains.clear();

	for (auto& iter : m_VerticalJointChains)
	{
		Safe_Release(iter);
	}
	m_VerticalJointChains.clear();

	for (auto& Pair : m_SkirtRigidBodies)
	{
		Safe_Release(Pair.second);
	}
	m_SkirtRigidBodies.clear();

	for (auto& Pair : m_ThighRigidBodies)
	{
		Safe_Release(Pair.second);
	}
	m_ThighRigidBodies.clear();

	for (auto& Pair : m_RootRigidBodies)
	{
		Safe_Release(Pair.second);
	}
	m_RootRigidBodies.clear();

	for (auto& Pair : m_UpperRigidBodies)
	{
		Safe_Release(Pair.second);
	}
	m_UpperRigidBodies.clear();

	Safe_Release(m_pMotionTrail);
}
