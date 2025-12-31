#include "Player_ScarletPhase2tLinkAttackState.h"

CPlayer_ScarletPhase2tLinkAttackState::CPlayer_ScarletPhase2tLinkAttackState()
{
}

HRESULT CPlayer_ScarletPhase2tLinkAttackState::Initialize(void* pArg)
{
	return E_NOTIMPL;
}

void CPlayer_ScarletPhase2tLinkAttackState::Start(void* pArg, _float fBlendRatio)
{
}

PLAYER_TRANSITION_DESC CPlayer_ScarletPhase2tLinkAttackState::Update(_float fTimeDelta)
{
	return PLAYER_TRANSITION_DESC();
}

_float CPlayer_ScarletPhase2tLinkAttackState::End()
{
	return m_fNextBlendRatio;
}

CPlayer_ScarletPhase2tLinkAttackState* CPlayer_ScarletPhase2tLinkAttackState::Create(void* pArg)
{
	CPlayer_ScarletPhase2tLinkAttackState* pState = new CPlayer_ScarletPhase2tLinkAttackState();

	if (FAILED(pState->Initialize(pArg)))
		return nullptr;

	return pState;
}

void CPlayer_ScarletPhase2tLinkAttackState::Free()
{
	__super::Free();
}
