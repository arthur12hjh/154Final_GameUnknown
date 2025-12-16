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
	void				Priority_Update(_float fTimeDelta);
	void				Update(_float fTimeDelta);
	void				Late_Update(_float fTimeDelta);

public :
	void				Shake(_float fShakeTime, _float fIntensity);

	HRESULT				Add_Camera(const WCHAR* szCameraTag, CCamera* pCamera);
	HRESULT				Remove_Camera(const WCHAR* szCameraTag);

	HRESULT				SetMainCamera(const WCHAR* szCameraTag, _float4x4* pPreCameraMatrix);

	CCamera*			GetCamrea(const WCHAR* szCameraTag);
	const unordered_map<_wstring, CCamera*>* GetAllCamera() { return &m_pCameras; }
	
	CCamera*			GetMainCamera();
	_matrix				GetMainCameraWorldMatrix();
	const _float4x4*	GetMainCameraWorldMatrixPtr();
	
	_matrix				GetCameraWorldMatrix(const WCHAR* szCameraTag);
	const _float4x4*	GetCameraWorldMatrixPtr(const WCHAR* szCameraTag);

	_bool				IsMainCamera(CCamera* pCamera);
	void				Clear_Cameras();
	void				Clear_DeadCameras();

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