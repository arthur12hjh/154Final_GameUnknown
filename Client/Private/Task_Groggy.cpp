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
        m_pOwner = static_cast<CNaytiba*>(m_pOwnerTree->GetOwner());

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
    if (CTask_Groggy::GROGGY_STATE::END == m_eState)
        Play_EndSound();

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
            m_pOwner->SetThesholdAction(NAYITBA_EXECUTION_TYPE::END);
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
        Play_Start_GorggySound();
        m_szAnimation = "Result_State_Groggy_S";
        m_pBlackBoard->SetCurState(CBossBlackBoard::BOSS_STATE::GROGGY);
        m_pOwner->Set_Animation(m_szAnimation.c_str(), false);
        break;
    case CTask_Groggy::GROGGY_STATE::LOOP:
    {
        Play_Loop_GorggySound();
        m_szAnimation = "Result_State_Groggy_L";
        CBossBlackBoard::BOSS_PAHSE ePhase = m_pBlackBoard->Get_BossPhase();
        switch (ePhase)
        {
        case CBossBlackBoard::BOSS_PAHSE::FIRST:
            m_pOwner->SetThesholdAction(NAYITBA_EXECUTION_TYPE::LINK_ATTACK);
            break;
        case CBossBlackBoard::BOSS_PAHSE::SECOND:
            m_pOwner->SetThesholdAction(NAYITBA_EXECUTION_TYPE::PHASE2_LINKATTACK);
            break;
        }
        m_pOwner->Set_Animation(m_szAnimation.c_str(), true);
    }
        break;
    case CTask_Groggy::GROGGY_STATE::END:
        ZeroMemory(m_bIsGorillaSound, sizeof(m_bIsGorillaSound));

        m_szAnimation = "Result_State_Groggy_E";
        m_pOwner->Set_Animation(m_szAnimation.c_str(), false);
        break;
    }
}

void CTask_Groggy::Play_Start_GorggySound()
{
    _uint iMonsterID = m_pOwner->GetMonsterID();
    _float fRandomIndex = m_pGameInstance->Random(0.f, 100.f);
    if (1 == iMonsterID)
    {
        if (33 >= fRandomIndex)
            m_pGameInstance->Manager_PlaySound(TEXT("mon_GorillaB_voice_growl_1.wav"), CHANNELID::EFFECT, 5.f);
        else if (66 >= fRandomIndex)
            m_pGameInstance->Manager_PlaySound(TEXT("mon_GorillaB_voice_growl_2.wav"), CHANNELID::EFFECT, 5.f);
        else
            m_pGameInstance->Manager_PlaySound(TEXT("mon_GorillaB_voice_growl_3.wav"), CHANNELID::EFFECT, 5.f);
    }
    else if (8 == iMonsterID)
    {
        if (20 >= fRandomIndex)
            m_pGameInstance->Manager_PlaySound(TEXT("vo_Scarlet_groggy_S_1_VO.wav"), CHANNELID::EFFECT, 5.f);
        else if (40 >= fRandomIndex)
            m_pGameInstance->Manager_PlaySound(TEXT("vo_Scarlet_groggy_S_2_VO.wav"), CHANNELID::EFFECT, 5.f);
        else if (60 >= fRandomIndex)
            m_pGameInstance->Manager_PlaySound(TEXT("vo_Scarlet_groggy_S_3_VO.wav"), CHANNELID::EFFECT, 5.f);
        else if (80 >= fRandomIndex)
            m_pGameInstance->Manager_PlaySound(TEXT("vo_Scarlet_groggy_S_4_VO.wav"), CHANNELID::EFFECT, 5.f);
        else
            m_pGameInstance->Manager_PlaySound(TEXT("vo_Scarlet_groggy_S_5_VO.wav"), CHANNELID::EFFECT, 5.f);

    }
}

void CTask_Groggy::Play_Loop_GorggySound()
{
    _uint iMonsterID = m_pOwner->GetMonsterID();
    _float fRandomIndex = m_pGameInstance->Random(0.f, 100.f);
   if (8 == iMonsterID)
    {
        if (20 >= fRandomIndex)
            m_pGameInstance->Manager_PlaySound(TEXT("vo_Scarlet_groggy_L_1_VO.wav"), CHANNELID::EFFECT, 5.f);
        else if (40 >= fRandomIndex)
            m_pGameInstance->Manager_PlaySound(TEXT("vo_Scarlet_groggy_L_2_VO.wav"), CHANNELID::EFFECT, 5.f);
        else if (60 >= fRandomIndex)
            m_pGameInstance->Manager_PlaySound(TEXT("vo_Scarlet_groggy_L_3_VO.wav"), CHANNELID::EFFECT, 5.f);
        else if (80 >= fRandomIndex)
            m_pGameInstance->Manager_PlaySound(TEXT("vo_Scarlet_groggy_L_4_VO.wav"), CHANNELID::EFFECT, 5.f);
        else
            m_pGameInstance->Manager_PlaySound(TEXT("vo_Scarlet_groggy_L_5_VO.wav"), CHANNELID::EFFECT, 5.f);

    }
}

void CTask_Groggy::Play_EndSound()
{
    switch (m_pOwner->GetMonsterID())
    {
    case 1 :
        Gorilla_GrooggyEnd();
        break;
    case 8 :
        Scarelt_GrooggyEnd();
        break;
    }
}

void CTask_Groggy::Gorilla_GrooggyEnd()
{
    _uint iIndexAnim = m_pOwner->GetAnimationFrameIndex();
    if (31 <= iIndexAnim && !m_bIsGorillaSound[0])
    {
        m_pGameInstance->Manager_PlaySound(TEXT("mon_GorillaB_voice_roar_M_2.wav"), CHANNELID::EFFECT, 5.f, 1.f);
        m_bIsGorillaSound[0] = true;
    }

    if (34 <= iIndexAnim && !m_bIsGorillaSound[1])
    {
        m_pGameInstance->Active_RadialBlur(1.f, 5, 0.5f);
        m_pGameInstance->Shake_Camera(1.0f, 0.3f);
        m_bIsGorillaSound[1] = true;
    }
}

void CTask_Groggy::Scarelt_GrooggyEnd()
{
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
