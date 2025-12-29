#pragma once
#include "Client_Defines.h"
#include "ClientStruct.h"
#include "Level.h"

NS_BEGIN(Client)
class CLevel_BeatSaber : public CLevel
{
public:
	typedef struct SavedObjectInfo
	{
		_float4x4	    worldMatrix;
	}SAVEDOBJECTINFO;

private:
	CLevel_BeatSaber(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevelID);
	virtual ~CLevel_BeatSaber() = default;

public:
	virtual HRESULT				Initialize() override;
	virtual void				Update(_float fTimeDelta) override;
	virtual HRESULT				Render() override;

private :
	_float						m_fTime = 10;
	_bool						m_bChangeLevel{ false };
	_bool						m_bLevelTransitioning{ false }; // 레벨 전환 중 체크
	_bool						m_isOverlay{ true };
private:
	HRESULT						Ready_Lights();
	HRESULT						Ready_Layer_BackGround(const _wstring& strLayerTag);
	HRESULT						Ready_Layer_Terrain(const _wstring& strLayerTag);
	HRESULT						Ready_Layer_Sky(const _wstring& strLayerTag);
	HRESULT						Ready_Layer_Camera(const _wstring& strLayerTag);
	HRESULT						Ready_Layer_Player(const _wstring& strLayerTag);
	HRESULT						Ready_Layer_UI(const _wstring& strLayerTag);

	//HRESULT Load_Map_Data();
	//HRESULT Load_Map_Format(std::ifstream& ifs, const _tchar* protoTag, const _tchar* pLayerTag);
	//HRESULT Load_Instancing_By_Layer(ifstream& ifs, const _tchar* protoTag, const _tchar* pLayerTag);

	HRESULT						Load_Light_Data();

public:
	static CLevel_BeatSaber*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevelID);
	virtual void				Free() override;

};
NS_END