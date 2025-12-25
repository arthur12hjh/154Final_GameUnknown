#pragma once
#include "Client_Defines.h"
#include "AIController.h"

NS_BEGIN(Engine)
class CBehaviorTree;
NS_END

NS_BEGIN(Client)
class CBossBlackBoard;

class CBossController final : public CAIController
{
public:
	typedef struct BossControllerDesc : public GAMEOBJECT_DESC
	{
		const WCHAR*			szBehaviorProtoType;
	}BOSS_CONTROLLER_DESC;

private:
	CBossController(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBossController(const CBossController& Prototype);
	virtual ~CBossController() = default;

public:
	virtual HRESULT						Initialize_Prototype() override;
	virtual HRESULT						Initialize(void* pArg) override;

	virtual void						Priority_Update(_float fTimeDelta) override;
	virtual void						Update(_float fTimeDelta) override;
	virtual void						Late_Update(_float fTimeDelta) override;

	virtual HRESULT						Render() override;

	virtual	void						Damage(void* pArg) override;
	virtual void						ActionSuccess(void* pArg) override;

	_bool								bIsLastAttack();
	_bool								bIsEntranceAttack();

private:
	CBossBlackBoard*					m_pBlackBoard = { nullptr };
	CBehaviorTree*						m_pBehaviorTree = { nullptr };

private :
	HRESULT								Ready_Behavior(const BOSS_CONTROLLER_DESC& pDesc);

public:
	static	CBossController*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*				Clone(void* pArg) override;
	virtual void						Free() override;

};
NS_END