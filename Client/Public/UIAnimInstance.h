#pragma once

#include "Client_Defines.h"
#include "Base.h"
#include "UIStruct.h"

NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(Client)

class CUIBase;

class CUIAnimInstance final : public CBase
{
private:
	CUIAnimInstance();
	virtual ~CUIAnimInstance() = default;

public:	
	HRESULT Initialize(CUIBase* pUI, void* Desc);
	_bool Update(_float fTimeDelta);

	void Set_UI_Anim_Desc(const UI_ANIM_DESC& Desc)
	{
		m_tUIAnimDesc = Desc;
	}

	UI_ANIM_DESC* Get_UI_Anim_Desc() { return &m_tUIAnimDesc; }

	_bool Play_Anim(UI_ANIM_DESC* pAnimDesc, UI_ANIM_TRACK_DESC* pTrackDesc, _float fTimeDelta);
	void Stop_Anim();

	// 디버그 용
	_float Get_PlayTime() { return m_fTimeStack; }

	CUIBase* Get_TargetUI() { return m_pTargetUI; }

private:
	_float m_fTimeStack = 0.f;

	_bool m_isAnimFinish = false;

	UI_ANIM_DESC m_tUIAnimDesc{};

	CUIBase* m_pTargetUI{ nullptr };

	vector<_bool> m_AnimFinishs{};
	_uint m_iTrackLength{0};

public:
	static CUIAnimInstance* Create(CUIBase* pUI, void* Desc);
	virtual void Free() override;
};

NS_END