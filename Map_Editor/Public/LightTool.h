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

class CLightTool final : public CBase
{
	typedef struct SavedObjectInfo
	{
		_float4x4	    worldMatrix;
	}SAVEDOBJECTINFO;

public:
	CLightTool();
	virtual ~CLightTool() = default;

public:
	HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void Priority_Update(_float fTimeDelta);
	void Update(_float fTimeDelta);
	void Late_Update(_float fTimeDelta);
	HRESULT Render();

	HRESULT Save_Light_Objects();
	HRESULT Load_Light_Objects();

	void Update_Light_Properties(); 
	void Load_Selected_Light_Desc();

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	CGameInstance* m_pGameInstance = { nullptr };
	CTransform* m_pTransform = { nullptr };

	// ¿ÀºêÁ§Æ® È¸Àü¿ë
	_float m_fRotX = { 0.f };
	_float m_fRotY = { 0.f };
	_float m_fRotZ = { 0.f };

	// ºû diffuse
	_float m_fDiffuseX = { 0.f };
	_float m_fDiffuseY = { 0.f };
	_float m_fDiffuseZ = { 0.f };

	// ºû ambient
	_float m_fAmbientX = { 0.f };
	_float m_fAmbientY = { 0.f };
	_float m_fAmbientZ = { 0.f };

	// ºû specular
	_float m_fSpecularX = { 0.f };
	_float m_fSpecularY = { 0.f };
	_float m_fSpecularZ = { 0.f };

	// ºû Direction
	_float m_fDirectionX = { 0.f };
	_float m_fDirectionY = { 0.f };
	_float m_fDirectionZ = { 0.f };

	// ºû range
	_float m_fRange = { 0.f };

	const list<CLight*>* m_pLights = { nullptr };
	list<string>		m_LightNames = {};
	class CLight*		m_pSelectedLight = { nullptr };

	TOOL_MODE			m_eToolMode = { TOOL_MODE::END };

private:
	class CPlayer* m_pPlayer = { nullptr };
	class CTerrain* m_pTerrain = { nullptr };

	_bool				m_bIsDeplayMode = { false };
	_bool				m_bIsMapMode = { false };
	_bool				m_bIsDragging = { false };

	// ºû
	_int				m_iSelectedLightIndex = {-1};

	_bool				m_bIsDeplayPointLight = { false };
	_bool				m_bIsDeplayDirLight = { false };
	_bool				m_bIsLightMode = { false };


public:
	virtual void Free() override;
};

NS_END