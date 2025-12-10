#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class DefaultCCTFilterCallback : public PxControllerFilterCallback, public CBase
{
private:
    DefaultCCTFilterCallback();
    virtual ~DefaultCCTFilterCallback() = default;
public:
    // 두 CCT가 충돌할 때 Step Up 가능 여부 / 충돌 허용 여부 반환
    virtual bool filter(const PxController& a, const PxController& b) override
    {
        // false → step-up 불가 및 special response 제거
        return false;
    }

private:

public:
    static DefaultCCTFilterCallback* Create();
    virtual void Free() override;
};

NS_END
