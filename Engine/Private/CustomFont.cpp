#include "CustomFont.h"

CCustomFont::CCustomFont(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice { pDevice }
	, m_pContext { pContext }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CCustomFont::Initialize(const _tchar* pFontFilePath)
{
	m_pBatch = new SpriteBatch(m_pContext);
	m_pFont = new SpriteFont(m_pDevice, pFontFilePath);

	return S_OK;
}

HRESULT CCustomFont::Render(const _tchar* pText, const _float2& vPosition, _fvector vColor, _float fScale)
{
	m_pBatch->Begin();

	_float2 Zero{ 0.f, 0.f };

	m_pFont->DrawString(m_pBatch, pText, vPosition, vColor, 0.f, Zero, fScale);

	m_pBatch->End();

	return S_OK;
}

_float2 CCustomFont::Get_Text_Size(const _tchar* pText, bool bIgnoreWhitespace, float fScale) const
{
	if (!pText || !m_pFont)
		return _float2{ 0.f, 0.f };

	// MeasureString: X = width, Y = height (여러 줄이면 최대폭 / 전체높이)
	XMVECTOR vSize = m_pFont->MeasureString(pText, bIgnoreWhitespace);
	float width = XMVectorGetX(vSize) * fScale;
	float height = XMVectorGetY(vSize) * fScale;

	return _float2{ width, height };
}

CCustomFont* CCustomFont::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pFontFilePath)
{
	CCustomFont* pInstance = new CCustomFont(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pFontFilePath)))
	{
		MSG_BOX("Failed to Created : CCustomFont");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCustomFont::Free()
{
	__super::Free();

	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);

	Safe_Delete(m_pBatch);
	Safe_Delete(m_pFont);
}
