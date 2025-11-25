#include "pch.h"
#include "MonsterTranslationState.h"

CMonsterTranslationState::CMonsterTranslationState() : 
    CState()
{
}

HRESULT CMonsterTranslationState::Initialize(void* pArg)
{
    MONSTER_TRANSLATION_STATE* pDesc = static_cast<MONSTER_TRANSLATION_STATE*>(pArg);
    m_szAnimationName



    return S_OK;
}

void CMonsterTranslationState::Start(void* pArg, CState* pPreState)
{
}

void CMonsterTranslationState::Update(_float fTimeDelta)
{
}

void CMonsterTranslationState::End()
{
}

CMonsterTranslationState* CMonsterTranslationState::Create(void* pArg)
{
    return nullptr;
}

void CMonsterTranslationState::Free()
{
}
