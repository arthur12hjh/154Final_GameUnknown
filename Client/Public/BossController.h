#pragma once
#include "Client_Defines.h"
#include "AIController.h"

NS_BEGIN(Engine)
class CBehaviorTree;
NS_END

NS_BEGIN(Client)
class CBossController final : public CAIController
{
public:
	typedef struct BossControllerDesc : public AI_CONTROLLER_DESC
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

private:
	CBehaviorTree*						m_pBehaviorTree = { nullptr };

public:
	static	CBossController*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*				Clone(void* pArg) override;
	virtual void						Free() override;

};
NS_END