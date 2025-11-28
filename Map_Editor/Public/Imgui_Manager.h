#pragma once

#include "MapTool_Defines.h"
#include "Base.h"

NS_BEGIN(Engine)
class CGameInstance;
class CTransform;
class CGameObject;
class CNavigation;
class CCell;
class CModel;
class CLight;

NS_END

NS_BEGIN(Tool_Map)

class CImgui_Manager final : public CBase
{
	DECLARE_SINGLETON(CImgui_Manager)

	typedef struct SavedObjectInfo
	{
		_float4x4	    worldMatrix;
	}SAVEDOBJECTINFO;

private:
	CImgui_Manager();
	virtual ~CImgui_Manager() = default;

public:
	HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void Priority_Update(_float fTimeDelta);
	void Update(_float fTimeDelta);
	void Late_Update(_float fTimeDelta);
	HRESULT Render();
	HRESULT Render_MainMenu();

	void Set_Terrain_Scarlet(class CTerrain* pTerrain) { m_pTerrain = pTerrain; }
	void Set_Terrain_Desert(class CTerrain_Desert* pTerrain) { m_pTerrain_Desert = pTerrain; }

	HRESULT Create_MapTool_For_Level(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	HRESULT Create_MapTool_For_Desert(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	// CLevel_Village에서 호출하여 MapTool을 해제합니다.
	void Destroy_MapTool_For_Level();
	void Destroy_MapTool_Desert_For_Level();
	class CMapTool* Get_MapTool() const { return m_pMapTool; }
	class CMapTool_Desert* Get_MapTool_Desert() const { return m_pMapTool_Desert; }
private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	CGameInstance* m_pGameInstance = { nullptr };
	CTransform* m_pCameraTransform = { nullptr };
	CTransform* m_pTransform = { nullptr };

	CGameObject* m_pLastAddedObject = { nullptr };
	CGameObject* m_pPickedObject = { nullptr };

	class CLightTool*		m_pLightTool = { nullptr };
	class CMapTool*			m_pMapTool = { nullptr };
	class CMapTool_Desert*	m_pMapTool_Desert = { nullptr };
	class CCamera_Tool*		m_pCameraTool = { nullptr };

	_bool m_bIsMapToolActive = false;
	_bool m_bIsMapToolDesertActive = false;
	_bool m_bIsLightToolActive = false;
	_bool m_bIsCameraToolActive = false;

	const _tchar*		m_CurrentLayerName = {};
	list<CGameObject*>* m_pObjects = { nullptr };
	CGameObject*		m_pObject = { nullptr };

	TOOL_MODE			m_eToolMode = { TOOL_MODE::END };

	_uint				m_iCurrentLevel = { ENUM_CLASS(LEVEL::END) };

private:
	class CPlayer*			m_pPlayer = { nullptr };
	class CTerrain*			m_pTerrain = { nullptr };
	class CTerrain_Desert*	m_pTerrain_Desert = { nullptr };

	_bool				m_bIsDeplayMode = { false };
	_bool				m_bIsMapMode = { false };
	_bool				m_bIsDragging = { false };


public:
	virtual void Free() override;
};

NS_END