#pragma once
#include "Component.h"

NS_BEGIN(Engine)
class CBehaviorNode;
class CBlackBoard;

class ENGINE_DLL CBehaviorTree : public CComponent
{
private :
	CBehaviorTree(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBehaviorTree(const CBehaviorTree& Prototype);
	virtual ~CBehaviorTree() = default;

public :
	virtual	HRESULT						Initialize_Prototype() override;
	virtual	HRESULT						Initialize(void* pArg) override;

	virtual	void						Update(_float fTimeDelta);

	//블랙보드 참조하면 래퍼런스 카운트 올라감
	CBlackBoard*						GetBlackBoard() const;

private :
	CBehaviorNode*						m_pRootNode = nullptr;
	CBlackBoard*						m_pBlackBoard = nullptr;

public :
	static	CBehaviorTree*				Create(ID3D11Device* pDevice, ID3D11DeviceContext*	pContext);
	virtual	CComponent*					Clone(void* pArg);
	virtual	void						Free() override;

};
NS_END