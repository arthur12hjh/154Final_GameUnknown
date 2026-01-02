#include "pch.h"
#include "NaytibaRightWeaponPart.h"

#include "GameInstance.h"
#include "GameManager.h"

CNaytibaRightWeaponPart::CNaytibaRightWeaponPart(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) :
    CPartObject(pDevice, pContext)
{
}

CNaytibaRightWeaponPart::CNaytibaRightWeaponPart(const CNaytibaRightWeaponPart& Prototype) :
    CPartObject(Prototype)
{
}

HRESULT CNaytibaRightWeaponPart::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CNaytibaRightWeaponPart::Initialize(void* pArg)
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

	m_pTransformCom->Rotation(XMConvertToRadians(90.f), XMConvertToRadians(-81.5f), XMConvertToRadians(0.f));


	return S_OK;
}

void CNaytibaRightWeaponPart::Priority_Update(_float fTimeDelta)
{
}

void CNaytibaRightWeaponPart::Update(_float fTimeDelta)
{
}

void CNaytibaRightWeaponPart::Late_Update(_float fTimeDelta)
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

	if (m_eVisibility == VISIBILITY::VISIBLE)
	{
		m_pGameInstance->Add_RenderGroup(RENDER::SHADOW, this);
		m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
	}

}

HRESULT CNaytibaRightWeaponPart::Render()
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

HRESULT CNaytibaRightWeaponPart::Render_Shadow()
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
		if (FAILED(m_pShaderCom->Begin(1)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}
	return S_OK;
}

void CNaytibaRightWeaponPart::Active_SFX(const _wstring& strObjectTag, const ANIM_NOTIFY& NotifyReference)
{
}

void CNaytibaRightWeaponPart::Activate_PartObject_Collider(const _wstring& strColliderTag, const ANIM_NOTIFY& NotifyRef)
{
}

void CNaytibaRightWeaponPart::EnableCollider(_bool bIsEnable)
{
}

HRESULT CNaytibaRightWeaponPart::Ready_Components(const WEAPON_DESC& Desc)
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

HRESULT CNaytibaRightWeaponPart::Bind_ShaderResources()
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

CNaytibaRightWeaponPart* CNaytibaRightWeaponPart::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CNaytibaRightWeaponPart* pInstance = new CNaytibaRightWeaponPart(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : Naytiba Right Weapon");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CNaytibaRightWeaponPart::Clone(void* pArg)
{
	CNaytibaRightWeaponPart* pInstance = new CNaytibaRightWeaponPart(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : Naytiba Right Weapon");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CNaytibaRightWeaponPart::Free()
{
	__super::Free();

}
