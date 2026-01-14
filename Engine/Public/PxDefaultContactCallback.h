#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class PxDefaultContactCallback : public PxSimulationEventCallback, public CBase
{
private:
    PxDefaultContactCallback();
    virtual ~PxDefaultContactCallback() = default;

public:
    virtual void onContact(const PxContactPairHeader& header, const PxContactPair* pairs, PxU32 nbPairs) override
    {
        PxActor* Sour = header.actors[0];
        PxActor* Dest = header.actors[1];

        if (nullptr == Sour->userData || nullptr == Dest->userData)
            return;

        PxU32 SourGroup = *static_cast<PxU32*>(static_cast<PxUserData*>(Sour->userData)->pWord0);
        PxU32 DestGroup = *static_cast<PxU32*>(static_cast<PxUserData*>(Dest->userData)->pWord0);

        if ((SourGroup == PHYSX_CUSTOM_3 && DestGroup == PHYSX_TERRAIN) ||
            (SourGroup == PHYSX_TERRAIN && DestGroup == PHYSX_CUSTOM_3))
            onHitTerrain(dynamic_cast<PxRigidDynamic*>(Dest));

        if ((SourGroup == PHYSX_CUSTOM_6 && DestGroup == PHYSX_CUSTOM_3) ||
            (SourGroup == PHYSX_CUSTOM_3 && DestGroup == PHYSX_CUSTOM_6))
            onHitTerrain(dynamic_cast<PxRigidDynamic*>(Dest));
    }

    virtual void onAdvance(const PxRigidBody* const* bodyBuffer, const PxTransform* poseBuffer, PxU32 count) override {}
    void onConstraintBreak(PxConstraintInfo*, PxU32) override {}
    virtual void onTrigger(PxTriggerPair* pairs, PxU32 count) override {};
    virtual void onSleep(PxActor** actors, PxU32 count) override {};
    virtual void onWake(PxActor** actors, PxU32 count) override {};

private:
    void onHitTerrain(PxRigidDynamic* DynamicActor)
    {
        if (nullptr == DynamicActor)
            return;

        DynamicActor->putToSleep();
    }

public:
    static PxDefaultContactCallback* Create();
    virtual void Free() override;
};

NS_END