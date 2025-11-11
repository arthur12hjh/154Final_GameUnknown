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

	HRESULT Map_Tool_Render();

	void Update_Rotation();
	HRESULT Save_Map_Objects();
	HRESULT Load_Map_Objects();


	void Set_NaviEditMode(_bool bMode); // 네비게이션 편집 모드 On/Off
	void Add_NaviPoint(_fvector vPickedPoint); // 클릭된 지점을 네비게이션 포인트로 등록
	void Reset_NaviPoints(); // 현재까지 선택된 네비게이션 포인트를 리셋
	HRESULT Add_NaviCell(); // 3개의 점으로 네비게이션 셀을 생성 및 CNavigation에 추가
	HRESULT Save_NavigationData(); // 현재 CNavigation 데이터를 파일로 저장
	void Delete_Latest_NaviCell(); // 가장 마지막에 추가된 셀 삭제

	CGameObject* Find_Object_To_Pick(_uint iLevelIndex, const _wstring& strLayerTag, _float3* pPickedPoint);
	void Compute_Picking_Ray(_float3* pRayOrigin, _float3* pRayDir);
	_bool Intersect_Ray_Sphere(_fvector vRayOrigin, _fvector vRayDir, _fvector vSphereCenter, _float fRadius, _float* pDistance);

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	CGameInstance* m_pGameInstance = { nullptr };
	CTransform* m_pCameraTransform = { nullptr };
	CTransform* m_pTransform = { nullptr };

	CGameObject* m_pLastAddedObject = { nullptr };
	CGameObject* m_pPickedObject = { nullptr };

	class CLightTool* m_pLightTool = { nullptr };
	class CMapTool* m_pMapTool = { nullptr };

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


	// 빛 diffuse
	_float m_fDiffuseX = { 0.f };
	_float m_fDiffuseY = { 0.f };
	_float m_fDiffuseZ = { 0.f };

	// 빛 ambient
	_float m_fAmbientX = { 0.f };
	_float m_fAmbientY = { 0.f };
	_float m_fAmbientZ = { 0.f };

	// 빛 specular
	_float m_fSpecularX = { 0.f };
	_float m_fSpecularY = { 0.f };
	_float m_fSpecularZ = { 0.f };

	// 빛 Direction
	_float m_fDirectionX = { 0.f };
	_float m_fDirectionY = { 0.f };
	_float m_fDirectionZ = { 0.f };

	// 빛 range
	_float m_fRange = { 0.f };

	ADD_OBJECT m_eCurrentObject = {};

	const _tchar*		m_CurrentLayerName = {};
	list<CGameObject*>* m_pObjects = { nullptr };
	CGameObject*		m_pObject = { nullptr };
	const list<CLight*>* m_pLights = { nullptr };
	list<string>		m_LightNames = {};

	TOOL_MODE			m_eToolMode = { TOOL_MODE::END };

private:
	CNavigation*		m_pNavigation = { nullptr }; // 현재 레벨의 네비게이션 컴포넌트
	
	class CVillage*		m_pVillage = { nullptr };
	class CPlayer*		m_pPlayer = { nullptr };
	class CTerrain*		m_pTerrain = { nullptr };

	_vector				m_vNaviPoints[3] = {}; 
	_float				m_fNaviSnapRadius = { 1.5f };
	_uint				m_iNaviPointCount = { 0 }; // 현재 선택된 점의 개수 (0, 1, 2)
	NAVI_MODE			m_eNaviMode = { NAVI_MODE::NONE }; // 현재 네비게이션 작업 모드
	_bool				m_bIsNaviEditMode = { false }; // 네비게이션 편집 모드


	_bool				m_bIsDeplayMode = { false };
	_bool				m_bIsMapMode = { false };
	_bool				m_bIsDragging = { false };


public:
	virtual void Free() override;
};

NS_END