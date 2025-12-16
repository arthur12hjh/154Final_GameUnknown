#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CCollider;
NS_END

NS_BEGIN(Client)
class CTriggerBox final : public CGameObject
{
public:
	typedef struct TriggerBoxDesc : public GAMEOBJECT_DESC
	{
		_int iTriggerCode = 0;
		COLLIDER eColType = COLLIDER::AABB;
		_float3 vScale = _float3(1.f, 1.f, 1.f);
		_float3 vRotation = _float3(0.f, 0.f, 0.f);
		_float3 vPosition = _float3(0.f, 0.f, 0.f);
		_float fDelayTime = -1.f;
	}TRIGGER_BOX_DESC;
private:
	CTriggerBox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTriggerBox(const CTriggerBox& Prototype);
	virtual ~CTriggerBox() = default;

public:
	virtual HRESULT				Initialize_Prototype() override;
	virtual HRESULT				Initialize(void* pArg) override;

	virtual void				Priority_Update(_float fTimeDelta) override;
	virtual void				Update(_float fTimeDelta) override;
	virtual void				Late_Update(_float fTimeDelta) override;

	virtual HRESULT				Render() override;

private:
	CCollider* m_pColliderCom = { nullptr };

	_int						m_iTriggerCode = {};

	_float						m_fDelayTime = {};

private:
	HRESULT						Ready_Components(const TRIGGER_BOX_DESC& pDesc);

	void						Begin_OverlapEvent(_float3 vHitPoint, _float3 vHitDir, CGameObject* pHitActor);
	void						OverlappingEvent(_float3 vHitPoint, _float3 vHitDir, CGameObject* pHitActor);
	void						End_OverlapEvent(_float3 vHitPoint, _float3 vHitDir, CGameObject* pHitActor);

public:
	static	CTriggerBox* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void				Free() override;

};
NS_END