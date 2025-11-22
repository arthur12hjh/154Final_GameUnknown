#pragma once
#include "Engine_Defines.h"
#include "UIObject.h"
#include <vector>

namespace Client
{
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

		_bool m_isHasTracks{ false };
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
		wstring szText{};
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
		float fAlpha{ 1.f };
		unsigned int iLevel{ 0 };
		unsigned int iDepth{ 0 };
		unsigned int iRenderGroup{ ENUM_CLASS(RENDER::UI) };

		unsigned int iVisiblity{ ENUM_CLASS(VISIBILITY::VISIBLE) };

		wstring szUIID{};
		wstring szUITag{};
		wstring szLayerTag{};
		wstring szProtoTag{};

		_bool m_isHasTextDesc{ false };
		UI_TEXT_DESC m_tUITextDesc{ };
		_bool m_isHasTextureDesc{ false };
		UI_TEXTURE_DESC m_tUITextureDesc{ };

		vector<wstring> m_EventTags{};
		map<wstring, wstring> m_AnimTags{}; // 애니메이션 태그, 프리팹 이름

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

	}UIBASE_DESC;
}