#include "pch.h"
#include "EffectSRV.h"

IMPLEMENT_SINGLETON(CEffectSRV);

HRESULT CEffectSRV::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	m_pDevice = pDevice;
	m_pContext = pContext;

	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	m_pContext->OMGetRenderTargets(1, nullptr, &m_pOriginalDSV);
	m_pOriginalDSV->GetResource(&m_pResourse);
	return S_OK;
}

void	CEffectSRV::Reset() {
	m_bisSRV = true;
}

ID3D11ShaderResourceView* CEffectSRV::Get_SRV()
{
	if (m_bisSRV) {
		m_bisSRV = false;
		ID3D11Texture2D* pDepthTexture = nullptr;
		D3D11_TEXTURE2D_DESC texDesc = {};
		ZeroMemory(&texDesc, sizeof(D3D11_TEXTURE2D_DESC));
		texDesc.Width = 1600;
		texDesc.Height = 900;
		texDesc.MipLevels = 1;
		texDesc.ArraySize = 1;
		texDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
		texDesc.SampleDesc.Count = 1;
		texDesc.Usage = D3D11_USAGE_DEFAULT;
		texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

		texDesc.SampleDesc.Quality = 0;
		texDesc.CPUAccessFlags = 0;
		texDesc.MiscFlags = 0;
		if (FAILED(m_pDevice->CreateTexture2D(&texDesc, nullptr, &pDepthTexture)))
			return nullptr;

		m_pContext->CopyResource(pDepthTexture, m_pResourse);
		Safe_Release(m_pSRV);
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = 1;
		srvDesc.Texture2D.MostDetailedMip = 0;
		if (FAILED(m_pDevice->CreateShaderResourceView(pDepthTexture, &srvDesc, &m_pSRV)))
			return nullptr;
		Safe_Release(pDepthTexture);
	}
    return m_pSRV;
}

void CEffectSRV::Free()
{
	__super::Free();
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pOriginalDSV);
	Safe_Release(m_pResourse);
	Safe_Release(m_pSRV);
}