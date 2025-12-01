#include "pch.h"
#include "LockonManager.h"

#include "GameInstance.h"
#include "Player.h"

CLockonManager::CLockonManager()
    : m_pGameInstance { CGameInstance::GetInstance() }
{
    Safe_AddRef(m_pGameInstance);
}

void CLockonManager::Bind_Player(CPlayer* pPlayer)
{
    m_pPlayer = pPlayer;
    m_pPlayerDesc = pPlayer->Get_Desc();
}

HRESULT CLockonManager::Initialize()
{
    return S_OK;
}

_bool CLockonManager::Find_NearestTarget(_float fTimeDelta)
{
    m_fLockonTimer += fTimeDelta;

    if (m_fLockonTimer <= 1.5f)
        return false;

    list<CGameObject*>* pTargets = m_pGameInstance->GetAllObejctToLayer(m_pGameInstance->GetCurrentLevelID(), m_strMonsterLayerTag.c_str());

    _float fMinDist = FLT_MAX;
    m_fCurrentMinDist = fMinDist;

    // 타겟 트랜스폼이 비어있다면, 타겟 트랜스폼 비우고 생략.
    // 플레이어의 락온 모드도 제거해줘야 한다.
    // 이 과정이 없으면 중간에 몬스터가 죽거나 사라지게 되면 터짐
    if (nullptr == pTargets)
        return false;

    if (true == pTargets->empty())
    {
        m_pTargetTransform = nullptr;
        if(PLAYER_MODE::LOCKON == m_pPlayerDesc->ePlayerMode)
            m_pPlayer->Change_PlayerMode(PLAYER_MODE::BATTLE);
        m_fLockonTimer = 0.f;

        return false;
    }

    for (auto& pTarget : *pTargets)
    {
        CTransform* pTargetTransform = pTarget->GetTransform();
        _float fDist = XMVectorGetX(XMVector3Length(m_pPlayerDesc->pPlayerTransform->Get_State(STATE::POSITION) - pTargetTransform->Get_State(STATE::POSITION)));

        // 더 가까운 타겟을 발견했다면
        if (fMinDist > fDist)
        {
            // 타겟 트랜스폼 변경.
            fMinDist = fDist;
            m_fCurrentMinDist = fMinDist;
            m_pTargetTransform = pTargetTransform;

            if(m_pTargetTransform != pTargetTransform)
                m_fLockonTimer = 0.f;
        }
    }

    if (fMinDist < 30.f)
    {
        m_fBattleToIdleTimeAcc = 0.f;

        if (PLAYER_MODE::IDLE == m_pPlayerDesc->ePlayerMode)
            m_pPlayer->Change_PlayerMode(PLAYER_MODE::BATTLE);

        return true;
    }

    return false;
}

CTransform* CLockonManager::Get_TargetTransform()
{
    return m_pTargetTransform;
}

void CLockonManager::Lockon(_float fTimeDelta)
{
    // 우선 가까운 타겟 찾기.
    m_isLock = Find_NearestTarget(fTimeDelta);

    // 가까운 타겟이 비어있다면, nullptr 반환.
    if (nullptr == m_pTargetTransform || (PLAYER_MODE::LOCKON != m_pPlayerDesc->ePlayerMode))
        return;

    // 만약 존재한다면, 락온 처리.
    if(true == m_isLock)
        m_pPlayerDesc->pPlayerTransform->LookAt_Lerp(m_pTargetTransform->Get_State(STATE::POSITION), 0.15f);

    if (false == m_isLock && PLAYER_MODE::LOCKON == m_pPlayerDesc->ePlayerMode)
    {
        m_pPlayer->Change_PlayerMode(PLAYER_MODE::BATTLE);
        m_fLockonTimer = -1.f; 
    }
}

void CLockonManager::Start_Lockon()
{
    //플레이어가 락온 모드가 아니라면
    if (PLAYER_MODE::LOCKON != m_pPlayerDesc->ePlayerMode)
    {
        // 락온 모드로 변경.
        m_pPlayer->Change_PlayerMode(PLAYER_MODE::LOCKON);
    }
}

CLockonManager* CLockonManager::Create()
{
    CLockonManager* pInstance = new CLockonManager();

    if (FAILED(pInstance->Initialize()))
    {
        Safe_Release(pInstance);
        MSG_BOX("Create Failed : Lockon Manager");
    }

    return pInstance;
}

void CLockonManager::Free()
{
    __super::Free();

    Safe_Release(m_pGameInstance);
}
