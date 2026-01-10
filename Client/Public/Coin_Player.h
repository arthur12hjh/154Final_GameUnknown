
#pragma once

#include "Client_Defines.h"
#include "Player_Parts.h"

NS_BEGIN(Engine)
class CModel;
class CShader;
class CCollider;
NS_END

NS_BEGIN(Client)

class CCoin_Player final : public CPlayer_Parts
{
public:
	typedef struct tagCoin_Desc : public CPartObject::PARTOBJECT_DESC
	{
		const _float4x4* pSocketMatrix = { nullptr };
	}COIN_DESC;
private:
	CCoin_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCoin_Player(const CCoin_Player& Prototype);
	virtual ~CCoin_Player() = default;

public:
	virtual HRESULT				Initialize_Prototype() override;
	virtual HRESULT				Initialize(void* pArg) override;

	virtual void				Priority_Update(_float fTimeDelta) override;
	virtual void				Update(_float fTimeDelta) override;
	virtual void				Late_Update(_float fTimeDelta) override;

	virtual HRESULT				Render() override;

private:
	PLAYER_DESC*	 m_pPlayerDesc = { nullptr };
	const _float4x4* m_pSocketMatrix = { nullptr };

private:
	HRESULT						Ready_Components();
	HRESULT						Bind_ShaderResources();

public:
	static CCoin_Player*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg) override;
	virtual void				Free() override;
};

NS_END