#include "Picking.h"
#include "GameInstance.h"

CPicking::CPicking(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice { pDevice }
	, m_pContext { pContext }
	, m_pGameInstance { CGameInstance::GetInstance()}
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pGameInstance);
}

HRESULT CPicking::Initialize(HWND hWnd, _uint iSizeX, _uint iSizeY)
{
	m_hWnd = hWnd;

	D3D11_TEXTURE2D_DESC	TextureDesc{};	

	TextureDesc.Width = iSizeX;
	TextureDesc.Height = iSizeY;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;

	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;

	
	TextureDesc.Usage = D3D11_USAGE_STAGING;	
	TextureDesc.BindFlags = 0;
	TextureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
	TextureDesc.MiscFlags = 0;

	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &m_pTexture2D)))
		return E_FAIL;

	m_iNumPixels = iSizeX * iSizeY;
	m_pPixels = new _float4[m_iNumPixels];
	m_iNumPixelW = iSizeX;
	m_iNumPixelH = iSizeY;

	return S_OK;
}

void CPicking::Update()
{
	GetCursorPos(&m_ptMouse);
	ScreenToClient(m_hWnd, &m_ptMouse);

	m_pGameInstance->Copy_RenderTarget(TEXT("Target_Depth"), m_pTexture2D);
	D3D11_MAPPED_SUBRESOURCE		SubResource{};
	m_pContext->Map(m_pTexture2D, 0, D3D11_MAP_READ_WRITE, 0, &SubResource);
	memcpy(m_pPixels, SubResource.pData, sizeof(_float4) * m_iNumPixels);
	m_pContext->Unmap(m_pTexture2D, 0);	


}

_bool CPicking::isPicking(_float3* pOut)
{
	_uint iMouseIndex = m_ptMouse.y * m_iNumPixelW + m_ptMouse.x;
	if (0 > iMouseIndex || m_iNumPixels <= iMouseIndex)
		return false;

	if (0.f == m_pPixels[iMouseIndex].w)
		return false;

	_float3 vMousePos = _float3(0.f, 0.f, 0.f);

	vMousePos.x = m_ptMouse.x / (m_iNumPixelW * 0.5f) - 1.f;
	vMousePos.y = m_ptMouse.y / (m_iNumPixelH * -0.5f) + 1.f;
	vMousePos.z = m_pPixels[iMouseIndex].x;

	XMStoreFloat3(&vMousePos, XMVector3TransformCoord(XMLoadFloat3(&vMousePos), m_pGameInstance->Get_Transform_Matrix_Inverse(D3DTS::PROJ)));
	XMStoreFloat3(&vMousePos, XMVector3TransformCoord(XMLoadFloat3(&vMousePos), m_pGameInstance->Get_Transform_Matrix_Inverse(D3DTS::VIEW)));
	*pOut = vMousePos;
	return true;
}

CPicking* CPicking::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, HWND hWnd, _uint iSizeX, _uint iSizeY)
{
	CPicking* pInstance = new CPicking(pDevice, pContext);

	if (FAILED(pInstance->Initialize(hWnd, iSizeX, iSizeY)))
	{
		MSG_BOX("Failed to Created : CPicking");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPicking::Free()
{
	__super::Free();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pTexture2D);
	Safe_Release(m_pGameInstance);
	Safe_Delete_Array(m_pPixels);
}
