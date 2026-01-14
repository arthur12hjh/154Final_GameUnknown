#pragma once

#include "Client_Defines.h"
#include "Base.h"

NS_BEGIN(Engine)
class CGameInstance;
class CGameObject;
NS_END

NS_BEGIN(Client)
class CPoolingManager final : public CBase
{
private :
	CPoolingManager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CPoolingManager() = default;

public :
	// 이건 세팅하는 함수
	HRESULT											Setting_PoolManager(_uint iLevelID);

	// 오브젝트 추가
	HRESULT											ADD_PoolManager(_uint iLevelID, _uint iProtoTypeLevel, const WCHAR* ProtoTypeName, void* pArg, const WCHAR* szPoolTag, _uint iCount);

	// 오브젝트를 반환해서 Layer에 넣고 오브젝트를 반환한다.
	CGameObject*									SetActivePoolObject(_uint iLevelID, _uint iProtoTypeLevel, const WCHAR* pLayerName, const WCHAR* szPoolTag);

	// 오브젝트를 다시 풀에다가 넣어준다.
	void											UnActivePoolObject(_uint iLevelID, const WCHAR* szPoolTag, CGameObject* pObject);
	
private :
	ID3D11Device*									m_pDevice = { nullptr };
	ID3D11DeviceContext*							m_pContext = { nullptr };
	CGameInstance*									m_pGameInstance = { nullptr };

	// 풀객체를 저장할 녀석
	map<const WCHAR*, vector<CGameObject*>>			m_PoolingList[ENUM_CLASS(LEVEL::END)] = {};

public :
	HRESULT											Ready_GamePlayPool();
	HRESULT											Ready_BeatSaberPool();
	HRESULT											Clear_PoolManager(_uint iLevelID);
	
public :
	static		CPoolingManager*					Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual		void								Free() override;
};
NS_END