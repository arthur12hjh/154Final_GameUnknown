#pragma once

#include "Tool_UI_Defines.h"
#include "Base.h"
#include "UIBase.h"

NS_BEGIN(Engine)
class CGameInstance;
class CGameObject;
class CHUDLayer;
class CTexture;
NS_END

NS_BEGIN(Client)
class CUIBase;
class CUIPanel;
class CUIWrapper;
class CUIHUD;
NS_END

namespace GUI
{
	// 제목 그리기 헬퍼
	// color: 텍스트 색상 (기본 노란색)
	// fontScale: 텍스트 크기 배율 (1.0f = 기본)
	inline void Title(const char* szText, ImVec4 color = ImVec4(1.f, 0.8f, 0.f, 1.f), float fontScale = 1.2f)
	{
		ImGuiIO& io = ImGui::GetIO();

		// 기본 폰트
		ImFont* fontBackup = ImGui::GetFont();
		ImGui::PushStyleColor(ImGuiCol_Text, color);

		// 폰트 배율 적용
		ImGui::PushFont(io.Fonts->Fonts[0]); // 필요 시 다른 폰트로 바꿀 수 있음
		ImGui::SetWindowFontScale(fontScale);

		// 텍스트 출력
		ImGui::Text("%s", szText);

		// 스타일 복원
		ImGui::PopFont();
		ImGui::PopStyleColor();
		ImGui::SetWindowFontScale(1.0f);

		// 구분선
		ImGui::Separator();
		ImGui::Spacing();
	}
}

NS_BEGIN(Tool_UI)
class CUIResourceStore;

class CGUIManager : public CBase
{
	DECLARE_SINGLETON(CGUIManager);

private:
	CGUIManager();
	virtual ~CGUIManager() = default;

public:
	HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void Update(_float fTimeDelta);
	void Render();
	void Release_GUI_Manager();

private:
	ID3D11Device* m_pDevice{ nullptr };
	ID3D11DeviceContext* m_pContext{ nullptr };

private:
	CGameInstance* m_pGameInstance{ nullptr };
	CUIResourceStore* m_pUIResourceStore{ nullptr };

	_uint m_iCurrentLevel = 0;
	_uint m_iPrevLevel = 0;

	_uint m_iUICnt = 0;

	class Client::CUIHUD* m_pUIHUD{ nullptr };

	vector<_wstring> m_ViewModes{};
	_wstring m_szCurViewMode{ TEXT("Editor") };

	unordered_map<_wstring, CHUDLayer*>	m_pLayers;

	_char m_szCloneProtoTag[MAX_PATH]{};
	_char m_szCloneLayerTag[MAX_PATH]{};
	_char m_szCloneTextureComTag[MAX_PATH]{};
	_uint m_iCloneProtoLevel{ 0 };

	vector<_wstring> m_ProtoTags{};
	vector<_wstring> m_TextureComTags{};
	_wstring m_szCurrentProtoTag{};
	_wstring m_szCurrentTextureComTag{};

	_uint m_iCurrentTextureIndex{ 0 };

	_bool m_bOpenViewOptions{ false };
	class Client::CUIBase* m_pTargetUI{ nullptr };

	_float2 m_vOldPos{};
	_float2 m_vOldSize{};

	_float2 m_vEditedPos = { };
	_float2 m_vEditedSize = { };
	
	_char m_szInputText[MAX_PATH]{};
	_float4 m_vColor{ 1.f, 1.f, 1.f, 1.f };

	_char m_szInputAnimTag[MAX_PATH]{};

	_float4 m_vPram{ 0.f, 0.f, 0.f, 0.f };

	vector<_wstring> m_AnimTrackTags{};
	_char m_szCurrentTrackTag[MAX_PATH]{};

	vector<_wstring> m_AnimPrefabs{};
	_char m_szCurrentAnimPrefab[MAX_PATH]{};

private:
	void ViewMode();

	void Editor_Window();
	void Show_UIObject_List();

	void Create_Layer();
	void Add_Child(class Client::CUIBase* pObj);

	void Select_UI_Proto_Tag(_char* Outstr);
	void Select_Texture_Tag(_char* Outstr);

	void SetUp_UI_Proto_Tags();
	void SetUp_Texture_Tags();
	
	void View_Textures(_wstring szTag, void* pDesc);

	void Draw_Hierarchy(Client::CUIBase* pObj);

	void View_Options();

	void Set_Size(_float2* pOldSize, _float2* pEditedSize);
	void Set_Position(_float2* pOldPos, _float2* pEditedPos);
	void Set_Texture();
	void Set_Text();

	void Set_Animation();
	void Add_Animation();

	void Select_AnimTrack_Tags(_char* Outstr);

	void Set_AnimTrack(_wstring szAnimTag, _wstring szTrackTag);
	void Add_AnimTrack(_wstring szAnimTag);

	void Select_Anim_Prefabs(_char* Outstr);

	ID3D11ShaderResourceView* LoadTextureSRV(const _wstring& path);

public:
	virtual void Free() override;
};

NS_END
