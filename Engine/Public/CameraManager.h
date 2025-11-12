#pragma once
#include "Base.h"

NS_BEGIN(Engine)
class CCamera;

class CCameraManager : public CBase
{
private:
	CCameraManager() = default;
	virtual ~CCameraManager() = default;

public:
	HRESULT				Initialize();

	/* 원형을 찾고 복제하여 레이어에 추가한다. */
	void				Priority_Update(_float fTimeDelta);
	void				Update(_float fTimeDelta);
	void				Late_Update(_float fTimeDelta);

public :
	HRESULT				Add_Camera(const WCHAR* szCameraTag, CCamera* pCamera);
	HRESULT				Remove_Camera(const WCHAR* szCameraTag);

	HRESULT				SetMainCamera(const WCHAR* szCameraTag, _float4x4* pPreCameraMatrix);

	//	카메라 매니저에서 카메라 포인터 받으면 래퍼런스 카운트 증가함
	//  가져갔으면 내려주세요
	CCamera*			GetCamrea(const WCHAR* szCameraTag);
	const unordered_map<_wstring, CCamera*>* GetAllCamera() { return &m_pCameras; }
	

	//	카메라 매니저에서 카메라 포인터 받으면 래퍼런스 카운트 증가함
	//  가져갔으면 내려주세요
	CCamera*			GetMainCamera();

	//메인카메라 월드 행렬 가져오기
	_matrix				GetMainCameraWorldMatrix();
	const _float4x4*	GetMainCameraWorldMatrixPtr();
	
	_matrix				GetCameraWorldMatrix(const WCHAR* szCameraTag);
	const _float4x4*	GetCameraWorldMatrixPtr(const WCHAR* szCameraTag);

private:
	CCamera*								m_pMainCamera = { nullptr };
	unordered_map<_wstring, CCamera*>		m_pCameras;

private :
	CCamera*						Find_Camera(const WCHAR* szCameraTag);


public:
	static CCameraManager*			Create();
	virtual void					Free() override;

};
NS_END