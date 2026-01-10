#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

NS_BEGIN(Client)
class CSoundTriggerBox final : public CGameObject
{
public:
	typedef struct TriggerBoxDesc : public GAMEOBJECT_DESC
	{
		SOUND_TRIGGER_TYPE		eSoundBoxType;
	}TRIGGER_BOX_DESC;

private:
	CSoundTriggerBox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSoundTriggerBox(const CSoundTriggerBox& Prototype);
	virtual ~CSoundTriggerBox() = default;

public:
	virtual HRESULT				Initialize_Prototype() override;
	virtual HRESULT				Initialize(void* pArg) override;

	virtual void				Priority_Update(_float fTimeDelta) override;
	virtual void				Update(_float fTimeDelta) override;
	virtual void				Late_Update(_float fTimeDelta) override;

	virtual HRESULT				Render() override;

private:
	CCollider*					m_pColliderCom = { nullptr };
	SOUND_TRIGGER_TYPE			m_eSoundTriggerType = {};

private:
	HRESULT						Ready_Components(const TRIGGER_BOX_DESC& pDesc);

	void						Begin_OverlapEvent(_float3 vHitPoint, _float3 vHitDir, CGameObject* pHitActor);
	void						OverlappingEvent(_float3 vHitPoint, _float3 vHitDir, CGameObject* pHitActor);
	void						End_OverlapEvent(_float3 vHitPoint, _float3 vHitDir, CGameObject* pHitActor);

public:
	static	CSoundTriggerBox*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg) override;
	virtual void				Free() override;

};
NS_END