#include "CollisionManager.h"
#include "Collider.h"

CCollisionManager::CCollisionManager()
{
}

void CCollisionManager::ADD_CollisionList(CCollider* pObject)
{
	auto iter = find(m_ColList.begin(), m_ColList.end(), pObject);

	if (iter == m_ColList.end())
	{
		m_ColList.push_back(pObject);
		Safe_AddRef(pObject);
	}
}

void CCollisionManager::Compute_Collision()
{
	m_ColHash.clear();
	for (auto& pSrc : m_ColList)
	{
		for (auto& pDest : m_ColList)
		{
			if (pSrc == pDest)				
				continue;

			if (pSrc->GetOwner() == pDest->GetOwner())
				continue;

			if (m_ColHash.find(make_pair(pSrc, pDest)) != m_ColHash.end())
				continue;

			if (pSrc->Intersect(pDest->GetCollisionType(), pDest) &&
				pDest->Intersect(pSrc->GetCollisionType(), pSrc))
			{
				pSrc->ADD_HitObejct(pDest->GetOwner());
				pDest->ADD_HitObejct(pSrc->GetOwner());
				m_ColHash.insert(make_pair(pSrc, pDest));
			}
		}
	}

	for (auto& iter : m_ColList)
	{
		iter->CallFunction();
		Safe_Release(iter);
	}

	m_ColList.clear();
}

CCollisionManager* CCollisionManager::Create()
{
	return new CCollisionManager();
}

void CCollisionManager::Free()
{
	for (auto& iter : m_ColList)
		Safe_Release(iter);

	m_ColList.clear();
}
