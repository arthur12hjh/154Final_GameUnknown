#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class ENGINE_DLL CControllerHitReporter abstract : public CBase, PxUserControllerHitReport
{
private:
	CControllerHitReporter(PxController* pController);
	virtual ~CControllerHitReporter() = default;

public:
	virtual void onShapeHit(const PxControllerShapeHit& hit) override {}
	virtual void onControllerHit(const PxControllersHit& hit) override {}
	virtual void onObstacleHit(const PxControllerObstacleHit& hit) override {}

private:
	PxController* m_pController = { nullptr };
	_wstring	  m_strControllerTag = { TEXT("") };

public:
	virtual void Free() override;
};

NS_END