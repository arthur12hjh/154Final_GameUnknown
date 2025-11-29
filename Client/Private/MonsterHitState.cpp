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

void CMonsterHitState::Start(void* pArg, CState* pPreState)
{
    // 대충 여기서 맞은 스킬따라서 분기
    // 뭐 기본스킬이면 맞았을때 공격아니면 피격모션 나오고 하는 등
    // 여기안에서 그로기 만들던가 하거나 아님 따로 상태주거나 이건
    auto pEntity = static_cast<CNayitba*>(m_pOwner);

    string   szAnimationName = "Result_Hit_Stand_Light";
    auto pDesc = static_cast<DEFAULT_DAMAGE_DESC*>(pArg);

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

    m_bIsEnableChange = false;
    if (0 == strcmp("None", pSkillData->szHitAnimationName))
    {
        _vector vDir = XMVector3Normalize(vAttackerPos - vOwnerPos);
        _float fScalar = XMVectorGetX(XMVector3Dot(m_pOwner->GetTransform()->Get_State(STATE::LOOK), vDir));
        if (0 <= fScalar)
        {
            szAnimationName += "_Fw";

            // Right 백터랑 맞은 방향이랑 내적하면 스칼라가 나오고
            // 그걸 acos해서 라디안으로 바꾸자
           /* _float fRadian = atan2f(pDesc->vHitDir.y, pDesc->vHitDir.x);
            
            fRadian = XMConvertToDegrees(fRadian);
            if (45.f <= fRadian && 135.f > fRadian)
            {
                szAnimationName += "_Fw_Uw";
            }
            else if (135.f <= fRadian && 225.f > fRadian)
            {
                szAnimationName += "_Lw";
            }
            else if (225.f <= fRadian && 315.f > fRadian)
            {
                szAnimationName += "_Fw_Dw";
            }
            else
            {
                szAnimationName += "_Rw";
            }*/

            switch (pSkillData->eATK_Direction)
            {
            case ATTACK_DIRECTION::ATK_LEFT :
                szAnimationName += "_Lw";
                break;
            case ATTACK_DIRECTION::ATK_RIGHT:
                szAnimationName += "_Rw";
                break;
            }
        }
        else
            szAnimationName += "_Bw";

        if (XMVector3Equal(XMLoadFloat3(&pDesc->vImpactDir), XMVectorZero()))
        {
            XMStoreFloat3(&m_vImpactDir, -1.f * vDir);
            m_vImpactDir.y += 0.1f;
        }
        else
        {
            m_vImpactDir = pDesc->vImpactDir;
        }
        m_fImpactForce = pDesc->fImpactForce;
    }
    else
    {
        szAnimationName = pSkillData->szHitAnimationName;
    }

    pEntity->Set_Animation(szAnimationName.c_str(), false, 1.5f, 0.12f, true);
}

void CMonsterHitState::Update(_float fTimeDelta)
{
    //맞으면 여기서 들어온 스킬 따라서 분기해서 하기
    auto pEntity = static_cast<CNayitba*>(m_pOwner);
 
    if (pEntity->Play_Animation(fTimeDelta))
    {
        m_bIsFinished = true;
        m_bIsEnableChange = true;
    }
    else
    {
        m_pOwner->GetTransform()->Move_Direction(fTimeDelta, XMLoadFloat3(&m_vImpactDir), m_fImpactForce);
    }
       
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
