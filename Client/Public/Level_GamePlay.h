#pragma once

#include "Client_Defines.h"
#include "ClientStruct.h"
#include "Level.h"
#include "VIBuffer_Instance_Model.h"

NS_BEGIN(Client)

class CChangeLevelEvent;
class CTeleportEvent;

class CLevel_GamePlay final : public CLevel
{
public:
	typedef struct SavedObjectInfo
	{
		_float4x4	    worldMatrix;
		_tchar			szComponentTag[256];
		_uint			iObjectID = 0;
	}SAVEDOBJECTINFO;

	typedef struct SavedInteractionObjectInfo :SAVEDOBJECTINFO
	{
		_uint			iInteractionID = 999;
	}SAVEDINTERACTIONOBJECTINFO;

	typedef struct SavedLiftControllerInfo :SAVEDINTERACTIONOBJECTINFO
	{
		_bool			bIsControllerType = false;
		_uint			iPlatformID = 0;
		_uint 			iPosition = 0;	 // Top(0), Bottom(1)
	}SAVED_LIFT_CONTROLLER_INFO;

	typedef struct SavedLiftPlatformInfo :SavedObjectInfo
	{
		_uint			iPlatformID = 0;
		_float			fMoveDistance = 0.f;
	}SAVED_LIFT_PLATFORM_INFO;

	typedef struct SavedMonsterInfo
	{
		_float4x4	    worldMatrix;
		_tchar			szComponentTag[256];
		_uint			iMonsterId = 0;
	}SAVEDMONSTERINFO;

	typedef struct SavedNpcInfo
	{
		_float4x4	    worldMatrix;
		_tchar			szComponentTag[256];
		_uint			iNpcID = 0;
	}SAVEDNPCINFO;

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
	HRESULT Ready_Layer_NPC(const _wstring& strLayerTag);
	HRESULT Ready_Layer_UI(const _wstring& strLayerTag);
	HRESULT Ready_Layer_Trigger(const _wstring& strLayerTag);

	HRESULT Load_Map_Desert_Data(const _char* szFilePath);
	HRESULT Load_Monster_Desert_Data(const _char* szFilePath);
	
	HRESULT Load_Map_Desert_Format(std::ifstream& ifs, const _tchar* protoTag, const _tchar* pLayerTag);

	HRESULT Load_Interaction_Objects_By_Layer(std::ifstream& ifs, const _tchar* protoTag, const _tchar* pLayerTag, _bool bIsGamePlay = false);
	HRESULT Load_Lift_Controller_By_Layer(ifstream& ifs, const _tchar* protoTag, const _tchar* pLayerTag);
	HRESULT Load_Lift_Platform_By_Layer(ifstream& ifs, const _tchar* protoTag, const _tchar* pLayerTag);

	HRESULT Load_Monster_Desert_Format(std::ifstream& ifs, const _tchar* protoTag, const _tchar* pLayerTag);
	HRESULT Load_Npc_Desert_Format(std::ifstream& ifs, const _tchar* protoTag, const _tchar* pLayerTag);
	HRESULT Load_Instancing_By_Layer(ifstream& ifs, const _tchar* pLayerTag);

	HRESULT Load_Level_CinematicObjectData(const _char* szFilePath);
	HRESULT Load_Light_Data();

	//void	Setting_Secret_Corpse();

	_float	m_fTime = 10;
	_bool m_isOverlay{ true };

	CChangeLevelEvent* m_pLevelChangeEvent{ nullptr };
	CTeleportEvent* m_pTeleportEvent{ nullptr };

	_bool m_bChangeLevel{ false };
	_bool m_bTeleport{ false };
	_bool m_bLevelTransitioning{ false }; // 레벨 전환 중 체크

public:
	static CLevel_GamePlay* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevelID);
	virtual void Free() override;
};

NS_END