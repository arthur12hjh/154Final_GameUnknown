#include "pch.h"
#include "ScarletBlackBoard.h"

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

void CScarletBlackBoard::SetEntarnceAttack(_bool bIsflag)
{
	m_bIsEntarnceAttack = bIsflag;
}

_bool CScarletBlackBoard::bIsEnableEntarnceAttack()
{
	return m_bIsEntarnceAttack;
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
