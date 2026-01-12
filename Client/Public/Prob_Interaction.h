#pragma once
#include "Actor.h"

NS_BEGIN(Engine)
class CInteraction_Component;
NS_END

NS_BEGIN(Client)
struct Interaction_Data;
class CGameManager;
class CUIBase;
class CInteractionBinder;

class CProb_Interaction abstract : public CActor
{
public :
	typedef struct Prob_Interaction_Desc : public ACTOR_DESC
	{
		_uint					iInteractionID;
	}PROB_INTERACTION_DESC;

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
	//void							Finished_Interaction();
	//void							Reset_Interaction();

	//INTERACTION_STATE				Get_InterState() const { return m_eInterState; }
	//const INTERACTION_DATA*			Get_InterDesc() { return m_InteractionDesc; }
	//const CInteractionBinder*		Get_InterCom() { return m_pInteractionCom; }

	//_float							Get_Ratio() { return m_fInteractionDuration / m_InteractionDesc->fInteractionTime; }

protected:
	CGameManager*					m_pGameManager = { nullptr }; 
	CInteractionBinder*			m_pInteractionCom = { nullptr };

	//const INTERACTION_DATA*			m_InteractionDesc = {};
	//INTERACTION_STATE				m_eInterState = { INTERACTION_STATE::DEFAULT };

	//_float							m_fInteractionDuration = {};
	//_uint							m_iInteractionID{};
	_bool							m_bIsSound = { false };

protected :
	virtual HRESULT					Begin_OverlapCallBack();
	virtual HRESULT					End_OverlapCallBack();
	virtual void					Excute_CallBack(_float fTimeDelta, CGameObject* pActionObject);

public:
	virtual		CGameObject*		Clone(void* pArg) override;
	virtual		void				Free() override;

};
NS_END