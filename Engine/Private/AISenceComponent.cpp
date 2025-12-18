#include "AISenceComponent.h"

#include "GameInstance.h"

#include "GameObject.h"
#include "ContainerObject.h"
#include "SphereCollider.h"

CAISenceComponent::CAISenceComponent(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) :
	CComponent(pDevice, pContext)
{
}

CAISenceComponent::CAISenceComponent(const CAISenceComponent& Prototype) :
	CComponent(Prototype)
{
}

HRESULT CAISenceComponent::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CAISenceComponent::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	AI_SENCE_COMPONENT_DESC* pSenceDesc = static_cast<AI_SENCE_COMPONENT_DESC*>(pArg);
	m_fAiSearchRadius = XMConvertToRadians(pSenceDesc->fAiSearchRadius);
	m_fAiTargetLostTime = pSenceDesc->m_fAiTargetLostTime;
	m_fAiTargetSearchDistance = pSenceDesc->fAiTargetSearchDistance;

	if (FAILED(ADD_Components()))
		return E_FAIL;

	return S_OK;
}

void CAISenceComponent::SetOwner(CGameObject* pGameObject)
{
	m_pOwner = pGameObject;
	m_pTargetSearchCol->SetOwner(m_pOwner);
}

void CAISenceComponent::UpdatSenceComponent(_float fDeletaTime)
{
	_vector vOwnerLook = m_pOwner->GetTransform()->Get_State(STATE::LOOK);
	_vector vOwnerPos = m_pOwner->GetTransform()->Get_State(STATE::POSITION);
	for (auto& pTarget : m_pCurSearchList)
	{
		_vector vTargetPos = pTarget->GetTransform()->Get_State(STATE::POSITION);

		// 방향 백터를 구한다. 이방향 백터와 Right를 내적해서 각도를 판별한다.
		_vector vDir = XMVector3Normalize(vTargetPos - vOwnerPos);
		_float fScalar = XMVectorGetX(XMVector3Dot(vOwnerLook, vDir));

		auto iter = m_pPreSearchList.find(pTarget);
		if (fScalar > 0 || m_fAiSearchRadius >= XM_2PI)
		{
			if(m_fAiSearchRadius > acosf(fScalar))
			{
				if (iter == m_pPreSearchList.end())
				{
					if (m_SearchFunc)
						m_SearchFunc(pTarget);

					m_pPreSearchList.emplace(pTarget, 0.f);
					m_pSearchList.push_back(pTarget);
				}
				else
				{
					if (m_TargetDetectedFunc)
						m_TargetDetectedFunc(pTarget);

					iter->second = 0.f;
				}
			}
		}
		else
		{
			if (iter != m_pPreSearchList.end())
			{
				iter->second += fDeletaTime;
				if (iter->second >= m_fAiTargetLostTime)
				{
					if (m_TargetLostFunc)
						m_TargetLostFunc(iter->first);

					auto pObject = find(m_pSearchList.begin(), m_pSearchList.end(), iter->first);
					m_pSearchList.erase(pObject);

					m_pPreSearchList.erase(iter);
				}
			}
		}
	}

	m_pCurSearchList.clear();
	_matrix OwnerWorldMatrix = XMLoadFloat4x4(m_pOwner->GetTransform()->Get_WorldMatrixPtr());
	m_pTargetSearchCol->UpdateColiision(OwnerWorldMatrix);

	m_pGameInstance->ADD_Collider(m_pTargetSearchCol);
}

void CAISenceComponent::Add_SenceTargetObject(CGameObject* pSenceObject)
{
	auto iter = m_pPreSearchList.find(pSenceObject);
	if (iter == m_pPreSearchList.end())
	{
		m_pPreSearchList.emplace(pSenceObject, 0.f);
		m_pSearchList.push_back(pSenceObject);
	}
	else
		return;

}

_bool CAISenceComponent::IsTagetEmpty()
{
	return m_pSearchList.empty();
}

#ifdef _DEBUG
void CAISenceComponent::Update_Debuge()
{
	m_pGameInstance->Add_DebugComponent(m_pTargetSearchCol);
}
#endif
void CAISenceComponent::Bind_TargetSearch(function<void(CGameObject*)> Func)
{
	m_SearchFunc = Func;
}

void CAISenceComponent::Bind_TargetLost(function<void(CGameObject*)> Func)
{
	m_TargetLostFunc = Func;
}

void CAISenceComponent::Bind_TargetDetected(function<void(CGameObject*)> Func)
{
	m_TargetDetectedFunc = Func;
}

void CAISenceComponent::SetTraceHitType(HIT_TYPE eHitType)
{
	m_pTargetSearchCol->SetColliderHitType(eHitType);
}

void CAISenceComponent::ADD_SenceIgnoreTraceObject(HIT_TYPE eHitType)
{
	m_pTargetSearchCol->ADD_IgnoreObjectType(eHitType);
}

void CAISenceComponent::ADD_SenceOnlyTraceObject(HIT_TYPE eHitType)
{
	m_pTargetSearchCol->ADD_OnlyHitObjectType(eHitType);
}

HRESULT CAISenceComponent::ADD_Components()
{
	CSphereCollider::SPHERE_COLLIDER_DESC SphereColDesc = {};
	SphereColDesc.fRadius = m_fAiTargetSearchDistance;
	SphereColDesc.vCenter = {0.f, SphereColDesc.fRadius * 0.5f, 0.f};

	m_pTargetSearchCol = CSphereCollider::Create(m_pDevice, m_pContext);
	if (nullptr == m_pTargetSearchCol)
		return E_FAIL;

	if (FAILED(m_pTargetSearchCol->Initialize(&SphereColDesc)))
		return E_FAIL;

	
	m_pTargetSearchCol->BindOverlappingEvent([&](_float3 vHitPoint, _float3 vHitDir, CGameObject* pHitActor) { OverlapEvent(vHitDir, pHitActor); });
	return S_OK;
}

void CAISenceComponent::OverlapEvent(_float3 vDir, CGameObject* pHitObject)
{
	// 이건 Hit 이벤트를 안받는게 최선인데
	// 받으면 무시
	if (pHitObject->GetTeam() == m_pOwner->GetTeam())
		return;

	auto Character = dynamic_cast<CContainerObject*>(pHitObject);
	if(Character)
		m_pCurSearchList.insert(pHitObject);



}

CAISenceComponent* CAISenceComponent::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CAISenceComponent* pAISenceComponent = new CAISenceComponent(pDevice, pContext);
	if (FAILED(pAISenceComponent->Initialize_Prototype()))
	{
		Safe_Release(pAISenceComponent);
		MSG_BOX("Create Fail : Sence Component");
	}
	return pAISenceComponent;
}

CComponent* CAISenceComponent::Clone(void* pArg)
{
	CAISenceComponent* pAISenceComponent = new CAISenceComponent(*this);
	if (FAILED(pAISenceComponent->Initialize(pArg)))
	{
		Safe_Release(pAISenceComponent);
		MSG_BOX("Clone Fail : Sence Component");
	}
	return pAISenceComponent;
}

void CAISenceComponent::Free()
{
	__super::Free();

	Safe_Release(m_pTargetSearchCol);
}
