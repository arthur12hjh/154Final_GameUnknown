#pragma once
#include "Client_Defines.h"
#include "AIController.h"

NS_BEGIN(Engine)
class CStateMachine;
NS_END

NS_BEGIN(Client)
class CMonsterController final : public CAIController
{
private:
	CMonsterController(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonsterController(const CMonsterController& Prototype);
	virtual ~CMonsterController() = default;

public:
	virtual HRESULT					Initialize_Prototype() override;
	virtual HRESULT					Initialize(void* pArg) override;

	virtual void					Priority_Update(_float fTimeDelta) override;
	virtual void					Update(_float fTimeDelta) override;
	virtual void					Late_Update(_float fTimeDelta) override;

	virtual HRESULT					Render() override;

private :
	CStateMachine*					m_pFSM = { nullptr };

private :
	HRESULT							Ready_FSM();

public:
	static	CMonsterController*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*			Clone(void* pArg) override;
	virtual void					Free() override;

};
NS_END