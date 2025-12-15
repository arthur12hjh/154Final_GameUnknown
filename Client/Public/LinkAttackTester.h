#pragma once

#include "Client_Defines.h"
#include "Entity.h"

NS_BEGIN(Engine)
class CCollider;
NS_END

NS_BEGIN(Client)

class CLinkAttackTester final : public CEntity
{
private:
	CLinkAttackTester(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLinkAttackTester(const CLinkAttackTester& Prototype);
	virtual ~CLinkAttackTester() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	void Set_Animation(const _char* szAnimationTag);
	void Set_Animation(_uint iAnimationIndex);

private:
	class CGameManager*				m_pGameManager = { nullptr };
	class CBody_LinkAttackTester*	m_pPart_Body = { nullptr };
	class CPlayer*					m_pPlayer = { nullptr };


	CCollider* m_pColliderCom = { nullptr };

	_float m_fMoveTime = 0.f;
	_int m_iAnimationSequence = 0;
	_bool m_bIsActive = FALSE;


private:
	HRESULT Ready_Components();
	HRESULT Ready_PartObjects();

public:
	static CLinkAttackTester* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END