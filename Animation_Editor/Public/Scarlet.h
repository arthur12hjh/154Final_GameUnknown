#pragma once

#include "Animation_Editor_Defines.h"
#include "Entity.h"

NS_BEGIN(Engine)
class CCollider;
NS_END

NS_BEGIN(Animation_Editor)

class CScarlet final : public CEntity
{
public:
	enum STATE {
		STATE_IDLE = 0x00000001, // 0000 0000 0000 0000 0000 0000 0000 0001
		STATE_WALK = 0x00000002, // 0000 0000 0000 0000 0000 0000 0000 0010
		STATE_ATTACK = 0x00000004, // 0000 0000 0000 0000 0000 0000 0000 0100
	};
private:
	CScarlet(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CScarlet(const CScarlet& Prototype);
	virtual ~CScarlet() = default;

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
	class CTool_Manager* m_pToolManager = { nullptr };
	class CBody_Scarlet* m_pPart_Body = { nullptr };

	CCollider* m_pColliderCom = { nullptr };

	_uint				m_iState = {};


private:
	HRESULT Ready_Components();
	HRESULT Ready_PartObjects();

public:
	static CScarlet* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END