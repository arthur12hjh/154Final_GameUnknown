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

	/* ������ ã�� �����Ͽ� ���̾ �߰��Ѵ�. */
	void				Priority_Update(_float fTimeDelta);
	void				Update(_float fTimeDelta);
	void				Late_Update(_float fTimeDelta);

public :
	HRESULT				Add_Camera(const WCHAR* szCameraTag, CCamera* pCamera);
	HRESULT				Remove_Camera(const WCHAR* szCameraTag);

	HRESULT				SetMainCamera(const WCHAR* szCameraTag, _float4x4* pPreCameraMatrix);

	//	ī�޶� �Ŵ������� ī�޶� ������ ������ ���۷��� ī��Ʈ ������
	//  ���������� �����ּ���
	CCamera*			GetCamrea(const WCHAR* szCameraTag);
	const unordered_map<_wstring, CCamera*>* GetAllCamera() { return &m_pCameras; }
	

	//	ī�޶� �Ŵ������� ī�޶� ������ ������ ���۷��� ī��Ʈ ������
	//  ���������� �����ּ���
	CCamera*			GetMainCamera();

	//����ī�޶� ���� ��� ��������
	_matrix				GetMainCameraWorldMatrix();
	const _float4x4*	GetMainCameraWorldMatrixPtr();
	
	_matrix				GetCameraWorldMatrix(const WCHAR* szCameraTag);
	const _float4x4*	GetCameraWorldMatrixPtr(const WCHAR* szCameraTag);

	_bool				IsMainCamera(CCamera* pCamera);
	void				Clear_Cameras();

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