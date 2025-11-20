#pragma once
#include "Client_Defines.h"
#include "Base.h"

NS_BEGIN(Engine)
class CEventHandle;
NS_END

NS_BEGIN(Client)
class CQuest;

class CQuestManager final : public CBase
{
private :
	CQuestManager();
	virtual ~CQuestManager() = default;

public :
	HRESULT						Initialize();
	CQuest*						Find_Quest(_uint iQuestID);

	// 레벨에 맞는 퀘스트들 받아오거나 퀘스트 전체리스트를 들고있자.
	// 퀘스트는  천천히 로드해도됨
	HRESULT						Ready_Quest(const WCHAR* szFilePath);

	// 퀘스트를 받는다.
	// 수락한 퀘스트가 어떤 이벤트를 참조해야하는지는 매개변수로
	_bool						Accept_Quest(_uint iQuestID);

	// 완료된 퀘스트들 여기다가 보관
	void						CompletedQuest(_uint iQuestID);

private :
	map<_uint, CQuest*>			m_Quests = {};
	
	set<_uint>					m_AcceptQuests = {};
	set<_uint>					m_CompletedQuests = {};

public :
	static	CQuestManager*		Create();
	virtual void				Free() override;

};
NS_END