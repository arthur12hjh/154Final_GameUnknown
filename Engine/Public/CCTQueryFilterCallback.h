#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class ENGINE_DLL CCTQueryFilterCallback abstract : public PxQueryFilterCallback, public CBase
{
protected:
    CCTQueryFilterCallback();
    virtual ~CCTQueryFilterCallback() = default;

public:
    virtual PxQueryHitType::Enum preFilter(const PxFilterData& filterData, const PxShape* shape,const PxRigidActor* actor, PxHitFlags& queryFlags) override = 0;
    virtual PxQueryHitType::Enum postFilter(const PxFilterData& filterData, const PxQueryHit& hit, const PxShape* shape, const PxRigidActor* actor) override = 0;

public:
    virtual void Free();
};

NS_END