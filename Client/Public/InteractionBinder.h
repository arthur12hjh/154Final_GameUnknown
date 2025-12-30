#pragma once
#include "Client_Defines.h"
#include "Interaction_Component.h"

NS_BEGIN(Client)
class CInteractionBinder final : public CInteraction_Component
{
private:
	CInteractionBinder(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CInteractionBinder(const CInteractionBinder& Prototype);
	virtual ~CInteractionBinder() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

	INTERACTION_STATE				Get_InterState() const { return m_eInterState; }
	void							Set_InterState(INTERACTION_STATE eState) { m_eInterState = eState; }
	const INTERACTION_DATA*			Get_InterDesc() { return m_InteractionDesc; }
	void							Set_InterDesc(const INTERACTION_DATA* pDesc) { m_InteractionDesc = pDesc; }

	_float							Get_Ratio() { return m_fInteractionDuration / m_InteractionDesc->fInteractionTime; }

	void							Set_Duration(_float fDuration) { m_fInteractionDuration = fDuration; }
	_float							Get_Duration() const { return m_fInteractionDuration; }
	void							Reset_Interaction() { m_fInteractionDuration = false; }

	void							Finished_Interaction();

	_bool							IsInteractionEnable();

private:
	const INTERACTION_DATA*			m_InteractionDesc = {};
	INTERACTION_STATE				m_eInterState = { INTERACTION_STATE::DEFAULT };

	_float m_fInteractionDuration{};

public:
	static		CInteractionBinder*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual		CComponent*			Clone(void* pArg) override;
	virtual		void				Free() override;

};
NS_END