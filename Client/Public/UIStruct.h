#pragma once
#include "Engine_Defines.h"
#include "UIObject.h"
#include <vector>

namespace Client
{

	enum class UI_SHADER_PASS {
		UI, DEBUG, GLOW, GLOWFX, HP_GAUGE,
		POTION, SHIELD, BETA, BETA_FX, SKILL_SLOT,
		SKILL_SLOT_GLOW, RUSH_SLOT, RUSH_SLOT_GLOW, SKILL_WRAPPER_ON_LINE, SKILL_WRAPPER_ON_FX,
		LOADING_BLUR, SIMPLE_KEY, INTERACTION_FX, INTERACTION_FX_GLOW, STAMINA,
		STAMINA_FX, LOCKON, OWNGOLD
	};

	typedef struct tagSkillInfoDesc
	{
		_int		iSkillIndex{ -1 };
		_uint		iSkillID{ 0 };
		_uint		iPrevSkillState{ 0 };
		_uint		iSkillState{ 0 };
	}UI_SKILL_INFO_DESC;

	typedef struct tagUIEventArg {
		enum ARG_TYPE {
			NONE,
			BTN_STATE,
			SKILL_STATE,
			SKILL_INFO,
			INTERACTION_STATE,
			INT,
			FLOAT,
			WSTRING,
			BOOL,
		} Type;

		_wstring szActionTag{};
		void* pData = nullptr;
	}UI_EVENT_ARG_DESC;

	typedef struct tagUIShader {
		bool bUseUV{ false };
		float fUVScaleX = { 1.f }, fUVScaleY = { 1.f };
		float fUVOffsetX = { 0.f }, fUVOffsetY = { 0.f };

		bool bUseFillClip{ false };
		float fFillAmount{ 1.f };

		bool bUseTintColor{ false };
		XMFLOAT4 vTintColor{ 1.f, 1.f, 1.f, 1.f };

		bool bDiscardBlack{ false };

		bool bUseScale{ false };
		float fScale{ 1.f };

		bool bUseGlow{ false };
		bool bUsePulseEffect{ false };
		bool bUseScroll{ false };
		float fScrollSpeed{ 0.f };
		float fPulseTime{ 0.f };
		float fPulseSpeed{ 0.f };
		float fGlowIntensity{ 0.f };
		float fGlowSpread{ 0.f };

	}UI_SHADER_DESC;

	// UI Event
	typedef struct tagUIEventDesc
	{
		wstring szActionTag{};
		vector<wstring> szSubscribeEventTags{}; // 구독 할 이벤트들
		wstring szTypeTag{};
		wstring szArg{}; // 애니메이션 이름, 액션 이름, 사운드 이름, 이펙트 이름 ...
	}UI_EVENT_DESC;

	// UI Animation 구조체
	typedef struct tagUIAnimTrackDesc
	{
		wstring szTrackTag{};
		XMFLOAT4 vStartParam{ 0.f, 0.f, 0.f, 0.f };
		XMFLOAT4 vEndParam{ 0.f, 0.f, 0.f, 0.f };
		float fStartTime{ 0.f };
		float fEndTime{ 0.f };
		float fSpeed{ 0.f };
	}UI_ANIM_TRACK_DESC;

	typedef struct tagUIAnimDesc
	{
		wstring szAnimTag{};
		float fDuration{ 1.f };
		bool isLoop = false;
		bool isInfluenceChildren = true;
		bool isBeapBeap{ false };

		bool m_isHasTracks{ false };
		map<wstring, UI_ANIM_TRACK_DESC> m_Tracks{};

	public:
		void Add_UI_Track_Desc(wstring szTrackTag, const UI_ANIM_TRACK_DESC& Desc) {
			auto TrackDesc = m_Tracks.find(szTrackTag);

			if (TrackDesc != m_Tracks.end())
				return;

			m_isHasTracks = true;
			m_Tracks.emplace(szTrackTag, Desc);
		}

		UI_ANIM_TRACK_DESC* Get_UI_Track_Desc(wstring szTrackTag) {
			auto TrackDesc = m_Tracks.find(szTrackTag);

			if (TrackDesc == m_Tracks.end())
				return nullptr;

			return m_isHasTracks ? &TrackDesc->second : nullptr;
		}

		map<wstring, UI_ANIM_TRACK_DESC> Get_UI_Track_Descs() {
			return m_Tracks;
		}

	}UI_ANIM_DESC;

	typedef struct tagUITextDesc
	{
		wstring szFont{};
		wstring szText{};
		float	fScale{ 1.f };
		XMFLOAT4 vColor{ 1.f, 1.f, 1.f, 1.f };

	}UI_TEXT_DESC;

	typedef struct tagUITextureDesc
	{
		wstring szTextureComTag{};
		wstring szProtoTag{};
		unsigned int iTextureIndex{ 0 };
		unsigned int iPass{ 0 };
	}UI_TEXTURE_DESC;

	typedef struct tagUIBaseDesc : public CUIObject::UIOBJECT_DESC
	{
		float fOffsetX{ 0.f }, fOffsetY{ 0.f };
		float fRotation{ 0.f };
		float fAlpha{ 1.f };
		unsigned int iLevel{ 0 };
		unsigned int iDepth{ 0 };
		unsigned int iRenderGroup{ ENUM_CLASS(RENDER::UI) };
		unsigned int iDrawType{ ENUM_CLASS(CUIObject::DRAW_TYPE::SCREEN) };

		unsigned int iVisiblity{ ENUM_CLASS(VISIBILITY::VISIBLE) };

		wstring szUIID{};
		wstring szUITag{};
		wstring szLayerTag{};
		wstring szProtoTag{};
		wstring szPoolTag{};

		bool m_isHasTextDesc{ false };
		UI_TEXT_DESC m_tUITextDesc{ };
		bool m_isHasTextureDesc{ false };
		UI_TEXTURE_DESC m_tUITextureDesc{ };
		bool m_isHasShaderDesc{ false };
		UI_SHADER_DESC m_tUIShaderDesc{};

		map<wstring, wstring> m_AnimTags{}; // 애니메이션 태그, 프리팹 이름
		map<wstring, vector<UI_EVENT_DESC>> m_Events{}; // 이벤트 태그, 이벤트 구조체

	public:
		// 텍스트
		void Set_UI_Text_Desc(const UI_TEXT_DESC& Desc) {
			m_isHasTextDesc = true;
			m_tUITextDesc = Desc;
		}

		UI_TEXT_DESC* Get_UI_Text_Desc() {
			return m_isHasTextDesc ? &m_tUITextDesc : nullptr;
		}

		// 텍스쳐
		void Set_UI_Texture_Desc(const UI_TEXTURE_DESC& Desc) {
			m_isHasTextureDesc = true;
			m_tUITextureDesc = Desc;
		}

		UI_TEXTURE_DESC* Get_UI_Texture_Desc() {
			return m_isHasTextureDesc ? &m_tUITextureDesc : nullptr;
		}

		// 이벤트
		map<wstring, vector<UI_EVENT_DESC>>* Get_Events() {
			return &m_Events;
		}

		void Add_UI_Event(const _wstring& szEventTag, const UI_EVENT_DESC& Desc)
		{
			m_Events[szEventTag].push_back(Desc);
		}

		void Delete_UI_Event(const _wstring& szEventTag)
		{
			m_Events.erase(szEventTag);
		}

		// 툴에서 쓰는거
		void Add_UI_Anim(wstring szAnimTag, wstring szAnimFileName) {
			auto Anim = m_AnimTags.find(szAnimTag);

			if (Anim != m_AnimTags.end())
			{
				Anim->second = szAnimFileName;
				return;
			}

			m_AnimTags.emplace(szAnimTag, szAnimFileName);
		}

		void Delete_UI_Anim(wstring szAnimTag)
		{
			auto Anim = m_AnimTags.find(szAnimTag);

			if (Anim != m_AnimTags.end())
			{
				Anim->second = TEXT("");
				return;
			}
		}

		map<wstring, wstring> Get_UI_Anim_Tags() { return m_AnimTags; }

		// 쉐이더
		UI_SHADER_DESC* Get_ShaderDesc() {
			return m_isHasShaderDesc ? &m_tUIShaderDesc : nullptr;
		}

	}UIBASE_DESC;
}