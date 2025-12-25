#include "pch.h"
#include "Player_GigasExecutionState.h"

#include "Player.h"
#include "Nayitba.h" 
#include "GameInstance.h"
#include "GameManager.h"

CPlayer_GigasExecutionState::CPlayer_GigasExecutionState()
    : CPlayerState{}
{

}

void CPlayer_GigasExecutionState::Start(void* pArg, _float fBlendRatio)
{
    DEFAULT_DAMAGE_DESC Desc;
    Desc.pSkillData = m_pGameManager->Find_SkillData(1010);
    m_Desc->pLinkAttackTarget->Damaged(&Desc);
}

PLAYER_TRANSITION_DESC CPlayer_GigasExecutionState::Update(_float fTimeDelta)
{

    return m_tNextState;
}

_float CPlayer_GigasExecutionState::End()
{
    m_Desc->isSuperArmor = false;
    m_Desc->pPlayerController->Set_Active(true);
    m_Desc->pLinkAttackTarget = nullptr;

    return m_fNextBlendRatio;
}

CPlayer_GigasExecutionState* CPlayer_GigasExecutionState::Create(void* pArg)
{
    CPlayer_GigasExecutionState* pState = new CPlayer_GigasExecutionState();

   /* if (FAILED(pState->Initialize(pArg)))
        return nullptr;*/

    return pState;
}

void CPlayer_GigasExecutionState::Free()
{
    __super::Free();
}

