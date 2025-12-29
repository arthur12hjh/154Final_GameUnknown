#include "pch.h"
#include "Task_Theshold.h"

#include "BossBlackBoard.h"
#include "GorillaBehaviorTree.h"
#include "Nayitba.h"

CTask_Theshold::CTask_Theshold()
{
}

HRESULT CTask_Theshold::Initialize_Prototype(CBehaviorTree* pOwnerTree)
{
    if (FAILED(__super::Initialize_Prototype(pOwnerTree)))
        return E_FAIL;

    if (nullptr == m_pOwner)
        m_pOwner = static_cast<CNaytiba*>(m_pOwnerTree->GetOwner());

    if (nullptr == m_pBlackBoard)
        m_pBlackBoard = static_cast<CBossBlackBoard*>(m_pOwnerTree->GetBlackBoard());

    return S_OK;
}

CBehaviorNode::NODE_STATE CTask_Theshold::Update(_float fTimeDelta)
{
    if (10 < m_pBlackBoard->GetBossInfo()->iCurrentHealth || m_pBlackBoard->bIsExcution())
        return NODE_STATE::FAIL;

    auto pHitData = m_pBlackBoard->GetHitData();
    if (pHitData)
    {
        const CHARACTER_SKILL_DESC* pSkillDesc = static_cast<const CHARACTER_SKILL_DESC*>(pHitData->pSkillData);
        if (pSkillDesc)
        {
            if (SKILL_PROPERTY::EXCUTION & pSkillDesc->eProPerty)
            {
                m_pBlackBoard->EnterExcution(NAYITBA_EXECUTION_TYPE::EXECUTION_ATTACK);
                m_pBlackBoard->SetHitData(pHitData);
                return NODE_STATE::FAIL;
            }
        }
    }

    // 애니메이션을 받아서 재생한다. 
    if (0 == m_iNumSelection)
    {
        m_pOwner->Set_Animation("Result_State_Groggy_S", false);
    }

    _bool bIsFinished = m_pOwner->Play_Animation(fTimeDelta);
    if (bIsFinished)
    {
        if (0 == m_iNumSelection)
        {
            m_pOwner->Set_Animation("Result_State_Groggy_L");
            m_pOwner->SetThesholdAction(NAYITBA_EXECUTION_TYPE::EXECUTION_ATTACK);
        }
        m_iNumSelection++;
    }

    return NODE_STATE::COMPLETE;
}

CTask_Theshold* CTask_Theshold::Create(CBehaviorTree* pOwnerTree)
{
    CTask_Theshold* pTask_Dead = new CTask_Theshold();
    if (FAILED(pTask_Dead->Initialize_Prototype(pOwnerTree)))
    {
        Safe_Release(pTask_Dead);
        MSG_BOX("Create Fail : Task Dead");
    }
    return pTask_Dead;
}

void CTask_Theshold::Free()
{
    __super::Free();

    Safe_Release(m_pBlackBoard);
}
