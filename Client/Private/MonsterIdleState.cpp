#include "pch.h"
#include "MonsterIdleState.h"

#include "GameInstance.h"

#include "MonsterStateMimesis.h"
#include "GameStruct.h"
#include "Nayitba.h"

CMonsterIdleState::CMonsterIdleState()
{
}

HRESULT CMonsterIdleState::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    return S_OK;
}

void CMonsterIdleState::Start(void* pArg, CState* pPreState)
{
    auto pEntity = static_cast<CNayitba*>(m_pOwner);
    m_pStaticOwnerInfo = pEntity->GetStaticMonsterData();
    m_pOwnerInfo = &pEntity->GetMonsterData();

    m_szAnimationName = m_pStaticOwnerInfo->szAnimationName;
    if (AI_TYPE::PASSIVE == m_pStaticOwnerInfo->eAI_Type)
    {
        auto pMimesisState = dynamic_cast<CMonsterStateMimesis*>(pPreState);
        if (nullptr != pMimesisState)
        {
            switch (pMimesisState->GetMimesisIndex())
            {
            case 1 :
                m_szAnimationName += "StanbyToNormal_01";
                break;
            case 2:
                m_szAnimationName += "StanbyToNormal_02";
                break;
            case 3:
                m_szAnimationName += "StanbyToNormal_03";
                break;
            }
            m_bIsEnableChange = false;
            m_bIsPlayStartAnim = true;
        }
    }
}

void CMonsterIdleState::Update(_float fTimeDelta)
{
    auto pEntity = static_cast<CNayitba*>(m_pOwner);
    
    if (false == m_bIsPlayStartAnim)
    {
        m_szAnimationName = m_pStaticOwnerInfo->szAnimationName;
        if (NAYTIBA_STATE::BATTLE == m_pOwnerInfo->eNaytiba)
            m_szAnimationName += "_Battle_Idle01";
        else
            m_szAnimationName += "_Idle01";
    }
    else
    {
        if (pEntity->IsAnmiationFinished())
        {
            m_bIsPlayStartAnim = false;
            m_bIsEnableChange = true;
        }
            
    }

    pEntity->Set_Animation(m_szAnimationName.c_str());
    pEntity->Play_Animation(fTimeDelta);
}

void CMonsterIdleState::End()
{
}

CMonsterIdleState* CMonsterIdleState::Create(void* pArg)
{
    CMonsterIdleState* pMonsterIdleState = new CMonsterIdleState();
    if (FAILED(pMonsterIdleState->Initialize(pArg)))
    {
        Safe_Release(pMonsterIdleState);
        MSG_BOX("Create Fail Monster Idle State");
    }
    return pMonsterIdleState;
}

void CMonsterIdleState::Free()
{
    __super::Free();
}
