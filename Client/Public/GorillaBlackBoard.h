#pragma once
#include "BossBlackBoard.h"

NS_BEGIN(Client)
class CGorillaBlackBoard final : public CBossBlackBoard
{
private:
	CGorillaBlackBoard();
	virtual ~CGorillaBlackBoard() = default;

public :
	HRESULT								Initialize();


public:
	static		CGorillaBlackBoard*		Create();
	virtual		void					Free() override;
};
NS_END