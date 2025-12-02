#include "BlackBoard.h"

CBlackBoard::CBlackBoard()
{
}

void CBlackBoard::SetOwner(CGameObject* pOwner)
{
    m_pOwner = pOwner;
}

void CBlackBoard::Free()
{
}
