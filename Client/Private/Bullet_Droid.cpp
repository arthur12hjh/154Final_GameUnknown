#include "pch.h"
#include "Bullet_Droid.h"

#include "GameInstance.h"
#include "Character.h"

#include "Player.h"
#include "Effect.h"

CBullet_Droid::CBullet_Droid(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) :
    CBullet(pDevice, pContext)
{
}

CBullet_Droid::CBullet_Droid(const CBullet_Droid& Prototype) :
    CBullet(Prototype)
{
}

HRESULT CBullet_Droid::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CBullet_Droid::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	BULLET_DESC* pDesc = static_cast<BULLET_DESC*>(pArg);
	if (FAILED(ADD_Components(*pDesc)))
		return E_FAIL;

	m_PointLists.resize(5, {});
	Shoot_Projectile(XMLoadFloat3(&pDesc->vTargetPoint), 30.f);
	CEffect::EFFECT_TRANSFORM_DESC EffectDesc;
	EffectDesc.fRotationPerSec = 1.f;
	EffectDesc.fSpeedPerSec = 1.f;

	EffectDesc.pRootMatrix = &m_CombinedWorldMatrix;
	EffectDesc.pWorldMatrix = nullptr;

	EffectDesc.vPos = XMVectorSet(0, 0, 0, 1);
	EffectDesc.fRot = _float3(0, 0, 0);
	EffectDesc.fSize = 0.1f;
	EffectDesc.iFloor = 0;
	EffectDesc.pDir = &m_vProjectileDir;
	m_pEffect = static_cast<CEffect*>(m_pGameInstance->Add_Get_GameObject(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Effect_Missile"),
		ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_Effect"), &EffectDesc));

	return S_OK;
}

void CBullet_Droid::Priority_Update(_float fTimeDelta)
{

}

void CBullet_Droid::Update(_float fTimeDelta)
{
	switch (m_eBulletType)
	{
	case BULLET_TYPE::PROJECTILE:
		m_pTransformCom->Move_Direction(fTimeDelta, XMLoadFloat3(&m_vProjectileDir), m_fSpeed);
		break;
	case BULLET_TYPE::ARCING:
	{
		m_fArcingTime.x += fTimeDelta * 5.f;
		_vector vTargetPoint = BezierCurve(5, m_PointLists.data(), m_fArcingTime.x / m_fArcingTime.y);
		m_pTransformCom->LookAt_Lerp(vTargetPoint + m_pTransformCom->Get_State(STATE::LOOK) * 5.f, fTimeDelta, 5.f);
		
		m_pTransformCom->Set_State(STATE::POSITION, vTargetPoint);
		if (m_fArcingTime.x >= m_fArcingTime.y)
			Set_Dead(true);
	}
	break;
	}

	Update_BulletCombinedMatrix();
	m_pColliderCom->UpdateColiision(XMLoadFloat4x4(&m_CombinedWorldMatrix));
}

void CBullet_Droid::Late_Update(_float fTimeDelta)
{
	if (m_pGameInstance->isIn_DistanceFrustum(m_pTransformCom->Get_State(STATE::POSITION), 200.f))
	{
#ifdef _DEBUG
		m_pGameInstance->Add_DebugComponent(m_pColliderCom);
#endif // _DEBUG

		if (false == m_isDead)
			m_pGameInstance->ADD_Collider(m_pColliderCom);
	}
	else
	{
		//m_pEffect->Set_Dead(true);
		Set_Dead(true);
	}
}

HRESULT CBullet_Droid::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	return S_OK;
}

void CBullet_Droid::Shoot_Projectile(_vector vTargetPoint, _float fSpeed)
{
	Update_BulletCombinedMatrix();
	_matrix CombinedMatrix = XMLoadFloat4x4(&m_CombinedWorldMatrix);

	_vector vCenterPos = (vTargetPoint + CombinedMatrix.r[3]) * 0.5f;
	vCenterPos.m128_f32[1] += 30.f;
	switch (m_eBulletType)
	{
	case BULLET_TYPE::PROJECTILE:
	{
		_vector vLook = m_pParent->GetTransform()->Get_State(STATE::LOOK);
		_vector vStartPos = CombinedMatrix.r[3];

		m_pTransformCom->Set_State(STATE::POSITION, vStartPos);
		vTargetPoint.m128_f32[1] += 3.f;
		XMStoreFloat3(&m_vProjectileDir, XMVector3Normalize(vTargetPoint - vStartPos));
		m_fSpeed = fSpeed;
	}
	break;
	case BULLET_TYPE::ARCING:
	{
		XMStoreFloat3(&m_PointLists[0], CombinedMatrix.r[3]);
		XMStoreFloat3(&m_PointLists[1], (vCenterPos + CombinedMatrix.r[3]) * 0.5f);
		XMStoreFloat3(&m_PointLists[2], vCenterPos);
		XMStoreFloat3(&m_PointLists[3], (vCenterPos + vTargetPoint) * 0.5f);
		XMStoreFloat3(&m_PointLists[4], vTargetPoint);
	}
	break;
	}
	m_bIsAttachment = false;
}

HRESULT CBullet_Droid::ADD_Components(BULLET_DESC& pDesc)
{
	COBBCollider::OBB_COLLIDER_DESC pOBBDesc = {};
	pOBBDesc.vSize = { 1.f, 1.f, 1.f };

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

HRESULT CBullet_Droid::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
		return E_FAIL;

	return S_OK;
}

void CBullet_Droid::Begin_OverlapEvent(_float3 vHitPoint, _float3 vHitDir, CGameObject* pHitActor)
{
	// 여기서 일단 데미지 처리한다
	// 맞으면 맞는 모션 나오게
	DEFAULT_DAMAGE_DESC pDamageDesc = {};
	pDamageDesc.pAttacker = m_pParent;
	pDamageDesc.vHitPoint = vHitPoint;
	pDamageDesc.vHitDir = vHitDir;
	pDamageDesc.pSkillData = m_pSkillData;

	auto pCharacter = dynamic_cast<CCharacter*>(pHitActor);
	if (pCharacter)
		pCharacter->Damaged(&pDamageDesc);

	if (false == ReflectBullet(pHitActor))
	{
		//m_pEffect->Set_Dead(true);
		Set_Dead(true);
	}
}

CBullet_Droid* CBullet_Droid::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBullet_Droid* pBullet_Droid = new CBullet_Droid(pDevice, pContext);
	if (FAILED(pBullet_Droid->Initialize_Prototype()))
	{
		Safe_Release(pBullet_Droid);
		MSG_BOX("Create Fail : Bullet Droid");
	}

	return pBullet_Droid;
}

CGameObject* CBullet_Droid::Clone(void* pArg)
{
	CBullet_Droid* pBullet_Droid = new CBullet_Droid(*this);
	if (FAILED(pBullet_Droid->Initialize(pArg)))
	{
		Safe_Release(pBullet_Droid);
		MSG_BOX("Clone Fail : Bullet Droid");
	}

	return pBullet_Droid;
}

void CBullet_Droid::Free()
{
	if (nullptr != m_pEffect) {
		m_pEffect->End();

		CEffect::EFFECT_TRANSFORM_DESC EffectDesc;
		EffectDesc.fRotationPerSec = 1.f;
		EffectDesc.fSpeedPerSec = 1.f;

		EffectDesc.pRootMatrix = nullptr;
		EffectDesc.pWorldMatrix = nullptr;
		_matrix CombinedMatrix = XMLoadFloat4x4(&m_CombinedWorldMatrix);
		EffectDesc.vPos = CombinedMatrix.r[3];
		EffectDesc.fRot = _float3(0, 0, 0);
		EffectDesc.fSize = 2.0f;
		EffectDesc.iFloor = 0;
		EffectDesc.pDir = &m_vProjectileDir;
		m_pGameInstance->Add_Get_GameObject(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Effect_Missile_Boom"), ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_Effect"), &EffectDesc);
	}
	__super::Free();
}
