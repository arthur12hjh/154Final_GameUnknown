#pragma once

#include "Client_Defines.h"
#include "Base.h"

NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(Client)

class CUIBase;

class CUIAnimInstance final : public CBase
{
public:
	typedef struct tagUIAnimTrackDesc
	{
		_wstring szTrackTag{};
		_float4 vStartParam{ 0.f, 0.f, 0.f, 0.f };
		_float4 vEndParam{ 0.f, 0.f, 0.f, 0.f };
		_float fStartTime{ 0.f };
		_float fEndTime{ 0.f };
		_float fSpeed{ 0.f };
	}UI_ANIM_TRACK_DESC;

	typedef struct tagUIAnimDesc
	{
		_wstring szAnimTag{};
		_float fDuration{ 1.f };
		_bool isLoop = false;
		_bool isInfluenceChildren = true;
		map<_wstring, UI_ANIM_TRACK_DESC*> m_Tracks{};

	public:
		void Add_UI_Track_Desc(_wstring szTrackTag, const UI_ANIM_TRACK_DESC& Desc) {

			auto TrackDesc = m_Tracks.find(szTrackTag);

			if (TrackDesc != m_Tracks.end())
				return;

			UI_ANIM_TRACK_DESC* pDesc = new UI_ANIM_TRACK_DESC(Desc);
			m_Tracks.emplace(szTrackTag, pDesc);
		}

		UI_ANIM_TRACK_DESC* Get_UI_Track_Desc(_wstring szTrackTag) {
			auto TrackDesc = m_Tracks.find(szTrackTag);

			if (TrackDesc == m_Tracks.end())
				return nullptr;

			return TrackDesc->second;
		}

		map<_wstring, UI_ANIM_TRACK_DESC*> Get_UI_Track_Descs() {
			return m_Tracks;
		}

	}UI_ANIM_DESC;
private:
	CUIAnimInstance();
	virtual ~CUIAnimInstance() = default;

public:	
	HRESULT Initialize(CUIBase* pUI, void* Desc);

	void Set_UI_Anim_Desc(const UI_ANIM_DESC& Desc)
	{
		m_tUIAnimDesc = Desc;
	}

	UI_ANIM_DESC* Get_UI_Anim_Desc() { return &m_tUIAnimDesc; }

	_bool Play_Anim(UI_ANIM_DESC* pAnimDesc, UI_ANIM_TRACK_DESC* pTrackDesc, _float fTimeDelta);

	// 디버그 용
	_float Get_PlayTime() { return m_fTimeStack; }
	//_float Get_DeltaTime() { return m_fDeltaTime; }

	_bool Update(_float fTimeDelta);

private:
	CGameInstance* m_pGameInstance{ nullptr };
	 
	_float m_fTimeStack = 0.f;
	_float m_fDeltaTime = 0.f;

	_bool m_isAnimFinish = false;

	UI_ANIM_DESC m_tUIAnimDesc{};

	CUIBase* m_pTargetUI{ nullptr };

public:
	static CUIAnimInstance* Create(CUIBase* pUI, void* Desc);
	virtual void Free() override;
};

NS_END