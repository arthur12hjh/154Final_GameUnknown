#include "pch.h"
#include "Bullet_Scarlet.h"

#include "GameInstance.h"
#include "Character.h"

#include "Effect.h"

CBullet_Scarlet::CBullet_Scarlet(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) :
	CBullet(pDevice, pContext)
{
}

CBullet_Scarlet::CBullet_Scarlet(const CBullet_Scarlet& Prototype) :
	CBullet(Prototype)
{
}

HRESULT CBullet_Scarlet::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBullet_Scarlet::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	BULLET_DESC* pDesc = static_cast<BULLET_DESC*>(pArg);
	if (FAILED(ADD_Components(*pDesc)))
		return E_FAIL;

	Shoot_Projectile(XMLoadFloat3(&pDesc->vTargetPoint), 30.f);
	Update_BulletCombinedMatrix();
	CEffect::EFFECT_TRANSFORM_DESC EffectDesc;
	EffectDesc.fRotationPerSec = 1.f;
	EffectDesc.fSpeedPerSec = 1.f;
	
	EffectDesc.pRootMatrix = &m_CombinedWorldMatrix;
	EffectDesc.pWorldMatrix = nullptr;
	
	EffectDesc.vPos = XMVectorSet(0, 0, 0, 1);
	EffectDesc.fRot = _float3(0, 0, 0);
	EffectDesc.fSize = 0.9f;
	EffectDesc.iFloor = 0;
	EffectDesc.pDir = &m_vProjectileDir;
	m_pEffect = static_cast<CEffect*>(m_pGameInstance->Add_Get_GameObject(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Effect_Scarlet_Disk"),
		ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_Effect"), &EffectDesc));

	EffectDesc.pRootMatrix = nullptr;
	EffectDesc.pWorldMatrix = nullptr;

	EffectDesc.vPos = m_pTransformCom->Get_State(STATE::POSITION) + XMLoadFloat3(&m_vProjectileDir) * 2;
	EffectDesc.fRot = _float3(0, XMConvertToRadians(90.f), 0);
	EffectDesc.fSize = 3.2f;
	EffectDesc.iFloor = 0;
	EffectDesc.fSpeed = 0.8f;
	EffectDesc.pDir = &m_vProjectileDir;
	m_pGameInstance->Add_Get_GameObject(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Effect_Scarlet_Shoot"), ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_Effect"), &EffectDesc);
	return S_OK;
}

void CBullet_Scarlet::Priority_Update(_float fTimeDelta)
{
}

void CBullet_Scarlet::Update(_float fTimeDelta)
{
	switch (m_eBulletType)
	{
	case BULLET_TYPE::PROJECTILE:
		m_pTransformCom->Move_Direction(fTimeDelta, XMLoadFloat3(&m_vProjectileDir), m_fSpeed);
	break;
	case BULLET_TYPE::HITSCAN:
	{
		m_iHitScanFrameIndex.x++;
		if (50 <= m_iHitScanFrameIndex.x)
			Set_Dead(true);
	}
	break;
	}
	
	Update_BulletCombinedMatrix();
	m_pColliderCom->UpdateColiision(XMLoadFloat4x4(&m_CombinedWorldMatrix));
}

void CBullet_Scarlet::Late_Update(_float fTimeDelta)
{
	if (m_pGameInstance->isIn_DistanceFrustum(m_pTransformCom->Get_State(STATE::POSITION), 200.f))
	{
#ifdef _DEBUG
		m_pGameInstance->Add_DebugComponent(m_pColliderCom);
#endif // _DEBUG

		if(false == m_isDead)
			m_pGameInstance->ADD_Collider(m_pColliderCom);
	}
	else
	{
		//m_pEffect->Set_Dead(true);
		Set_Dead(true);
	}
}

HRESULT CBullet_Scarlet::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	return S_OK;
}

void CBullet_Scarlet::Shoot_Projectile(_vector vTargetPoint, _float fSpeed)
{
	switch (m_eBulletType)
	{
	case BULLET_TYPE::PROJECTILE:
	{
		Update_BulletCombinedMatrix();
		_matrix CombinedMatrix = XMLoadFloat4x4(&m_CombinedWorldMatrix);

		_vector vLook = m_pParent->GetTransform()->Get_State(STATE::LOOK);
		_vector vStartPos = CombinedMatrix.r[3] + vLook * 5.f;

		m_pTransformCom->Set_State(STATE::POSITION, vStartPos);
		vTargetPoint.m128_f32[1] += 2.f;
		XMStoreFloat3(&m_vProjectileDir, XMVector3Normalize(vTargetPoint - vStartPos));
		m_fSpeed = fSpeed;
	}
		break;
	case BULLET_TYPE::HITSCAN:
	{
		vTargetPoint.m128_f32[1] += 2.f;
		m_pTransformCom->Set_State(STATE::POSITION, vTargetPoint);
	}
		break;
	}
	m_bIsAttachment = false;
}

HRESULT CBullet_Scarlet::ADD_Components(BULLET_DESC& pDesc)
{
	COBBCollider::OBB_COLLIDER_DESC pOBBDesc = {};
	pOBBDesc.vSize = { 1.f, 0.5f, 1.f };

	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider_OBB"), reinterpret_cast<CComponent**>(&m_pColliderCom), &pOBBDesc)))
		return E_FAIL;

	m_pColliderCom->BindBeginOverlapEvent([&](_float3 vHitPoint, _float3 vHitDir, CGameObject* pHitActor) { Begin_OverlapEvent(vHitPoint, vHitDir, pHitActor); });
	m_pColliderCom->SetColliderHitType(HIT_TYPE(pDesc.iHitType));

	m_pColliderCom->ADD_IgnoreObjectType(HIT_TYPE(pDesc.iHitType));
	m_pColliderCom->ADD_IgnoreObjectType(HIT_TYPE::SENCE);
	m_pColliderCom->ADD_IgnoreObjectType(HIT_TYPE::INTERACTION);

	return S_OK;
}

HRESULT CBullet_Scarlet::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
		return E_FAIL;

	return S_OK;
}

void CBullet_Scarlet::Begin_OverlapEvent(_float3 vHitPoint, _float3 vHitDir, CGameObject* pHitActor)
{
	// 여기서 일단 데미지 처리한다
	// 맞으면 맞는 모션 나오게
	DEFAULT_DAMAGE_DESC pDamageDesc = {};
	pDamageDesc.pAttacker = m_pParent;
	pDamageDesc.vHitPoint = vHitPoint;
	pDamageDesc.vHitDir = vHitDir;
	pDamageDesc.pSkillData = m_pSkillData;

	if (ReflectBullet(pHitActor))
	{
		CHARACTER_SKILL_DESC DummySkill = {};
		pDamageDesc.pSkillData = &DummySkill;
	}
	else
		Set_Dead(true);

	auto pCharacter = dynamic_cast<CCharacter*>(pHitActor);
	if (pCharacter) {
		pCharacter->Damaged(&pDamageDesc);
		CEffect::EFFECT_TRANSFORM_DESC EffectDesc;
		EffectDesc.fRotationPerSec = 1.f;
		EffectDesc.fSpeedPerSec = 1.f;

		EffectDesc.pRootMatrix = nullptr;
		EffectDesc.pWorldMatrix = nullptr;
		
		EffectDesc.vPos = XMLoadFloat4x4(&m_CombinedWorldMatrix).r[3] + XMLoadFloat3(&m_vProjectileDir) * 2 - XMVector3Normalize(XMVector3Cross(XMLoadFloat3(&m_vProjectileDir), XMVectorSet(1, 0, 0, 0))) * 1;
		EffectDesc.fRot = _float3(0, 0, 0);
		EffectDesc.fSize = 2.5f;
		EffectDesc.iFloor = 0;
		EffectDesc.pDir = &m_vProjectileDir;
		m_pGameInstance->Add_Get_GameObject(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Effect_Scarlet_Disk_Boom"), ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_Effect"), &EffectDesc);
	}

	//m_pEffect->Set_Dead(true);
}

CBullet_Scarlet* CBullet_Scarlet::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBullet_Scarlet* pBullet_Scarlet = new CBullet_Scarlet(pDevice, pContext);
	if (FAILED(pBullet_Scarlet->Initialize_Prototype()))
	{
		Safe_Release(pBullet_Scarlet);
		MSG_BOX("Create Fail : Bullet Scarlet");
	}

	return pBullet_Scarlet;
}

CGameObject* CBullet_Scarlet::Clone(void* pArg)
{
	CBullet_Scarlet* pBullet_Scarlet = new CBullet_Scarlet(*this);
	if (FAILED(pBullet_Scarlet->Initialize(pArg)))
	{
		Safe_Release(pBullet_Scarlet);
		MSG_BOX("Clone Fail : Bullet Scarlet");
	}

	return pBullet_Scarlet;
}

void CBullet_Scarlet::Free()
{
	__super::Free();
	if (nullptr != m_pEffect)
		m_pEffect->End();
}
