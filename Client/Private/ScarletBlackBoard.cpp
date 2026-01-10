#include "pch.h"
#include "ScarletBlackBoard.h"

#include "Nayitba.h"

CScarletBlackBoard::CScarletBlackBoard() :
    CBossBlackBoard()
{
}

HRESULT CScarletBlackBoard::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

	PHASE_CHANGE_DESC PhaseDesc = {};
	PhaseDesc.bIsCutScene = false;
	PhaseDesc.bIsLastAttack = false;
	m_ChangePhaseRatio.emplace_back(0.5f, PhaseDesc);

	PhaseDesc.bIsCutScene = false;
	PhaseDesc.bIsLastAttack = true;
	m_ChangePhaseRatio.emplace_back(0.02f, PhaseDesc);

    return S_OK;
}

_bool CScarletBlackBoard::EnterExcution(NAYITBA_EXECUTION_TYPE eExcution)
{
	auto pNaytiba = static_cast<CNaytiba*>(m_pOwner);
	m_eExcution = eExcution;
	if (NAYITBA_EXECUTION_TYPE::END != m_eExcution)
		pNaytiba->SetThesholdAction(NAYITBA_EXECUTION_TYPE::END);

	if (BOSS_PAHSE::SECOND == m_eBossPhase)
	{
		m_eCurState = CBossBlackBoard::BOSS_STATE::IDLE;
		m_eExcution = NAYITBA_EXECUTION_TYPE::END;

		pNaytiba->SetThesholdAction(NAYITBA_EXECUTION_TYPE::END);
		m_bIsReflectExcution = true;
	}

	pNaytiba->ResetToBaseState();

	return true;
}

_bool CScarletBlackBoard::UnconditionallyAttack()
{
	if (m_bIsPhaseLastAttack || m_bIsReflectExcution)
		return true;

	return false;
}

void CScarletBlackBoard::SetEntarnceAttack(_bool bIsflag)
{
	m_bIsEntarnceAttack = bIsflag;
}

_bool CScarletBlackBoard::bIsEnableEntarnceAttack()
{
	return m_bIsEntarnceAttack;
}

void CScarletBlackBoard::SetRefelctExcution(_bool bIsflag)
{
	m_bIsReflectExcution = bIsflag;
}

CScarletBlackBoard* CScarletBlackBoard::Create(void* pArg)
{
	CScarletBlackBoard* pScarlet_BlackBoard = new CScarletBlackBoard();
	if (FAILED(pScarlet_BlackBoard->Initialize(pArg)))
	{
		Safe_Release(pScarlet_BlackBoard);
		MSG_BOX("Create Fail : Scarlet Black Board");
	}
	return pScarlet_BlackBoard;
}

void CScarletBlackBoard::Free()
{
    __super::Free();
}
