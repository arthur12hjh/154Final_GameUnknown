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
private:
	typedef struct tagKeyFrame
	{
		//_float fTime{ 0.f };
		_wstring szPramTag{};
		_float4 vValue{ 0.f, 0.f, 0.f, 0.f };
		_wstring szEasing{ TEXT("Linear") };
	}KEY_FRAME;

	enum class UIANIM{ALPHA, POSITION, SIZE, COLOR, ROTATION, END};
	typedef struct tagUIAnimTrack
	{
		_uint iTarget{ ENUM_CLASS(UIANIM::END) };
		vector<KEY_FRAME> KeyFrames;
		_bool isLoop{ false };

		_float fDuration{ 0.f };
	}UI_ANIM_TRACK;

private:
	CUIAnimationCom();
	virtual ~CUIAnimationCom() = default;

public:	
	HRESULT Initialize();

	/*void AddTrack(const UI_ANIM_TRACK& track) {
		AnimTracks.push_back(track);
	}*/

	// 여기서 애니메이션 태그나 트랙 태그 받아다가 돌려주면 될..듯?
	void Play(_wstring szAnimTag);
	void Pause();
	void Stop();
	void Tick(_float fTimeDelta);

	void Play_Anim(void* pAnimDesc, void* pTrackDesc);
	//void Play_PosAnim(void* pAnimDesc, void* pTrackDesc);
	//void Play_AlphaAnim(void* pAnimDesc, void* pTrackDesc);

	void Set_Owner(CUIBase* pOwner) {
		m_pOwner = pOwner;
	}

	_float Get_PlayTime() { return m_fTimeStack; }
	_float Get_DeltaTime() { return m_fDeltaTime; }

private:
	CGameInstance* m_pGameInstance{ nullptr };

	//_wstring szAnimTag{};
	//_float duration{ 0.f };
	//_uint iTickCount{ 0 };
	//_uint iCurrentFrame{ 0 };
	//_bool isLoop{ false };
	 
	_float m_fTimeStack = 0.f;
	_float m_fDeltaTime = 0.f;

	//vector<UI_ANIM_TRACK> AnimTracks{};
	CUIBase* m_pOwner{ nullptr };

public:
	static CUIAnimationCom* Create();
	virtual void Free() override;
};

NS_END