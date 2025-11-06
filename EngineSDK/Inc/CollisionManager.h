#pragma once
#include "Base.h"

NS_BEGIN(Engine)
class CCollider;

typedef struct PairHash
{
	size_t operator () (pair<CCollider*, CCollider*> const& v) const {

		CCollider* MinData = min(v.first, v.second);
		CCollider* MaxData = max(v.first, v.second);

		return hash<CCollider*>()(MinData) ^ (hash<CCollider*>()(MaxData) << 1 );
	}
}PAIR_HASH;

typedef struct ColCompare
{
	_bool operator() (const pair<CCollider*, CCollider*>& Src, const pair<CCollider*, CCollider*>& Dest) const
	{
		if ((Src.first == Dest.first && Src.second == Dest.second) ||
			(Src.second == Dest.first && Src.first == Dest.second))
			return true;

		return false;
	}

}COL_COMPARE;


class CCollisionManager final : public CBase
{
private:
	CCollisionManager();
	virtual ~CCollisionManager() = default;

public:
	void							ADD_CollisionList(CCollider* pObject);
	void							Compute_Collision();

private:
	unordered_set<pair<CCollider*, CCollider*>, PAIR_HASH, COL_COMPARE>	m_ColHash;
	list<CCollider*>														m_ColList = {};

public :
	static		CCollisionManager*	Create();
	virtual		void				Free() override;

};
NS_END