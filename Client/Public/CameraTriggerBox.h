#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

NS_BEGIN(Client)
class CCameraTriggerBox final : public CGameObject
{
public:
	typedef struct TriggerBoxDesc : public GAMEOBJECT_DESC
	{
		CAMERA_TYPE eCameraType;
	}CAMERA_TRIGGER_BOX_DESC;

private:
	CCameraTriggerBox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCameraTriggerBox(const CCameraTriggerBox& Prototype);
	virtual ~CCameraTriggerBox() = default;

public:
	virtual HRESULT				Initialize_Prototype() override;
	virtual HRESULT				Initialize(void* pArg) override;

	virtual void				Priority_Update(_float fTimeDelta) override;
	virtual void				Update(_float fTimeDelta) override;
	virtual void				Late_Update(_float fTimeDelta) override;

	virtual HRESULT				Render() override;

private:
	CCollider*					m_pColliderCom = { nullptr };
	CAMERA_TYPE					m_eCameraType = {};
	class CCamera_Player*		m_pCamera = { nullptr };

	_bool 						m_bIsRender = { false };
	_bool						m_bIsContainer = { false };

private:
	HRESULT						Ready_Components(const CAMERA_TRIGGER_BOX_DESC& pDesc);

	void						Begin_OverlapEvent(_float3 vHitPoint, _float3 vHitDir, CGameObject* pHitActor);
	void						OverlappingEvent(_float3 vHitPoint, _float3 vHitDir, CGameObject* pHitActor);
	void						End_OverlapEvent(_float3 vHitPoint, _float3 vHitDir, CGameObject* pHitActor);

public:
	static	CCameraTriggerBox* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void				Free() override;

};
NS_END