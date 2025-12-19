#include "pch.h"
#include "TriggerBox.h"

#include "GameInstance.h"
#include "GameManager.h"
#include "Player.h"

CTriggerBox::CTriggerBox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CTriggerBox::CTriggerBox(const CTriggerBox& Prototype)
	: CGameObject(Prototype)
{
}

HRESULT CTriggerBox::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTriggerBox::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	TRIGGER_BOX_DESC* pDesc = static_cast<TRIGGER_BOX_DESC*>(pArg);
	m_pTransformCom->Set_Scale(XMLoadFloat3(&pDesc->vScale));
	m_pTransformCom->Set_State(STATE::POSITION, XMLoadFloat3(&pDesc->vPosition));
	m_pTransformCom->Rotation(pDesc->vRotation.x, pDesc->vRotation.y, pDesc->vRotation.z);

	m_iTriggerCode = pDesc->iTriggerCode;
	m_fDelayTime = pDesc->fDelayTime;

	if (FAILED(Ready_Components(*pDesc)))
		return E_FAIL;

	m_pCullingCollider->UpdateColiision(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));
	
	return S_OK;
}

void CTriggerBox::Priority_Update(_float fTimeDelta)
{
}

void CTriggerBox::Update(_float fTimeDelta)
{
	_matrix WorldMatrix = XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr());

	m_pCullingCollider->UpdateColiision(WorldMatrix);
	m_pColliderCom->UpdateColiision(WorldMatrix);
}

void CTriggerBox::Late_Update(_float fTimeDelta)
{
	if (!m_isDead)
		m_pGameInstance->ADD_Collider(m_pColliderCom);

#ifdef _DEBUG
	m_pGameInstance->Add_DebugComponent(m_pColliderCom);
#endif // _DEBUG
}

HRESULT CTriggerBox::Render()
{
	return S_OK;
}

HRESULT CTriggerBox::Ready_Components(const TRIGGER_BOX_DESC& pDesc)
{
	switch (pDesc.eColType)
	{
	case COLLIDER::AABB:
	{
		/* Com_Collider_AABB */
		CBoxCollider::BOX_COLLIDER_DESC		AABBDesc{};
		AABBDesc.vSize = pDesc.vScale;
		AABBDesc.vCenter = _float3(0.f, AABBDesc.vSize.y, 0.f);

		if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Collider_AABB"),
			TEXT("Com_Collider_AABB"), reinterpret_cast<CComponent**>(&m_pColliderCom), &AABBDesc)))
			return E_FAIL;
	}
	break;
	case COLLIDER::OBB:
	{
		/* Com_Collider_AABB */
		COBBCollider::OBB_COLLIDER_DESC		OBBDesc{};
		OBBDesc.vSize = pDesc.vScale;
		OBBDesc.vCenter = _float3(0.f, OBBDesc.vSize.y, 0.f);

		if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Collider_OBB"),
			TEXT("Com_Collider_OBB"), reinterpret_cast<CComponent**>(&m_pColliderCom), &OBBDesc)))
			return E_FAIL;
	}
	break;
	case COLLIDER::SPHERE:
	{
		/* Com_Collider_Sphere */
		CSphereCollider::SPHERE_COLLIDER_DESC		SphereDesc{};
		SphereDesc.fRadius = pDesc.vScale.x;
		SphereDesc.vCenter = _float3(0.f, SphereDesc.fRadius, 0.f);

		if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Collider_Sphere"),
			TEXT("Com_Collider_Sphere"), reinterpret_cast<CComponent**>(&m_pColliderCom), &SphereDesc)))
			return E_FAIL;
	}
	break;
	}

	// 충돌 끝낫을때 이벤트는 사용해야하는 때가 오면 그때 만들게요
	m_pColliderCom->BindBeginOverlapEvent([&](_float3 vHitPoint, _float3 vHitDir, CGameObject* pHitActor) { Begin_OverlapEvent(vHitPoint, vHitDir, pHitActor); });
	m_pColliderCom->BindOverlappingEvent([&](_float3 vHitPoint, _float3 vHitDir, CGameObject* pHitActor) { OverlappingEvent(vHitPoint, vHitDir, pHitActor); });
	//m_pCollider->BindBeginOverlapEvent([&](_float3 vHitPoint, _float3 vHitDir, CGameObject* pHitActor) { Begin_OverlapEvent(vHitPoint, vHitDir, pHitActor); });

	m_pColliderCom->SetColliderHitType(HIT_TYPE::STATIC);

	// 일단은 플레이어만 충돌처리 피직스 오브젝트 들어가면 그녀석들 충돌처리
	// 공통으로 할수있는 Tag 만들어서 하기
	m_pColliderCom->ADD_IgnoreObjectType(HIT_TYPE::STATIC);
	m_pColliderCom->ADD_IgnoreObjectType(HIT_TYPE::INTERACTION);
	m_pColliderCom->ADD_IgnoreObjectType(HIT_TYPE::SENCE);
	static_cast<COBBCollider*>(m_pCullingCollider)->SetCollision(_float3(0.f, pDesc.vScale.y, 0.f), {}, pDesc.vScale);
	return S_OK;
}

void CTriggerBox::Begin_OverlapEvent(_float3 vHitPoint, _float3 vHitDir, CGameObject* pHitActor)
{
	auto pCharacter = dynamic_cast<CCharacter*>(pHitActor);
	if (nullptr == pCharacter)
		return;

	if (m_iTriggerCode)
	{
		// 게임매니저에서 시네마틱 실행
 		CGameManager::GetInstance()->Play_Cinematic(static_cast<_uint>(m_iTriggerCode));
	}
		
	m_isDead = true;
}

void CTriggerBox::OverlappingEvent(_float3 vHitPoint, _float3 vHitDir, CGameObject* pHitActor)
{
}

void CTriggerBox::End_OverlapEvent(_float3 vHitPoint, _float3 vHitDir, CGameObject* pHitActor)
{
}

CTriggerBox* CTriggerBox::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTriggerBox* pTriggerBox = new CTriggerBox(pDevice, pContext);
	if (FAILED(pTriggerBox->Initialize_Prototype()))
	{
		Safe_Release(pTriggerBox);
		MSG_BOX("Create Failed : TriggerBox");
	}
	return pTriggerBox;
}

CGameObject* CTriggerBox::Clone(void* pArg)
{
	CTriggerBox* pTriggerBox = new CTriggerBox(*this);
	if (FAILED(pTriggerBox->Initialize(pArg)))
	{
		Safe_Release(pTriggerBox);
		MSG_BOX("Clone Failed : TriggerBox");
	}
	return pTriggerBox;
}

void CTriggerBox::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
}

/*
738.660, 2.022, 608.569
5,5,5
*/