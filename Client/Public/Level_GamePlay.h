#pragma once

#include "Client_Defines.h"
#include "ClientStruct.h"
#include "Level.h"
#include "VIBuffer_Instance_Model.h"

NS_BEGIN(Client)

class CLevel_GamePlay final : public CLevel
{
public:
	typedef struct SavedObjectInfo
	{
		_float4x4	    worldMatrix;
		_tchar			szComponentTag[256];
	}SAVEDOBJECTINFO;

	typedef struct tagModelInstanceLoadDesc
	{
		_uint iNumInstance = 0;
		vector<VTX_INSTANCE_MODEL> InstancingData;
		CVIBuffer_Instance_Model::MODEL_INSTANCE_DESC InstanceDesc;

	}MODEL_INSTANCE_LOAD_DESC;

private:
	CLevel_GamePlay(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevelID);
	virtual ~CLevel_GamePlay() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	void		FontRender();

private:
	HRESULT Ready_Lights();
	HRESULT Ready_Layer_BackGround(const _wstring& strLayerTag);
	HRESULT Ready_Layer_Terrain(const _wstring& strLayerTag);
	HRESULT Ready_Layer_Sky(const _wstring& strLayerTag);
	HRESULT Ready_Layer_Camera(const _wstring& strLayerTag);
	HRESULT Ready_Layer_Player(const _wstring& strLayerTag);
	HRESULT Ready_Layer_Monster(const _wstring& strLayerTag);
	HRESULT Ready_Layer_Effect(const _wstring& strLayerTag);
	HRESULT Ready_Layer_UI(const _wstring& strLayerTag);

	HRESULT Load_Map_Desert_Data(const _char* szFilePath);
	HRESULT Load_Map_Desert_Format(std::ifstream& ifs, const _tchar* protoTag, const _tchar* pLayerTag);
	HRESULT Load_Instancing_By_Layer(ifstream& ifs, const _tchar* pLayerTag);


	HRESULT Load_Light_Data();
	_float	m_fTime = 10;
	_bool m_bChangeLevel{ false };

public:
	static CLevel_GamePlay* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevelID);
	virtual void Free() override;
};

NS_END