#pragma once
#include "Client_Defines.h"
#include "AIController.h"

NS_BEGIN(Client)
class CNpcFSM;

class CNpcInteractionController final : public CAIController
{
private:
	CNpcInteractionController(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNpcInteractionController(const CNpcInteractionController& Prototype);
	virtual ~CNpcInteractionController() = default;

public:
	virtual HRESULT						Initialize_Prototype() override;
	virtual HRESULT						Initialize(void* pArg) override;

	virtual void						Priority_Update(_float fTimeDelta) override;
	virtual void						Update(_float fTimeDelta) override;
	virtual void						Late_Update(_float fTimeDelta) override;

	virtual HRESULT						Render() override;

private:
	CNpcFSM*							m_pFSM = { nullptr };

private:
	HRESULT								Ready_FSM();

public:
	static	CNpcInteractionController*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*				Clone(void* pArg) override;
	virtual void						Free() override;

};
NS_END