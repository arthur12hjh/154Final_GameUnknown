#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class ENGINE_DLL CCTBehaviorCallback abstract : public CBase, public PxControllerBehaviorCallback
{
protected:
	CCTBehaviorCallback();
	virtual ~CCTBehaviorCallback() = default;

public:
	virtual PxControllerBehaviorFlags getBehaviorFlags(const PxShape& PxShape, const PxActor& PxActor) override;
	virtual PxControllerBehaviorFlags getBehaviorFlags(const PxController& controller);
	virtual PxControllerBehaviorFlags getBehaviorFlags(const PxObstacle& obstacle);

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