#pragma once

#include "Client_Defines.h"
#include "CCTQueryFilterCallback.h"

NS_BEGIN(Client)

class CPlayerCCTQueryFilterCallback : public CCTQueryFilterCallback
{
private:
	CPlayerCCTQueryFilterCallback();
	virtual ~CPlayerCCTQueryFilterCallback() = default;

public:
	PxQueryHitType::Enum preFilter(const PxFilterData& filterData, const PxShape* shape, const PxRigidActor* actor, PxHitFlags& queryFlags) override
	{
		PxUserData* pUserData = static_cast<PxUserData*>(actor->userData);

		if (nullptr != pUserData && pUserData->szActorTag == TEXT("Non_Collidable"))
			return PxQueryHitType::eNONE;   // 완전 무시

		return PxQueryHitType::eBLOCK;
	}

    virtual PxQueryHitType::Enum postFilter(const PxFilterData& filterData, const PxQueryHit& hit, const PxShape* shape, const PxRigidActor* actor) override
    {
		PxUserData* pUserData = static_cast<PxUserData*>(actor->userData);

		if (nullptr != pUserData && pUserData->szActorTag == TEXT("Non_Collidable"))
			return PxQueryHitType::eNONE;   // 완전 무시

		return PxQueryHitType::eBLOCK;
    }


public:
	static CPlayerCCTQueryFilterCallback* Create();
	virtual void Free() override;
};

NS_END
