#include "pch.h"
#include "Lift_Platform.h"
#include "GameInstance.h"

CLift_Platform::CLift_Platform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CActor{ pDevice, pContext }
{
}

CLift_Platform::CLift_Platform(const CLift_Platform& Prototype)
	: CActor{ Prototype }
{
}

HRESULT CLift_Platform::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CLift_Platform::Initialize(void* pArg)
{
	RuinComponentDesc* pDesc = nullptr;
	if (pArg != nullptr)
	{
		pDesc = reinterpret_cast<RuinComponentDesc*>(pArg);
	}

	if (FAILED(__super::Initialize(nullptr)))
		return E_FAIL;

	if (pDesc && pDesc->pComponentTag)
	{
		wcsncpy_s(m_ComponentTag, 256, pDesc->pComponentTag, _TRUNCATE);
	}

	if (FAILED(Ready_Components(m_ComponentTag)))
		return E_FAIL;

	XMStoreFloat3(&m_vRootPos, m_pTransformCom->Get_State(STATE::POSITION));
	return S_OK;
}

void CLift_Platform::Priority_Update(_float fTimeDelta)
{
}

void CLift_Platform::Update(_float fTimeDelta)
{
	if (m_fMoveDistance)
	{
		LerpTargetPoint(fTimeDelta);

		_matrix WorldMat = XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr());
		m_pCullingCollider->UpdateColiision(WorldMat);
		m_pRigidBody->Update_PxTransform(WorldMat);
	}
}

void CLift_Platform::Late_Update(_float fTimeDelta)
{
	if (m_pGameInstance->isIn_WorldFrustum(m_pCullingCollider))
	{
		m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);

#ifdef _DEBUG
		m_pGameInstance->Add_DebugComponent(m_pCullingCollider);
		m_pGameInstance->Add_PhysxGeometry(m_pRigidBody->Get_PxRigidBody(), m_pRigidBody->Get_PxShape());
#endif
	}
}

HRESULT CLift_Platform::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{

		if (FAILED(m_pModelCom->Bind_Material(i, m_pShaderCom, "g_DiffuseTexture", aiTextureType_DIFFUSE, 0)))
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

_bool CLift_Platform::SetPlatformMove(LIFT_PLATFORM_STATE eState)
{
	_vector vPlatformPos = m_pTransformCom->Get_State(STATE::POSITION);
	_vector vDir = {};

	switch (eState)
	{
	case CLift_Platform::LIFT_PLATFORM_STATE::UPPER:
	{
		// 여기서 위인지 아래인지
		if (m_vRootPos.y + m_fMoveDistance > vPlatformPos.m128_f32[1])
		{
			vDir = m_pTransformCom->Get_State(STATE::UP);
			m_bIsPaltformMove = true;
		}
		else
			XMStoreFloat3(&m_vTargetPoint, vPlatformPos);
	}
		break;
	case CLift_Platform::LIFT_PLATFORM_STATE::DWON:
	{
		if (m_vRootPos.y < vPlatformPos.m128_f32[1])
		{
			vDir = -1 * m_pTransformCom->Get_State(STATE::UP);
			m_bIsPaltformMove = true;
		}
		else
			XMStoreFloat3(&m_vTargetPoint, vPlatformPos);
	}
		break;
	}

	if (m_bIsPaltformMove)
	{
		XMStoreFloat3(&m_vTargetPoint, vPlatformPos + vDir * m_fMoveDistance);
		m_ePlatform_State = eState;
		return true;
	}
	
	return false;
}

void CLift_Platform::LerpTargetPoint(_float fTimeDelta)
{
	_vector vPlatformPos = m_pTransformCom->Get_State(STATE::POSITION);
	_vector vTargetPos = XMLoadFloat3(&m_vTargetPoint);

	_float fDistance = XMVectorGetX(XMVector3Length(vTargetPos - vPlatformPos));
	if (fDistance < 0.3f)
		m_bIsPaltformMove = false;

	m_pTransformCom->Chase_Lerp(vTargetPos, fTimeDelta * m_vLerpSpeed);
}

HRESULT CLift_Platform::Ready_Components(const _tchar* pComponentTag)
{
	/* Com_Model */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), pComponentTag,
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* Com_Shader */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	PxUserData tUserData;
	tUserData.szActorTag = TEXT("KIMETIC_Actor2");

	//리지드 바디 Desc 세팅. 머테리얼이랑 Mass, userdata, shape, type 부분 위주로 살펴보세요.
	CRigidBody::RIGIDBODY_DESC RigidBodyDesc;
	// 콜라이더 모양
	RigidBodyDesc.eRigidBodyShape = CRigidBody::RIGIDBODY_SHAPE::BOX;

	// 충돌처리를 할지말지 
	// DYNAMIC : 충돌 
	// KINEMATIC : 충돌 X
	RigidBodyDesc.eRigidBodyType = CRigidBody::RIGIDBODY_TYPE::KINEMATIC;

	RigidBodyDesc.StartWorldMatrix = *m_pTransformCom->Get_WorldMatrixPtr();
	RigidBodyDesc.tUserData = tUserData;
	RigidBodyDesc.vMaterial = _float3(0.5f, 0.5f, 0.3f);
	RigidBodyDesc.vSize = { 2.f, 1.f, 2.f };
	RigidBodyDesc.fMass = { 0.3f };

	/* Com_RigidBody */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_RigidBody"),
		TEXT("Com_RigidBody"), reinterpret_cast<CComponent**>(&m_pRigidBody), &RigidBodyDesc)))
		return E_FAIL;

	// 리지드 바디 세팅 끝났으면 Physx 매니저에 집어넣는 과정도 있어야돼요.
	// 없으면 충돌 안됨
	m_pGameInstance->Add_RigidBody_ToPhysx(this, m_pRigidBody);
	static_cast<COBBCollider*>(m_pCullingCollider)->SetCollision({}, {}, { 2.f, 2.f, 2.f });
	return S_OK;
}

HRESULT CLift_Platform::Bind_ShaderResources()
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

CLift_Platform* CLift_Platform::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLift_Platform* pInstance = new CLift_Platform(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CLift_Platform::Clone(void* pArg)
{
	CLift_Platform* pInstance = new CLift_Platform(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CLift_Platform");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLift_Platform::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}
