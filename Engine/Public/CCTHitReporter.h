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
	void Set_Controller(PxController* pController, const _wstring& szControllerTag) {
		m_pController = pController; 
		m_szControllerTag = szControllerTag;
	}

protected:
	PxController* m_pController = { nullptr };
	_wstring	  m_szControllerTag = { TEXT("") };

public:
	virtual void Free() override;
};

NS_END