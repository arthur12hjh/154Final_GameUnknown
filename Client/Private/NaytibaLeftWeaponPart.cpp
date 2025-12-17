#include "pch.h"
#include "NaytibaLeftWeaponPart.h"

#include "GameInstance.h"
#include "GameManager.h"

CNaytibaLeftWeaponPart::CNaytibaLeftWeaponPart(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) :
    CPartObject(pDevice, pContext)
{
}

CNaytibaLeftWeaponPart::CNaytibaLeftWeaponPart(const CNaytibaLeftWeaponPart& Prototype) :
    CPartObject(Prototype)
{
}

HRESULT CNaytibaLeftWeaponPart::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CNaytibaLeftWeaponPart::Initialize(void* pArg)
{
	WEAPON_DESC* pDesc = static_cast<WEAPON_DESC*>(pArg);

	m_pSocketMatrix = pDesc->pSocketMatrix;
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

void CNaytibaLeftWeaponPart::Priority_Update(_float fTimeDelta)
{
}

void CNaytibaLeftWeaponPart::Update(_float fTimeDelta)
{
}

void CNaytibaLeftWeaponPart::Late_Update(_float fTimeDelta)
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
	//m_pTransformCom->Get_WorldMatrixPtr())

	XMStoreFloat4x4(&m_CombinedWorldMatrix,
		MyMatrix * SocketMatrix * ParentMatrix);
	//XMStoreFloat4x4(&m_CombinedWorldMatrix, XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));

	m_pGameInstance->Add_RenderGroup(RENDER::SHADOW, this);
	m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
}

HRESULT CNaytibaLeftWeaponPart::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_AllMaterials(i, m_pShaderCom, 0)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CNaytibaLeftWeaponPart::Render_Shadow()
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
		if (FAILED(m_pShaderCom->Begin(1)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}
	return S_OK;
}

void CNaytibaLeftWeaponPart::Active_SFX(const _wstring& strObjectTag, const ANIM_NOTIFY& NotifyReference)
{
}

void CNaytibaLeftWeaponPart::Activate_PartObject_Collider(const _wstring& strColliderTag, const ANIM_NOTIFY& NotifyRef)
{
}

void CNaytibaLeftWeaponPart::EnableCollider(_bool bIsEnable)
{
}

HRESULT CNaytibaLeftWeaponPart::Ready_Components(const WEAPON_DESC& Desc)
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

HRESULT CNaytibaLeftWeaponPart::Bind_ShaderResources()
{
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

CNaytibaLeftWeaponPart* CNaytibaLeftWeaponPart::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CNaytibaLeftWeaponPart* pInstance = new CNaytibaLeftWeaponPart(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : Naytiba Left Weapon");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CNaytibaLeftWeaponPart::Clone(void* pArg)
{
	CNaytibaLeftWeaponPart* pInstance = new CNaytibaLeftWeaponPart(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : Naytiba Left Weapon");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CNaytibaLeftWeaponPart::Free()
{
	__super::Free();
}
