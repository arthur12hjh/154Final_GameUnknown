#include "pch.h"
#include "Face_Character.h"
#include "Body_Character.h"

#include "Bone.h"
#include "Model.h"

#include "GameInstance.h"

#include "Character.h"

CFace_Character::CFace_Character(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject{ pDevice, pContext }
{
}

CFace_Character::CFace_Character(const CFace_Character& Prototype)
	: CPartObject{ Prototype }
{
}


HRESULT CFace_Character::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CFace_Character::Initialize(void* pArg)
{
	FACE_CHARACTER_DESC* pDesc = static_cast<FACE_CHARACTER_DESC*>(pArg);

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

void CFace_Character::Priority_Update(_float fTimeDelta)
{
	int a = 10;
}

void CFace_Character::Update(_float fTimeDelta)
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

void CFace_Character::Late_Update(_float fTimeDelta)
{
	//m_pGameInstance->Add_RenderGroup(RENDER::SHADOW, this);
	m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);

#ifdef _DEBUG

#endif
}

HRESULT CFace_Character::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{

		if (FAILED(m_pModelCom->Bind_BoneSRV(i, m_pShaderCom, "g_BoneMatrixBuffer")))
			return E_FAIL;

		//if (FAILED(m_pBodyModelCom->Bind_BoneMatrixSRV(m_pShaderCom, "g_BoneMatrixBuffer")))
		//	return E_FAIL;
		//
		//if (FAILED(m_pBodyModelCom->Bind_PreBoneMatrixSRV(m_pShaderCom)))
		//	return E_FAIL;
		//
		//if (FAILED(m_pModelCom->Bind_GlobalOffsetMatrices(m_pShaderCom)))
		//	return E_FAIL;

		if (FAILED(m_pModelCom->Bind_Material(i, m_pShaderCom, "g_DiffuseTexture", aiTextureType_DIFFUSE, 0)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_Material(i, m_pShaderCom, "g_EmissiveTexture", aiTextureType_EMISSIVE, 0)))
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

HRESULT CFace_Character::Render_Shadow()
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

		//if (FAILED(m_pBodyModelCom->Bind_BoneMatrixSRV(m_pShaderCom, "g_BoneMatrixBuffer")))
		//	return E_FAIL;
		//
		//if (FAILED(m_pBodyModelCom->Bind_PreBoneMatrixSRV(m_pShaderCom)))
		//	return E_FAIL;
		//
		//if (FAILED(m_pModelCom->Bind_GlobalOffsetMatrices(m_pShaderCom)))
		//	return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(1)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}
	return S_OK;
}

HRESULT CFace_Character::Ready_Components()
{
	/* Com_Model */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::EDITOR), TEXT("Prototype_Component_Model_Face_Eve"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* Com_Shader */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::EDITOR), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CFace_Character::Bind_ShaderResources()
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

HRESULT CFace_Character::Bind_BoneToPartBody(void* pArg)
{
	CBody_Character* pBody = static_cast<CBody_Character*>(pArg);

	m_pBodyModelCom = static_cast<CModel*>(pBody->Find_Component(TEXT("Com_Model")));



	return S_OK;
}

CFace_Character* CFace_Character::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CFace_Character* pInstance = new CFace_Character(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CFace_Character");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CFace_Character::Clone(void* pArg)
{
	CFace_Character* pInstance = new CFace_Character(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CFace_Character");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFace_Character::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}
