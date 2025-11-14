#include "pch.h"
#include "GorillaBlackBoard.h"

CGorillaBlackBoard::CGorillaBlackBoard() : CBlackBoard()
{
}

HRESULT CGorillaBlackBoard::Initialize()
{
	// 여기서 구조체 초기화


	return S_OK;
}

CGorillaBlackBoard* CGorillaBlackBoard::Create()
{
	CGorillaBlackBoard* pGorilla_BlackBoard = new CGorillaBlackBoard();
	if (FAILED(pGorilla_BlackBoard->Initialize()))
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
