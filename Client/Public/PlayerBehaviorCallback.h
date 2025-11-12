#pragma once

#include "Client_Defines.h"
#include "CCTBehaviorCallback.h"

NS_BEGIN(Engine)
class CCharacterController;
NS_END

/*
이 객체 건드리고싶으면 서민석한테 디코 ㄱㄱ

행동 콜백이라는데 HitReporter랑 큰 차이 없어보여요.
*/

NS_BEGIN(Client)

class CPlayerBehaviorCallback final : public CCTBehaviorCallback
{
public:
	CPlayerBehaviorCallback();
	virtual ~CPlayerBehaviorCallback() = default;

public:
	virtual PxControllerBehaviorFlags getBehaviorFlags(const PxShape& PxShape, const PxActor& PxActor) override;
	virtual PxControllerBehaviorFlags getBehaviorFlags(const PxController& controller);
	virtual PxControllerBehaviorFlags getBehaviorFlags(const PxObstacle& obstacle);
public:
	static CPlayerBehaviorCallback* Create();
	virtual void Free() override;
};

NS_END