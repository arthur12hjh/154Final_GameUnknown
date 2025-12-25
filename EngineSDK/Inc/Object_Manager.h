#pragma once

#include "Base.h"

/* 레벨별로 사용해야 할 객체들을 분리하여 보관한다. */

NS_BEGIN(Engine)
class CGameObject;

class CObject_Manager final : public CBase
{
private:
	CObject_Manager();
	virtual ~CObject_Manager() = default;

public:
	class CComponent*			Get_Component(_uint iLevelIndex, const _wstring& strLayerTag, const _wstring& strComponentTag, _uint iIndex = 0);
	class CComponent*			Get_PartObject_Component(_uint iLevelIndex, const _wstring& strLayerTag, const _wstring& strPartTag, const _wstring& strComponentTag, _uint iIndex = 0);
	
	list<CGameObject*>*			Get_LayerObjects(_uint iLevelIndex, const WCHAR* szLayerTag);
	map<const _wstring, class CLayer*>*	GetLayer();

public:
	HRESULT				Initialize(_uint iNumLevels);

	/* 원형을 찾고 복제하여 레이어에 추가한다. */
	HRESULT				Add_GameObject_ToLayer(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, _uint iLayerLevelIndex, const _wstring& strLayerTag, void* pArg);
	CGameObject*		Add_Get_GameObject(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, _uint iLayerLevelIndex, const _wstring& strLayerTag, void* pArg);
	void				ADD_ToLayer(_uint iLayerLevelIndex, const _wstring& strLayerTag, CGameObject* pObject);

	void				Priority_Update(_float fTimeDelta);
	void				Update(_float fTimeDelta);
	void				Late_Update(_float fTimeDelta);	

	void				Clear(_uint iLevelIndex);
	void				Clear_DeadObj();

private:
	_uint								m_iNumLevels = {};
	_int								m_iPreLevel = { -1 };

	map<const _wstring, class CLayer*>* m_pLayers = { nullptr };
	class CGameInstance*				m_pGameInstance = { nullptr };

private:
	class CLayer* Find_Layer(_uint iLayerLevelIndex, const _wstring& strLayerTag);

public:
	static CObject_Manager* Create(_uint iNumLevels);
	virtual void Free() override;

};

NS_END