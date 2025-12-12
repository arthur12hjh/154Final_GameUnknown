#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class CLayer final : public CBase
{
private:
	CLayer();
	virtual ~CLayer() = default;

public:
	class CComponent*	Get_Component(const _wstring& strComponentTag, _uint iIndex = 0);
	class CComponent*	Get_PartObject_Component(const _wstring& strPartTag, const _wstring& strComponentTag, _uint iIndex = 0);
public:
	HRESULT				Add_GameObject(class CGameObject* pGameObject);

	void				Priority_Update(_float fTimeDelta);
	void				Update(_float fTimeDelta);
	void				Late_Update(_float fTimeDelta);

	void				Clear();
	void				Clear_DeadObj();

	list<class CGameObject*>*			GetObjects() { return &m_GameObjects; }

private:
	list<class CGameObject*>			m_GameObjects;

public:
	static CLayer* Create();
	virtual void Free() override;
};

NS_END