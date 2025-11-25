#pragma once

#include "Character.h"

NS_BEGIN(Engine)
class CCollider;
class CNavigation;
class CCharacterController;
NS_END

NS_BEGIN(Client)

class CPlayer final : public CCharacter
{
private:
	CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer(const CPlayer& Prototype);
	virtual ~CPlayer() = default;

public:
	// Desc ¹ÝÈ¯.
	const struct Player_Desc* Get_Desc() { return &m_PlayerDesc; }
	const class CBody_Player* Get_BodyPtr() { return m_pBody; }
	_float Get_AnimationRatio();

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	struct Player_Desc		m_PlayerDesc = {};

	class CBody_Player*		m_pBody = { nullptr };

	class CTrailEffect*		m_pTrail = { nullptr };
	class CPlayerFSM*		m_pPlayerFSM = { nullptr };

	CCharacterController*	m_pCCT = { nullptr };
	CCollider*				m_pColliderCom = { nullptr };
private:
	HRESULT Ready_Components();
	HRESULT Ready_PartObjects();
	HRESULT Ready_PlayerDesc();

public:
	static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END