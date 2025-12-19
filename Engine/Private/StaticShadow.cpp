#include "StaticShadow.h"
#include "Shader.h"

#include "GameInstance.h"
#include "GameObject.h"

CStaticShadow::CStaticShadow(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) :
	CDeferred{ pDevice, pContext }
{
}

HRESULT CStaticShadow::Initialize()
{
	if (FAILED(Ready_RenderTargets()))
		return E_FAIL;

	if (FAILED(Ready_StaticShadowDSVs(m_vStaticShadowMapSize.x, m_vStaticShadowMapSize.y)))
		return E_FAIL;

	return S_OK;
}

HRESULT CStaticShadow::Bind_RenderTarget(CShader* pShader, const _char* pConstantName)
{
	if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_StaticShadow"), pShader, pConstantName)))
		return E_FAIL;

	return S_OK;
}

HRESULT CStaticShadow::Ready_Shadow_Light(const STATIC_SHADOW_DESC& Desc)
{
	m_tShadowLightDesc = Desc;

	auto* pCascadeDesc = static_cast<CASCADE_SHADOW_DESC*>(m_pGameInstance->Get_Cascade_Desc());
	if (nullptr == pCascadeDesc)
		return E_FAIL;

	// 캐스케이드와 동일한 태양 방향 사용
	_vector vDir = XMVector3Normalize(XMLoadFloat4(&pCascadeDesc->vDir));
	_vector vAt = XMLoadFloat4(&Desc.vAt);
	_float fDistance = Desc.fFar * 0.5f;
	_vector vEye = vAt - vDir * fDistance;

	_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);

	XMStoreFloat4x4(&m_StaticTransformationMatrices[ENUM_CLASS(D3DTS::VIEW)],
		XMMatrixLookAtLH(vEye, vAt, vUp));

	XMStoreFloat4x4(&m_StaticTransformationMatrices[ENUM_CLASS(D3DTS::PROJ)],
		XMMatrixOrthographicLH(m_vStaticOrhtoSize.x, m_vStaticOrhtoSize.y, Desc.fNear, Desc.fFar));

	return S_OK;
}

#ifdef _DEBUG
HRESULT CStaticShadow::Ready_Debug(_float fX, _float fY, _float fSizeX, _float fSizeY)
{
	if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_StaticShadow"), 150.0f, 750.0f, 300.f, 300.f)))
		return E_FAIL;

	return S_OK;
}

HRESULT CStaticShadow::Render_Debug(CVIBuffer_Rect* pVIBuffer, CShader* pShader)
{
	if (FAILED(m_pGameInstance->Render_RT_Debug(TEXT("MRT_StaticShadow"), pShader, pVIBuffer)))
		return E_FAIL;

	return S_OK;
}
#endif

HRESULT CStaticShadow::Bind_Shader_Resource(CShader* pShader, const _char* pConstantName, D3DTS eType)
{
	if (FAILED(pShader->Bind_Matrix(pConstantName, &m_StaticTransformationMatrices[ENUM_CLASS(eType)])))
		return E_FAIL;

	return S_OK;
}

HRESULT CStaticShadow::Add_RenderObject(CGameObject* pRenderObject)
{
	m_StaticShadowObjects.push_back(pRenderObject);
	Safe_AddRef(pRenderObject);

	return S_OK;
}

HRESULT CStaticShadow::Render(CVIBuffer_Rect* pVIBuffer)
{
#ifdef _DEBUG
	m_pGameInstance->BeginMarker(m_pContext, TEXT("###### STAITC SHADOW RENDER"));
#endif

	_uint2 vScreenSize = m_pGameInstance->GetScreenSize();
	m_pGameInstance->Set_ScreenSize(m_vStaticShadowMapSize.x, m_vStaticShadowMapSize.y);

	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_StaticShadow"), m_pStaticShadowDSV)))
		return E_FAIL;

	for (auto& pRenderObject : m_StaticShadowObjects)
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render_Shadow();

		Safe_Release(pRenderObject);
	}
	m_StaticShadowObjects.clear();

	m_pGameInstance->Set_ScreenSize(vScreenSize.x, vScreenSize.y);

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

#ifdef _DEBUG
	m_pGameInstance->EndMarker(m_pContext);
#endif

	return S_OK;
}

HRESULT CStaticShadow::Ready_RenderTargets()
{
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_StaticShadow"), m_vStaticShadowMapSize.x, m_vStaticShadowMapSize.y, DXGI_FORMAT_R32_FLOAT, _float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_StaticShadow"), TEXT("Target_StaticShadow"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CStaticShadow::Ready_StaticShadowDSVs(_uint iSizeX, _uint iSizeY)
{
	if (nullptr == m_pDevice)
		return E_FAIL;

	ID3D11Texture2D* pDepthStencilTexture = nullptr;
	D3D11_TEXTURE2D_DESC	TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	/* 깊이 버퍼의 픽셀은 백버퍼의 픽셀과 갯수가 동일해야만 깊이 텍스트가 가능해진다. */
	/* 픽셀의 수가 다르면 아에 렌더링을 못함. */
	TextureDesc.Width = iSizeX;
	TextureDesc.Height = iSizeY;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;
	TextureDesc.Usage = D3D11_USAGE_DEFAULT /* 정적 */;
	/* 추후에 어떤 용도로 바인딩 될 수 있는 View타입의 텍스쳐를 만들기위한 Texture2D입니까? */
	TextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	/*| D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE*/
	TextureDesc.CPUAccessFlags = 0;
	TextureDesc.MiscFlags = 0;

	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &pDepthStencilTexture)))
		return E_FAIL;

	if (FAILED(m_pDevice->CreateDepthStencilView(pDepthStencilTexture, nullptr, &m_pStaticShadowDSV)))
		return E_FAIL;

	Safe_Release(pDepthStencilTexture);

	return S_OK;
}

CStaticShadow* CStaticShadow::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CStaticShadow* pInstance = new CStaticShadow(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Create Failed : CStaticShadow");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStaticShadow::Free()
{
	__super::Free();

	Safe_Release(m_pStaticShadowDSV);

	for (auto& iter : m_StaticShadowObjects)
		Safe_Release(iter);
	m_StaticShadowObjects.clear();
}
