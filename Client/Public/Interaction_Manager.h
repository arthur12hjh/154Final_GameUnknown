#pragma once
#include "Base.h"

NS_BEGIN(Engine)
class CGameInstance;
class CGameObject;
NS_END

NS_BEGIN(Client)
class CInteraction_Component;

// 인터렉션 메니저는 특정 내가 원하는 레벨에서
// 매니저 업데이트를 돌려줘야 돌아간다.
class CInteraction_Manager final : public CBase
{
private:
	CInteraction_Manager();
	virtual ~CInteraction_Manager() = default;

	HRESULT									Initalize();

public :
	void									Update();

	void									SetInteractionBaseObject(CGameObject* pObject);
	void									ADD_Interaction(CInteraction_Component* pInteraction_Com);

	CInteraction_Component*					GetNearInteraction() { return m_pNearInteraction; }
	vector<CInteraction_Component*>*		GetAllInteraction() { return &m_pInteractionList; }

private:
	CGameInstance*							m_pGameInstance = { nullptr };
	CGameObject*							m_pBaseObject = { nullptr };

	vector<CInteraction_Component*>			m_pInteractionList;
	CInteraction_Component*					m_pNearInteraction = { nullptr };

public:
	static CInteraction_Manager*			Create();
	virtual void							Free() override;

};
NS_END