#include "pch.h"
#include "LockonManager.h"

#include "GameInstance.h"
#include "Player.h"
#include "Nayitba.h"

#include "UIHUD.h"
#include "UIBase.h"

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

    list<CGameObject*>* pTargets =
        m_pGameInstance->GetAllObejctToLayer(
            m_pGameInstance->GetCurrentLevelID(),
            m_strMonsterLayerTag.c_str());

    // 기본값 초기화
    m_pPlayerDesc->HasTarget = false;
    m_pPlayerDesc->fCurrentMinDist = FLT_MAX;

    if (nullptr == pTargets || pTargets->empty())
    {
        m_pTarget = nullptr;
        m_fLockonTimer = 0.f;
        return false;
    }

    _float fMinDist = FLT_MAX;
    CNayitba* pBest = nullptr;

    for (auto& pTarget : *pTargets)
    {
        CTransform* pTargetTransform = pTarget->GetTransform();
        _float fDist = XMVectorGetX(XMVector3Length(
            m_pPlayerDesc->pPlayerTransform->Get_State(STATE::POSITION) -
            pTargetTransform->Get_State(STATE::POSITION)));

        if (fMinDist > fDist)
        {
            fMinDist = fDist;
            pBest = static_cast<CNayitba*>(pTarget);
            m_fCurrentMinDist = fMinDist;
            m_pTarget = static_cast<CNayitba*>(pTarget);

            if(m_pTarget != pTarget)
                m_fLockonTimer = 0.f;
        }
    }

    m_pTarget = pBest;

    if (m_pTarget)
    {
        m_pPlayerDesc->HasTarget = true;
        m_pPlayerDesc->fCurrentMinDist = fMinDist;
        m_fCurrentMinDist = fMinDist;
        m_isLock = true; 
    }

    return m_pPlayerDesc->HasTarget;
}

CTransform* CLockonManager::Get_TargetTransform()
{
    if (nullptr == m_pTarget)
        return nullptr;

    return m_pTarget->GetTransform();
}

_vector CLockonManager::Get_LockOnPoint()
{
#ifdef _DEBUG
    WCHAR Debug[MAX_PATH] = {};
    wsprintf(Debug, TEXT("[DEBUG] Lock On Point : %d %d %d\n"), (_int)m_pTarget->GetMonsterData().vLockOnPoint.x,
                                                                (_int)m_pTarget->GetMonsterData().vLockOnPoint.y,
                                                                (_int)m_pTarget->GetMonsterData().vLockOnPoint.z);

    OutputDebugStringW(Debug);
#endif // _DEBUG
   
    return XMLoadFloat3(&m_pTarget->GetMonsterData().vLockOnPoint);
}

void CLockonManager::Lockon(_float fTimeDelta)
{
    // 타겟 정보 업데이트 (bHasTarget, fCurrentMinDist 셋팅)
    Find_NearestTarget(fTimeDelta);

    if (!m_pTarget)
        return;

    // 타겟이 바뀌면 풀한테 돌려주기
    if (m_pLockonUI && m_pTarget != m_pLockonUI->GetParent())
    {
        CUIHUD* pUIHUD = dynamic_cast<CUIHUD*>(m_pGameInstance->GetCurrentLevelHUD());
        pUIHUD->Return_WorldUI(m_pLockonUI);
        Safe_Release(pUIHUD);
    }

    if (true == m_isLock && PLAYER_MODE::LOCKON == m_pPlayerDesc->ePlayerMode)
    {
        Get_LockOnPoint();

        if (!m_pLockonUI)
        {
            CUIHUD* pUIHUD = dynamic_cast<CUIHUD*>(m_pGameInstance->GetCurrentLevelHUD());
            m_pLockonUI = pUIHUD->Rent_WorldUI(TEXT("Pool_LockOnMark"), m_pTarget, &m_pTarget->GetMonsterData().vLockOnPoint);
            Safe_Release(pUIHUD);
        }

        if(false == m_pPlayerDesc->isLookFixed)
            m_pPlayerDesc->pPlayerTransform->LookAt_Lerp(
                XMVectorSetY(m_pTarget->GetTransform()->Get_State(STATE::POSITION), XMVectorGetY(m_pPlayerDesc->pPlayerTransform->Get_State(STATE::POSITION))), 0.15f, 1.f);
    }
    else
    {
        if (m_pLockonUI)
        {
            CUIHUD* pUIHUD = dynamic_cast<CUIHUD*>(m_pGameInstance->GetCurrentLevelHUD());

            pUIHUD->Return_WorldUI(m_pLockonUI);
            Safe_Release(pUIHUD);
        }
    }
}

void CLockonManager::Start_Lockon()
{
    // 락온모드 시작. 키입력으로 넣을때 추가할 것
    m_pPlayerDesc->isRequestLockonToggle = true;
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
