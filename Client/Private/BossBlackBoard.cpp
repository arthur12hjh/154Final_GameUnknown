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

void CBossBlackBoard::SetTargetDistacne()
{
    if (nullptr == m_pTarget)
    {
        m_fTargetDistance = -1;
        return;
    }

    _vector pOwnerPos = m_pOwner->GetTransform()->Get_State(STATE::POSITION);
    _vector pTargetPos = m_pTarget->GetTransform()->Get_State(STATE::POSITION);

    pOwnerPos.m128_f32[1] = pTargetPos.m128_f32[1] = 0.f;
    m_fTargetDistance = XMVectorGetX(XMVector3Length(pTargetPos - pOwnerPos));
}

void CBossBlackBoard::SetCurState(BOSS_STATE eState)
{
    m_ePreState = m_eCurState;
    m_eCurState = eState;
}

void CBossBlackBoard::Set_BossPhase(BOSS_PAHSE ePhase)
{
    m_eBossPhase = ePhase;
}

void CBossBlackBoard::Set_PlayCutScene()
{
    m_ChangePhaseRatio[ENUM_CLASS(m_eBossPhase)].second.bIsCutScene = true;
}

_bool CBossBlackBoard::Is_PlayPhaseChangeCutScene()
{
    _uint iPhaseIndex = ENUM_CLASS(m_eBossPhase);
    if (0 > iPhaseIndex || iPhaseIndex >= m_BossDefualtInfo->iNumPhase)
        return true;

    return  m_ChangePhaseRatio[ENUM_CLASS(m_eBossPhase)].second.bIsCutScene;
}

_uint CBossBlackBoard::Get_NumBossPhases()
{
    return m_BossDefualtInfo->iNumPhase;
}

_float CBossBlackBoard::Get_CurrentPhaseLitmitPercent()
{
    _uint iPhaseIndex = ENUM_CLASS(m_eBossPhase);
    if (0 > iPhaseIndex || iPhaseIndex >= m_BossDefualtInfo->iNumPhase)
        return 0.f;

    return   m_ChangePhaseRatio[iPhaseIndex].first;
}

_bool CBossBlackBoard::IsLastPhase()
{
    return ENUM_CLASS(m_eBossPhase) == m_BossDefualtInfo->iNumPhase - 1 ? true : false;
}

void CBossBlackBoard::SetHitData(const Default_Damage_Desc* pDamageData)
{
    if (nullptr == pDamageData)
    {
        ZeroMemory(&m_pHit_Data, sizeof(Default_Damage_Desc));
    }
    else
    {
        m_pHit_Data = *pDamageData;
        m_bIsHit = true;
        SetCurState(BOSS_STATE::HIT);
    }

}

const Default_Damage_Desc* CBossBlackBoard::GetHitData()
{
    if (m_bIsHit)
    {
        m_bIsHit = false;
        return &m_pHit_Data;
    }

    return nullptr;
}

void CBossBlackBoard::AccAttackDelay(_float fTimeDelta)
{
    m_fAttackDelay.x += fTimeDelta;
}

void CBossBlackBoard::ClearAttackTimer()
{
    m_fAttackDelay.x = 0.f;
}

void CBossBlackBoard::SetAttackDelay(_float fDelay)
{
    m_fAttackDelay.y = fDelay;
}

void CBossBlackBoard::AccGroggyTime(_float fTimeDelta)
{
    m_vGroggyTime.x += fTimeDelta;
}

void CBossBlackBoard::EnterGroggy()
{
    SetCurState(BOSS_STATE::GROGGY);
    m_vGroggyTime.x = 0.f;
}

_bool CBossBlackBoard::ExitGroggy()
{
    if (m_vGroggyTime.x > m_vGroggyTime.y)
        return true;

    return false;
}

void CBossBlackBoard::SetPhaseLastAttack(_bool bIsFlag)
{
    _uint iPhaseIndex = ENUM_CLASS(m_eBossPhase);
    if (0 > iPhaseIndex || iPhaseIndex >= m_BossDefualtInfo->iNumPhase)
        return;

    if (bIsFlag)
    {
        if(8 == m_BossDefualtInfo->iMonsetID)
            static_cast<CNayitba*>(m_pOwner)->RecoveryPoint(RECOVERY_TYPE::RECOVERY_SHILED);

        m_bIsPhaseLastAttack = bIsFlag;
        m_ChangePhaseRatio[iPhaseIndex].second.bIsLastAttack = false;
    }
    else
    {
        m_bIsPhaseLastAttack = bIsFlag;
    }
}

_bool CBossBlackBoard::IsCurrentPhaseLastAttackAction()
{
    _uint iPhaseIndex = ENUM_CLASS(m_eBossPhase);
    if (0 > iPhaseIndex || iPhaseIndex >= m_BossDefualtInfo->iNumPhase)
        return false;

    return   m_ChangePhaseRatio[iPhaseIndex].second.bIsLastAttack;
}

void CBossBlackBoard::EnterExcution(NAYITBA_EXECUTION_TYPE eExcution)
{
    m_eExcution = eExcution;
    if (NAYITBA_EXECUTION_TYPE::END != m_eExcution)
    {
        static_cast<CNayitba*>(m_pOwner)->SetThesholdAction(NAYITBA_EXECUTION_TYPE::END);
    }
}

_bool CBossBlackBoard::bIsExcution()
{
    if (NAYITBA_EXECUTION_TYPE::END != m_eExcution)
        return true;

    return false;
}

_bool CBossBlackBoard::IsAttackEnable()
{
    if (m_fAttackDelay.x > m_fAttackDelay.y)
        return true;
        
    return false;
}

void CBossBlackBoard::SetAttackData(const Character_Skill_Desc* pAttack_Data)
{
    m_pAttack_Skill = pAttack_Data;
    static_cast<CNayitba*>(m_pOwner)->SetAttackData(pAttack_Data);
}

const Character_Skill_Desc* CBossBlackBoard::GetAttackData()
{
    return m_pAttack_Skill;
}

void CBossBlackBoard::SetParryAttack()
{
    m_bIsParryAttack = true;
    m_fAttackDelay.x = m_fAttackDelay.y + 0.1f;
}

void CBossBlackBoard::ResetParryAttack()
{
    m_bIsParryAttack = false;
}

void CBossBlackBoard::Reset_State()
{
    m_pAttack_Skill = nullptr;
    m_bIsHit = false;

}

void CBossBlackBoard::Free()
{
    __super::Free();
}
