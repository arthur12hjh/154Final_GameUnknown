#pragma once

#include "Base.h"

NS_BEGIN(Engine)

PxFilterFlags MyFilterShader(
    PxFilterObjectAttributes attributes0, PxFilterData filterData0,
    PxFilterObjectAttributes attributes1, PxFilterData filterData1,
    PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize)
{
    //  Group/Mask 검사
    // (A가 B를 원하거나) OR (B가 A를 원하면) 충돌
    _bool SourCollision = (filterData0.word1 & filterData1.word0) != 0;
    _bool DestCollision = (filterData1.word1 & filterData0.word0) != 0;

    // 둘 다 충돌을 원하지 않으면 무시 (AND 조건이 아니라 OR 조건으로 통과시킴)
    if (!SourCollision || !DestCollision)
        return PxFilterFlag::eKILL;

    // Trigger 검사 
    // 두 물체 중 하나라도 Trigger라면 물리적 반발(SOLVE)을 끄고 트리거 이벤트만 켭니다.
    if (PxFilterObjectIsTrigger(attributes0) || PxFilterObjectIsTrigger(attributes1))
    {
        pairFlags = PxPairFlag::eTRIGGER_DEFAULT;
        // eTRIGGER_DEFAULT는 보통 eDETECT_DISCRETE_CONTACT | eNOTIFY_TOUCH_FOUND | eNOTIFY_TOUCH_LOST 를 포함함
        return PxFilterFlag::eDEFAULT;
    }

    // 일반 물리 충돌 (Solid vs Solid). 
    // 기본 물리 반발력 + 충돌 이벤트(Notify) 발생
    pairFlags = PxPairFlag::eCONTACT_DEFAULT;

    pairFlags |= PxPairFlag::eNOTIFY_TOUCH_FOUND;
    // 필요에 따라 eNOTIFY_TOUCH_PERSISTS, eNOTIFY_TOUCH_LOST, eNOTIFY_TOUCH_CCD 등 추가

    return PxFilterFlag::eDEFAULT;
}

NS_END