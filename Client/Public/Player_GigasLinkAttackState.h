#pragma once

#include "PlayerState.h"

NS_BEGIN(Client)

class CPlayer_GigasLinkAttackState final : public CPlayerState
{
private:
	CPlayer_GigasLinkAttackState();
	virtual ~CPlayer_GigasLinkAttackState() = default;

public:
	// 이거 초기화할때 혹시나 초기값 바뀌는 경우 있을수 있으니
	// void* 디폴트 매개변수 잡아서 넘겨받습니다.
	HRESULT								Initialize(void* pArg);
	virtual void						Start(void* pArg = nullptr, _float fBlendRatio = 0.12f) override;
	virtual PLAYER_TRANSITION_DESC	    Update(_float fTimeDelta) override;
	virtual _float End() override;

private:
	vector<_bool>						m_isEndList;
	_int								m_iAnimationIndex;

	const _float4x4* m_pSocketMatrix = { nullptr };
	const _float4x4* m_pParentTransformMatrix = { nullptr };

public:
	static	CPlayer_GigasLinkAttackState* Create(void* pArg);
	virtual	void			   Free() override;

};

NS_END
