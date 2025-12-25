#include "pch.h"
#include "Task_Dead.h"

#include "BossBlackBoard.h"
#include "GorillaBehaviorTree.h"
#include "Nayitba.h"

CTask_Dead::CTask_Dead()
{
}

HRESULT CTask_Dead::Initialize_Prototype(CBehaviorTree* pOwnerTree)
{
    if (FAILED(__super::Initialize_Prototype(pOwnerTree)))
        return E_FAIL;

    if (nullptr == m_pOwner)
        m_pOwner = static_cast<CNayitba*>(m_pOwnerTree->GetOwner());

    if (nullptr == m_pBlackBoard)
        m_pBlackBoard = static_cast<CBossBlackBoard*>(m_pOwnerTree->GetBlackBoard());

    return S_OK;
}

CBehaviorNode::NODE_STATE CTask_Dead::Update(_float fTimeDelta)
{
    const NAYTIBA_DESC* CharacterInfo = m_pBlackBoard->GetBossInfo();
    const NAYTIBA_NETWORK_DESC* CharacterInitInfo = m_pBlackBoard->GetBossDefaultInfo();
    _float fPercent = (_float)CharacterInfo->iCurrentHealth / (_float)CharacterInitInfo->iMaxHealth;

    if (0.f >= fPercent)
    {
        // 애니메이션을 받아서 재생한다. 
        m_pOwner->Set_Animation("M_Finish_DeadLink");
        m_pOwner->Play_Animation(fTimeDelta);

        m_fDeadEndTime += fTimeDelta;
        if (!m_bIsDeadEffect && 0.5f <= m_fDeadEndTime)
        {
            m_pOwner->PlayDeadEffect();
            m_bIsDeadEffect = true;
        }
    }

    return NODE_STATE::COMPLETE;
}

CTask_Dead* CTask_Dead::Create(CBehaviorTree* pOwnerTree)
{
    CTask_Dead* pTask_Dead = new CTask_Dead();
    if (FAILED(pTask_Dead->Initialize_Prototype(pOwnerTree)))
    {
        Safe_Release(pTask_Dead);
        MSG_BOX("Create Fail : Task Dead");
    }
    return pTask_Dead;
}

void CTask_Dead::Free()
{
    __super::Free();

    Safe_Release(m_pBlackBoard);
}
