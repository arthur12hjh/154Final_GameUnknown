#pragma once
#include "Client_Defines.h"
#include "AIController.h"

NS_BEGIN(Engine)
class CStateMachine;
NS_END

NS_BEGIN(Client)
class CTargetComponent;
struct Character_Skill_Desc;
struct Naytiba_Desc;

class CMonsterMimesisController final : public CAIController
{
private:
	CMonsterMimesisController(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonsterMimesisController(const CMonsterMimesisController& Prototype);
	virtual ~CMonsterMimesisController() = default;

public:
	virtual HRESULT					Initialize_Prototype() override;
	virtual HRESULT					Initialize(void* pArg) override;

	virtual void					Priority_Update(_float fTimeDelta) override;
	virtual void					Update(_float fTimeDelta) override;
	virtual void					Late_Update(_float fTimeDelta) override;

	virtual HRESULT					Render() override;

	// 이거 일단 스킬데이터만 넣어주세요
	// 다른거 넣을거 있으면 말해주고 추가하면됨
	virtual	void					Damage(void* pArg) override;
	virtual void					ActionSuccess(void* pArg) override;

private:
	CTargetComponent*					m_pTargetCom = { nullptr };
	CStateMachine*						m_pFSM = { nullptr };

	const		Naytiba_Desc*			m_pOwnerData = { nullptr };

	_bool								m_bIsMimesis = { true };
	_bool								m_bIsDead = {false};

	_float								m_fAttackDelay = {};
	_float2								m_vAttackTime = {};

	_float2								m_vDelayTime = { 0.f, 1.f };

private:
	HRESULT								Ready_Components();
	HRESULT								Ready_FSM();

	void								Battle_Action(_float fTimeDelta);
	void								Default_Action(_float fTimeDelta);

	void								AttackCompleted(_float fDelayTime);
	void								DelayAction(_float fDelayTime);
	void								MoveAction(_bool bIsTarget);

public:
	static	CMonsterMimesisController*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*				Clone(void* pArg) override;
	virtual void						Free() override;

};
NS_END