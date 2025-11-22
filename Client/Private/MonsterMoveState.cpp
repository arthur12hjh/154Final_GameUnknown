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

void CMonsterMoveState::Start(void* pArg)
{
    MOVE_STATE_DESC* pMoveStateDesc = static_cast<MOVE_STATE_DESC*>(pArg);
    size_t iNumPathFindingPoint = pMoveStateDesc->PathFindingPoints->size();

    CNayitba* pOwner = static_cast<CNayitba*>(m_pOwner);
    m_pOwnerInfo = &pOwner->GetMonsterData();

    if (m_pOwnerInfo->bIsBattle)
    {
        _uint iNumPath = (_uint)m_pGameInstance->Random(0, iNumPathFindingPoint);
        m_vMovePoint = (*pMoveStateDesc->PathFindingPoints)[iNumPath];
    }
    else
    {
        // 이건 여기서 패트롤 또는 움직임을 제어
    }
}

void CMonsterMoveState::Update(_float fTimeDelta)
{
    //순찰같은거 있다면 이거 하게
    auto pEntity = static_cast<CNayitba*>(m_pOwner);
    string AnimationName = pEntity->GetStaticMonsterData()->szName;

    AnimationName += "_Caution_Cw";
    pEntity->Set_Animation(AnimationName.c_str());
    pEntity->Play_Animation(fTimeDelta);

    _vector vOwnerPos = m_pOwner->GetTransform()->Get_State(STATE::POSITION);
    _float fDistance = XMVectorGetX(XMVector3Length(XMLoadFloat3(&m_vMovePoint) - vOwnerPos));

    if (fDistance >= 1.f)
    {
        _vector vDir = XMVector3Normalize(XMLoadFloat3(&m_vMovePoint) - vOwnerPos);
        m_pOwner->GetTransform()->Move_Direction(fTimeDelta, vDir, 2.f);
    }
    else
        m_bIsFinished = true;
}

void CMonsterMoveState::End()
{
    m_bIsFinished = false;
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
