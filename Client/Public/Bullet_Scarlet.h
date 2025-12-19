#pragma once
#include "Bullet.h"

NS_BEGIN(Client)
class CEffect;

class CBullet_Scarlet final : public CBullet
{
private:
	CBullet_Scarlet(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBullet_Scarlet(const CBullet_Scarlet& Prototype);
	virtual ~CBullet_Scarlet() = default;

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
	CEffect*					m_pEffect = {};

public:
	static	CBullet_Scarlet*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg) override;
	virtual void				Free() override;
};
NS_END