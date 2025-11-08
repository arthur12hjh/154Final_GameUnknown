#pragma once
#include "Base.h"

NS_BEGIN(Engine)
class CCamera;

class CCameraManager : public CBase
{
private:
	CCameraManager();
	virtual ~CCameraManager() = default;

public:
	HRESULT				Initialize();

	/* 원형을 찾고 복제하여 레이어에 추가한다. */
	void				Priority_Update(_float fTimeDelta);
	void				Update(_float fTimeDelta);
	void				Late_Update(_float fTimeDelta);

private:
	CCamera*								m_pMainCamera = { nullptr };
	unordered_map<_wstring, CCamera*>		m_pCameras;

public:
	static CCameraManager*			Create();
	virtual void					Free() override;

};
NS_END