#pragma once
#include "MapTool_Defines.h"
#include "Base.h"

NS_BEGIN(Engine)
class CGameInstance;
class CCinemaData;
NS_END

NS_BEGIN(Tool_Map)
class CCamera_Tool final : public CBase
{
public  :
	static const WCHAR*			m_szCinemaComponentName;

private  :
	CCamera_Tool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CCamera_Tool() = default;

public:
	HRESULT						Initialize();

	void						Priority_Update(_float fTimeDelta);
	void						Update(_float fTimeDelta);
	void						Late_Update(_float fTimeDelta);

	HRESULT						Render();

private:
	ID3D11Device*									m_pDevice = { nullptr };
	ID3D11DeviceContext*							m_pContext = { nullptr };
	CGameInstance*									m_pGameInstance = { nullptr };
	CCinemaData*									m_pCameraAnimation = { nullptr };

	_bool											m_bIsPause = false;
	ImVec2											m_vTimeLinePos = {};
	_float2											m_fTime = {};
	_float											m_fRateTime = {};

	char											m_szSelectable[MAX_PATH] = {};
	char											m_szViewCinemaScene[MAX_PATH] = {};
	
	const unordered_map<_wstring, CCinemaData*>*	m_pSelectSceneDatas = { nullptr };
	char											m_szViewCinemaData[MAX_PATH] = {};

#pragma region ADD_CinemaScene
	_bool											m_bIsADDCinemaScene = { false };
	_char											m_szCinemaSceneTag[MAX_PATH] = {};
#pragma endregion

#pragma region ADD_CinemaData
	_bool											m_bIsAddCinemaData = { false };
	_char											m_szCinemaDataTag[MAX_PATH] = {};
#pragma endregion

#pragma region ADD_KeyFrame
	_bool											m_bIsKeyFrame = { false };

	_float											m_vRotation[3];
	_float											m_vTranslate[3];

	_float											m_fFov = {};
	_float											m_fFar = {};
	_float											m_fNear = {};
#pragma endregion

#pragma region SAVE & LOAD
	char											m_szSaveFielPath[MAX_PATH] = {};
	char											m_szLoadFielPath[MAX_PATH] = {};
#pragma endregion

#pragma region KEY_INFO
	_bool											m_bShowKeyInfo = { false };
	
	char											m_KeyFrameNum[20] = {};
	CAMERA_KEYFRAME									m_CameraKeyFrameInfo = {};
#pragma endregion


private :
	void											ADD_CinemaScene();
	void											ADD_CinemaData();
	void											ADD_KeyFrame();

	void											Show_KeyFrameInfo();
	void											Show_KeyFrame(_float fTimeDelta);

	HRESULT											Save_Cinematic_Action();
	HRESULT											Load_Cinematic_Actions();

public:
	static	CCamera_Tool*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void				Free() override;
};

NS_END