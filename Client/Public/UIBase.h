#pragma once

#include "Client_Defines.h"
#include "UIObject.h"

NS_BEGIN(Engine)
class CVIBuffer_Rect;
class CTexture;
class CShader;
NS_END

NS_BEGIN(Client)

class CGameManager;

class CUIBase abstract : public CUIObject
{
public:
	typedef struct tagUITextDesc
	{
		_wstring szText;
		_float4 vColor{ 1.f, 1.f, 1.f, 1.f };

	}UI_TEXT_DESC;

	typedef struct tagUITextureDesc
	{
		_wstring szTextureComTag;
		_uint iTextureIndex{ 0 };
		_uint iPass{ 0 };

	}UI_TEXTURE_DESC;

public:
	typedef struct tagUIBaseDesc : public CUIObject::UIOBJECT_DESC
	{
		_float fOffsetX{ 0.f }, fOffsetY{ 0.f };
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

		//void Clear() {
		//	//delete m_pUITextDesc; m_pUITextDesc = nullptr;
		//	//delete m_pUITextureDesc; m_pUITextureDesc = nullptr;
		//	
		//	Safe_Delete(m_pUITextDesc);
		//	Safe_Delete(m_pUITextureDesc);
		//}

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

	//UIBASE_DESC Get_UIBase_OriginDesc() { return m_tOriginUIDesc; }
	//void Set_UIBase_OriginDesc(UIBASE_DESC tDesc) {
	//	m_tOriginUIDesc = tDesc;
	//	m_tUIDesc = m_tOriginUIDesc;
	//}

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
	HRESULT Set_TextureCom(_wstring szTextureTag, _uint iTextureIndex);

protected:
	CVIBuffer_Rect*		m_pVIBufferCom = { nullptr };
	CTexture*			m_pTextureCom = { nullptr };
	CShader*			m_pShaderCom = { nullptr };

	CGameManager*		m_pGameManager = { nullptr };

	//UIBASE_DESC m_tOriginUIDesc{};
	UIBASE_DESC m_tUIDesc{};

	vector<CUIBase*>		m_Children = {};

private:
	HRESULT Ready_Texture();

protected:
	virtual HRESULT Ready_Components();
	virtual HRESULT Bind_ShaderResources();

public:
	virtual void Free() override;
};

NS_END