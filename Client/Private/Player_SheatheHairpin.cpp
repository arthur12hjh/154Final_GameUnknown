#include "pch.h"

#include "Player_SheatheHairPin.h"

#include "Player.h"
#include "GameInstance.h"

CPlayer_SheatheHairpin::CPlayer_SheatheHairpin()
	: CPlayerState {}
{
}

void CPlayer_SheatheHairpin::Start(void* pArg)
{
}

PLAYER_TRANSITION_DESC CPlayer_SheatheHairpin::Update(_float fTimeDelta)
{
	return PLAYER_TRANSITION_DESC();
}

void CPlayer_SheatheHairpin::End()
{
}

CPlayer_SheatheHairpin* CPlayer_SheatheHairpin::Create(void* pArg)
{
	return new CPlayer_SheatheHairpin();
}

void CPlayer_SheatheHairpin::Free()
{
	__super::Free();
}
