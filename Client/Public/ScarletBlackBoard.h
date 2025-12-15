#pragma once
#include "BossBlackBoard.h"

NS_BEGIN(Client)
class CScarletBlackBoard final : public CBossBlackBoard
{
private:
	CScarletBlackBoard();
	virtual ~CScarletBlackBoard() = default;

	virtual		HRESULT					Initialize(void* pArg);

public:
	static		CScarletBlackBoard*		Create(void* pArg);
	virtual		void					Free() override;
};
NS_END