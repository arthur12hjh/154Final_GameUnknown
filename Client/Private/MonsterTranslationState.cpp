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
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    return S_OK;
}

void CMonsterTranslationState::Start(void* pArg, CState* pPreState)
{
    auto pNaytiba = static_cast<CNayitba*>(m_pOwner);
    MONSTER_TRANSLATION_STATE* pDesc = static_cast<MONSTER_TRANSLATION_STATE*>(pArg);

    m_szAnimationName = pNaytiba->GetStaticMonsterData()->szAnimationName;
    m_szAnimationName += pDesc->szTranslationAnimName;
    m_szNextStateName = pDesc->szNextStateName;
    m_pArg = pDesc->pArg;
    m_CompletedFunc = pDesc->CompletedFunc;
    m_bIsEnableChange = false;
    
    if (pDesc->pTarget)
    {
        m_bIsLerp = true;
        m_vLerpTime = { 0.f, 1.0f };
        m_pTarget = pDesc->pTarget;
        XMStoreFloat3(&m_vLerpStartLook, m_pOwner->GetTransform()->Get_State(STATE::LOOK));
    }

    pNaytiba->Set_Animation(m_szAnimationName.c_str(), false);
}

void CMonsterTranslationState::Update(_float fTimeDelta)
{
    auto pNaytiba = static_cast<CNayitba*>(m_pOwner);

    if (m_bIsLerp)
    {
        _vector vOwnerPos = m_pOwner->GetTransform()->Get_State(STATE::POSITION);
        _vector vTargetPos = m_pTarget->GetTransform()->Get_State(STATE::POSITION);
        vOwnerPos.m128_f32[1] = vTargetPos.m128_f32[1] = 0.f;

        _vector vLerpEndDir = XMVector3Normalize(vTargetPos - vOwnerPos);
        _float fRatio = m_vLerpTime.x / m_vLerpTime.y;
        fRatio = Clamp<_float>(fRatio, 0.f, 1.f);

        _vector LerpLook = XMVectorLerp(XMLoadFloat3(&m_vLerpStartLook), vLerpEndDir, fRatio);
        m_pOwner->GetTransform()->LookAt(vOwnerPos + LerpLook);

        m_vLerpTime.x += 0.3f;
        if (m_vLerpTime.x > m_vLerpTime.y)
            m_bIsLerp = false;
    }
    else
    {
        if (pNaytiba->Play_Animation(fTimeDelta))
        {
            m_bIsEnableChange = true;
            m_CompletedFunc(m_szNextStateName, m_pArg);
        }
    }
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
