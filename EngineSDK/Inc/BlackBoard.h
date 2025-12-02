#pragma once
#include "Base.h"

// 블랙 보드는 변수를 세팅하는 클래스입니다.
// 상속 받아서 변수 정의해서 GetSet 만들어서 사용하는 형태임

NS_BEGIN(Engine)
class CGameObject;

class ENGINE_DLL CBlackBoard abstract : public CBase
{
protected :
	CBlackBoard();
	virtual ~CBlackBoard() = default;

public :
	virtual		HRESULT				Initialize(void* pArg) = 0;

	void							SetOwner(CGameObject* pOwner);
	CGameObject*					GetOwner() { return m_pOwner; }

protected :
	CGameObject*					m_pOwner = { nullptr };

public :
	virtual		void				Free() override; 
};
NS_END