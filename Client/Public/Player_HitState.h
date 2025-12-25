#pragma once

#include "PlayerState.h"

NS_BEGIN(Client)

class CPlayer_HitState final : public CPlayerState
{
private:
	CPlayer_HitState(PLAYER_HIT_DESC* pHitDesc);
	virtual ~CPlayer_HitState() = default;

public:
	// 이거 초기화할때 혹시나 초기값 바뀌는 경우 있을수 있으니
	// void* 디폴트 매개변수 잡아서 넘겨받습니다.
	virtual void			   Start(void* pArg = nullptr, _float fBlendRatio = 0.12f) override;
	virtual PLAYER_TRANSITION_DESC	   Update(_float fTimeDelta) override;
	virtual _float End() override;

private:
	_float3 m_vImpactDir = {};
	_float3 m_vHitDir = {};
	_float4 m_vAttackerPos = {};
	_float  m_fImpact = {};
	_float3 m_vHitPoint = {};

	_float3 m_vDirection = {};
public:
	static	CPlayer_HitState* Create(void* pArg);
	virtual	void			   Free() override;

};

NS_END
