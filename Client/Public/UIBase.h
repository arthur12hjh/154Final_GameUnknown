#pragma once

#include "Client_Defines.h"
#include "UIObject.h"

NS_BEGIN(Engine)
class CVIBuffer_Rect;
class CVIBuffer_Point;
class CTexture;
class CShader;
NS_END

NS_BEGIN(Client)
class CUIAnimationCom;

class CUIBase abstract : public CUIObject
{
public:
	enum class ANIM_STATE { STOP, PLAY, PAUSE, IDLE };

public:
	typedef struct tagUITextDesc
	{
		_wstring szText{};
		_float4 vColor{ 1.f, 1.f, 1.f, 1.f };

	}UI_TEXT_DESC;

	typedef struct tagUITextureDesc
	{
		_wstring szTextureComTag{};
		_wstring szProtoTag{};
		_uint iTextureIndex{ 0 };
		_uint iPass{ 0 };

	}UI_TEXTURE_DESC;

	typedef struct tagUIAnimTrackDesc
	{
		_wstring szTrackTag{};
		_float4 vStartParam{0.f, 0.f, 0.f, 0.f};
		_float4 vEndParam{0.f, 0.f, 0.f, 0.f};
		_float fStartTime{ 0.f };
		_float fEndTime{ 0.f };
		_float fSpeed{ 0.f };
	}UI_ANIM_TRACK_DESC;

	typedef struct tagUIAnimDesc
	{
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

public:
	typedef struct tagUIBaseDesc : public CUIObject::UIOBJECT_DESC
	{
		_float fOffsetX{ 0.f }, fOffsetY{ 0.f };
		_float fAlpha{ 1.f };
		_uint iLevel{ 0 };
		_uint iDepth{ 0 };
		_uint iRenderGroup{ ENUM_CLASS(RENDER::UI) };

		_wstring szUITag{};
		_wstring szLayerTag{};
		_wstring szProtoTag{};

		UI_TEXT_DESC* m_pUITextDesc{ nullptr };
		UI_TEXTURE_DESC* m_pUITextureDesc{ nullptr };
		map<_wstring, UI_ANIM_DESC*> m_pUIAnimDescs{};

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

		// 애니메이션
		void Add_UI_Anim_Desc(_wstring szAnimTag, const UI_ANIM_DESC& Desc) {
			
			auto AnimDesc = m_pUIAnimDescs.find(szAnimTag);

			if (AnimDesc != m_pUIAnimDescs.end())
				return;

			UI_ANIM_DESC* pDesc = new UI_ANIM_DESC(Desc);
			m_pUIAnimDescs.emplace(szAnimTag, pDesc);
		}

		UI_ANIM_DESC* Get_UI_Anim_Desc(_wstring szAnimTag) {
			auto AnimDesc = m_pUIAnimDescs.find(szAnimTag);

			if (AnimDesc == m_pUIAnimDescs.end())
				return nullptr;

			return AnimDesc->second;
		}

		map<_wstring, UI_ANIM_DESC*> Get_UI_Anim_Descs() {
			return m_pUIAnimDescs;
		}

	}UIBASE_DESC;

protected:
	CUIBase(ID3D11Device* pDevice, ID3D11DeviceContext* pContext); 
	CUIBase(const CUIBase& Prototype);
	virtual ~CUIBase() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	UIBASE_DESC Get_UIBase_Desc() { return m_tUIDesc; }
	void Set_UIBase_Desc(UIBASE_DESC pDesc) {
		m_tUIDesc = pDesc;
	}

	UIBASE_DESC Get_UIBase_OriginDesc() { return m_tOriginUIDesc; }
	void Set_UIBase_OriginDesc(UIBASE_DESC tDesc) {
		m_tOriginUIDesc = tDesc;
		m_tUIDesc = m_tOriginUIDesc;
	}

	_uint Get_Depth() { return m_tUIDesc.iDepth; }
	void Set_Depth(_uint iDepth) {
		m_tUIDesc.iDepth = iDepth;
	}

	void Set_Parent(CUIBase* pParent) {
		m_pParent = pParent;
	}
	CUIBase* Get_Parent() {
		return dynamic_cast<CUIBase*>(m_pParent);
	}

	HRESULT Add_Child(CGameObject* pObj);

	const vector<CUIBase*>* Get_Children() {
		return &m_Children;
	}

	void Set_Position(_float fX, _float fY);
	void Set_Size(_float fSizeX, _float fSizeY);
	void Set_Alpha(_float fAlpha);
	void Set_Pass(_uint iPass);
	HRESULT Set_TextureCom(_wstring szTextureTag, _wstring szProtoTag, _uint iTextureIndex);

	void Play_Anim(_wstring szAnimTag) {
		m_eAnimState = ANIM_STATE::PLAY;
		m_szCurrentAnimTag = szAnimTag;
	}
	void Pause_Anim() {
		m_eAnimState = ANIM_STATE::PAUSE;
	}
	void Stop_Anim() {
		m_eAnimState = ANIM_STATE::STOP;
	}

	ANIM_STATE Get_Anim_State() { return m_eAnimState; }

	CUIAnimationCom* Get_AnimationCom() { return m_pUIAnimCom; }

protected:
	CVIBuffer_Rect*		m_pVIBufferCom = { nullptr };
	CTexture*			m_pTextureCom = { nullptr };
	CShader*			m_pShaderCom = { nullptr };

	UIBASE_DESC m_tUIDesc{};
	UIBASE_DESC m_tOriginUIDesc{};

	vector<CUIBase*>		m_Children = {};
	CUIAnimationCom*		m_pUIAnimCom = { nullptr };

	_wstring				m_szCurrentAnimTag = {};

private:
	HRESULT Ready_Texture();
	HRESULT Ready_UIAnimation();

	ANIM_STATE m_eAnimState{ ANIM_STATE::IDLE };

#ifdef _DEBUG
	HRESULT Ready_Components_For_Debug();
	void Render_Debug_Rect();
	HRESULT Bind_Debug_ShaderResources();

	CVIBuffer_Point* m_pVIDebugBufferCom = { nullptr };
#endif

protected:
	virtual HRESULT Ready_Components();
	virtual HRESULT Bind_ShaderResources();

public:
	virtual void Free() override;
};

NS_END