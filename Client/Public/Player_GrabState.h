#pragma once

#include "PlayerState.h"

NS_BEGIN(Client)

class CPlayer_GrabState final : public CPlayerState
{
private:
	CPlayer_GrabState(CHARACTER_SKILL_DESC* pDesc);
	virtual ~CPlayer_GrabState() = default;

public:
	virtual void					Start(void* pArg = nullptr, _float fBlendRatio = 0.12f) override;
	virtual PLAYER_TRANSITION_DESC	Update(_float fTimeDelta) override;
	virtual _float					End() override;

private:
	_float3 m_vImpactDir = {};
	_float3 m_vHitDir = {};
	_float4 m_vAttackerPos = {};
	_float  m_fImpact = {};
	_float3 m_vHitPoint = {};

	_float3 m_vDirection = {};

	//플레이 해야되는 애니메이션 이름
	_char   m_szAnimName[256] = {};
	//타겟 본 이름
	_char   m_szTargetBoneName[256] = {};
	_bool	m_isStandUp = { false };

public:
	static	CPlayer_GrabState* Create(void* pArg);
	virtual	void			   Free() override;

};

NS_END
