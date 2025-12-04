#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class CCustomFont final : public CBase
{
private:
	CCustomFont(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CCustomFont() = default;

public:
	HRESULT Initialize(const _tchar* pFontFilePath);
	HRESULT Render(const _tchar* pText, const _float2& vPosition, _fvector vColor, _float fScale = 1.f);	

	_float2 Get_Text_Size(const _tchar* pText, bool bIgnoreWhitespace = true, float fScale = 1.f) const;

private:
	ID3D11Device*				m_pDevice = { nullptr };
	ID3D11DeviceContext*		m_pContext = { nullptr };
	SpriteBatch*		m_pBatch = { nullptr };
	SpriteFont*			m_pFont = { nullptr };

public:
	static CCustomFont* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pFontFilePath);
	virtual void Free() override;
};

NS_END