#include "pch.h"

#include "Player_DrawHairpin.h"

#include "Player.h"
#include "GameInstance.h"

CPlayer_DrawHairpin::CPlayer_DrawHairpin()
	: CPlayerState{}
{
}

void CPlayer_DrawHairpin::Start(void* pArg, _float fBlendRatio)
{
    m_eState = PLAYER_STATE::DRAW_HAIRPIN;
	m_pPlayer->Set_Animation("Eve_Weapon_Start_Anim", false, 1.2f);
}

PLAYER_TRANSITION_DESC CPlayer_DrawHairpin::Update(_float fTimeDelta)
{
    _bool isAnimFinished = m_pPlayer->Play_Animation(fTimeDelta);
    _float fAnimationRatio = m_pPlayer->Get_AnimationRatio();

    //추후 비녀 가리는 로직도 추가할 것.
    if (9 / 102.f <= fAnimationRatio)
        m_Desc->isWeaponVisible = true;

    if (true == isAnimFinished)
    {
        m_tNextState.eNextState = PLAYER_STATE::IDLE;
        m_tNextState.eMode = PLAYER_MODE::BATTLE;
        m_tNextState.isChangeMode = true;
        PLAYER_BATTLEWALK_DESC Desc;
        Desc.isEvade = true;
        Desc.isLand = true;
        m_tNextState.pArg = &Desc;
    }

    return m_tNextState;
}

_float CPlayer_DrawHairpin::End()
{
    return m_fNextBlendRatio;
}

CPlayer_DrawHairpin* CPlayer_DrawHairpin::Create(void* pArg)
{
	return new CPlayer_DrawHairpin();
}

void CPlayer_DrawHairpin::Free()
{
	__super::Free();
}
