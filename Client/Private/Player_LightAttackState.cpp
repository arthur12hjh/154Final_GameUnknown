#include "pch.h"
#include "Player_LightAttackState.h"

#include "Player.h"
#include "GameInstance.h"

CPlayer_LightAttackState::CPlayer_LightAttackState()
    : CPlayerState {}
{
}

void CPlayer_LightAttackState::Start(void* pArg)
{
    m_eState = PLAYER_STATE::LIGHT_ATTACK;

    m_pPlayer->Set_Animation("Proto_Sword_Lightattack_01_Root", false, 1.2f);
    m_eCombo = COMBO::LIGHT_ATTACK1;
    m_fLimitProgress = 0.10f;
}

PLAYER_TRANSITION_DESC CPlayer_LightAttackState::Update(_float fTimeDelta)
{
    _bool isAnimFinished = m_pPlayer->Play_Animation(fTimeDelta, m_Desc->pPlayerTransform, 0.8f);
    _float fAnimationRatio = m_pPlayer->Get_AnimationRatio();

    //공격 도중에도 걸을 수 있게
    if ((m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_W) ||
        m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_A) ||
        m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_S) ||
        m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_D)) &&
        fAnimationRatio >= m_fLimitProgress * 1.75f)
        m_tNextState.eNextState = PLAYER_STATE::WALK;

    if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_LSHIFT) &&
        fAnimationRatio >= m_fLimitProgress)
        m_tNextState.eNextState = PLAYER_STATE::EVADE;

    if ((m_pGameInstance->KeyDown(KEY_INPUT::MOUSE, ENUM_CLASS(MOUSEKEYSTATE::LBUTTON))  &&
        fAnimationRatio >= m_fLimitProgress))
    {
        //같은 상태가 두번 생기는걸 방지
        
        switch (m_eCombo)
        {
        case COMBO::LIGHT_ATTACK1:
            m_tNextState.eNextState = PLAYER_STATE::STATE_END;
            m_pPlayer->Set_Animation("Proto_Sword_Lightattack_02_Root", false, 1.75f);
            m_eCombo = COMBO::LIGHT_ATTACK2;
            m_fLimitProgress = 0.15f;
            break;

        case COMBO::LIGHT_ATTACK2:
            m_tNextState.eNextState = PLAYER_STATE::STATE_END;
            m_pPlayer->Set_Animation("Proto_Sword_Lightattack_03_Root", false, 1.2f);
            m_eCombo = COMBO::LIGHT_ATTACK3;
            m_fLimitProgress = 0.135f;
            break;

        case COMBO::LIGHT_ATTACK3:
            m_tNextState.eNextState = PLAYER_STATE::STATE_END;
            m_pPlayer->Set_Animation("Proto_Sword_Lightattack_04_Root", false, 1.2f);
            m_eCombo = COMBO::LIGHT_ATTACK4;
            m_fLimitProgress = 0.35f;
            break;

        case COMBO::LIGHT_ATTACK4:
            m_tNextState.eNextState = PLAYER_STATE::STATE_END;
            m_pPlayer->Set_Animation("Proto_Sword_Lightattack_01_Root", false, 1.2f);
            m_eCombo = COMBO::LIGHT_ATTACK1;
            m_fLimitProgress = 0.10f;
            break;

        default:
            break;
        }
    }

    //예외없이 애니메이션 끝났으면 idle로
    if (true == isAnimFinished)
        m_tNextState.eNextState = PLAYER_STATE::IDLE;

    if (COMBO::LIGHT_ATTACK5 == m_eCombo && fAnimationRatio <= 0.2f)
        m_Desc->pPlayerTransform->Go_Straight(fTimeDelta * 3.f * (0.3f - fAnimationRatio));

    return m_tNextState;
}

void CPlayer_LightAttackState::End()
{
}

CPlayer_LightAttackState* CPlayer_LightAttackState::Create(void* pArg)
{
    return new CPlayer_LightAttackState();
}

void CPlayer_LightAttackState::Free()
{
    __super::Free();
}
