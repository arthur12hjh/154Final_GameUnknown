#pragma once

#include "Client_Defines.h"
#include "GameStruct.h"
#include "Base.h"

NS_BEGIN(Engine)
class CGameInstance;
class CTransform;
NS_END

NS_BEGIN(Client)

class CCinematicObject;
class CCamera_Action;

class CCinematicManager : public CBase
{
private:
	CCinematicManager();
	virtual ~CCinematicManager() = default;

public:
	HRESULT						Initialize();
	HRESULT						Update(_float fTimeDelta);

	HRESULT						Play_Cinematic(_uint iCinematicID, function<void()> FinishedFunc);

	HRESULT						Emplace_CinematicObject(CCinematicObject* pObject);
	HRESULT						Emplace_ActionCamera(const _tchar* strCameraTag);

	HRESULT						Load_Level_CinematicObjectData(const _char* szFilePath);

	HRESULT						Change_MainCamera();

	HRESULT						Skip_Cinematic();
	_bool						Is_CinematicPlaying() const { return m_bIsCinematicPlaying; }

private:
	CGameInstance*				m_pGameInstance = { nullptr };
	class CPlayer*				m_pPlayer = { nullptr };
	PLAYER_DESC*				m_pPlayerDesc = { nullptr };

	// 시네마틱 전용 데이터
	map<_uint, CINEMATIC_DESC>* m_pCinematicDatas = { nullptr };

	// 시네마틱에 실행하는 객체들 데이터
	map<_wstring, CCinematicObject*>					m_CinematicObjectsMap;
	map<_wstring, CCamera_Action*>						m_ActionCameraMap;

	_float						m_fCinematicTimer = 0.f;
	_uint						m_iCurrentCinematicID = -1;
	_uint						m_iCurrentCinematicNodeIndex = 0;
	CINEMATIC_DESC*				m_pCurrentCinematicDesc = { nullptr };
	_bool						m_bIsCinematicPlaying = false;
	_bool						m_bIsCinematicSkip = false;
	function<void()>			m_FinishedCinematic = {nullptr};


private:
	void						Play_Node(const CINEMATIC_NODE_DESC& CinematicNodeDesc);

	HRESULT						Reset_Cinematic();

public:
	static CCinematicManager* Create();
	virtual void Free() override;
};

NS_END