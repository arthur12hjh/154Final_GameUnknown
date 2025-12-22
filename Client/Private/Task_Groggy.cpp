#include "pch.h"
#include "Task_Groggy.h"

#include "GameInstance.h"
#include "BossBlackBoard.h"
#include "BehaviorTree.h"
#include "Nayitba.h"

CTask_Groggy::CTask_Groggy()
{
}

HRESULT CTask_Groggy::Initialize_Prototype(CBehaviorTree* pOwnerTree)
{
    if (FAILED(__super::Initialize_Prototype(pOwnerTree)))
        return E_FAIL;

    if (nullptr == m_pOwner)
        m_pOwner = static_cast<CNayitba*>(m_pOwnerTree->GetOwner());

    if (nullptr == m_pBlackBoard)
        m_pBlackBoard = static_cast<CBossBlackBoard*>(m_pOwnerTree->GetBlackBoard());

    return S_OK;
}

CBehaviorNode::NODE_STATE CTask_Groggy::Update(_float fTimeDelta)
{
    if (CBossBlackBoard::BOSS_STATE::GROGGY != m_pBlackBoard->GetCurState())
        return NODE_STATE::FAIL;

    if (m_pBlackBoard->GetCurState() == m_pBlackBoard->GetPreState())
    {
        m_pBlackBoard->AccGroggyTime(fTimeDelta);
        if (m_pBlackBoard->ExitGroggy())
        {
            if(GROGGY_STATE::LOOP == m_eState)
                ChangeGroggyState(GROGGY_STATE::END);
        }
    }
    else
        ChangeGroggyState(GROGGY_STATE::START);

    _bool bIsFinished = m_pOwner->Play_Animation(fTimeDelta, m_pOwner->GetTransform(), 1.f);
    if (bIsFinished)
    {
        switch (m_eState)
        {
        case CTask_Groggy::GROGGY_STATE::START:
            ChangeGroggyState(GROGGY_STATE::LOOP);
            break;
        case CTask_Groggy::GROGGY_STATE::END:
        {
            m_pOwner->RecoveryPoint(RECOVERY_TYPE::RECOVERY_STEMINA);
            m_pBlackBoard->SetCurState(CBossBlackBoard::BOSS_STATE::IDLE);
            m_pOwner->SetThesholdAction(EXCUTION_TYPE::END);
            return NODE_STATE::COMPLETE;
        }
        }
    }

    return NODE_STATE::COMPLETE;
}

void CTask_Groggy::ChangeGroggyState(GROGGY_STATE eState)
{
    m_eState = eState;
    switch (m_eState)
    {
    case CTask_Groggy::GROGGY_STATE::START:
        m_szAnimation = "Result_State_Groggy_S";
        m_pBlackBoard->SetCurState(CBossBlackBoard::BOSS_STATE::GROGGY);
        m_pOwner->Set_Animation(m_szAnimation.c_str(), false);
        break;
    case CTask_Groggy::GROGGY_STATE::LOOP:
        m_szAnimation = "Result_State_Groggy_L";
        m_pOwner->Set_Animation(m_szAnimation.c_str(), true);
        break;
    case CTask_Groggy::GROGGY_STATE::END:
        m_szAnimation = "Result_State_Groggy_E";
        m_pOwner->Set_Animation(m_szAnimation.c_str(), false);
        break;
    }
}

CTask_Groggy* CTask_Groggy::Create(CBehaviorTree* pOwnerTree)
{
    CTask_Groggy* pTask_Groggy = new CTask_Groggy();
    if (FAILED(pTask_Groggy->Initialize_Prototype(pOwnerTree)))
    {
        Safe_Release(pTask_Groggy);
        MSG_BOX("Create Fail : Task Groggy");
    }

    return pTask_Groggy;
}

void CTask_Groggy::Free()
{
    __super::Free();

    Safe_Release(m_pBlackBoard);
}
