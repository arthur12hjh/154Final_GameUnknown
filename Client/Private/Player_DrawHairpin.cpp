#include "pch.h"

#include "Player_DrawHairpin.h"

#include "Player.h"
#include "GameInstance.h"

CPlayer_DrawHairpin::CPlayer_DrawHairpin()
	: CPlayerState{}
{
}

void CPlayer_DrawHairpin::Start(void* pArg)
{
}

PLAYER_TRANSITION_DESC CPlayer_DrawHairpin::Update(_float fTimeDelta)
{
	return PLAYER_TRANSITION_DESC();
}

void CPlayer_DrawHairpin::End()
{
}

CPlayer_DrawHairpin* CPlayer_DrawHairpin::Create(void* pArg)
{
	return new CPlayer_DrawHairpin();
}

void CPlayer_DrawHairpin::Free()
{
	__super::Free();
}
