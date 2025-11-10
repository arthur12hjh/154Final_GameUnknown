#pragma once

#include "Client_Defines.h"
#include "CCTHitReporter.h"

NS_BEGIN(Engine)
class CCharacterController;
NS_END

/*
이 객체 건드리고싶으면 서민석한테 디코 ㄱㄱ
*/

NS_BEGIN(Client)

class CPlayerCCTHitReporter final : public CCTHitReporter
{
public:
	CPlayerCCTHitReporter();
	virtual ~CPlayerCCTHitReporter() = default;

public:
    // CCT가 Shape와 충돌했을 때 호출
	virtual void onShapeHit(const PxControllerShapeHit& hit) override;
    virtual void onControllerHit(const PxControllersHit& hit) override;
    virtual void onObstacleHit(const PxControllerObstacleHit& hit) override;

public:
	static CPlayerCCTHitReporter* Create();
	virtual void Free() override;
};

NS_END