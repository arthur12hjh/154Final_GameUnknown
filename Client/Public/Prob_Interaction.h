#pragma once
#include "Actor.h"
#include "UIStruct.h"

NS_BEGIN(Engine)
class CInteraction_Component;
NS_END

NS_BEGIN(Client)
class CGameManager;
class CUIBase;

class CProb_Interaction abstract : public CActor
{
protected:
	CProb_Interaction(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CProb_Interaction(const CProb_Interaction& Prototype);
	virtual ~CProb_Interaction() = default;

public:
	virtual HRESULT					Initialize_Prototype() override;
	virtual HRESULT					Initialize(void* pArg) override;

	virtual void					Priority_Update(_float fTimeDelta) override;
	virtual void					Update(_float fTimeDelta) override;
	virtual void					Late_Update(_float fTimeDelta) override;

	virtual HRESULT					Render() override;

	INTERACTION_STATE				Get_InterState() const { return m_eInterState; }

protected:
	CGameManager*					m_pGameManager = { nullptr };

	CInteraction_Component*			m_pInteractionCom = { nullptr };
	_bool							m_bIsInteractionAble = { false };

	// UI Ç¥½Ã 
	CUIBase*						m_pInteractionUI = { nullptr };

	INTERACTION_STATE				m_eInterState{ INTERACTION_STATE::DEFAULT };
	_float							m_fMaxUnlockDuration{ 0.f };
	_float							m_fUnlockDuration{ 0.f };

protected :
	virtual HRESULT					Begin_OverlapCallBack() = 0;
	virtual HRESULT					End_OverlapCallBack() = 0;
	virtual void					Excute_CallBack(CGameObject* pActionObject) = 0;

public:
	virtual		CGameObject*		Clone(void* pArg) override;
	virtual		void				Free() override;

};
NS_END