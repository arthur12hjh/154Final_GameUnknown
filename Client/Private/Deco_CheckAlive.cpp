#include "pch.h"
#include "Deco_CheckAlive.h"

#include "BossBlackBoard.h"
#include "BehaviorTree.h"

CDeco_CheckAlive::CDeco_CheckAlive()
{
}

HRESULT CDeco_CheckAlive::Initialize_Prototype(CBehaviorTree* pOwnerTree, _float fDeadPercent)
{
    if (FAILED(__super::Initialize_Prototype(pOwnerTree)))
        return E_FAIL;

    m_pBlackBoard = static_cast<CBossBlackBoard*>(m_pOwnerTree->GetBlackBoard());
    m_fDeadPercent = fDeadPercent / 100.f;
    return S_OK;
}

CBehaviorNode::NODE_STATE CDeco_CheckAlive::Update(_float fTimeDelta)
{
    auto CurrentState = m_pBlackBoard->GetCurState();
    const NAYTIBA_DESC* CharacterInfo = m_pBlackBoard->GetBossInfo();
    const NAYTIBA_NETWORK_DESC* CharacterInitInfo = m_pBlackBoard->GetBossDefaultInfo();

    _float fPercent = (_float)CharacterInfo->iCurrentHealth / (_float)CharacterInitInfo->iMaxHealth;
    if (m_fDeadPercent < fPercent && CBossBlackBoard::BOSS_STATE::DEAD != CurrentState)
        return NODE_STATE::COMPLETE;

    return NODE_STATE::FAIL;
}

CDeco_CheckAlive* CDeco_CheckAlive::Create(CBehaviorTree* pOwnerTree, _float fDeadPercent)
{
    CDeco_CheckAlive* pAliveDescorator = new CDeco_CheckAlive();
    if (FAILED(pAliveDescorator->Initialize_Prototype(pOwnerTree, fDeadPercent)))
    {
        Safe_Release(pAliveDescorator);
        MSG_BOX("Create Fail : Alive Descorator");
    }
    return pAliveDescorator;
}

void CDeco_CheckAlive::Free()
{
    __super::Free();

    Safe_Release(m_pBlackBoard);
}
