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

	void Update_Rotation();
	HRESULT Save_Map_Objects();
	HRESULT Load_Map_Objects();


	void Set_NaviEditMode(_bool bMode); // 네비게이션 편집 모드 On/Off
	void Add_NaviPoint(_fvector vPickedPoint); // 클릭된 지점을 네비게이션 포인트로 등록
	void Reset_NaviPoints(); // 현재까지 선택된 네비게이션 포인트를 리셋
	HRESULT Add_NaviCell(); // 3개의 점으로 네비게이션 셀을 생성 및 CNavigation에 추가
	HRESULT Save_NavigationData(); // 현재 CNavigation 데이터를 파일로 저장
	void Delete_Latest_NaviCell(); // 가장 마지막에 추가된 셀 삭제
	_bool Picking(_float3 vPickPos);

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	CGameInstance* m_pGameInstance = { nullptr };
	CTransform* m_pCameraTransform = { nullptr };
	CTransform* m_pTransform = { nullptr };

	CGameObject* m_pLastAddedObject = { nullptr };

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

	ADD_OBJECT m_eCurrentObject = {};

	const _tchar* m_CurrentLayerName = {};

private:

	CNavigation*		m_pNavigation = { nullptr }; // 현재 레벨의 네비게이션 컴포넌트
	class CVillage*		m_pVillage = { nullptr };
	_vector				m_vNaviPoints[3] = {}; 
	_float				m_fNaviSnapRadius = { 1.5f };
	_uint				m_iNaviPointCount = { 0 }; // 현재 선택된 점의 개수 (0, 1, 2)
	NAVI_MODE			m_eNaviMode = { NAVI_MODE::NONE }; // 현재 네비게이션 작업 모드
	_bool				m_bIsNaviEditMode = { false }; // 네비게이션 편집 모드



public:
	virtual void Free() override;
};

NS_END