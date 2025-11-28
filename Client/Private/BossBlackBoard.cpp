#include "pch.h"
#include "BossBlackBoard.h"

#include "GameManager.h"

CBossBlackBoard::CBossBlackBoard() : CBlackBoard()
{
}

HRESULT CBossBlackBoard::Initialize(_uint iBossID)
{
    //여기서 데이터 찾아오자 일단 기가스 데이터만
    auto pGameManager = CGameManager::GetInstance();
    m_BossDefualtInfo = pGameManager->Find_BossData(iBossID);

    //for (auto& iter : m_BossDefualtInfo->iAttackList)
    //    m_BossCurrentInfo.iAttackList.push_back(pGameManager->Find_SkillData(iter));
   
    m_BossCurrentInfo.iCurrentHealth = m_BossDefualtInfo->iMaxHealth;
    m_BossCurrentInfo.iCurrentShield = m_BossDefualtInfo->iMaxShield;
    m_BossCurrentInfo.iCurrentPhase = 1;

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
