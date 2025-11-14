#pragma once
#include "Client_Defines.h"
#include "BlackBoard.h"

NS_BEGIN(Client)
class CGorillaBlackBoard : public CBlackBoard
{
private:
	CGorillaBlackBoard();
	virtual ~CGorillaBlackBoard() = default;

public :
	HRESULT		Initialize();

private :


public:
	static		CGorillaBlackBoard*		Create();
	virtual		void					Free() override;
};
NS_END