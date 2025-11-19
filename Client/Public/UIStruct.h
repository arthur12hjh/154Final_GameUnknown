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
		map<wstring, UI_ANIM_TRACK_DESC*> m_Tracks{};

	public:
		void Add_UI_Track_Desc(wstring szTrackTag, const UI_ANIM_TRACK_DESC& Desc) {

			auto TrackDesc = m_Tracks.find(szTrackTag);

			if (TrackDesc != m_Tracks.end())
				return;

			UI_ANIM_TRACK_DESC* pDesc = new UI_ANIM_TRACK_DESC(Desc);
			m_Tracks.emplace(szTrackTag, pDesc);
		}

		UI_ANIM_TRACK_DESC* Get_UI_Track_Desc(wstring szTrackTag) {
			auto TrackDesc = m_Tracks.find(szTrackTag);

			if (TrackDesc == m_Tracks.end())
				return nullptr;

			return TrackDesc->second;
		}

		map<wstring, UI_ANIM_TRACK_DESC*> Get_UI_Track_Descs() {
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

		UI_TEXT_DESC* m_pUITextDesc{ nullptr };
		UI_TEXTURE_DESC* m_pUITextureDesc{ nullptr };

		vector<wstring> m_EventTags{};
		map<wstring, wstring> m_AnimTags{}; // 애니메이션 태그, 프리팹 이름

	public:
		// 텍스트
		void Set_UI_Text_Desc(const UI_TEXT_DESC& Desc) {
			Safe_Delete(m_pUITextDesc);
			m_pUITextDesc = new UI_TEXT_DESC(Desc);
		}

		UI_TEXT_DESC* Get_UI_Text_Desc() {
			return m_pUITextDesc;
		}

		// 텍스쳐
		void Set_UI_Texture_Desc(const UI_TEXTURE_DESC& Desc) {
			Safe_Delete(m_pUITextureDesc);
			m_pUITextureDesc = new UI_TEXTURE_DESC(Desc);
		}

		UI_TEXTURE_DESC* Get_UI_Texture_Desc() {
			return m_pUITextureDesc;
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
				Anim->second = TEXT("Unknown");
				return;
			}
		}

		map<wstring, wstring> Get_UI_Anim_Tags() { return m_AnimTags; }

		void Clear() {
			Safe_Delete(m_pUITextDesc);
			Safe_Delete(m_pUITextureDesc);
		}

	}UIBASE_DESC;
}