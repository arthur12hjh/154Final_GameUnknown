#include "pch.h"
#include "TargetComponent.h"

#include "GameObject.h"

CTargetComponent::CTargetComponent(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) :
	CComponent(pDevice, pContext)
{
}

HRESULT CTargetComponent::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTargetComponent::Initialize(void* pArg)
{
	TARGET_COMPONENT_DESC* pDesc = static_cast<TARGET_COMPONENT_DESC*>(pArg);
	m_fRadius = pDesc->fRadius;
	m_iNumPoints = pDesc->iNumPoints;

	m_vPoints.resize(m_iNumPoints);
	return S_OK;
}

void CTargetComponent::Target_Search(const list<CGameObject*>* pList, function<_bool(CGameObject*, CGameObject*)> Func)
{
	if (0 >= pList->size())
		return;

	list<CGameObject *> TargetCandidate = *pList;
	if (Func)
	{
		TargetCandidate.sort(Func);
	}
	else
	{
		TargetCandidate.sort([&](CGameObject* pSrc, CGameObject* pDest)->_bool
			{
				_vector vOwnerPos = m_pOwner->GetTransform()->Get_State(STATE::POSITION);
				_vector vSrcPos = pSrc->GetTransform()->Get_State(STATE::POSITION);
				_vector vDestPos = pDest->GetTransform()->Get_State(STATE::POSITION);

				_float SrcDistance = XMVectorGetX(XMVector3Length(vOwnerPos - vSrcPos));
				_float DestDistance = XMVectorGetX(XMVector3Length(vOwnerPos - vDestPos));

				return SrcDistance < DestDistance;
			});
	}

	m_pTarget = TargetCandidate.front();
	_vector vTargetPos = m_pTarget->GetTransform()->Get_State(STATE::POSITION);
	_float fDegree = 360.f / m_iNumPoints;

	for (_uint i = 0; i < m_iNumPoints; ++i)
	{
		_float fRadian = XMConvertToRadians(fDegree * i);
		vTargetPos.m128_f32[0] += cosf(fRadian) * m_fRadius;
		vTargetPos.m128_f32[2] += sinf(fRadian) * m_fRadius;

		// 나중에 진돌이형이 만들어주면 여기서 Compute Hegiht 까지 하겠음
		XMStoreFloat3(&m_vPoints[i], vTargetPos);
	}
}

CTargetComponent* CTargetComponent::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTargetComponent* pTargetComponent = new CTargetComponent(pDevice, pContext);
	if (FAILED(pTargetComponent->Initialize_Prototype()))
	{
		Safe_Release(pTargetComponent);
		MSG_BOX("Create Fail : Target Component");
	}
	return pTargetComponent;
}

CComponent* CTargetComponent::Clone(void* pArg)
{
	CTargetComponent* pTargetComponent = new CTargetComponent(*this);
	if (FAILED(pTargetComponent->Initialize(pArg)))
	{
		Safe_Release(pTargetComponent);
		MSG_BOX("Clone Fail : Target Component");
	}
	return pTargetComponent;
}

void CTargetComponent::Free()
{
	__super::Free();
}