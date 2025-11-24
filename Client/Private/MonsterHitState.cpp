#include "pch.h"
#include "MonsterHitState.h"

#include "GameInstance.h"
#include "GameStruct.h"
#include "Nayitba.h"

CMonsterHitState::CMonsterHitState() :
    CState()
{
}

HRESULT CMonsterHitState::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    return S_OK;
}

void CMonsterHitState::Start(void* pArg)
{
    // 대충 여기서 맞은 스킬따라서 분기
    // 뭐 기본스킬이면 맞았을때 공격아니면 피격모션 나오고 하는 등
    // 여기안에서 그로기 만들던가 하거나 아님 따로 상태주거나 이건
    auto pEntity = static_cast<CNayitba*>(m_pOwner);

    string   szAnimationName = "Result_Hit_Stand_Light";
    auto pDesc = static_cast<MONSTER_HIT_STATE_DESC*>(pArg);

    auto pSkillData = static_cast<CHARACTER_SKILL_DESC*>(pDesc->pSkillData);
    // 이거 공격한 대상이랑 외적으로 하든 내적으로하든 앞뒤 판단해서 
    // 이름 더해주자
    if (nullptr == pDesc->pAttacker)
    {
        m_bIsFinished = true;
        MSG_BOX("Not Bind Attacker");
        return;
    }

    _vector vOwnerPos = m_pOwner->GetTransform()->Get_State(STATE::POSITION);
    _vector vAttackerPos = pDesc->pAttacker->GetTransform()->Get_State(STATE::POSITION);

    _vector vDir = XMVector3Normalize(vAttackerPos - vOwnerPos);
    _float fScalar = XMVectorGetX(XMVector3Dot(m_pOwner->GetTransform()->Get_State(STATE::LOOK), vDir));
    if (0 <= fScalar)
    {
        szAnimationName += "_Fw";
        switch (pSkillData->eATK_Direction)
        {
        case ATTACK_DIRECTION::ATK_RIGHT:
            szAnimationName += "_Rw";
            break;
        case ATTACK_DIRECTION::ATK_LEFT:
            szAnimationName += "_Lw";
            break;
        case ATTACK_DIRECTION::ATK_DOWN:
            szAnimationName += "_Dw";
            break;
        case ATTACK_DIRECTION::ATK_UP:
            szAnimationName += "_Uw";
            break;
        }
    }
    else
        szAnimationName += "_Bw";

    pEntity->Set_Animation(szAnimationName.c_str(), false);
}

void CMonsterHitState::Update(_float fTimeDelta)
{
    //맞으면 여기서 들어온 스킬 따라서 분기해서 하기
    auto pEntity = static_cast<CNayitba*>(m_pOwner);
    m_bIsFinished = pEntity->Play_Animation(fTimeDelta);
}

void CMonsterHitState::End()
{
    m_bIsFinished = false;
    m_bIsGroggy = false;
    m_iSectionIndex = 0;
}

CMonsterHitState* CMonsterHitState::Create(void* pArg)
{
    CMonsterHitState* pMonsterHitState = new CMonsterHitState();
    if (FAILED(pMonsterHitState->Initialize(pArg)))
    {
        Safe_Release(pMonsterHitState);
        MSG_BOX("Create Fail : Monster Hit State");
    }
    return pMonsterHitState;
}

void CMonsterHitState::Free()
{
    __super::Free();
}
