#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class CPicking final : public CBase
{
private:
	CPicking(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CPicking() = default;

public:
	HRESULT					Initialize(HWND hWnd, _uint iSizeX, _uint iSizeY);
	void					Update();
	_bool					isPicking(_float3* pOut);

private:
	HWND					m_hWnd = { };
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };
	ID3D11Texture2D*		m_pTexture2D = { nullptr };
	class CGameInstance*	m_pGameInstance = { nullptr };

	_uint					m_iNumPixels = {};
	_float4*				m_pPixels = { nullptr };
	
	_uint					m_iNumPixelW = {};
	_uint					m_iNumPixelH = {};

public:
	static CPicking*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, HWND hWnd, _uint iSizeX, _uint iSizeY);
	virtual void			Free();
};

NS_END