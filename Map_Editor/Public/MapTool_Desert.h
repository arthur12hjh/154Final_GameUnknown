#pragma once

#include "MapTool_Defines.h"
#include "Base.h"
#include "VIBuffer_Instance_Model.h"
#include "GameObject.h"

NS_BEGIN(Engine)
//class CVIBuffer_Instance_Model;
class CGameInstance;
class CTransform;
class CGameObject;
class CNavigation;
class CCell;
class CModel;
class CLight;
class CComponent;
NS_END

NS_BEGIN(Tool_Map)

class CMapTool_Desert final : public CBase
{
public:
	typedef struct SavedObjectInfo
	{
		_float4x4	    worldMatrix;
		_tchar			szComponentTag[256];
		_uint			iObjectID = 0;	
	}SAVEDOBJECTINFO;

	typedef struct SavedInteractionObjectInfo:SAVEDOBJECTINFO
	{
		_uint			iInteractionID = 999;
	}SAVEDINTERACTIONOBJECTINFO;

	typedef struct SavedLiftControllerInfo:SAVEDINTERACTIONOBJECTINFO
	{
		_bool			bIsControllerType = false;
		_uint			iPlatformID = 0;
		_uint 			iPosition = 0;
	}SAVED_LIFT_CONTROLLER_INFO;

	typedef struct SavedLiftPlatformInfo:SavedObjectInfo
	{
		_uint			iPlatformID = 0;
		_float			fMoveDistance = 0.f;
	}SAVED_LIFT_PLATFORM_INFO;

	typedef struct SavedMonsterInfo
	{
		_float4x4	    worldMatrix;
		_tchar			szComponentTag[256];
		_uint			iMonsterID = 0;
	}SAVEDMONSTERINFO;

	typedef struct SavedNpcInfo
	{
		_float4x4	    worldMatrix;
		_tchar			szComponentTag[256];
		_uint			iNpcID = 0;
	}SAVEDNPCINFO;

	typedef struct SavedDororongSaberInfo
	{
		_float4x4	    worldMatrix;
	}SAVEDDORORONGSABERINFO;

	typedef struct SavedSoundTriggerBoxInfo
	{
		_float4x4			worldMatrix;
		GROUND_SOUND_TYPE	eType = GROUND_SOUND_TYPE::END;
	}SAVED_SOUND_TRIGGER_BOX_INFO;

	typedef struct SavedCameraTriggerBoxInfo
	{
		_float4x4			worldMatrix;
		CAMERA_TYPE			eType = CAMERA_TYPE::END;
	}SAVED_CAMERA_TRIGGER_BOX_INFO;

	typedef struct SavedMusicTriggerBoxInfo
	{
		_float4x4			worldMatrix;
	}SAVED_MUSIC_TRIGGER_BOX_INFO;

	typedef struct tagModelInstanceLoadDesc
	{
		_uint iNumInstance = 0;
		vector<VTX_INSTANCE_MODEL> InstancingData;
		CVIBuffer_Instance_Model::MODEL_INSTANCE_DESC InstanceDesc;

	}MODEL_INSTANCE_LOAD_DESC;

	/*typedef struct Nayitba_Desc : public CGameObject::GAMEOBJECT_DESC
	{
		_uint						iMonsterID = {};
	}NAYITBA_DESC;*/

public:
	CMapTool_Desert();
	virtual ~CMapTool_Desert() = default;

public:
	HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void Priority_Update(_float fTimeDelta);
	void Update(_float fTimeDelta);
	void Late_Update(_float fTimeDelta);
	HRESULT Render();

	void Update_Rotation();
	HRESULT Save_Map_Objects(const _char* szFilePath);
	HRESULT Save_Monster_Objects(const _char* szFilePath);
	HRESULT Save_Dororong_Saber_Objects(const _char* szFilePath);
	HRESULT Save_Sound_Trigger_Box_Objects(const _char* szFilePath);

	HRESULT Save_Objects_By_Layer(ofstream& ofs, const _tchar* pLayerTag);
	HRESULT Save_Interaction_Objects_By_Layer(ofstream& ofs, const _tchar* pLayerTag);
	HRESULT Save_Lift_Controller_By_Layer(ofstream& ofs, const _tchar* pLayerTag);
	HRESULT Save_Lift_Platform_By_Layer(ofstream& ofs, const _tchar* pLayerTag);
	HRESULT Save_Monsters_By_Layer(ofstream& ofs, const _tchar* pLayerTag);
	HRESULT Save_Npcs_By_Layer(ofstream& ofs, const _tchar* pLayerTag);
	HRESULT Save_Dororong_Saber_By_Layer(ofstream& ofs, const _tchar* pLayerTag);
	HRESULT Save_Sound_Trigger_Box_By_Layer(ofstream& ofs, const _tchar* pLayerTag);
	HRESULT Save_Camera_Trigger_Box_By_Layer(ofstream& ofs, const _tchar* pLayerTag);
	HRESULT Save_Music_Trigger_Box_By_Layer(ofstream& ofs, const _tchar* pLayerTag);


	HRESULT Load_Map_Objects(const _char* szFilePath);
	HRESULT Load_Monster_Objects(const _char* szFilePath);
	HRESULT Load_Dororong_Saber_Objects(const _char* szFilePath);
	HRESULT Load_Sound_Trigger_Box_Objects(const _char* szFilePath);

	HRESULT Load_Objects_By_Layer(ifstream& ifs, const _tchar* protoTag, const _tchar* pLayerTag);
	HRESULT Load_Interaction_Objects_By_Layer(ifstream& ifs, const _tchar* protoTag, const _tchar* pLayerTag);
	HRESULT Load_Lift_Controller_By_Layer(ifstream& ifs, const _tchar* protoTag, const _tchar* pLayerTag);
	HRESULT Load_Lift_Platform_By_Layer(ifstream& ifs, const _tchar* protoTag, const _tchar* pLayerTag);
	HRESULT Load_Monsters_By_Layer(ifstream& ifs, const _tchar* protoTag, const _tchar* pLayerTag);
	HRESULT Load_Npcs_By_Layer(ifstream& ifs, const _tchar* protoTag, const _tchar* pLayerTag);
	HRESULT Load_Instancing_By_Layer(ifstream& ifs, const _tchar* pLayerTag);
	HRESULT Load_Dororong_Saber_By_Layer(ifstream& ifs, const _tchar* protoTag, const _tchar* pLayerTag);
	HRESULT Load_Sound_Trigger_Box_By_Layer(ifstream& ifs, const _tchar* protoTag, const _tchar* pLayerTag);
	HRESULT Load_Camera_Trigger_Box_By_Layer(ifstream& ifs, const _tchar* protoTag, const _tchar* pLayerTag);
	HRESULT Load_Music_Trigger_Box_By_Layer(ifstream& ifs, const _tchar* protoTag, const _tchar* pLayerTag);
	
	void Delete_All_Before_Load(const _tchar* pLayerTag);

	HRESULT Save_Terrain_HeightMap(const _char* szHeightMapFilePath);

	HRESULT Save_MaskMap(const _char* szFilePath);

	HRESULT Set_LoadMaskMap(const _char* szFilePath);
	HRESULT Set_NewMaskMap();
	void Change_MaskMap_Black(_float3 vPickedPoint);
	void Change_MaskMap_Red(_float3 vPickedPoint);
	void Change_MaskMap_Green(_float3 vPickedPoint);
	void Change_MaskMap_Blue(_float3 vPickedPoint);

	void Set_Terrain(class CTerrain_Desert* pTerrain) {
		m_pTerrain = pTerrain;
	}

	void Set_NaviEditMode(_bool bMode); // 네비게이션 편집 모드 On/Off
	void Add_NaviPoint(_fvector vPickedPoint); // 클릭된 지점을 네비게이션 포인트로 등록
	void Reset_NaviPoints(); // 현재까지 선택된 네비게이션 포인트를 리셋
	HRESULT Add_NaviCell(); // 3개의 점으로 네비게이션 셀을 생성 및 CNavigation에 추가
	HRESULT Save_NavigationData(); // 현재 CNavigation 데이터를 파일로 저장
	void Delete_Latest_NaviCell(); // 가장 마지막에 추가된 셀 삭제

	CGameObject* Find_Object_To_Pick(_uint iLevelIndex, const _wstring& strLayerTag, _float3* pPickedPoint);
	void Compute_Picking_Ray(_float3* pRayOrigin, _float3* pRayDir);
	_bool Intersect_Ray_Sphere(_fvector vRayOrigin, _fvector vRayDir, _fvector vSphereCenter, _float fRadius, _float* pDistance);
	CComponent* m_pSelectComponent = { nullptr };

public:
	ID3D11ShaderResourceView* Get_MaskSRV() { return m_pMaskSRV; }


private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	CGameInstance* m_pGameInstance = { nullptr };

	CTransform* m_pCameraTransform = { nullptr };
	CTransform* m_pPlayerTransform = { nullptr };

	CGameObject* m_pLastAddedObject = { nullptr };
	CGameObject* m_pPickedObject = { nullptr };

	ID3D11Texture2D* m_pMaskTexture2D = { nullptr };
	ID3D11Texture2D* m_pRenderMaskTexture2D = { nullptr };

	ID3D11ShaderResourceView* m_pMaskSRV = { nullptr };

	// 카메라 이동할 좌표
	_float m_fX = { 0.f };
	_float m_fY = { 0.f };
	_float m_fZ = { 0.f };

	// 오브젝트 회전용
	_float m_fRotX = { 0.f };
	_float m_fRotY = { 0.f };
	_float m_fRotZ = { 0.f };

	// 오브젝트 스케일용
	_float m_fScaleX = { 0.f };
	_float m_fScaleY = { 0.f };
	_float m_fScaleZ = { 0.f };

	_float m_fHeight = { 0.f };
	_float m_fRadius = { 2.f };
	_float m_fMaxHeight = { 0.f };
	_float m_fSmoothFactor = { 0.f };
	_float m_fMoveSpeed = { 3.f };

	DESESRT_RUIN_OBJECT m_eCurrentObject = {};

	const _tchar* m_CurrentLayerName = {};
	const _tchar* m_ComponentTag = {};
	list<CGameObject*>* m_pObjects = { nullptr };
	CGameObject* m_pObject = { nullptr };


	// 리프트 변수용 
	_float m_fPlatformMoveDistance = { 0.f };
	_int m_iPlatformID = { 0 };
	_int m_iPosition = { 0 };
	_bool m_bIsLiftControllerType = { false };

	_bool m_bShowLiftControllerWindow = { false };
	_bool m_bShowLiftPlatformWindow = { false };

	TOOL_MODE			m_eToolMode = { TOOL_MODE::END };
	DESERT_THEME		m_eCurrentMap = { DESERT_THEME::END };

private:
	CNavigation* m_pNavigation = { nullptr }; // 현재 레벨의 네비게이션 컴포넌트

	class CPlayer_Test*			m_pPlayer = { nullptr };
	class CTerrain_Desert*	m_pTerrain = { nullptr };
	class CCamera_Free* m_pCamera = { nullptr };


	_vector				m_vNaviPoints[3] = {};
	_float				m_fNaviSnapRadius = { 2.f };
	_uint				m_iNaviPointCount = { 0 }; // 현재 선택된 점의 개수 (0, 1, 2)
	NAVI_MODE			m_eNaviMode = { NAVI_MODE::NONE }; // 현재 네비게이션 작업 모드
	_bool				m_bIsNaviEditMode = { false }; // 네비게이션 편집 모드

	_bool				m_bIsDeplayMode = { false };
	_bool				m_bIsMapMode = { false };
	_bool				m_bIsDragging = { false };

	ID3D11Texture2D* m_pMaskRenderTexture = { nullptr };

	vector<vector<VTX_INSTANCE_MODEL>*> m_vecInstancingData;

public:
	virtual void Free() override;
};

NS_END