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
        m_fTargetDistance = -1;

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

void CBossBlackBoard::SetAttackDelay(_float fDelay)
{
    m_fAttackDelay = fDelay;
}

void CBossBlackBoard::Free()
{
    __super::Free();
}
