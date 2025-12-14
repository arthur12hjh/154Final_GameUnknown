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
        PxUserData* SourData = static_cast<PxUserData*>(Sour.getUserData());
        PxUserData* DestData = static_cast<PxUserData*>(Dest.getUserData());
    
        // userData가 없으면 기본은 충돌로 두는 편이 안전
        if (nullptr == SourData || nullptr == DestData) 
            return true;

        // 둘 다 active일 때만 충돌. 하나라도 inactive면 서로 통과.
        return SourData->isActive && DestData->isActive;
    }

private:

public:
    static CCTFilterCallback* Create();
    virtual void Free() override;
};

NS_END
