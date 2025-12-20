#include "pch.h"

#include "Player.h"
#include "PlayerState.h"

#include "GameInstance.h"
#include "GameManager.h"

CPlayerState::CPlayerState()
	: m_pGameInstance { CGameInstance::GetInstance() }
	, m_pGameManager { CGameManager::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pGameManager);

	//순환참조나요..
	m_pPlayer = static_cast<CPlayer*>(m_pGameManager->GetGameCharacter());
	m_Desc = m_pPlayer->Get_Desc();
	Safe_Release(m_pPlayer);

	m_tNextState.eNextState = PLAYER_STATE::STATE_END;
	m_tNextState.pArg = nullptr;
}

_bool CPlayerState::isTransferAble(PLAYER_MODE ePlayerMode, PLAYER_STATE ePlayerState)
{
	return true;
}

void CPlayerState::Start(void* pArg, _float fBlendRatio)
{
}

PLAYER_TRANSITION_DESC CPlayerState::Update(_float fTimeDelta)
{
	return m_tNextState;
}

_float CPlayerState::End()
{
	return m_fNextBlendRatio;
}

_vector CPlayerState::Calc_PlayerDirection(_vector vDir)
{
	_vector vPlayerLook = m_Desc->pPlayerTransform->Get_State(STATE::LOOK);
	_vector vPlayerRight = m_Desc->pPlayerTransform->Get_State(STATE::RIGHT);

	vPlayerLook = XMVectorSetY(vPlayerLook, 0.f);
	vPlayerRight = XMVectorSetY(vPlayerRight, 0.f);

	_vector vPlayerDir = (vPlayerLook + vPlayerRight) * XMVector2Normalize(vDir);

	return vPlayerDir;
}

void CPlayerState::Free()
{
	__super::Free();

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pGameManager);
}
