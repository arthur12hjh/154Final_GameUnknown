#include "pch.h"
#include "MonsterIdleState.h"

#include "GameInstance.h"
#include "GameStruct.h"
#include "Nayitba.h"

CMonsterIdleState::CMonsterIdleState()
{
}

HRESULT CMonsterIdleState::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    return S_OK;
}

void CMonsterIdleState::Start(void* pArg)
{
}

void CMonsterIdleState::Update(_float fTimeDelta)
{
    auto pEntity = static_cast<CNayitba*>(m_pOwner);
    string AnimationName = pEntity->GetStaticMonsterData()->szAnimationName;
    AnimationName += "_Idle01";

    pEntity->Set_Animation(AnimationName.c_str());
    pEntity->Play_Animation(fTimeDelta);
}

void CMonsterIdleState::End()
{
}

CMonsterIdleState* CMonsterIdleState::Create(void* pArg)
{
    CMonsterIdleState* pMonsterIdleState = new CMonsterIdleState();
    if (FAILED(pMonsterIdleState->Initialize(pArg)))
    {
        Safe_Release(pMonsterIdleState);
        MSG_BOX("Create Fail Monster Idle State");
    }
    return pMonsterIdleState;
}

void CMonsterIdleState::Free()
{
    __super::Free();
}
