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

private:
	_bool				m_bNaviEditMode = { false }; // 네비게이션 편집 모드
	_uint				m_iNaviPointCount = { 0 }; // 현재 선택된 점의 개수 (0, 1, 2)
	_vector				m_vNaviPoints[3] = {}; 
	class CNavigation*	m_pNavigation = { nullptr }; // 현재 레벨의 네비게이션 컴포넌트
	NAVI_MODE			m_eNaviMode = { NAVI_MODE::NONE }; // 현재 네비게이션 작업 모드
	_float				m_fNaviSnapRadius = { 1.5f };



public:
	virtual void Free() override;
};

NS_END