#pragma once
#include "Quest.h"

NS_BEGIN(Engine)
class CEventHandle;
NS_END

NS_BEGIN(Client)
class CTestQuest : public CQuest
{
private:
	CTestQuest();
	virtual ~CTestQuest() = default;

public:
	virtual		HRESULT				Initialize() override;

	virtual		void				Accept_Quest() override;
	virtual		void				Clear() override;

private :
	CEventHandle*					m_pEvent = { nullptr };
	CEventHandle*					m_pRewardEvent = { nullptr };

private :
	HRESULT							Ready_Events();

	void							CallBackEvent(void* pArg);

public:
	static			CTestQuest*		Create();
	virtual			void			Free() override;

};
NS_END