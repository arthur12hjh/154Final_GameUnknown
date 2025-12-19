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

	m_ChangePhaseRatio.emplace_back(make_pair(0.5f, false));
	m_ChangePhaseRatio.emplace_back(make_pair(0.01f, false));

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
