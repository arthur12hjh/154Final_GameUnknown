#include "pch.h"
#include "PonyTail_Player.h"
#include "Body_Player.h"

#include "Bone.h"
#include "Model.h"

#include "GameInstance.h"

#include "Player.h"

CPonyTail_Player::CPonyTail_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPlayer_Parts{ pDevice, pContext }
{
}

CPonyTail_Player::CPonyTail_Player(const CPonyTail_Player& Prototype)
	: CPlayer_Parts{ Prototype }
{
}


HRESULT CPonyTail_Player::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPonyTail_Player::Initialize(void* pArg)
{
	PONYTAIL_PLAYER_DESC* pDesc = static_cast<PONYTAIL_PLAYER_DESC*>(pArg);

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

void CPonyTail_Player::Priority_Update(_float fTimeDelta)
{
	int a = 10;
}

void CPonyTail_Player::Update(_float fTimeDelta)
{
	vector<CBone*>* pFaceBone = m_pModelCom->Get_Bones();

	for (auto& pBone : *pFaceBone)
	{
		for (auto& pBodyBone : m_mapBodyBones)
		{
			if (TRUE == pBone->Compare_Name(pBodyBone.first))
			{
				_matrix matBuffer = pBodyBone.second->Get_TransformationMatrix();
				pBone->Set_TransformationMatrix(matBuffer);
				break;
			}
		}

	}

	m_pModelCom->Attach_CombinedTransformationMatrix();

	XMStoreFloat4x4(&m_CombinedWorldMatrix,
		XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()) * XMLoadFloat4x4(m_pParentTransformCom->Get_WorldMatrixPtr()));

}

void CPonyTail_Player::Late_Update(_float fTimeDelta)
{
	//m_pGameInstance->Add_RenderGroup(RENDER::SHADOW, this);
	m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);

#ifdef _DEBUG

#endif
}

HRESULT CPonyTail_Player::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_BoneSRV(i, m_pShaderCom, "g_BoneMatrixBuffer")))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_Material(i, m_pShaderCom, "g_DiffuseTexture", aiTextureType_DIFFUSE, 0)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_Material(i, m_pShaderCom, "g_EmissiveTexture", aiTextureType_EMISSIVE, 0)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_Material(i, m_pShaderCom, "g_ORMTexture", aiTextureType_METALNESS, 0)))
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
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

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

	// map<_char*, _float4x4*>의 형태로 각 본의 위치 포인터를 전달 받음

	vector<CBone*>* pBodyBones = static_cast<CModel*>(pBody->Find_Component(TEXT("Com_Model")))->Get_Bones();

	for (auto& pBone : *pBodyBones)
	{
		m_mapBodyBones.emplace(pBone->Get_Name(), pBone);
	}

	return S_OK;
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

}
