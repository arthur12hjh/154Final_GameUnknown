#pragma once
#include "Bullet.h"

NS_BEGIN(Client)
class CEffect;

class CBullet_Split final : public CBullet
{
private:
	CBullet_Split(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBullet_Split(const CBullet_Split& Prototype);
	virtual ~CBullet_Split() = default;

public:
	virtual HRESULT				Initialize_Prototype() override;
	virtual HRESULT				Initialize(void* pArg) override;

	virtual void				Priority_Update(_float fTimeDelta) override;
	virtual void				Update(_float fTimeDelta) override;
	virtual void				Late_Update(_float fTimeDelta) override;

	virtual HRESULT				Render() override;
	virtual void				Shoot_Projectile(_vector vTargetPoint, _float fSpeed);

protected:
	HRESULT						ADD_Components(BULLET_DESC& pDesc);
	HRESULT                     Bind_ShaderResources();

	void						Begin_OverlapEvent(_float3 vHitPoint, _float3 vHitDir, CGameObject* pHitActor);

private:
	CEffect*					m_pEffect = { nullptr };

	_float2						m_fArcingTime = { 0.f, 2.f };
	vector<_float3>				m_PointLists = {};

public:
	static	CBullet_Split*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg) override;
	virtual void				Free() override;
};
NS_END