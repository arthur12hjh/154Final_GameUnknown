#pragma once

/* 레퍼런스 카운트의 기능을 자식객체에게 부여해주기 위해서. */
/* 레퍼런스 카운트를 증가시키낟. */
/* 레퍼런스 카운트를 감소시킨다. or 삭젷나다. */
#include "Engine_Defines.h"

NS_BEGIN(Engine)

class ENGINE_DLL CBase abstract
{
protected:
	CBase();
	virtual ~CBase() = default;

public:
	/* 레퍼런스 카운트를 증가시킨다.  */
	/* 리턴 : 증가시킨 결과 값. */
	_uint AddRef();

	/* 레퍼런스 카운트를 감소시킨다. or 삭젷나다. */
	/* 리턴 : 감소시키기 이전의 값. */
	_uint Release();

private:
	_uint		m_iRefCnt = {};

public:
	virtual void Free();
};

NS_END



