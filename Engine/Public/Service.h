#pragma once
#include "BehaviorNode.h"

NS_BEGIN(Engine)
class ENGINE_DLL CService abstract : public CBehaviorNode
{
protected:
	CService();
	virtual ~CService() = default;

public:
	virtual	HRESULT						Initialize_Prototype();
	virtual	HRESULT						Initialize(void* pArg);

	virtual	_bool						Update(_float fTimeDelta);

public:
	virtual	void						Free() override;
};
NS_END