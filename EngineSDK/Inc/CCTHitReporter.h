#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class ENGINE_DLL CCTHitReporter abstract : public CBase, public PxUserControllerHitReport
{
protected:
	CCTHitReporter();
	virtual ~CCTHitReporter() = default;

public:
	virtual void onShapeHit(const PxControllerShapeHit& hit) override {}
	virtual void onControllerHit(const PxControllersHit& hit) override {}
	virtual void onObstacleHit(const PxControllerObstacleHit& hit) override {}

public:
	void Set_Controller(PxController* pController, PxUserData tUserData) {
		m_pController = pController; 
		m_tUserData = tUserData;
	}

protected:
	PxController*	m_pController = { nullptr };
	PxUserData		m_tUserData = {};

public:
	virtual void Free() override;
};

NS_END