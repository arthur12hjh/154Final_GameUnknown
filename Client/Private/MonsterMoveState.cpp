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
    size_t iNumPathFindingPoint = pMoveStateDesc->PathFindingPoints->size();

    CNayitba* pOwner = static_cast<CNayitba*>(m_pOwner);
    m_pTarget = pMoveStateDesc->pTarget;
    auto pOwnerStaticInfo = pOwner->GetStaticMonsterData();
    m_pOwnerInfo = &pOwner->GetMonsterData();
    m_OnMoveCompleted = pMoveStateDesc->OnMoveCompleted;

    if (AI_TYPE::PASSIVE == pOwnerStaticInfo->eAI_Type)
    {

    }

    switch (m_pOwnerInfo->eNaytiba)
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
        _vector vPreMovePoint = m_pOwner->GetTransform()->Get_State(STATE::POSITION);
        vector<_float3> PathList = *pMoveStateDesc->PathFindingPoints;
        sort(PathList.begin(), PathList.end(), [&](auto& pSrc, auto& pDest)
            {
                _float SrcLength = XMVectorGetX(XMVector3Length(XMLoadFloat3(&pSrc) - vPreMovePoint));
                _float DestLength = XMVectorGetX(XMVector3Length(XMLoadFloat3(&pDest) - vPreMovePoint));

                return SrcLength < DestLength;
            });

        _uint iNumPath = (_uint)m_pGameInstance->Random(1, iNumPathFindingPoint / 2);
        m_vMovePoint = (*pMoveStateDesc->PathFindingPoints)[iNumPath];
    }
    break;
    }
}

void CMonsterMoveState::Update(_float fTimeDelta)
{
    //순찰같은거 있다면 이거 하게
    _vector vOwnerPos = m_pOwner->GetTransform()->Get_State(STATE::POSITION);
    _bool bIsMove = true;
    if (NAYTIBA_STATE::BATTLE == m_pOwnerInfo->eNaytiba)
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
    }

    if (bIsMove)
        Update_Move(fTimeDelta);
}

void CMonsterMoveState::End()
{
    m_pTarget = nullptr;
    m_bIsFinished = false;
}

void CMonsterMoveState::Compute_MovePointDirection()
{
    _vector vOwnerPos = m_pOwner->GetTransform()->Get_State(STATE::POSITION);
    _vector vOwnerLook = m_pOwner->GetTransform()->Get_State(STATE::LOOK);
    _vector vOwnerRight = m_pOwner->GetTransform()->Get_State(STATE::RIGHT);

    _vector vDir = XMVector3Normalize(XMLoadFloat3(&m_vMovePoint) - vOwnerPos);
    _float fRightScalar = GetAngleVecToVecRadian2D(vOwnerRight, vDir);

    if (0.1f < fRightScalar)
        m_vMoveDirection = DIRECTION::RIGHT;
    else if(-0.1f > fRightScalar)
        m_vMoveDirection = DIRECTION::LEFT;
    else
    {
        _float fFrontScalar = GetAngleVecToVecRadian2D(vOwnerLook, vDir);
        if(0 <= fFrontScalar)
            m_vMoveDirection = DIRECTION::FRONT;
        else
            m_vMoveDirection = DIRECTION::BACK;
    }
}

void CMonsterMoveState::Update_Caution(_float fTimeDelta)
{
    auto pEntity = static_cast<CNayitba*>(m_pOwner);
    string AnimationName = pEntity->GetStaticMonsterData()->szAnimationName;

    AnimationName += "_Caution";
    _vector vOwnerPos = m_pOwner->GetTransform()->Get_State(STATE::POSITION);
    _float fDistance = XMVectorGetX(XMVector3Length(XMLoadFloat3(&m_vMovePoint) - vOwnerPos));

    Compute_MovePointDirection();
    switch (m_vMoveDirection)
    {
    case DIRECTION::FRONT:
        AnimationName += "_Fw";
        break;
    case DIRECTION::LEFT:
        AnimationName += "_Lw";
        break;
    case DIRECTION::RIGHT:
        AnimationName += "_Rw";
        break;
    case DIRECTION::BACK:
        AnimationName += "_Bw";
        break;
    }

    pEntity->Set_Animation(AnimationName.c_str());
    pEntity->Play_Animation(fTimeDelta);

    if (fDistance >= 0.1f)
    {
        if (nullptr == m_pTarget)
            return;

        _vector vDir = XMVector3Normalize(XMLoadFloat3(&m_vMovePoint) - vOwnerPos);
        _vector vTargetPos = m_pTarget->GetTransform()->Get_State(STATE::POSITION);

        m_pOwner->GetTransform()->LookAt(vTargetPos);

        // 애니메이션 속도 제어하는거 지금 되긴하는데 그거 테스트하면서
        // 속도 제어로 바꿀예정
        m_pOwner->GetTransform()->Move_Direction(fTimeDelta, vDir, 0.5f);
    }
    else
    {
        m_OnMoveCompleted(0.7f);
        m_bIsFinished = true;
    }
     
}

void CMonsterMoveState::Update_Move(_float fTimeDelta)
{
    auto pEntity = static_cast<CNayitba*>(m_pOwner);
    _bool   bIsAnimLoop = true;

    string AnimationName = pEntity->GetStaticMonsterData()->szAnimationName;
    _vector vOwnerPos = m_pOwner->GetTransform()->Get_State(STATE::POSITION);

    if (NAYTIBA_STATE::BATTLE == m_pOwnerInfo->eNaytiba)
    {
        if (0 == m_iSectionIndex)
        {
            if(pEntity->IsAnmiationFinished())
                m_iSectionIndex++;
            else
            {
                _vector vTargetPos = m_pTarget->GetTransform()->Get_State(STATE::POSITION);
                AnimationName += "_Run_S";
                bIsAnimLoop = false;
                m_bIsCaution = false;
            }
        }
        else if (1 == m_iSectionIndex)
        {
            AnimationName += "_Run_L";
            // 전투 상태라면 이거 Target을 향해서 뛰어간다.
           
            _vector vTargetPos = m_pTarget->GetTransform()->Get_State(STATE::POSITION);
            _vector vDir = XMVector3Normalize(vTargetPos - vOwnerPos);

            m_pOwner->GetTransform()->LookAt(vTargetPos);
            m_pOwner->GetTransform()->Move_Direction(fTimeDelta, vDir, 1.5f);

            _float fDistance = XMVectorGetX(XMVector3Length(vTargetPos - vOwnerPos));
            if (m_pOwnerInfo->fAttackRange - 1 >= fDistance)
            {
                m_iSectionIndex++;
            }
        }
        else if (2 == m_iSectionIndex)
        {
            AnimationName += "_Run_E";
            m_iSectionIndex = 0;
            m_bIsCaution = true;
            bIsAnimLoop = false;
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

            m_pOwner->GetTransform()->LookAt(vMovePoint);
            m_pOwner->GetTransform()->Move_Direction(fTimeDelta, vDir, 0.5f);
        }
        else
        {
            m_OnMoveCompleted(10.f);
            m_bIsFinished = true;
        }
    }

    pEntity->Set_Animation(AnimationName.c_str(), bIsAnimLoop);
    pEntity->Play_Animation(fTimeDelta);
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
