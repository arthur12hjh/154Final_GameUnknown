#pragma once
#include "Client_Defines.h"
#include "GameStruct.h"
#include "Base.h"

NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(Client)
class CQuest abstract : public CBase
{
protected :
	CQuest();
	virtual ~CQuest() = default;

public :
	virtual		HRESULT			Initialize();

	virtual		void			Accept_Quest() = 0;
	virtual		void			Clear() = 0;

	const QUEST_STRUCT&			GetQuestInfo() { return m_QuestInfo; }
	_bool						ClearCondition() { m_bIsClear; }

protected:
	CGameInstance*				m_pGameInstance = { nullptr };

	// 대충여기서 퀘스트에 필요한 데이터 보관
	QUEST_STRUCT				m_QuestInfo = {};
	_bool						m_bIsClear = { false };

public :
	virtual			void		Free() override;

};
NS_END