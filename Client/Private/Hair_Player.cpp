#include "pch.h"
#include "Hair_Player.h"
#include "Body_Player.h"

#include "Bone.h"
#include "Model.h"

#include "GameInstance.h"

#include "Player.h"

CHair_Player::CHair_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPlayer_Parts{ pDevice, pContext }
{
}

CHair_Player::CHair_Player(const CHair_Player& Prototype)
	: CPlayer_Parts{ Prototype }
{
}


HRESULT CHair_Player::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CHair_Player::Initialize(void* pArg)
{
	HAIR_PLAYER_DESC* pDesc = static_cast<HAIR_PLAYER_DESC*>(pArg);
	//m_pSocketMatrix = pDesc->pSocketMatrix;
	//strcpy_s(m_szBoneTag, pDesc->szBoneTag);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Bind_BoneToPartBody(pDesc->pBodyPtr);

	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));

	m_pModelCom->Bind_MaterialTag(TEXTURE_TYPE::DIFFUSE, "g_DiffuseTexture");
	m_pModelCom->Bind_MaterialTag(TEXTURE_TYPE::OPACITY, "g_OpacityTexture");
	m_pModelCom->Bind_MaterialTag(TEXTURE_TYPE::NORMAL, "g_NormalTexture");
	//m_pModelCom->Bind_MaterialTag(TEXTURE_TYPE::EMISSIVE, "g_EmissiveTexture");
	m_pModelCom->Bind_MaterialTag(TEXTURE_TYPE::ORM, "g_ORMTexture");
	m_pModelCom->Bind_MaterialTag(TEXTURE_TYPE::ORSS, "g_ORSSTexture");
	return S_OK;
}

void CHair_Player::Priority_Update(_float fTimeDelta)
{

}

void CHair_Player::Update(_float fTimeDelta)
{
}

void CHair_Player::Late_Update(_float fTimeDelta)
{
	XMStoreFloat4x4(&m_CombinedWorldMatrix,
		XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()) * XMLoadFloat4x4(m_pParentTransformCom->Get_WorldMatrixPtr()));

	//m_pGameInstance->Add_RenderGroup(RENDER::SHADOW, this);
	m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);

#ifdef _DEBUG

#endif
}

HRESULT CHair_Player::Render()
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
		if (FAILED(m_pShaderCom->Begin(4)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CHair_Player::Render_Shadow()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_Shadow_Resource(m_pShaderCom, "g_ViewMatrix", D3DTS::VIEW)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_Shadow_Resource(m_pShaderCom, "g_ProjMatrix", D3DTS::PROJ)))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
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

HRESULT CHair_Player::Ready_Components()
{
	/* Com_Model */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_Hair_Eve"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* Com_Shader */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Shader_Eve_Hair"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CHair_Player::Bind_ShaderResources()
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

HRESULT CHair_Player::Bind_BoneToPartBody(void* pArg)
{
	CBody_Player* pBody = static_cast<CBody_Player*>(pArg);

	m_pBodyModelCom = static_cast<CModel*>(pBody->Find_Component(TEXT("Com_Model")));



	return S_OK;
}

CHair_Player* CHair_Player::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CHair_Player* pInstance = new CHair_Player(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CHair_Player");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CHair_Player::Clone(void* pArg)
{
	CHair_Player* pInstance = new CHair_Player(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CHair_Player");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHair_Player::Free()
{
	__super::Free();
}
