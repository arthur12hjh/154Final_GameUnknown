#pragma once

#include "Client_Defines.h"
#include "UIBase.h"

NS_BEGIN(Engine)
class CTexture;
NS_END

NS_BEGIN(Client)

class CUISongSelector final : public CUIBase
{
private:
	typedef struct tagSongDesc
	{
		_wstring szSongName{};
		_tchar szVideoTag[256]{};
	}SONG_DESC;

private:
	CUISongSelector(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUISongSelector(const CUISongSelector& Prototype);
	virtual ~CUISongSelector() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	void Open_Song_Selector();
	void Close_Song_Selector();

protected:
	virtual HRESULT Ready_Components() override;
	virtual HRESULT Bind_ShaderResources() override;
	virtual HRESULT Execute(const UI_EVENT_DESC& EventDesc) override;
	virtual void CallbackEvent(void* pArg) override;

private:
	CTexture* m_pArrowTextureCom{ nullptr };
	CTexture* m_pSelectedBGTextureCom{ nullptr };
	CTexture* m_pScoreTextureCom{ nullptr };

	vector<SONG_DESC> m_SongDescs{};

	ID3D11RasterizerState* m_pScissorRasterizer{ nullptr };

	_int m_iCurrentIndex = 0;
	_float m_fScroll = 0.f;      // 누적 스크롤 (픽셀 단위)
	_float m_fItemGap = 84.f;    // 항목 간격
	_float m_fTargetScroll = 0.f;// 반드시 도달해야 할 위치
	_uint m_iVisibleCount = 5;
	_bool  m_bScrolling = false;

	_float m_fEnterKeyPressedTime = 0.f;
	_float m_fEscKeyPressedTime = 0.f;

	_bool m_isOpen = true;
	_bool m_isOpening = true;
	_bool m_isClosing = false;

	_bool m_bActiveEnter = false;
	_bool m_bActiveEsc = false;

private:
	HRESULT Render_Text();

	void Update_SongIndex(_float fTimeDelta);
	void Update_Thumbnail();

public:
	static CUISongSelector* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END