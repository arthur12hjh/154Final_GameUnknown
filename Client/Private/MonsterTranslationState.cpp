#include "pch.h"
#include "MonsterTranslationState.h"

#include "GameInstance.h"
#include "GameStruct.h"
#include "Nayitba.h"

CMonsterTranslationState::CMonsterTranslationState() : 
    CState()
{
}

HRESULT CMonsterTranslationState::Initialize(void* pArg)
{

    return S_OK;
}

void CMonsterTranslationState::Start(void* pArg, CState* pPreState)
{
    auto pNaytiba = static_cast<CNayitba*>(m_pOwner);
    MONSTER_TRANSLATION_STATE* pDesc = static_cast<MONSTER_TRANSLATION_STATE*>(pArg);
    m_szAnimationName = pDesc->szTranslationAnimName;
    m_szNextStateName = pDesc->szNextStateName;
    m_pArg = pDesc->pArg;
    m_CompletedFunc = pDesc->CompletedFunc;

    pNaytiba->Set_Animation(m_szAnimationName.c_str(), false);
}

void CMonsterTranslationState::Update(_float fTimeDelta)
{
    auto pNaytiba = static_cast<CNayitba*>(m_pOwner);

    if (pNaytiba->Play_Animation(fTimeDelta))
        m_CompletedFunc(m_szNextStateName, m_pArg);
}

void CMonsterTranslationState::End()
{
    m_szNextStateName = TEXT("");
    m_pArg = nullptr;
    m_CompletedFunc = nullptr;
}

CMonsterTranslationState* CMonsterTranslationState::Create(void* pArg)
{
    CMonsterTranslationState* pMonsterTranslationState = new CMonsterTranslationState();
    if (FAILED(pMonsterTranslationState->Initialize(pArg)))
    {
        Safe_Release(pMonsterTranslationState);
        MSG_BOX("Create Fail : Monster Translation State");
    }
    return pMonsterTranslationState;
}

void CMonsterTranslationState::Free()
{
    __super::Free();
}
