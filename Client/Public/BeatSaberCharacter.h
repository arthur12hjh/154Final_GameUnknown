#pragma once
#include "Character.h"

NS_BEGIN(Engine)
class CCollider;
NS_END

NS_BEGIN(Client)
class CBeatSaberCharacterBody;
class CBeatSaberFsm;

class CBeatSaberCharacter : public CCharacter
{
private:
	CBeatSaberCharacter(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBeatSaberCharacter(const CBeatSaberCharacter& Prototype);
	virtual ~CBeatSaberCharacter() = default;

public:
	virtual HRESULT					Initialize_Prototype() override;
	virtual HRESULT					Initialize(void* pArg) override;

	virtual void					Priority_Update(_float fTimeDelta) override;
	virtual void					Update(_float fTimeDelta) override;
	virtual void					Late_Update(_float fTimeDelta) override;

	virtual HRESULT					Render() override;
	virtual HRESULT					Render_Shadow() override;
	virtual HRESULT					Render_MotionBlur() override;

	const BEATSABER_CHARACTER_DESC& GetBeatSaberCharacterDesc() { return m_CharacterDesc; }

private :
	BEATSABER_CHARACTER_DESC		m_CharacterDesc = {};
	CBeatSaberFsm*					m_pFsm = { nullptr };
	CPartObject*					m_pPartBody = { nullptr };
	CCollider*						m_pCollider = { nullptr };

private :
	HRESULT							Ready_CharacterData();
	HRESULT							ADD_PartObjects();
	HRESULT							ADD_Components();

	void							Key_Input(_float fTimeDelta);

public:
	static	CBeatSaberCharacter*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*			Clone(void* pArg) override;
	virtual void					Free() override;

};
NS_END