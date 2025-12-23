#include "pch.h"
#include "Body_Player.h"

#include "GameInstance.h"

#include "Effect.h"
#include "GameManager.h"
#include "Nayitba.h"
#include "StringHelper.h"
#include "Player.h"

CBody_Player::CBody_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPlayer_Parts{ pDevice, pContext }
{
}

CBody_Player::CBody_Player(const CBody_Player& Prototype) 
	: CPlayer_Parts{ Prototype }
{
}

_bool CBody_Player::isFinish_Att()
{
	return false;
}

void CBody_Player::Active_SFX(const _wstring& strObjectTag, const ANIM_NOTIFY& NotifyReference)
{
	if (strObjectTag == TEXT("Play_Effect"))
	{

	}
	else if (strObjectTag == TEXT("Hand_Blood"))
	{
		//m_pBlood->Play();
	}
	else if (strObjectTag == TEXT("Hand_Blood_End"))
	{
		//m_pBlood->Stop();
	}
}

void CBody_Player::Activate_PartObject_Collider(const _wstring& strColliderTag, const ANIM_NOTIFY& NotifyRef)
{
	auto pComponents = Find_Component(strColliderTag);
	if (nullptr == pComponents)
		return;

	m_bIsEnableCollider = NotifyRef.iNumData01;
	if (false == m_bIsEnableCollider)
	{
		static_cast<CCollider*>(pComponents)->ResetCollision();
	}
	//else
	//	m_pGameInstance->GamePauseDurationTime(1, 0.01f, 10.f);
}

HRESULT CBody_Player::Mapping_Shader_Material(_uint iIdx)
{
	if (strcmp(m_pModelCom->Get_MaterialName(m_pModelCom->Get_Mesh_MaterialIndex(iIdx)), "MI_Basebody_V02_F1") == 0)
	{
		if (FAILED(m_pShaderCom->Begin(6)))
			return E_FAIL;
	}

	else 
	{
		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CBody_Player::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBody_Player::Initialize(void* pArg)
{	
	BODY_PLAYER_DESC* pDesc = static_cast<BODY_PLAYER_DESC*>(pArg);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;


	m_pModelCom->Set_AnimationIndex(1);

	return S_OK;
}

void CBody_Player::Priority_Update(_float fTimeDelta)
{
	Update_PreCombinedMatrix();
}

void CBody_Player::Update(_float fTimeDelta)
{ 
	// FSM쪽에서 애니 재생.
	// m_isAnimFinish = m_pModelCom->Play_Animation(fTimeDelta);
}

void CBody_Player::Late_Update(_float fTimeDelta)
{
	XMStoreFloat4x4(&m_CombinedWorldMatrix,
		XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()) * XMLoadFloat4x4(m_pParentTransformCom->Get_WorldMatrixPtr()));

	_matrix vResult = XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()) *
		XMLoadFloat4x4(m_pModelCom->Get_BoneMatrixPtr("Ab-R-Calf-Tw1")) * XMLoadFloat4x4(m_pParentTransformCom->Get_WorldMatrixPtr());

	m_pColliderCom->UpdateColiision(vResult);

	if (m_bIsEnableCollider)
	{
		m_pGameInstance->ADD_Collider(m_pColliderCom);
	}

#ifdef _DEBUG
	m_pGameInstance->Add_DebugComponent(m_pColliderCom);
#endif

	_float fDist = XMVectorGetX(XMVector3Length(m_pParentTransformCom->Get_State(STATE::POSITION) - XMLoadFloat4(m_pGameInstance->Get_CamPosition())));
	
	if(fDist < 100.f)
		m_pGameInstance->Add_RenderGroup(RENDER::MOTIONBLUR, this);
}

HRESULT CBody_Player::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(i, m_pShaderCom, "g_DiffuseTexture", aiTextureType_DIFFUSE, 0)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_Material(i, m_pShaderCom, "g_ORMTexture", aiTextureType_METALNESS, 0)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_Material(i, m_pShaderCom, "g_ORSSTexture", aiTextureType_CLEARCOAT, 0)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_Material(i, m_pShaderCom, "g_NormalTexture", aiTextureType_NORMALS, 0)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_BoneSRV(i, m_pShaderCom, "g_BoneMatrixBuffer")))
			return E_FAIL;

		//rimlight 버전
		if (FAILED(Mapping_Shader_Material(i)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CBody_Player::Render_Shadow()
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
		if (FAILED(m_pModelCom->Bind_BoneSRV(i, m_pShaderCom, "g_BoneMatrixBuffer")))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(1)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}
	return S_OK;
}

HRESULT CBody_Player::Render_MotionBlur()
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
		if (FAILED(m_pModelCom->Bind_BoneSRV(i, m_pShaderCom, "g_BoneMatrixBuffer")))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(4)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CBody_Player::Ready_Components()
{
	/* Com_Model */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_Eve_Body_24_TypeB"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;
	
	/* Com_Shader */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	COBBCollider::OBB_COLLIDER_DESC		OBBDesc{};

	OBBDesc.vSize = _float3(0.1f, 0.3f, 0.1f);
	OBBDesc.vCenter = _float3(0.f, 0.15f, 0.f);
	OBBDesc.vAngles = _float3(0.f, 0.f, 0.f);
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider_OBB"), reinterpret_cast<CComponent**>(&m_pColliderCom), &OBBDesc)))
		return E_FAIL;

	m_pColliderCom->BindBeginOverlapEvent([&](_float3 vHitPoint, _float3 vHitDir, CGameObject* pHitActor) { Begin_OverlapEvent(vHitPoint, vHitDir, pHitActor); });

	m_pColliderCom->SetColliderHitType(HIT_TYPE::PLAYER);
	m_pColliderCom->ADD_IgnoreObjectType(HIT_TYPE::SENCE);
	m_pColliderCom->ADD_IgnoreObjectType(HIT_TYPE::PLAYER);
	m_pColliderCom->ADD_IgnoreObjectType(HIT_TYPE::INTERACTION);

	return S_OK;
}

HRESULT CBody_Player::Bind_ShaderResources()
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

void CBody_Player::Begin_OverlapEvent(_float3 vHitPoint, _float3 vHitDir, CGameObject* pHitActor)
{
	CNayitba* pNaytiba = dynamic_cast<CNayitba*>(pHitActor);
	if (pNaytiba)
	{
		DEFAULT_DAMAGE_DESC pDamageDesc = {};
		pDamageDesc.pAttacker = m_pParent;
		pDamageDesc.vHitPoint = vHitPoint;
		pDamageDesc.vHitDir = vHitDir;

		_uint iSkillID = static_cast<CPlayer*>(m_pParent)->GetSkillDataID();
		if (-1 == iSkillID)
			return;

		pDamageDesc.pSkillData = m_pGameManager->Find_SkillData(iSkillID);
		pNaytiba->Damaged(&pDamageDesc);
	}


}

CBody_Player* CBody_Player::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBody_Player* pInstance = new CBody_Player(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBody_Player::Clone(void* pArg)
{
	CBody_Player* pInstance = new CBody_Player(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBody_Player");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBody_Player::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
}
