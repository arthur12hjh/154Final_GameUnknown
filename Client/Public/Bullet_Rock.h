#pragma once
#include "Bullet.h"

NS_BEGIN(Client)
class CBullet_Rock final : public CBullet
{
private:
	CBullet_Rock(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBullet_Rock(const CBullet_Rock& Prototype);
	virtual ~CBullet_Rock() = default;

public:
	virtual HRESULT				Initialize_Prototype() override;
	virtual HRESULT				Initialize(void* pArg) override;

	virtual void				Priority_Update(_float fTimeDelta) override;
	virtual void				Update(_float fTimeDelta) override;
	virtual void				Late_Update(_float fTimeDelta) override;

	virtual HRESULT				Render() override;
protected:
	HRESULT						ADD_Components(BULLET_DESC& pDesc);
	HRESULT                     Bind_ShaderResources();

	void						Begin_OverlapEvent(_float3 vHitPoint, _float3 vHitDir, CGameObject* pHitActor);

public:
	static	CBullet_Rock*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg) override;
	virtual void				Free() override;
};
NS_END