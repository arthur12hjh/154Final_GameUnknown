#include "pch.h"
#include "Scabbard_Scarlet.h"

#include "GameInstance.h"

#include "Character.h"

CScabbard_Scarlet::CScabbard_Scarlet(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject{ pDevice, pContext }
{
}

CScabbard_Scarlet::CScabbard_Scarlet(const CScabbard_Scarlet& Prototype)
	: CPartObject{ Prototype }
{
}

HRESULT CScabbard_Scarlet::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CScabbard_Scarlet::Initialize(void* pArg)
{
	SCABBARD_DESC* pDesc = static_cast<SCABBARD_DESC*>(pArg);

	m_pParentState = pDesc->pParentState;
	m_pSocketMatrix = pDesc->pSocketMatrix;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_vRotationQuaternion = { 90.f, -81.5f, 0.f };

	m_pTransformCom->Rotation(XMConvertToRadians(m_vRotationQuaternion.x), XMConvertToRadians(m_vRotationQuaternion.y), XMConvertToRadians(m_vRotationQuaternion.z));

	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));

	return S_OK;
}

void CScabbard_Scarlet::Priority_Update(_float fTimeDelta)
{
	/*if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_P))
		m_vRotationQuaternion.x += fTimeDelta * 30.f;
	if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_O))
		m_vRotationQuaternion.x -= fTimeDelta * 30.f;
	
	if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_L))
		m_vRotationQuaternion.y += fTimeDelta * 30.f;
	if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_K))
		m_vRotationQuaternion.y -= fTimeDelta * 30.f;
	
	if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_M))
		m_vRotationQuaternion.z += fTimeDelta * 30.f;
	if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_N))
		m_vRotationQuaternion.z -= fTimeDelta * 30.f;
	
	
	m_pTransformCom->Rotation(XMConvertToRadians(m_vRotationQuaternion.x), XMConvertToRadians(m_vRotationQuaternion.y), XMConvertToRadians(m_vRotationQuaternion.z));
	
	swprintf_s(m_szRotationAngle, L"%.1f, %.1f, %.1f", m_vRotationQuaternion.x, m_vRotationQuaternion.y, m_vRotationQuaternion.z);*/

}

void CScabbard_Scarlet::Update(_float fTimeDelta)
{
	_matrix		SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);

	for (size_t i = 0; i < 3; i++)
		SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);


	XMStoreFloat4x4(&m_CombinedWorldMatrix,
		XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()) * SocketMatrix * XMLoadFloat4x4(m_pParentTransformCom->Get_WorldMatrixPtr()));
	//m_pColliderCom->UpdateColiision(XMLoadFloat4x4(&m_CombinedWorldMatrix));
}

void CScabbard_Scarlet::Late_Update(_float fTimeDelta)
{
	//m_pGameInstance->Add_RenderGroup(RENDER::SHADOW, this);
	m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);

#ifdef _DEBUG
	//m_pGameInstance->Add_DebugComponent(m_pColliderCom);

#endif
}

HRESULT CScabbard_Scarlet::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;


	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(i, m_pShaderCom, "g_DiffuseTexture", aiTextureType_DIFFUSE, 0)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

	//m_pGameInstance->Render_Text(TEXT("KoPub"), m_szRotationAngle, _float2(g_iWinSizeX / 2 - 180, 100), XMVectorSet(1.f, 1.f, 1.f, 0.1f));



	return S_OK;
}

HRESULT CScabbard_Scarlet::Render_Shadow()
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

HRESULT CScabbard_Scarlet::Ready_Components()
{
	/* Com_Model */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::EDITOR), TEXT("Prototype_Component_Model_Scarlet_Scabbard"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* Com_Shader */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::EDITOR), TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	///* Com_Collider_OBB */
	//COBBCollider::OBB_COLLIDER_DESC		OBBDesc{};
	//
	//OBBDesc.vSize = _float3(2.0f, 2.5f, 3.5f);
	//OBBDesc.vCenter = _float3(0.f, OBBDesc.vSize.y * 0.5f, 0.f);
	//OBBDesc.vAngles = _float3(0.f,  0.f/*XMConvertToRadians(45.0f)*/, 0.f);
	//if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Collider_OBB"),
	//	TEXT("Com_Collider_OBB"), reinterpret_cast<CComponent**>(&m_pColliderCom), &OBBDesc)))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CScabbard_Scarlet::Bind_ShaderResources()
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

CScabbard_Scarlet* CScabbard_Scarlet::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CScabbard_Scarlet* pInstance = new CScabbard_Scarlet(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CScabbard_Scarlet::Clone(void* pArg)
{
	CScabbard_Scarlet* pInstance = new CScabbard_Scarlet(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CScabbard_Scarlet");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CScabbard_Scarlet::Free()
{
	__super::Free();

	//Safe_Release(m_pColliderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}
