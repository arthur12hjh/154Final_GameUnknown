#include "pch.h"

#include "Hairpin_Player.h"

#include "GameInstance.h"
#include "GameManager.h"

#include "Body_Player.h"
#include "Player.h"

CHairpin_Player::CHairpin_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CPlayer_Parts{ pDevice, pContext }
{
}

CHairpin_Player::CHairpin_Player(const CHairpin_Player& Prototype)
    : CPlayer_Parts{ Prototype }
{
}

HRESULT CHairpin_Player::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CHairpin_Player::Initialize(void* pArg)
{
	HAIRPIN_PLAYER_DESC* pDesc = static_cast<HAIRPIN_PLAYER_DESC*>(pArg);
	//m_pSocketMatrix = pDesc->pSocketMatrix;
	//strcpy_s(m_szBoneTag, pDesc->szBoneTag);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	Bind_BoneToPartBody(pDesc->pBodyPtr);

	if (FAILED(Ready_Components()))
		return E_FAIL;

	//m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(-0.02f, 0.01f, -0.0f, 1.f));
	m_pTransformCom->Rotation(XMConvertToRadians(36.9f), XMConvertToRadians(-141.7f), XMConvertToRadians(18.9f));

	m_pModelCom->Bind_MaterialTag(TEXTURE_TYPE::DIFFUSE, "g_DiffuseTexture");
	m_pModelCom->Bind_MaterialTag(TEXTURE_TYPE::OPACITY, "g_OpacityTexture");
	m_pModelCom->Bind_MaterialTag(TEXTURE_TYPE::NORMAL, "g_NormalTexture");
	//m_pModelCom->Bind_MaterialTag(TEXTURE_TYPE::EMISSIVE, "g_EmissiveTexture");
	m_pModelCom->Bind_MaterialTag(TEXTURE_TYPE::ORM, "g_ORMTexture");
	//m_pModelCom->Bind_MaterialTag(TEXTURE_TYPE::ORSS, "g_ORSSTexture");
	
	m_vRotationQuaternion = { 10.4f, -55.4f, 55.0f };

	return S_OK;
}

void CHairpin_Player::Priority_Update(_float fTimeDelta)
{

}

void CHairpin_Player::Update(_float fTimeDelta)
{
}

void CHairpin_Player::Late_Update(_float fTimeDelta)
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

	m_pMotionTrail->Update_Trail(fTimeDelta);

	if (m_bIsActive)
	{
		m_pGameInstance->Add_RenderGroup(RENDER::NONLIGHT, m_pMotionTrail);
		m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
		m_pGameInstance->Add_RenderGroup(RENDER::SHADOW, this);
	}

	//m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
}

HRESULT CHairpin_Player::Render()
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

HRESULT CHairpin_Player::Render_Shadow()
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

HRESULT CHairpin_Player::Render_MotionBlur()
{
	/* 이전 프레임 월드매트릭스도 바인딩 */
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_PreWorldMatrix", &m_PreCombinedWorldMatrix)))
		return E_FAIL;

	/* 이전 뷰 매트릭스도 바인딩 */
	if (FAILED(m_pShaderCom->Bind_Matrix("g_PreViewMatrix", m_pGameInstance->Get_PreTransform_Float4x4(D3DTS::VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pShaderCom->Begin(4)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CHairpin_Player::Ready_Components()
{
	/* Com_Model */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_Hairpin"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* Com_Shader */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* Com_MotionTrail */
	CMotionTrailComponent::MOTION_TRAIL_COMPONENT_DESC MotionTrailCom = {};
	MotionTrailCom.pModel = m_pModelCom;
	MotionTrailCom.pPreBoneModel = m_pBodyModelCom;
	MotionTrailCom.pTransform = &m_CombinedWorldMatrix;
	MotionTrailCom.fUpdateTime = 0.2f;
	MotionTrailCom.fLifeTime = 3.f;

	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_MotionTrail"),
		TEXT("Com_MotionTrail"), reinterpret_cast<CComponent**>(&m_pMotionTrail), &MotionTrailCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CHairpin_Player::Bind_ShaderResources()
{
	CPlayer* pPlayer = m_pGameManager->GetGameCharacter();
	Safe_Release(pPlayer);
	_bool bFlag = static_cast<CCharacter*>(pPlayer)->Get_DepthMaskingB();

	if (FAILED(m_pShaderCom->Bind_RawValue("g_IsMaskingDepthB", &bFlag, sizeof(_bool))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_IsMaskingDepthW", &bFlag, sizeof(_bool))))
		return E_FAIL;

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

HRESULT CHairpin_Player::Bind_BoneToPartBody(void* pArg)
{
	CBody_Player* pBody = static_cast<CBody_Player*>(pArg);

	m_pBodyModelCom = static_cast<CModel*>(pBody->Find_Component(TEXT("Com_Model")));

	m_pSocketMatrix = m_pBodyModelCom->Get_BoneMatrixPtr("SC_HairACC");

	return S_OK;
}

CHairpin_Player* CHairpin_Player::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CHairpin_Player* pInstance = new CHairpin_Player(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CHairpin_Player");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CHairpin_Player::Clone(void* pArg)
{
	CHairpin_Player* pInstance = new CHairpin_Player(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CHairpin_Player");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHairpin_Player::Free()
{
	__super::Free();
}
