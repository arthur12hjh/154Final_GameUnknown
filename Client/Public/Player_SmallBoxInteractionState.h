#pragma once

#include "PlayerState.h"

/*
모든 Interaction 류 상태들은 Idle에서만 사용 가능하다.
*/

NS_BEGIN(Engine)
class CInteraction_Component;
NS_END

NS_BEGIN(Client)

class CPlayer_SmallBoxInteractionState final : public CPlayerState
{
private:
	CPlayer_SmallBoxInteractionState(void* pArg);
	virtual ~CPlayer_SmallBoxInteractionState() = default;

public:
	// 이거 초기화할때 혹시나 초기값 바뀌는 경우 있을수 있으니
	// void* 디폴트 매개변수 잡아서 넘겨받습니다.
	virtual void			   Start(void* pArg = nullptr, _float fBlendRatio = 0.12f) override;
	virtual PLAYER_TRANSITION_DESC	   Update(_float fTimeDelta) override;
	virtual _float End() override;

private:
	CInteraction_Component* m_pInteractionCom = { nullptr };
	_bool					m_isLerpFinished = { false }; 
public:
	static	CPlayer_SmallBoxInteractionState* Create(void* pArg);
	virtual	void			   Free() override;

};

NS_END
