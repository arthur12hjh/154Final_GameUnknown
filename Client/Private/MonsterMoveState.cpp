#include "pch.h"
#include "MonsterMoveState.h"

#include "GameInstance.h"
#include "Nayitba.h"

#include "GameStruct.h"
#include "GameObject.h"

CMonsterMoveState::CMonsterMoveState() :
    CState()
{
    m_iStateID = 3;
}

HRESULT CMonsterMoveState::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    return S_OK;
}

void CMonsterMoveState::Start(void* pArg, CState* pPreState)
{
    MOVE_STATE_DESC* pMoveStateDesc = static_cast<MOVE_STATE_DESC*>(pArg);

    CNaytiba* pOwner = static_cast<CNaytiba*>(m_pOwner);
    m_pTarget = pMoveStateDesc->pTarget;
    m_pInitOwnerInfo = pOwner->GetStaticMonsterData();
    m_pOwnerInfo = &pOwner->GetMonsterData();
    m_OnMoveCompleted = pMoveStateDesc->OnMoveCompleted;

    switch (m_pOwnerInfo->eNaytibaState)
    {
    case NAYTIBA_STATE::DEFAULT :
    {
        XMStoreFloat3(&m_vMovePoint, pOwner->GetTransform()->Get_State(STATE::POSITION));

        // 이건 여기서 패트롤 또는 움직임을 제어
        m_vMovePoint.x += m_pGameInstance->Random(-5.f, 5.f);
        m_vMovePoint.z += m_pGameInstance->Random(-5.f, 5.f);
    }
        break;
    case NAYTIBA_STATE::MIMESSIS:
    {

    }
    break;

    case NAYTIBA_STATE::BATTLE:
    {
        _vector vOwnerPos = m_pOwner->GetTransform()->Get_State(STATE::POSITION);
        _vector vTargetPos = m_pTarget->GetTransform()->Get_State(STATE::POSITION);
        _float  fDistance = XMVectorGetX(XMVector3Length(vTargetPos - vOwnerPos));

        if (m_pOwnerInfo->fAttackRange * 5.f < fDistance)
        {
            string AnimationName = pOwner->GetStaticMonsterData()->szAnimationName;
            
            m_iSectionIndex = 0;
            AnimationName += "_Run_S";
            m_bIsEnableChange = false;
            m_bIsCaution = false;

            m_fMoveSpeed = m_pInitOwnerInfo->fMoveSpeed;
            if (6 == m_pInitOwnerInfo->iMonsetID)
                m_fMoveSpeed = m_pInitOwnerInfo->fMoveSpeed * 2.f;

            pOwner->Set_Animation(AnimationName.c_str(), false, 1.5f, 0.24f);
        }
        else
        {
            Compute_MoveDirection();
            m_bIsCaution = true;
        }
        
    }
    break;
    }
}

void CMonsterMoveState::Update(_float fTimeDelta)
{
    //순찰같은거 있다면 이거 하게
    _vector vOwnerPos = m_pOwner->GetTransform()->Get_State(STATE::POSITION);
    _bool bIsMove = true;
    if (NAYTIBA_STATE::BATTLE == m_pOwnerInfo->eNaytibaState)
    {
        if (nullptr == m_pTarget)
        {
            m_bIsFinished = true;
            return;
        }

        _float fDistance = XMVectorGetX(XMVector3Length(m_pTarget->GetTransform()->Get_State(STATE::POSITION) - vOwnerPos));

        // 여기서 범위가 딱 걸쳐있을때 계속
        // 애니메이션이 변경되어 이상하게 나오는거
        if (m_bIsCaution)
        {
            bIsMove = false;
            Update_Caution(fTimeDelta);
        }
        else
        {
            m_bIsFinished = true;
        }
    }

    if (bIsMove)
        Update_Move(fTimeDelta);
}

void CMonsterMoveState::End()
{
    m_pTarget = nullptr;
    m_bIsFinished = false;
    m_bIsEnableChange = true;
}

void CMonsterMoveState::Update_Caution(_float fTimeDelta)
{
    auto pEntity = static_cast<CNaytiba*>(m_pOwner);
    string AnimationName = pEntity->GetStaticMonsterData()->szAnimationName;

    _vector vOwnerPos = m_pOwner->GetTransform()->Get_State(STATE::POSITION);
    _vector vOwnerLook = m_pOwner->GetTransform()->Get_State(STATE::LOOK);
    _float fDistance = XMVectorGetX(XMVector3Length(m_pTarget->GetTransform()->Get_State(STATE::POSITION) - vOwnerPos));

    if(m_pOwnerInfo->fAttackRange * 5.f <= fDistance)
    {
        m_bIsFinished = true;
        m_bIsEnableChange = true;
        return;
    }
    else if (m_pOwnerInfo->fAttackRange * 0.6f <= fDistance)
    {
        if (DIRECTION::BACK == m_vMoveDirection)
        {
            m_bIsFinished = true;
            m_bIsEnableChange = true;
            return;
        }
    }

    AnimationName += "_Caution";
    _vector vMoveDir = {};
    switch (m_vMoveDirection)
    {
    case DIRECTION::FRONT:
    {
        AnimationName += "_Fw";
        vMoveDir = m_pOwner->GetTransform()->Get_State(STATE::LOOK);
    }
        break;
    case DIRECTION::LEFT:
    {
        AnimationName += "_Lw";
        vMoveDir = m_pOwner->GetTransform()->Get_State(STATE::RIGHT) * -1.f;
    }
        break;
    case DIRECTION::RIGHT:
    {
        vMoveDir = m_pOwner->GetTransform()->Get_State(STATE::RIGHT);
        AnimationName += "_Rw";
    }
        break;
    case DIRECTION::BACK:
    {
        AnimationName += "_Bw";
        vMoveDir = m_pOwner->GetTransform()->Get_State(STATE::LOOK) * -1.f;
    }
        break;
    }

    if (nullptr == m_pTarget)
        return;

    pEntity->Set_Animation(AnimationName.c_str(), true, 1.f, m_pInitOwnerInfo->fLerpRatio);
    pEntity->Play_Animation(fTimeDelta);
    
    // 애니메이션 속도 제어하는거 지금 되긴하는데 그거 테스트하면서
    // 속도 제어로 바꿀예정
    auto pOwnerTransform = m_pOwner->GetTransform();
    _vector vTargetPos = m_pTarget->GetTransform()->Get_State(STATE::POSITION);

    _vector vTempOwnerPos = vOwnerPos;
    vOwnerLook.m128_f32[1] = vTargetPos.m128_f32[1] = vTempOwnerPos.m128_f32[1] = 0.f;
    _vector vDir = XMVector3Normalize(vTargetPos - vTempOwnerPos);
    _float fScalar = acosf(XMVectorGetX(XMVector3Dot(vOwnerLook, vDir)));

    if (0.2f <= fabsf(fScalar))
        LerpLookAt(fTimeDelta, 5.f);
    else
        pOwnerTransform->LookAt(vOwnerPos + vDir);
    pOwnerTransform->Move_Direction(fTimeDelta, vMoveDir, m_fMoveSpeed);
}

void CMonsterMoveState::Update_Move(_float fTimeDelta)
{
    auto pEntity = static_cast<CNaytiba*>(m_pOwner);
    _vector vDir = {};
    _bool   bIsAnimLoop = true;

    string AnimationName = pEntity->GetStaticMonsterData()->szAnimationName;
    _vector vOwnerPos = m_pOwner->GetTransform()->Get_State(STATE::POSITION);

    if (NAYTIBA_STATE::BATTLE == m_pOwnerInfo->eNaytibaState)
    {
        _vector vTargetPos = m_pTarget->GetTransform()->Get_State(STATE::POSITION);
        
        vDir = XMVector3Normalize(vTargetPos - vOwnerPos);
        _float fDistance = XMVectorGetX(XMVector3Length(vTargetPos - vOwnerPos));
       

        if (2 == m_iSectionIndex)
        {
            AnimationName += "_Run_E";
            m_bIsEnableChange = true;
            bIsAnimLoop = false;
        }
        else
        {
            if (1 == m_iSectionIndex)
            {
                AnimationName += "_Run_L";
                if (m_pOwnerInfo->fAttackRange * 2.f >= fDistance)
                    m_iSectionIndex++;
            }

            LerpLookAt(fTimeDelta, 2.f);
            m_pOwner->GetTransform()->Move_Direction(fTimeDelta, vDir, m_fMoveSpeed);
        }
    }
    else
    {
        // 전투 상태가 아니라면 걸어서 배회
        AnimationName += "_Walk_L";
        m_vMovePoint.y = vOwnerPos.m128_f32[1] = 0.f;
        vDir = XMVector3Normalize(XMLoadFloat3(&m_vMovePoint) - vOwnerPos);
        _float fDistance = XMVectorGetX(XMVector3Length(XMLoadFloat3(&m_vMovePoint) - vOwnerPos));

        if (fDistance >= 0.3f)
        {
            LerpLookAt(fTimeDelta, 3.f);
            m_pOwner->GetTransform()->Move_Direction(fTimeDelta, vDir, m_fMoveSpeed * 0.5f);
        }
        else
        {
            m_OnMoveCompleted(10.f);
            m_bIsFinished = true;
        }
    }

    _float fAnimSpeed = m_fMoveSpeed / m_pInitOwnerInfo->fMoveSpeed;
    pEntity->Set_Animation(AnimationName.c_str(), bIsAnimLoop, 1.f, m_pInitOwnerInfo->fLerpRatio);

    pEntity->Play_Animation(fTimeDelta);
    if (pEntity->IsAnmiationFinished())
    {
        if (NAYTIBA_STATE::BATTLE == m_pOwnerInfo->eNaytibaState)
        {
            if (0 == m_iSectionIndex)
            {
                m_fMoveSpeed = m_pInitOwnerInfo->fMoveSpeed * 1.3f;
                m_iSectionIndex++;
            }
            else if (2 == m_iSectionIndex)
            {
                m_iSectionIndex = 0;
                m_bIsCaution = true;
            }
        }
    }
        
}

void CMonsterMoveState::LerpLookAt(_float fTimeDelta, _float fSpeed)
{
    _vector vOwnerPos = m_pOwner->GetTransform()->Get_State(STATE::POSITION);
    _vector vTempOwnerPos = vOwnerPos;

    _vector vTargetPos{}, vLerpEndDir{};
    _vector vOwnerLook = m_pOwner->GetTransform()->Get_State(STATE::LOOK);
    if (NAYTIBA_STATE::BATTLE == m_pOwnerInfo->eNaytibaState)
    {
        vTargetPos = m_pTarget->GetTransform()->Get_State(STATE::POSITION);
        vTargetPos.m128_f32[1] = vTempOwnerPos.m128_f32[1] = 0.f;
        vLerpEndDir = XMVector3Normalize(vTargetPos - vTempOwnerPos);
    }
    else if(NAYTIBA_STATE::DEFAULT == m_pOwnerInfo->eNaytibaState)
    {
        _vector vTempOwnerPos = vOwnerPos;
        vTempOwnerPos.m128_f32[3] = 0.f;
        vLerpEndDir = XMVector3Normalize(XMLoadFloat3(&m_vMovePoint) - vTempOwnerPos);
    }

    vLerpEndDir.m128_f32[1] = 0.f;
    m_pOwner->GetTransform()->LookAt_Lerp(vOwnerPos + vLerpEndDir, fTimeDelta, fSpeed);
}

void CMonsterMoveState::Compute_MoveDirection()
{
    _vector vOwnerPos = m_pOwner->GetTransform()->Get_State(STATE::POSITION);
    _float fDistance = XMVectorGetX(XMVector3Length(m_pTarget->GetTransform()->Get_State(STATE::POSITION) - vOwnerPos));

    _float fRandom = m_pGameInstance->Random(0.f, 100.f);
    if (50 <= fRandom)
    {
        m_vMoveDirection = DIRECTION::RIGHT;
    }
    else
    {
        m_vMoveDirection = DIRECTION::LEFT;
    }
    m_fMoveSpeed = 2.0f;
}

CMonsterMoveState* CMonsterMoveState::Create(void* pArg)
{
    CMonsterMoveState* pMonsterPatrolState = new CMonsterMoveState();
    if (FAILED(pMonsterPatrolState->Initialize(pArg)))
    {
        Safe_Release(pMonsterPatrolState);
        MSG_BOX("Create Fail : Monster Patrol State");
    }
    return pMonsterPatrolState;
}

void CMonsterMoveState::Free()
{
    __super::Free();
}
