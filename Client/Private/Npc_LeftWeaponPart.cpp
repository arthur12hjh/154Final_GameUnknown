#include "pch.h"
#include "Npc_LeftWeaponPart.h"

#include "GameInstance.h"

CNpc_LeftWeaponPart::CNpc_LeftWeaponPart(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) :
    CPartObject(pDevice, pContext)
{
}

CNpc_LeftWeaponPart::CNpc_LeftWeaponPart(const CNpc_LeftWeaponPart& Prototype) :
    CPartObject(Prototype)
{
}

HRESULT CNpc_LeftWeaponPart::Initialize_Prototype()
{

    return S_OK;
}

HRESULT CNpc_LeftWeaponPart::Initialize(void* pArg)
{
	WEAPON_DESC* pDesc = static_cast<WEAPON_DESC*>(pArg);

	m_pSocketMatrix = pDesc->pSocketMatrix;

	SetVisibility(VISIBILITY::VISIBLE);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(*pDesc)))
		return E_FAIL;

	m_pModelCom->Bind_MaterialTag(TEXTURE_TYPE::DIFFUSE, "g_DiffuseTexture");
	m_pModelCom->Bind_MaterialTag(TEXTURE_TYPE::NORMAL, "g_NormalTexture");
	//m_pModelCom->Bind_MaterialTag(TEXTURE_TYPE::EMISSIVE, "g_EmissiveTexture");
	m_pModelCom->Bind_MaterialTag(TEXTURE_TYPE::ORM, "g_ORMTexture");

	return S_OK;
}

void CNpc_LeftWeaponPart::Priority_Update(_float fTimeDelta)
{
	
}

void CNpc_LeftWeaponPart::Update(_float fTimeDelta)
{

}

void CNpc_LeftWeaponPart::Late_Update(_float fTimeDelta)
{
	_matrix		SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);
	_matrix		ParentMatrix = XMLoadFloat4x4(m_pParentTransformCom->Get_WorldMatrixPtr());
	_matrix		MyMatrix = XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr());

	for (size_t i = 0; i < 3; i++)
		MyMatrix.r[i] = XMVector3Normalize(MyMatrix.r[i]);
	for (size_t i = 0; i < 3; i++)
		SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);
	for (size_t i = 0; i < 3; i++)
		ParentMatrix.r[i] = XMVector3Normalize(ParentMatrix.r[i]);

	XMStoreFloat4x4(&m_CombinedWorldMatrix,
		MyMatrix * SocketMatrix * ParentMatrix);

	if (m_eVisibility == VISIBILITY::VISIBLE)
	{
		m_pGameInstance->Add_RenderGroup(RENDER::SHADOW, this);
		m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
	}
}

HRESULT CNpc_LeftWeaponPart::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(i, m_pShaderCom, "g_DiffuseTexture", aiTextureType_DIFFUSE, 0)))
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

HRESULT CNpc_LeftWeaponPart::Render_Shadow()
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
		if (FAILED(m_pShaderCom->Begin(6)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}
	return S_OK;
}

HRESULT CNpc_LeftWeaponPart::Ready_Components(const WEAPON_DESC& Desc)
{
	/* Com_Model */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), Desc.szWeaponModelPrototype,
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* Com_Shader */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CNpc_LeftWeaponPart::Bind_ShaderResources()
{
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

CNpc_LeftWeaponPart* CNpc_LeftWeaponPart::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CNpc_LeftWeaponPart* pNpc_WeaponPart = new CNpc_LeftWeaponPart(pDevice, pContext);
	if (FAILED(pNpc_WeaponPart->Initialize_Prototype()))
	{
		Safe_Release(pNpc_WeaponPart);
		MSG_BOX("Create Fail : Npc Left Weapon Part");
	}
    return pNpc_WeaponPart;
}

CGameObject* CNpc_LeftWeaponPart::Clone(void* pArg)
{
	CNpc_LeftWeaponPart* pNpc_WeaponPart = new CNpc_LeftWeaponPart(*this);
	if (FAILED(pNpc_WeaponPart->Initialize(pArg)))
	{
		Safe_Release(pNpc_WeaponPart);
		MSG_BOX("Clone Fail : Npc Left Weapon Part");
	}
	return pNpc_WeaponPart;
}

void CNpc_LeftWeaponPart::Free()
{
	__super::Free();
}
