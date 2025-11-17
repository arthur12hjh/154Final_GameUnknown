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
class CUIHUD;

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
	void Set_Anim_State(ANIM_STATE eState) { m_eAnimState = eState; }

	CUIAnimationCom* Get_AnimationCom() { return m_pUIAnimCom; }

#ifdef _DEBUG
	void Render_Debug_Rect();
#endif

protected:
	CVIBuffer_Rect*		m_pVIBufferCom = { nullptr };
	CTexture*			m_pTextureCom = { nullptr };
	CShader*			m_pShaderCom = { nullptr };

	UIBASE_DESC m_tUIDesc{};
	UIBASE_DESC m_tOriginUIDesc{};

	vector<CUIBase*>		m_Children = {};
	CUIAnimationCom*		m_pUIAnimCom = { nullptr };
	CUIHUD*					m_pUIHUD = { nullptr };

	_wstring				m_szCurrentAnimTag = {};

private:
	HRESULT Ready_Texture();
	HRESULT Ready_UIAnimation();

	ANIM_STATE m_eAnimState{ ANIM_STATE::IDLE };

#ifdef _DEBUG
	HRESULT Ready_Components_For_Debug();
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