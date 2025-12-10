#include "pch.h"
#include "PxTestProp.h"

#include "GameInstance.h"

CPxTestProp::CPxTestProp(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject { pDevice, pContext }
{
}

CPxTestProp::CPxTestProp(const CPxTestProp& Prototype)
    : CGameObject { Prototype }
{
}

HRESULT CPxTestProp::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CPxTestProp::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(
		m_pGameInstance->Random(0.f, 30.f),
		4.f,
		m_pGameInstance->Random(0.f, 30.f),
		1.f
	));

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CPxTestProp::Priority_Update(_float fTimeDelta)
{
}

void CPxTestProp::Update(_float fTimeDelta)
{
	//리지드 바디 갱신. 객체 내의 모든 움직임이 끝난 뒤 갱신해주세요. 
	m_pRigidBody->Update_PxTransform(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));
}

void CPxTestProp::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(RENDER::BLEND, this);
}

HRESULT CPxTestProp::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{

		if (FAILED(m_pModelCom->Bind_Material(i, m_pShaderCom, "g_DiffuseTexture", aiTextureType_DIFFUSE, 0)))
			return E_FAIL;
		if (FAILED(m_pModelCom->Bind_Material(i, m_pShaderCom, "g_NormalTexture", aiTextureType_NORMALS, 0)))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Begin(3)))
			return E_FAIL;
		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CPxTestProp::Ready_Components()
{
	/* Com_Model */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_ForkLift"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* Com_Shader */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	PxUserData tUserData;
	tUserData.szActorTag = TEXT("Prop_Actor");

	//리지드 바디 Desc 세팅. 머테리얼이랑 Mass, userdata, shape, type 부분 위주로 살펴보세요.
	CRigidBody::RIGIDBODY_DESC RigidBodyDesc;
	RigidBodyDesc.eRigidBodyShape	= CRigidBody::RIGIDBODY_SHAPE::SPHERE;
	RigidBodyDesc.eRigidBodyType	= CRigidBody::RIGIDBODY_TYPE::DYNAMIC;
	RigidBodyDesc.StartWorldMatrix	= *m_pTransformCom->Get_WorldMatrixPtr();
	RigidBodyDesc.tUserData			= tUserData;
	RigidBodyDesc.vMaterial			= _float3(0.5f, 0.5f, 0.6f);
	RigidBodyDesc.vSize				=  m_pTransformCom->Get_Scale();
	RigidBodyDesc.fMass				= { 0.25f };

	/* Com_RigidBody */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_RigidBody"),
		TEXT("Com_RigidBody"), reinterpret_cast<CComponent**>(&m_pRigidBody), &RigidBodyDesc)))
		return E_FAIL;

	// 리지드 바디 세팅 끝났으면 Physx 매니저에 집어넣는 과정도 있어야돼요.
	// 없으면 충돌 안됨
	m_pGameInstance->Add_RigidBody_ToPhysx(this, m_pRigidBody);

	return S_OK;
}

HRESULT CPxTestProp::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
		return E_FAIL;

	return S_OK;
}

CPxTestProp* CPxTestProp::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPxTestProp* pInstance = new CPxTestProp(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CPxTestProp");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPxTestProp::Clone(void* pArg)
{
	CPxTestProp* pInstance = new CPxTestProp(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CPxTestProp");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPxTestProp::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRigidBody);
}
