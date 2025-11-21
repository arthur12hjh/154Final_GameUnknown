#include "pch.h"
#include "MonsterHitState.h"

CMonsterHitState::CMonsterHitState() :
    CState()
{
}

HRESULT CMonsterHitState::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    return S_OK;
}

void CMonsterHitState::Start(void* pArg)
{
}

void CMonsterHitState::Update(_float fTimeDelta)
{
    //맞으면 여기서 들어온 스킬 따라서 분기해서 하기
}

void CMonsterHitState::End()
{
}

CMonsterHitState* CMonsterHitState::Create(void* pArg)
{
    CMonsterHitState* pMonsterHitState = new CMonsterHitState();
    if (FAILED(pMonsterHitState->Initialize(pArg)))
    {
        Safe_Release(pMonsterHitState);
        MSG_BOX("Create Fail : Monster Hit State");
    }
    return pMonsterHitState;
}

void CMonsterHitState::Free()
{
    __super::Free();
}
