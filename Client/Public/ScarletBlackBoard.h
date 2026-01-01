#pragma once
#include "BossBlackBoard.h"

NS_BEGIN(Client)
class CScarletBlackBoard final : public CBossBlackBoard
{
private:
	CScarletBlackBoard();
	virtual ~CScarletBlackBoard() = default;

public :
	virtual	HRESULT						Initialize(void* pArg);
	virtual	_bool						EnterExcution(NAYITBA_EXECUTION_TYPE eExcution);

public :
	void								SetEntarnceAttack(_bool bIsflag);
	_bool								bIsEnableEntarnceAttack();
	_bool								bIsReflectExcution() { return m_bIsReflectExcution; }

private :
	_bool								m_bIsEntarnceAttack = { true };
	_bool								m_bIsReflectExcution = { };

public:
	static		CScarletBlackBoard*		Create(void* pArg);
	virtual		void					Free() override;
};
NS_END