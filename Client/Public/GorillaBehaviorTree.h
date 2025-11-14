#pragma once
#include "Client_Defines.h"
#include "BehaviorTree.h"

NS_BEGIN(Client)
class CGorillaBehaviorTree final : public CBehaviorTree
{
private:
	CGorillaBehaviorTree(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGorillaBehaviorTree(const CGorillaBehaviorTree& Prototype);
	virtual ~CGorillaBehaviorTree() = default;

public:
	virtual	HRESULT						Initialize_Prototype() override;
	virtual	HRESULT						Initialize(void* pArg) override;

	virtual	void						Update(_float fTimeDelta);

private:
	CBehaviorNode*						m_pRootNode = nullptr;
	CBlackBoard*						m_pBlackBoard = nullptr;

private :
	HRESULT								Ready_BlackBoard();
	HRESULT								Ready_TreeNodes();

public:
	static	CGorillaBehaviorTree*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual	CComponent*					Clone(void* pArg);
	virtual	void						Free() override;

};
NS_END