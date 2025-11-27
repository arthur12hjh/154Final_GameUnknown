#include "pch.h"
#include "MonsterMoveState.h"

#include "GameInstance.h"
#include "Nayitba.h"

#include "GameStruct.h"
#include "GameObject.h"

CMonsterMoveState::CMonsterMoveState() :
    CState()
{
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

    CNayitba* pOwner = static_cast<CNayitba*>(m_pOwner);
    m_pTarget = pMoveStateDesc->pTarget;
    auto pOwnerStaticInfo = pOwner->GetStaticMonsterData();
    m_pOwnerInfo = &pOwner->GetMonsterData();
    m_OnMoveCompleted = pMoveStateDesc->OnMoveCompleted;

    if (AI_TYPE::PASSIVE == pOwnerStaticInfo->eAI_Type)
    {

    }

    switch (m_pOwnerInfo->eNaytibaState)
    {
    case NAYTIBA_STATE::DEFAULT :
    {
        XMStoreFloat3(&m_vMovePoint, pOwner->GetTransform()->Get_State(STATE::POSITION));

        m_bIsLerp = true;
        XMStoreFloat3(&m_vLerpStartPos, m_pOwner->GetTransform()->Get_State(STATE::LOOK));
        m_vLerpTime = { 0.f, 0.7f };

        // 이건 여기서 패트롤 또는 움직임을 제어
        m_vMovePoint.x += m_pGameInstance->Random(-5.f, 5.f);
        m_vMovePoint.y = 0.f;
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
        _float  fDistance = XMVectorGetX(XMVector3Length(m_pTarget->GetTransform()->Get_State(STATE::POSITION) - vOwnerPos));

        if (m_pOwnerInfo->fAttackRange < fDistance)
        {
            _vector vTargetPos = m_pTarget->GetTransform()->Get_State(STATE::POSITION);
            string AnimationName = pOwner->GetStaticMonsterData()->szAnimationName;
            
            m_bIsLerp = true;
            XMStoreFloat3(&m_vLerpStartPos, m_pOwner->GetTransform()->Get_State(STATE::LOOK));
            m_vLerpTime = { 0.f, 0.5f };

            AnimationName += "_Run_S";
            m_bIsEnableChange = false;
            m_bIsCaution = false;
            pOwner->Set_Animation(AnimationName.c_str(), false, 1.3f);
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
        if (m_pOwnerInfo->fAttackRange >= fDistance && m_bIsCaution)
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
    m_iSectionIndex = 0;
    m_pTarget = nullptr;
    m_bIsFinished = false;
    m_bIsEnableChange = true;
}

void CMonsterMoveState::Update_Caution(_float fTimeDelta)
{
    auto pEntity = static_cast<CNayitba*>(m_pOwner);
    string AnimationName = pEntity->GetStaticMonsterData()->szAnimationName;

    _vector vOwnerPos = m_pOwner->GetTransform()->Get_State(STATE::POSITION);
    _float fDistance = XMVectorGetX(XMVector3Length(m_pTarget->GetTransform()->Get_State(STATE::POSITION) - vOwnerPos));

    if (m_pOwnerInfo->fAttackRange * 0.5f > fDistance)
    {
        if (DIRECTION::BACK != m_vMoveDirection)
        {
            m_bIsFinished = true;
            return;
        }
    }
    else
    {
        if (DIRECTION::BACK == m_vMoveDirection)
        {
            m_bIsFinished = true;
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

    pEntity->Set_Animation(AnimationName.c_str());
    pEntity->Play_Animation(fTimeDelta);
    
    // 애니메이션 속도 제어하는거 지금 되긴하는데 그거 테스트하면서
    // 속도 제어로 바꿀예정
    auto pOwnerTransform = m_pOwner->GetTransform();
    _vector vTargetPos = m_pTarget->GetTransform()->Get_State(STATE::POSITION);

    _vector vTempOwnerPos = vOwnerPos;
    vTargetPos.m128_f32[1] = vTempOwnerPos.m128_f32[1] = 0.f;
    _vector vDir = XMVector3Normalize(vTargetPos - vTempOwnerPos);

    pOwnerTransform->LookAt(vOwnerPos + vDir);
    pOwnerTransform->Move_Direction(fTimeDelta, vMoveDir, m_fMoveSpeed);
}

void CMonsterMoveState::Update_Move(_float fTimeDelta)
{
    auto pEntity = static_cast<CNayitba*>(m_pOwner);
    _bool   bIsAnimLoop = true;

    string AnimationName = pEntity->GetStaticMonsterData()->szAnimationName;
    _vector vOwnerPos = m_pOwner->GetTransform()->Get_State(STATE::POSITION);

    if (NAYTIBA_STATE::BATTLE == m_pOwnerInfo->eNaytibaState)
    {
        _vector vTargetPos = m_pTarget->GetTransform()->Get_State(STATE::POSITION);
        
        vTargetPos.m128_f32[1] = vOwnerPos.m128_f32[1] = 0.f;
        _vector vDir = XMVector3Normalize(vTargetPos - vOwnerPos);
        _float fDistance = XMVectorGetX(XMVector3Length(vTargetPos - vOwnerPos));

        if (m_pOwnerInfo->fAttackRange < fDistance || 0 < m_iSectionIndex)
        {
            if (0 == m_iSectionIndex)
            {
                /*AnimationName += "_Run_S";
                bIsAnimLoop = false;
                m_bIsEnableChange = false;
                m_bIsCaution = false;*/
                if (m_bIsLerp)
                    LerpLookAt(fTimeDelta);

                m_pOwner->GetTransform()->Move_Direction(fTimeDelta, vDir, m_fMoveSpeed * 2.f);
            }
            else if (1 == m_iSectionIndex)
            {
                AnimationName += "_Run_L";
                // 전투 상태라면 이거 Target을 향해서 뛰어간다.
                m_pOwner->GetTransform()->LookAt(vOwnerPos + vDir);
                m_pOwner->GetTransform()->Move_Direction(fTimeDelta, vDir, m_fMoveSpeed * 2.f);

                if (m_pOwnerInfo->fAttackRange * 0.5f > fDistance)
                {
                    //Compute_CautionAngle();
                    m_iSectionIndex++;
                }
            }
            else if (2 == m_iSectionIndex)
            {
                AnimationName += "_Run_E";
                m_bIsEnableChange = true;
                bIsAnimLoop = false;
            }
        }
        else if(m_pOwnerInfo->fAttackRange > fDistance && 0 != m_iSectionIndex)
        {
            m_iSectionIndex = 0;
            m_bIsCaution = true;
        }
    }
    else
    {
        // 전투 상태가 아니라면 걸어서 배회
        AnimationName += "_Walk_L";
        _vector vDir = XMVector3Normalize(XMLoadFloat3(&m_vMovePoint) - vOwnerPos);
        _float fDistance = XMVectorGetX(XMVector3Length(XMLoadFloat3(&m_vMovePoint) - vOwnerPos));

        if (fDistance >= 0.3f)
        {
            _vector vMovePoint = XMLoadFloat3(&m_vMovePoint);
            vMovePoint.m128_f32[3] = 1.f;

            if (m_bIsLerp)
                LerpLookAt(fTimeDelta);
            else
                m_pOwner->GetTransform()->Move_Direction(fTimeDelta, vDir, m_fMoveSpeed * 0.5f);
        }
        else
        {
            m_OnMoveCompleted(10.f);
            m_bIsFinished = true;
        }
    }

    pEntity->Set_Animation(AnimationName.c_str(), bIsAnimLoop);

    if(0 == m_iSectionIndex && m_bIsRootMotion)
        pEntity->Play_Animation(fTimeDelta, m_pOwner->GetTransform(), 1.f);
    else
        pEntity->Play_Animation(fTimeDelta);

    if (pEntity->IsAnmiationFinished())
    {
        if (NAYTIBA_STATE::BATTLE == m_pOwnerInfo->eNaytibaState)
        {
            if (0 == m_iSectionIndex)
                m_iSectionIndex++;
            else if (2 == m_iSectionIndex)
            {
                m_iSectionIndex = 0;
                m_bIsCaution = true;
            }
        }
    }
        
}

void CMonsterMoveState::LerpLookAt(_float fTimeDelta)
{
    _vector vOwnerPos = m_pOwner->GetTransform()->Get_State(STATE::POSITION);
    _vector vTargetPos{}, vLerpEndDir{};
    if (NAYTIBA_STATE::BATTLE == m_pOwnerInfo->eNaytibaState)
    {
        vLerpEndDir = XMVector3Normalize(m_pTarget->GetTransform()->Get_State(STATE::POSITION) - vOwnerPos);
    }
    else if(NAYTIBA_STATE::DEFAULT == m_pOwnerInfo->eNaytibaState)
    {
        _vector vTempOwnerPos = vOwnerPos;
        vTempOwnerPos.m128_f32[3] = 0.f;
        vLerpEndDir = XMVector3Normalize(XMLoadFloat3(&m_vMovePoint) - vTempOwnerPos);
    }

    vLerpEndDir.m128_f32[1] = m_vLerpStartPos.y = 0.f;
    _float fRatio = m_vLerpTime.x / m_vLerpTime.y;

    fRatio = Clamp<_float>(fRatio, 0.f, 1.f);
    _vector LerpLook = XMVectorLerp(XMLoadFloat3(&m_vLerpStartPos), vLerpEndDir, fRatio);
    m_pOwner->GetTransform()->LookAt(vOwnerPos + LerpLook);

    m_vLerpTime.x += fTimeDelta;
    if (m_vLerpTime.x > m_vLerpTime.y)
        m_bIsLerp = false;
}

void CMonsterMoveState::Compute_MoveDirection()
{
    _vector vOwnerPos = m_pOwner->GetTransform()->Get_State(STATE::POSITION);
    _float fDistance = XMVectorGetX(XMVector3Length(m_pTarget->GetTransform()->Get_State(STATE::POSITION) - vOwnerPos));

    _float fSafeDistance = m_pOwnerInfo->fAttackRange * 0.5f;
    if (fSafeDistance < fDistance)
    {
        _float fRandom = m_pGameInstance->Random(0.f, 100.f);
        if (50 <= fRandom)
        {
            m_vMoveDirection = DIRECTION::RIGHT;
          //  XMStoreFloat3(&m_vMoveDir, m_pOwner->GetTransform()->Get_State(STATE::RIGHT));
        }
        else
        {
            m_vMoveDirection = DIRECTION::LEFT;
          //  XMStoreFloat3(&m_vMoveDir, -1 * m_pOwner->GetTransform()->Get_State(STATE::RIGHT));
        }
    }
    else
    {
        m_vMoveDirection = DIRECTION::BACK;
      //  XMStoreFloat3(&m_vMoveDir, -1 * m_pOwner->GetTransform()->Get_State(STATE::LOOK));
    }
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
