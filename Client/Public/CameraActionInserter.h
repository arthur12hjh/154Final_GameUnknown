#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CLight;
class CCamera;
NS_END

NS_BEGIN(Client)
class CGameManager;
class CImGuiManager;

class CCameraActionInserter : public CGameObject
{
private:
	CCameraActionInserter(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CCameraActionInserter() = default;

public:
	HRESULT						Initialize();

	void						Update(_float fTimeDeleta);
	HRESULT						Render();

private:
#ifdef _DEBUG
	CGameManager* m_pGameManager = nullptr;
	CImGuiManager* m_pImGuiManager = nullptr;
	
	CCamera* m_pSelectCamera = nullptr;

	map<_uint, CAMERA_ANIMATION_DATA>* m_pCameraAnimationDatas = { nullptr };

#endif // _DEBUG

private:
	void						WriteCameraActionDesc();

	void						Play_CameraAnimationData(int iSelectedPlayCameraAnimationIndex);

public:
	static	CCameraActionInserter* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

	virtual CGameObject* Clone(void* pArg) override;
	virtual void				Free() override;
};
NS_END