#include "pch.h"
#include "BossBlackBoard.h"

#include "GameManager.h"
#include "Nayitba.h"

CBossBlackBoard::CBossBlackBoard() : CBlackBoard()
{
}

HRESULT CBossBlackBoard::Initialize(void* pArg)
{
    BOSS_BLACKBOARD_DESC* pDesc = static_cast<BOSS_BLACKBOARD_DESC*>(pArg);
    m_pOwner = pDesc->pOwner;

    auto pOwner = static_cast<CNayitba*>(m_pOwner);
    m_BossDefualtInfo = pOwner->GetStaticMonsterData();
    m_BossCurrentInfo = &pOwner->GetMonsterData();

    return S_OK;
}

void CBossBlackBoard::SetTarget(CGameObject* pGameObject)
{
    m_pTarget = pGameObject;
}

void CBossBlackBoard::Free()
{
    __super::Free();
}
