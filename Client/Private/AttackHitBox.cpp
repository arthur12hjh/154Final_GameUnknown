#include "pch.h"
#include "AttackHitBox.h"

#include "GameInstance.h"
#include "Player.h"

CAttackHitBox::CAttackHitBox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) :
	CGameObject(pDevice, pContext)
{
}

CAttackHitBox::CAttackHitBox(const CAttackHitBox& Prototype) :
	CGameObject(Prototype)
{
}

HRESULT CAttackHitBox::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CAttackHitBox::Initialize(void* pArg)
{
	if(FAILED(__super::Initialize(pArg)))
		return E_FAIL;


	HIT_BOX_DESC* pHit_BoxDesc = static_cast<HIT_BOX_DESC*>(pArg);
	m_pAttacker = pHit_BoxDesc->pAttacker;
	m_pData = pHit_BoxDesc->pData;

	if (FAILED(Ready_Components(*pHit_BoxDesc)))
		return E_FAIL;

	return S_OK;
}

void CAttackHitBox::Priority_Update(_float fTimeDelta)
{
}

void CAttackHitBox::Update(_float fTimeDelta)
{
	if (!m_bIsDelayDead)
		m_pColliderCom->UpdateColiision(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));
	else
	{
		m_vDelayDead.x += fTimeDelta;
		if (m_vDelayDead.x > m_vDelayDead.y)
			Set_Dead(true);
	}
}

void CAttackHitBox::Late_Update(_float fTimeDelta)
{
	if (!m_bIsDelayDead)
	{
		m_pGameInstance->ADD_Collider(m_pColliderCom);
	}

#ifdef _DEBUG
	if (m_pGameInstance->isIn_WorldFrustum(m_pCullingCollider))
	{
		m_pGameInstance->Add_DebugComponent(m_pColliderCom);
	}
#endif // _DEBUG

}

HRESULT CAttackHitBox::Render()
{

	return S_OK;
}

HRESULT CAttackHitBox::Ready_Components(const HIT_BOX_DESC& pDesc)
{
	switch (pDesc.eColType)
	{
	case COLLIDER::AABB :
	{
		/* Com_Collider_AABB */
		CBoxCollider::BOX_COLLIDER_DESC		AABBDesc{};
		AABBDesc.vSize = pDesc.vScale;
		AABBDesc.vCenter = _float3(0.f, AABBDesc.vSize.y * 0.5f, 0.f);

		if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Collider_AABB"),
			TEXT("Com_Collider_AABB"), reinterpret_cast<CComponent**>(&m_pColliderCom), &AABBDesc)))
			return E_FAIL;
	}
		break;
	case COLLIDER::OBB:
	{
		/* Com_Collider_AABB */
		CSphereCollider::SPHERE_COLLIDER_DESC		SphereDesc{};
		SphereDesc.fRadius = pDesc.vScale.x;
		SphereDesc.vCenter = _float3(0.f, SphereDesc.fRadius * 0.5f, 0.f);

		if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Collider_Sphere"),
			TEXT("Com_Collider_OBB"), reinterpret_cast<CComponent**>(&m_pColliderCom), &SphereDesc)))
			return E_FAIL;
	}
		break;
	case COLLIDER::SPHERE:
	{
		/* Com_Collider_AABB */
		COBBCollider::OBB_COLLIDER_DESC		OBBDesc{};
		OBBDesc.vSize = pDesc.vScale;
		OBBDesc.vCenter = _float3(0.f, OBBDesc.vSize.y * 0.5f, 0.f);

		if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Collider_OBB"),
			TEXT("Com_Collider_Sphere"), reinterpret_cast<CComponent**>(&m_pColliderCom), &OBBDesc)))
			return E_FAIL;
	}
		break;
	}

	// 충돌 끝낫을때 이벤트는 사용해야하는 때가 오면 그때 만들게요
	m_pColliderCom->BindBeginOverlapEvent([&](_float3 vHitPoint, _float3 vHitDir, CGameObject* pHitActor) { Begin_OverlapEvent(vHitPoint, vHitDir, pHitActor); });
	m_pColliderCom->BindOverlappingEvent([&](_float3 vHitPoint, _float3 vHitDir, CGameObject* pHitActor) { OverlappingEvent(vHitPoint, vHitDir, pHitActor); });
	//m_pCollider->BindBeginOverlapEvent([&](_float3 vHitPoint, _float3 vHitDir, CGameObject* pHitActor) { Begin_OverlapEvent(vHitPoint, vHitDir, pHitActor); });

	m_pColliderCom->SetColliderHitType(pDesc.eHitBoxType);

	// 일단은 플레이어만 충돌처리 피직스 오브젝트 들어가면 그녀석들 충돌처리
	// 공통으로 할수있는 Tag 만들어서 하기
	m_pColliderCom->ADD_OnlyHitObject(pDesc.eHitObjectType);
	static_cast<COBBCollider*>(m_pCullingCollider)->SetCollision(_float3(0.f, pDesc.vScale.y * 0.5f, 0.f), {}, pDesc.vScale);
	return S_OK;
}

void CAttackHitBox::Begin_OverlapEvent(_float3 vHitPoint, _float3 vHitDir, CGameObject* pHitActor)
{
	auto pPlayer = dynamic_cast<CPlayer*>(pHitActor);
	if (nullptr == pPlayer)
		return;

	auto pDesc = static_cast<const CHARACTER_SKILL_DESC*>(m_pData);
	CHARACTER_SKILL_DESC TempDesc = *pDesc;
	DEFAULT_DAMAGE_DESC pDamageDesc = {};
	pDamageDesc.pAttacker = m_pAttacker;
	pDamageDesc.vHitDir = vHitDir;
	pDamageDesc.vHitPoint = vHitPoint;
	pDamageDesc.pSkillData = &TempDesc;

	pPlayer->Damaged(&pDamageDesc);
	m_vDelayDead = {0.f, 10.f};
	m_bIsDelayDead = true;
}

void CAttackHitBox::OverlappingEvent(_float3 vHitPoint, _float3 vHitDir, CGameObject* pHitActor)
{
	// 여기서 지속딜 같은거 나중에 추가할예정
}

void CAttackHitBox::End_OverlapEvent(_float3 vHitPoint, _float3 vHitDir, CGameObject* pHitActor)
{
	// 이건 거의 쓸모없을듯

}

CAttackHitBox* CAttackHitBox::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CAttackHitBox* pAttackHitBox = new CAttackHitBox(pDevice, pContext);
	if (FAILED(pAttackHitBox->Initialize_Prototype()))
	{
		Safe_Release(pAttackHitBox);
		MSG_BOX("Create Fail : Attack Hit Box");
	}
	return pAttackHitBox;
}

CGameObject* CAttackHitBox::Clone(void* pArg)
{
	CAttackHitBox* pAttackHitBox = new CAttackHitBox(*this);
	if (FAILED(pAttackHitBox->Initialize(pArg)))
	{
		Safe_Release(pAttackHitBox);
		MSG_BOX("Clone Fail : Attack Hit Box");
	}
	return pAttackHitBox;
}

void CAttackHitBox::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
}
