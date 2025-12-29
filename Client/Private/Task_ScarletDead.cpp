#include "pch.h"
#include "Task_ScarletDead.h"

#include "BossBlackBoard.h"
#include "GorillaBehaviorTree.h"
#include "Nayitba.h"

CTask_ScarletDead::CTask_ScarletDead()
{
}

HRESULT CTask_ScarletDead::Initialize_Prototype(CBehaviorTree* pOwnerTree)
{
    if (FAILED(__super::Initialize_Prototype(pOwnerTree)))
        return E_FAIL;

    if (nullptr == m_pOwner)
        m_pOwner = static_cast<CNaytiba*>(m_pOwnerTree->GetOwner());

    if (nullptr == m_pBlackBoard)
        m_pBlackBoard = static_cast<CBossBlackBoard*>(m_pOwnerTree->GetBlackBoard());

    return S_OK;
}

CBehaviorNode::NODE_STATE CTask_ScarletDead::Update(_float fTimeDelta)
{
    // 애니메이션을 받아서 재생한다. 
    m_pOwner->Set_Animation("M_Scarlet_BattleIdle01");
    m_pOwner->Play_Animation(fTimeDelta);
    
    if (false == m_bIsDeadEffect)
    {
        MSG_BOX("Play Cut Scene");
        m_bIsDeadEffect = true;
    }

    return NODE_STATE::COMPLETE;
}

CTask_ScarletDead* CTask_ScarletDead::Create(CBehaviorTree* pOwnerTree)
{
    CTask_ScarletDead* pTask_ScarletDead = new CTask_ScarletDead();
    if (FAILED(pTask_ScarletDead->Initialize_Prototype(pOwnerTree)))
    {
        Safe_Release(pTask_ScarletDead);
        MSG_BOX("Create Fail : Task Scarlet Dead");
    }
    return pTask_ScarletDead;
}

void CTask_ScarletDead::Free()
{
    __super::Free();

    Safe_Release(m_pBlackBoard);
}
