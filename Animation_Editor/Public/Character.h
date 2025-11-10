#pragma once

#include "Animation_Editor_Defines.h"
#include "ContainerObject.h"

NS_BEGIN(Engine)
class CCollider;
NS_END

NS_BEGIN(Animation_Editor)

class CCharacter final : public CContainerObject
{
public:
	enum STATE {
		STATE_IDLE = 0x00000001, // 0000 0000 0000 0000 0000 0000 0000 0001
		STATE_WALK = 0x00000002, // 0000 0000 0000 0000 0000 0000 0000 0010
		STATE_ATTACK = 0x00000004, // 0000 0000 0000 0000 0000 0000 0000 0100
	};
private:
	CCharacter(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCharacter(const CCharacter& Prototype);
	virtual ~CCharacter() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	_uint				m_iState = {};
	CCollider* m_pColliderCom = { nullptr };

private:
	HRESULT Ready_Components();
	HRESULT Ready_PartObjects();

public:
	static CCharacter* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END