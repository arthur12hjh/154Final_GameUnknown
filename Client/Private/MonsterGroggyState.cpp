#include "pch.h"
#include "MonsterGroggyState.h"

#include "GameInstance.h"
#include "Nayitba.h"

CMonsterGroggyState::CMonsterGroggyState() :
    CState()
{
    m_iStateID = 5;
}

HRESULT CMonsterGroggyState::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    return S_OK;
}

void CMonsterGroggyState::Start(void* pArg, CState* pPreState)
{
    CNaytiba* pNaytiba = static_cast<CNaytiba*>(m_pOwner);
    m_vGroogyTime.y = m_pGameInstance->Random(3.f, 4.f);
    m_vGroogyTime.x = 0.f;

    m_iSectionIndex = 0;
    m_bIsEnableChange = false;
    m_bIsFinished = false;
    m_szAnimationName = "Result_State_Groggy_S";
    pNaytiba->Set_Animation(m_szAnimationName.c_str(), false);
    pNaytiba->SetThesholdAction(NAYITBA_EXECUTION_TYPE::LINK_ATTACK);
}

void CMonsterGroggyState::Update(_float fTimeDelta)
{
    CNaytiba* pNaytiba = static_cast<CNaytiba*>(m_pOwner);
    if (1 == m_iSectionIndex)
    {
        m_vGroogyTime.x += fTimeDelta;
        if (m_vGroogyTime.x > m_vGroogyTime.y)
        {
            m_szAnimationName = "Result_State_Groggy_E";
            pNaytiba->Set_Animation(m_szAnimationName.c_str(), false);
        }
    }

    _bool bIsAnimFinished = pNaytiba->Play_Animation(fTimeDelta, m_pOwner->GetTransform(), 1.f);
    if (bIsAnimFinished)
    {
        if (0 == m_iSectionIndex)
        {
            m_szAnimationName = "Result_State_Groggy_L";
            pNaytiba->Set_Animation(m_szAnimationName.c_str(), true);
        }
        else
        {
            m_bIsEnableChange = true;
            m_bIsFinished = true;
        }

        m_iSectionIndex++;
    }
}

void CMonsterGroggyState::End()
{
    CNaytiba* pNaytiba = static_cast<CNaytiba*>(m_pOwner);
    pNaytiba->SetThesholdAction(NAYITBA_EXECUTION_TYPE::END);
    pNaytiba->RecoveryPoint(RECOVERY_TYPE::RECOVERY_STEMINA);
}

CMonsterGroggyState* CMonsterGroggyState::Create(void* pArg)
{
    CMonsterGroggyState* pMonsterGroggyState = new CMonsterGroggyState();
    if (FAILED(pMonsterGroggyState->Initialize(pArg)))
    {
        Safe_Release(pMonsterGroggyState);
        MSG_BOX("Create Fail : Monster Groggy State");
    }
    return pMonsterGroggyState;
}

void CMonsterGroggyState::Free()
{
    __super::Free();
}
