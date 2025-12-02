#pragma once
#include "BossBlackBoard.h"

NS_BEGIN(Client)
class CGorillaBlackBoard final : public CBossBlackBoard
{
private:
	CGorillaBlackBoard();
	virtual ~CGorillaBlackBoard() = default;

	virtual		HRESULT					Initialize(void* pArg);



public:
	static		CGorillaBlackBoard*		Create(void* pArg);
	virtual		void					Free() override;
};
NS_END