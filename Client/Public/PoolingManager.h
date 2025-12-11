#pragma once

#include "Client_Defines.h"
#include "Base.h"

NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(Client)
class CPoolingManager final : public CBase
{
private :
	CPoolingManager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CPoolingManager() = default;

public :
	HRESULT											Setting_PoolManager(_uint iLevelID);
	// 오브젝트를 반환해서 Layer에 넣고 오브젝트를 반환한다.
	CGameObject*									SetActivePoolObject(_uint iLevel, const WCHAR* pLayerName, const WCHAR* szPoolTag);

	// 오브젝트를 다시 풀에다가 넣어준다.
	void											UnActivePoolObject(const WCHAR* szPoolTag, CGameObject* pObject);
	
private :
	ID3D11Device*									m_pDevice = { nullptr };
	ID3D11DeviceContext*							m_pContext = { nullptr };
	CGameInstance*									m_pGameInstance = { nullptr };

	// 풀객체를 저장할 녀석
	map<const WCHAR*, vector<CGameObject*>>			m_PoolingList = {};

public :
	HRESULT											Ready_GamePlayPool();
	HRESULT											Clear_PoolManager();

public :
	static		CPoolingManager*					Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual		void								Free() override;
};
NS_END