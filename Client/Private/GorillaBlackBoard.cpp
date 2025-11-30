#include "pch.h"
#include "GorillaBlackBoard.h"

CGorillaBlackBoard::CGorillaBlackBoard() : CBossBlackBoard()
{
}

HRESULT CGorillaBlackBoard::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

CGorillaBlackBoard* CGorillaBlackBoard::Create(void* pArg)
{
	CGorillaBlackBoard* pGorilla_BlackBoard = new CGorillaBlackBoard();
	if (FAILED(pGorilla_BlackBoard->Initialize(pArg)))
	{
		Safe_Release(pGorilla_BlackBoard);
		MSG_BOX("Create Fail : Gorilla Black Board");
	}
	return pGorilla_BlackBoard;
}

void CGorillaBlackBoard::Free()
{
	__super::Free();
}
