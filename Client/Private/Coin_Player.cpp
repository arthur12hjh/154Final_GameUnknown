#include "pch.h"
#include "Coin_Player.h"

#include "GameManager.h"
#include "GameInstance.h"
#include "Player.h"

CCoin_Player::CCoin_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CPlayer_Parts{ pDevice, pContext }
{
}

CCoin_Player::CCoin_Player(const CCoin_Player& Prototype)
    : CPlayer_Parts{ Prototype }
{
}

HRESULT CCoin_Player::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CCoin_Player::Initialize(void* pArg)
{
	COIN_DESC* pDesc = static_cast<COIN_DESC*>(pArg);

	m_pSocketMatrix = pDesc->pSocketMatrix;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pPlayerDesc = m_pGameManager->Get_PlayerDesc();

	return S_OK;
}

void CCoin_Player::Priority_Update(_float fTimeDelta)
{
}

void CCoin_Player::Update(_float fTimeDelta)
{
	_float3 vRotationQuaternion = { -90.f, 0.f, 0.f };

	//m_pTransformCom->Set_Scale(1.f, 1.f, 1.f);
	m_pTransformCom->Rotation(XMConvertToRadians(vRotationQuaternion.x), XMConvertToRadians(vRotationQuaternion.y), XMConvertToRadians(vRotationQuaternion.z));
}

void CCoin_Player::Late_Update(_float fTimeDelta)
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

	XMStoreFloat4x4(&m_CombinedWorldMatrix,	MyMatrix * SocketMatrix * ParentMatrix);
	
	m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
}

HRESULT CCoin_Player::Render()
{
	if (false == m_pPlayerDesc->isCoinVisible)
		return S_OK;

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

	//m_pGameInstance->Render_Text(TEXT("KoPub"), m_szRotationAngle, _float2(g_iWinSizeX / 2 - 180, 0), XMVectorSet(1.f, 1.f, 1.f, 0.1f));

	return S_OK;

}

HRESULT CCoin_Player::Ready_Components()
{
	/* Com_Model */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_Coin"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* Com_Shader */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;
}

HRESULT CCoin_Player::Bind_ShaderResources()
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

CCoin_Player* CCoin_Player::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCoin_Player* pInstance = new CCoin_Player(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCoin_Player");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCoin_Player::Clone(void* pArg)
{
	CCoin_Player* pInstance = new CCoin_Player(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCoin_Player");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCoin_Player::Free()
{
	__super::Free();
}
