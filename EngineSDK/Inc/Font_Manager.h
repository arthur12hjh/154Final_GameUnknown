#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class CFont_Manager final : public CBase
{
private:
	CFont_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CFont_Manager() = default;

public:
	HRESULT Add_Font(const _wstring& strFontTag, const _tchar* pFontFilePath);
	HRESULT Render(const _wstring& strFontTag, const _tchar* pText, const _float2& vPosition, _fvector vColor, _float fScale);

	_float2 Get_Text_Size(const _wstring& strFontTag, const _tchar* pText, bool bIgnoreWhitespace = true, float fScale = 1.f);

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	map<const _wstring, class CCustomFont*>			m_Fonts;

private:
	class CCustomFont* Find_Font(const _wstring& strFontTag);

public:
	static CFont_Manager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

NS_END