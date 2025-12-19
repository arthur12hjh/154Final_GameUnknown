#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class CCTFilterCallback : public PxControllerFilterCallback, public CBase
{
private:
    CCTFilterCallback();
    virtual ~CCTFilterCallback() = default;
public:
    // 두 CCT가 충돌할 때 Step Up 가능 여부 / 충돌 허용 여부 반환
    virtual bool filter(const PxController& Sour, const PxController& Dest) override
    {
        PxUserData* pSourData = static_cast<PxUserData*>(Sour.getUserData());
        PxUserData* pDestData = static_cast<PxUserData*>(Dest.getUserData());
    
        // userData가 없으면 기본은 충돌
        if (nullptr == pSourData || nullptr == pDestData)
            return true;

        // 둘중 하나라도 CCT간 충돌이 꺼져있다면 false 반환
        if (false == pSourData->isCollisionCCTToCCT || false == pDestData->isCollisionCCTToCCT)
            return false;

        // 둘 다 active일 때만 충돌. 하나라도 inactive면 서로 통과.
        return pSourData->isActive && pDestData->isActive;
    }

private:

public:
    static CCTFilterCallback* Create();
    virtual void Free() override;
};

NS_END
