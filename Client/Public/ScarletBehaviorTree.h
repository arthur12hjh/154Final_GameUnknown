#pragma once
#include "Client_Defines.h"
#include "BehaviorTree.h"

NS_BEGIN(Client)
class CScarletBehaviorTree final : public CBehaviorTree
{
private:
	CScarletBehaviorTree(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CScarletBehaviorTree(const CScarletBehaviorTree& Prototype);
	virtual ~CScarletBehaviorTree() = default;

public:
	virtual	HRESULT						Initialize_Prototype() override;
	virtual	HRESULT						Initialize(void* pArg) override;

	virtual	void						Update(_float fTimeDelta);

private:
	HRESULT								Ready_BlackBoard();
	HRESULT								Ready_TreeNodes();

public:
	static	CScarletBehaviorTree*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual	CComponent*					Clone(void* pArg);
	virtual	void						Free() override;

};
NS_END