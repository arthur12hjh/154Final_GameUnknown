#pragma once

#include "Character.h"

NS_BEGIN(Engine)
class CCollider;
class CNavigation;
NS_END

NS_BEGIN(Client)

class CPlayer final : public CCharacter
{
private:
	CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer(const CPlayer& Prototype);
	virtual ~CPlayer() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	_uint				m_iState = {};
	CNavigation* m_pNavigationCom = { nullptr };
	CCollider* m_pColliderCom = { nullptr };


	class CBody_Player* m_pPart_Body = { nullptr };

#pragma region GARA_STATE


	_float				m_fEvadeTime;
	_float				m_fAttackTime;

	_int				m_iAttackComboIndex;
	_bool				m_bIsEvade;
	_bool				m_bIsAttacking;

	const _char* ComboAnims[5] = {
	"Proto_Sword_Lightattack_01_Root",
	"Proto_Sword_Lightattack_02_Root",
	"Proto_Sword_Lightattack_03_Root",
	"Proto_Sword_Lightattack_04_Root",
	"Proto_Sword_Lightattack_05"
	};

	const _float ComboDelayTime[5] =
	{
		1.5f, 1.5f, 1.7f, 1.5f, 1.5f
	};


#pragma endregion

private:
	HRESULT Ready_Components();
	HRESULT Ready_PartObjects();

public:
	static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END