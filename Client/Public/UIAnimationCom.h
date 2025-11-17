#pragma once

#include "Client_Defines.h"
#include "Base.h"

NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(Client)

class CUIBase;

class CUIAnimationCom final : public CBase
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
	CUIAnimationCom();
	virtual ~CUIAnimationCom() = default;

public:	
	HRESULT Initialize();

	void Play(_wstring szAnimTag);
	void Pause();
	void Stop();
	void Tick(_float fTimeDelta);

	void Set_UI_Anim_Desc(const UI_ANIM_DESC& Desc)
	{
		m_tUIAnimDesc = Desc;
	}

	UI_ANIM_DESC* Get_UI_Anim_Desc() { return &m_tUIAnimDesc; }

	void Play_Anim(void* pAnimDesc, void* pTrackDesc);

	void Set_Owner(CUIBase* pOwner) {
		m_pOwner = pOwner;
	}

	_float Get_PlayTime() { return m_fTimeStack; }
	_float Get_DeltaTime() { return m_fDeltaTime; }

private:
	CGameInstance* m_pGameInstance{ nullptr };
	 
	_float m_fTimeStack = 0.f;
	_float m_fDeltaTime = 0.f;

	_bool m_isAnimFinish = false;

	CUIBase* m_pOwner{ nullptr };

	UI_ANIM_DESC m_tUIAnimDesc{};

public:
	static CUIAnimationCom* Create();
	virtual void Free() override;
};

NS_END