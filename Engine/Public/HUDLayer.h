#pragma once
#include "Base.h"

NS_BEGIN(Engine)
class CGameObject;

class CHUDLayer final : public CBase
{
private:
	CHUDLayer() = default;
	virtual ~CHUDLayer() = default;

protected :
	virtual HRESULT				Initialize_Layer();

public:
	virtual	 void				Priority_Update(_float fDeletaTime);
	virtual	 void				Update(_float fDeletaTime);
	virtual	 void				Late_Update(_float fDeletaTime);

	HRESULT						ADD_GameObject(const WCHAR* UITag, CGameObject* pUserInterface);
	HRESULT						Remove_GameObject(const WCHAR* UITag);
	
	CGameObject*				Find_GameObject(const WCHAR* UITag);

private:
	unordered_map<_wstring, CGameObject*>		m_UserInterfaces = {};

public:
	static		CHUDLayer*		Create();
	virtual		void			Free()		override;

};
NS_END