#include "CascadeShadow.h"
#include "Shader.h"

#include "GameInstance.h"
#include "Camera.h"

CCascadeShadow::CCascadeShadow(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) :
	CDeferred{ pDevice, pContext }
{
}

HRESULT CCascadeShadow::Initialize()
{
	if (FAILED(Ready_RenderTargets()))
		return E_FAIL;

	if (FAILED(Ready_CascadeShadowDSVs(m_vCascadeShadowMapSize.x, m_vCascadeShadowMapSize.y, CASCADE_LEVEL)))
		return E_FAIL;

	return S_OK;
}

HRESULT CCascadeShadow::Bind_RenderTarget(CShader* pShader, const _char* pConstantName)
{
	if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_CascadeShadow"), pShader, pConstantName)))
		return E_FAIL;

	return S_OK;
}

#ifdef _DEBUG
HRESULT CCascadeShadow::Ready_Debug(_float fX, _float fY, _float fSizeX, _float fSizeY)
{
	if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_CascadeShadow"), 150.0f, 750.0f, 300.f, 300.f)))
		return E_FAIL;

	return S_OK;
}

HRESULT CCascadeShadow::Render_Debug(CVIBuffer_Rect* pVIBuffer, CShader* pShader)
{
	if (FAILED(m_pGameInstance->Render_RT_Debug(TEXT("MRT_CascadeShadow"), pShader, pVIBuffer)))
		return E_FAIL;

	return S_OK;
}
#endif

HRESULT CCascadeShadow::Ready_Shadow_Light(const CASCADE_SHADOW_DESC& Desc)
{
	m_fCascadeEnds[0] = 0.1f;
	m_fCascadeEnds[1] = 10.0f,
	m_fCascadeEnds[2] = 20.0f,
	m_fCascadeEnds[3] = 40.f;
	m_fCascadeEnds[4] = 100.f;
	m_fCascadeEnds[5] = 500.f;
	m_tShadowLightDesc = Desc;

	return S_OK;
}

HRESULT CCascadeShadow::Bind_Shader_Resource(CShader* pShader, const _char* pConstantName, D3DTS eType)
{
	if (D3DTS::VIEW == eType)
		return pShader->Bind_Matrices(pConstantName, m_CasCadeTransformationViewMatrices, CASCADE_LEVEL);
	if (D3DTS::PROJ == eType)
		return pShader->Bind_Matrices(pConstantName, m_CasCadeTransformationProjMatrices, CASCADE_LEVEL);

	return E_FAIL;
}

HRESULT CCascadeShadow::Bind_CascadeEnds(CShader* pShader, const _char* pConstantName, const _char* pConstantName2)
{
	_float4 vPacked[2] =
	{
		{ m_fCascadeEnds[0], m_fCascadeEnds[1], m_fCascadeEnds[2], m_fCascadeEnds[3] },
		{ m_fCascadeEnds[4], m_fCascadeEnds[5], 0.f, 0.f }
	};

	pShader->Bind_RawValue("g_CascadeEnds0", &vPacked[0], sizeof(_float4));
	pShader->Bind_RawValue("g_CascadeEnds1", &vPacked[1], sizeof(_float4));

	return S_OK;
}

void CCascadeShadow::Update(_float fTimeDelta)
{
	Seperate_CascadeFrustum();
	Calc_CascadeMatrices();
}

HRESULT CCascadeShadow::Add_RenderObject(CGameObject* pRenderObject)
{
	m_CascadeShadowObjects.push_back(pRenderObject);
	Safe_AddRef(pRenderObject);

	return S_OK;
}

HRESULT CCascadeShadow::Render(CVIBuffer_Rect* pVIBuffer)
{
#ifdef _DEBUG
	m_pGameInstance->BeginMarker(m_pContext, TEXT("###### CASCADE SHADOW RENDER"));
#endif

	_uint2 vScreenSize = m_pGameInstance->GetScreenSize();
	m_pGameInstance->Set_ScreenSize(m_vCascadeShadowMapSize.x, m_vCascadeShadowMapSize.y);

	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_CascadeShadow"), m_pCascadeShadowDSV)))
		return E_FAIL;

	for (auto& pRenderObject : m_CascadeShadowObjects)
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render_Shadow();

		Safe_Release(pRenderObject);
	}
	m_CascadeShadowObjects.clear();

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;


	m_pGameInstance->Set_ScreenSize(vScreenSize.x, vScreenSize.y);

#ifdef _DEBUG
	m_pGameInstance->EndMarker(m_pContext);
#endif
	return S_OK;
}

void CCascadeShadow::Seperate_CascadeFrustum()
{
	CCamera* pCam = m_pGameInstance->GetMainCamera();
	if (nullptr == pCam)
		return;

	CAMERA_INFO CamDesc = {};
	CamDesc = pCam->GetCameraInfo();

	//월드 점 8개랑 모서리 벡터 4개 구하기 
	const _float4* pFrustumPoints = m_pGameInstance->Get_FrustumWorldPoints();
	// 얘가 들고 있는건 월드 상의 벡터고, Near에서 Far로 향하니까.. 
	// 진짜 비율 구해서 구하기만 해주면 된다.
	const _float4* pFrustumRays = m_pGameInstance->Get_FrustumWorldRays();

	for (_uint i = 0; i < CASCADE_LEVEL; ++i)
	{
		for (_uint j = 0; j < 4; ++j)
		{
			_vector vNearPlanePoints = XMLoadFloat4(&pFrustumPoints[j]);
			//near
			XMStoreFloat4(&m_vCascadeNearCorner[i][j], vNearPlanePoints + XMLoadFloat4(&pFrustumRays[j]) * (m_fCascadeEnds[i] - CamDesc.fNear) / (CamDesc.fFar - CamDesc.fNear));
			//far
			XMStoreFloat4(&m_vCascadeFarCorner[i][j], vNearPlanePoints + XMLoadFloat4(&pFrustumRays[j]) * (m_fCascadeEnds[i + 1] - CamDesc.fNear) / (CamDesc.fFar - CamDesc.fNear));
		}
	}

	Safe_Release(pCam);
}

void CCascadeShadow::Calc_CascadeMatrices()
{
	for (_uint i = 0; i < CASCADE_LEVEL; ++i)
	{
		_vector vCenter = {};

		//Center 구하기
		for (_uint j = 0; j < 4; ++j)
		{
			_float4 vNear = m_vCascadeNearCorner[i][j];
			_float4 vFar = m_vCascadeFarCorner[i][j];

			vCenter += XMLoadFloat4(&vNear) + XMLoadFloat4(&vFar);
		}
		vCenter /= 8.f;

		//거리가 음수일린 없으니까 ㅇㅇ
		_float fRadius = { -1.f };
		_float4 vFarthestPoint = {};

		//Radius 구하기 
		//near에 대해서 연산
		for (_uint j = 0; j < 4; ++j)
		{
			_float4 vPoint = m_vCascadeNearCorner[i][j];
			_float fDist = XMVectorGetX(XMVector3Length(XMLoadFloat4(&vPoint) - vCenter));

			if (fDist > fRadius)
			{
				fRadius = fDist;
				vFarthestPoint = m_vCascadeNearCorner[i][j];
			}
		}

		//far에 대해서 연산
		for (_uint j = 0; j < 4; ++j)
		{
			_float4 vPoint = m_vCascadeFarCorner[i][j];
			_float fDist = XMVectorGetX(XMVector3Length(XMLoadFloat4(&vPoint) - vCenter));

			if (fDist > fRadius)
			{
				fRadius = fDist;
				vFarthestPoint = m_vCascadeFarCorner[i][j];
			}
		}

		if (fRadius <= 0.f)
			return;

		//월드 상의 Cascade Position 저장.
		XMStoreFloat4(&m_vCascadePositions[i], XMVectorSetW(vCenter +
			(fRadius + 30.f) * XMVector3Normalize(XMLoadFloat4(&m_tShadowLightDesc.vDir)) * -1.f, 1.f));

		// 뷰 행렬 생성 (Position도 따로 연산해서)
		XMStoreFloat4x4(&m_CasCadeTransformationViewMatrices[i],
			XMMatrixLookAtLH(XMLoadFloat4(&m_vCascadePositions[i]), vCenter, XMVectorSet(0.f, 1.f, 0.f, 0.f)));

		_float fMinX = FLT_MAX, fMaxX = -FLT_MAX;
		_float fMinY = FLT_MAX, fMaxY = -FLT_MAX;
		_float fMinZ = FLT_MAX, fMaxZ = -FLT_MAX;

		for (_uint j = 0; j < 4; ++j)
		{
			_vector vNearWS = XMLoadFloat4(&m_vCascadeNearCorner[i][j]);
			_vector vNearLS = XMVector3TransformCoord(vNearWS, XMLoadFloat4x4(&m_CasCadeTransformationViewMatrices[i]));

			fMinX = min(fMinX, XMVectorGetX(vNearLS));
			fMaxX = max(fMaxX, XMVectorGetX(vNearLS));
			fMinY = min(fMinY, XMVectorGetY(vNearLS));
			fMaxY = max(fMaxY, XMVectorGetY(vNearLS));
			fMinZ = min(fMinZ, XMVectorGetZ(vNearLS));
			fMaxZ = max(fMaxZ, XMVectorGetZ(vNearLS));

			_vector vFarWS = XMLoadFloat4(&m_vCascadeFarCorner[i][j]);
			_vector vFarLS = XMVector3TransformCoord(vFarWS, XMLoadFloat4x4(&m_CasCadeTransformationViewMatrices[i]));

			fMinX = min(fMinX, XMVectorGetX(vFarLS));
			fMaxX = max(fMaxX, XMVectorGetX(vFarLS));
			fMinY = min(fMinY, XMVectorGetY(vFarLS));
			fMaxY = max(fMaxY, XMVectorGetY(vFarLS));
			fMinZ = min(fMinZ, XMVectorGetZ(vFarLS));
			fMaxZ = max(fMaxZ, XMVectorGetZ(vFarLS));
		}

		_float fPadding = 10.f;
		fMinZ -= fPadding;
		fMaxZ += fPadding;
		fMinX -= fPadding;
		fMaxX += fPadding;
		fMinY -= fPadding;
		fMaxY += fPadding;

		// 예외처리 
		if (fMaxX <= fMinX) fMaxX = fMinX + 1.f;
		if (fMaxY <= fMinY) fMaxY = fMinY + 1.f;
		if (fMaxZ <= fMinZ) fMaxZ = fMinZ + 1.f;

		_float fShadowResX = 2048.f; // 보통 2048
		_float fShadowResY = 2048.f; // 보통 2048 (정사각 텍스처면 X=Y)

		_float fExtentX = fMaxX - fMinX;
		_float fExtentY = fMaxY - fMinY;

		// 안전장치
		if (fExtentX <= 0.f) fExtentX = 1.f;
		if (fExtentY <= 0.f) fExtentY = 1.f;

		_float fTexelSizeX = fExtentX / fShadowResX;
		_float fTexelSizeY = fExtentY / fShadowResY;

		// min / max을 텍셀 단위로 양자화
		fMinX = floorf(fMinX / fTexelSizeX) * fTexelSizeX;
		fMaxX = ceilf(fMaxX / fTexelSizeX) * fTexelSizeX;

		fMinY = floorf(fMinY / fTexelSizeY) * fTexelSizeY;
		fMaxY = ceilf(fMaxY / fTexelSizeY) * fTexelSizeY;

		_float fZExtra = 200.f; // 장면 스케일에 맞게 크게
		fMinZ -= fZExtra;
		fMaxZ += fZExtra;

		// Proj 행렬 생성
		XMStoreFloat4x4(&m_CasCadeTransformationProjMatrices[i],
			XMMatrixOrthographicOffCenterLH(fMinX, fMaxX, fMinY, fMaxY, fMinZ, fMaxZ));
	}
}

HRESULT CCascadeShadow::Ready_RenderTargets()
{
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_CascadeShadow"), m_vCascadeShadowMapSize.x, m_vCascadeShadowMapSize.y, DXGI_FORMAT_R32_FLOAT, _float4(1.f, 0.f, 0.f, 0.f), CASCADE_LEVEL)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_CascadeShadow"), TEXT("Target_CascadeShadow"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CCascadeShadow::Ready_CascadeShadowDSVs(_uint iSizeX, _uint iSizeY, _uint iCSMLevel)
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
	TextureDesc.ArraySize = iCSMLevel;
	TextureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;
	TextureDesc.Usage = D3D11_USAGE_DEFAULT /* 정적 */;
	/* 추후에 어떤 용도로 바인딩 될 수 있는 View타입의 텍스쳐를 만들기위한 Texture2D입니까? */
	TextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	/*| D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE*/
	TextureDesc.CPUAccessFlags = 0;
	TextureDesc.MiscFlags = 0;

	D3D11_DEPTH_STENCIL_VIEW_DESC DsvDesc = {};

	DsvDesc.Format = TextureDesc.Format;
	DsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
	DsvDesc.Texture2DArray.FirstArraySlice = 0;
	DsvDesc.Texture2DArray.MipSlice = 0;
	DsvDesc.Texture2DArray.ArraySize = iCSMLevel;

	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &pDepthStencilTexture)))
		return E_FAIL;

	if (FAILED(m_pDevice->CreateDepthStencilView(pDepthStencilTexture, &DsvDesc, &m_pCascadeShadowDSV)))
		return E_FAIL;

	Safe_Release(pDepthStencilTexture);

	return S_OK;
}

CCascadeShadow* CCascadeShadow::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCascadeShadow* pInstance = new CCascadeShadow(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Create Failed : CCascadeShadow");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCascadeShadow::Free()
{
	__super::Free();

	Safe_Release(m_pCascadeShadowDSV);

	for (auto& iter : m_CascadeShadowObjects)
		Safe_Release(iter);
	m_CascadeShadowObjects.clear();
}
