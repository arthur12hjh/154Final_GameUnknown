#include "pch.h"
#include "PonyTail_Character.h"
#include "Body_Character.h"

#include "Bone.h"
#include "Model.h"

#include "GameInstance.h"

#include "Character.h"

CPonyTail_Character::CPonyTail_Character(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject{ pDevice, pContext }
{
}

CPonyTail_Character::CPonyTail_Character(const CPonyTail_Character& Prototype)
	: CPartObject{ Prototype }
{
}


HRESULT CPonyTail_Character::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPonyTail_Character::Initialize(void* pArg)
{
	PONYTAIL_CHARACTER_DESC* pDesc = static_cast<PONYTAIL_CHARACTER_DESC*>(pArg);

	m_pParentState = pDesc->pParentState;
	//m_pSocketMatrix = pDesc->pSocketMatrix;
	//strcpy_s(m_szBoneTag, pDesc->szBoneTag);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Bind_BoneToPartBody(pDesc->pBodyPtr);

	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));



	return S_OK;
}

void CPonyTail_Character::Priority_Update(_float fTimeDelta)
{
	int a = 10;
}

void CPonyTail_Character::Update(_float fTimeDelta)
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

void CPonyTail_Character::Late_Update(_float fTimeDelta)
{
	//m_pGameInstance->Add_RenderGroup(RENDER::SHADOW, this);
	m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);

#ifdef _DEBUG

#endif
}

HRESULT CPonyTail_Character::Render()
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

		if (FAILED(m_pModelCom->Bind_Material(i, m_pShaderCom, "g_DiffuseTexture", aiTextureType_DIFFUSE, 0)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_Material(i, m_pShaderCom, "g_EmissiveTexture", aiTextureType_EMISSIVE, 0)))
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

HRESULT CPonyTail_Character::Render_Shadow()
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

HRESULT CPonyTail_Character::Ready_Components()
{
	/* Com_Model */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::EDITOR), TEXT("Prototype_Component_Model_PonyTail_Eve"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* Com_Shader */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::EDITOR), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CPonyTail_Character::Bind_ShaderResources()
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

HRESULT CPonyTail_Character::Bind_BoneToPartBody(void* pArg)
{
	CBody_Character* pBody = static_cast<CBody_Character*>(pArg);

	m_pBodyModelCom = static_cast<CModel*>(pBody->Find_Component(TEXT("Com_Model")));

	return S_OK;
}

CPonyTail_Character* CPonyTail_Character::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPonyTail_Character* pInstance = new CPonyTail_Character(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CPonyTail_Character");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPonyTail_Character::Clone(void* pArg)
{
	CPonyTail_Character* pInstance = new CPonyTail_Character(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CPonyTail_Character");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPonyTail_Character::Free()
{
	__super::Free();

}
