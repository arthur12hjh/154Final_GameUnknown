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
	PxVec3 Get_HitPosition() { return m_vHitPos; }

public:
	void Set_Controller(class CCharacterController* pCCT, PxController* pController, PxUserData tUserData) {
		m_pCCT = pCCT;
		m_pController = pController; 
		m_tUserData = tUserData;
	}

protected:
	class CCharacterController* m_pCCT = { nullptr };
	PxController*	m_pController = { nullptr };
	PxUserData		m_tUserData = {};
	PxVec3			m_vHitPos = {};

public:
	virtual void Free() override;
};

NS_END