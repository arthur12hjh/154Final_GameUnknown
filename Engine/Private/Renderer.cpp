#include "Renderer.h"

#include "GameObject.h"

#include "BlendObject.h"
#include "UIObject.h"
#include "GameInstance.h"
#include "Occlusion.h"

#include "ColliderRenderer.h"

#include "RadialBlur.h"
#include "Blur.h"
#include "Distortion.h"
#include "Glow.h"
#include "Bloom.h"
#include "Fog.h"
#include "DepthofField.h"
#include "MotionBlur.h"
#include "SSAO.h"
#include "Emissive.h"

CRenderer::CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
	, m_pGameInstance { CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
    Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CRenderer::Set_ScreenSize(_uint iSizeX, _uint iSizeY)
{
	D3D11_VIEWPORT			ViewPortDesc;
	ZeroMemory(&ViewPortDesc, sizeof(D3D11_VIEWPORT));
	ViewPortDesc.TopLeftX = 0;
	ViewPortDesc.TopLeftY = 0;
	ViewPortDesc.Width = (_float)iSizeX;
	ViewPortDesc.Height = (_float)iSizeY;
	ViewPortDesc.MinDepth = 0.f;
	ViewPortDesc.MaxDepth = 1.f;

	m_pContext->RSSetViewports(1, &ViewPortDesc);

	return S_OK;
}

HRESULT CRenderer::Initialize()
{
	/* 스크린 사이즈 세팅 */
	m_vScreenSize = m_pGameInstance->GetScreenSize();
	/* 섀도우 맵 사이즈 세팅 */
	m_vShadowMapSize = _uint2{ 8192, 4608 };

	/* 렌더타겟 세팅 */
	if (FAILED(Ready_RenderTargets()))
		return E_FAIL;

	/* 세팅한 렌더타겟 MRT에 등록*/
	if (FAILED(Ready_MRTs()))
		return E_FAIL;

	/* 디퍼드 셰이더 파일 로딩 */
	m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Deferred.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements);
	if (nullptr == m_pShader)
		return E_FAIL;

	/* 직교용 렉트 하나 생성. */
	m_pVIBuffer = CVIBuffer_Rect::Create(m_pDevice, m_pContext);
	if (nullptr == m_pVIBuffer)
		return E_FAIL;

	/* 직교용 월드 뷰 투영 세팅 */
	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixScaling((_float)m_vScreenSize.x, (_float)m_vScreenSize.y, 1.f));
	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)m_vScreenSize.x, (_float)m_vScreenSize.y, 0.f, 1.f));

	m_pBlur = CBlur::Create(m_pDevice, m_pContext);
	if (nullptr == m_pBlur)
		return E_FAIL;

	m_pDistortion = CDistortion::Create(m_pDevice, m_pContext);
	if (nullptr == m_pDistortion)
		return E_FAIL;

	m_pGlow = CGlow::Create(m_pDevice, m_pContext);
	if (nullptr == m_pGlow)
		return E_FAIL;

	m_pBloom = CBloom::Create(m_pDevice, m_pContext);
	if (nullptr == m_pBloom)
		return E_FAIL;

	m_pFog = CFog::Create(m_pDevice, m_pContext);
	if (nullptr == m_pFog)
		return E_FAIL;

	m_pRadialBlur = CRadialBlur::Create(m_pDevice, m_pContext);
	if (nullptr == m_pRadialBlur)
		return E_FAIL; 

	m_pDepthofField = CDepthofField::Create(m_pDevice, m_pContext);
	if (nullptr == m_pDepthofField)
		return E_FAIL;

	m_pMotionBlur = CMotionBlur::Create(m_pDevice, m_pContext);
	if (nullptr == m_pMotionBlur)
		return E_FAIL;

	m_pSSAO = CSSAO::Create(m_pDevice, m_pContext);
	if (nullptr == m_pSSAO)
		return E_FAIL;

	m_pEmissive = CEmissive::Create(m_pDevice, m_pContext);
	if (nullptr == m_pEmissive)
		return E_FAIL;

	/* 스크린 사이즈는 미리 바인딩 한다. */
	if (FAILED(m_pShader->Bind_RawValue("g_iWinSizeX", &m_vScreenSize.x, sizeof(_int))))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_RawValue("g_iWinSizeY", &m_vScreenSize.y, sizeof(_int))))
		return E_FAIL;

	/* 디버그 렌더링 준비 */
#ifdef _DEBUG
	if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_Scene"), 150.0f, 150.0f, 300.f, 300.f)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_Depth"), 150.0f, 150.0f, 300.f, 300.f)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_Normal"), 150.0f, 450.0f, 300.f, 300.f)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_ORM"), 450.0f, 150.0f, 300.f, 300.f)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_Shade"), 450.0f, 450.0f, 300.f, 300.f)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_Specular"), 750.0f, 450.0f, 300.f, 300.f)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_Shadow"), 150.0f, 750.0f, 300.f, 300.f)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_Velocity"), 750.0f, 150.0f, 300.f, 300.f)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_SpecDetail"), 750.0f, 450.0f, 300.f, 300.f)))
		return E_FAIL;

	if (FAILED(m_pGlow->Ready_Debug(m_vScreenSize.x - 450.f, 150.f, 300.f, 300.f)))
		return E_FAIL;
	if (FAILED(m_pBlur->Ready_Debug(m_vScreenSize.x - 150.f, 450.f, 300.f, 300.f)))
		return E_FAIL;
	if (FAILED(m_pDistortion->Ready_Debug(m_vScreenSize.x - 150.f, 150.f, 300.f, 300.f)))
		return E_FAIL;
	if (FAILED(m_pBloom->Ready_Debug(m_vScreenSize.x - 450.f, 450.f, 300.f, 300.f)))
		return E_FAIL;
	if (FAILED(m_pMotionBlur->Ready_Debug(150.f, 150.f, 300.f, 300.f)))
		return E_FAIL;
	if (FAILED(m_pSSAO->Ready_Debug(450.f, 450.f, 300.f, 300.f)))
		return E_FAIL;
	//if (FAILED(m_pFog->Ready_Debug(750.f, 150.f, 300, 300)))
	//	return E_FAIL;

	m_pColliderRenderer = CColliderRenderer::Create(m_pDevice, m_pContext);
	if (nullptr == m_pColliderRenderer)
		return E_FAIL;
#endif

    return S_OK;
}

void CRenderer::Update(_float fTimeDelta)
{
#ifdef _DEBUG
	if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_F2))
		m_isDebugVisible = !m_isDebugVisible;
#endif
	if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_F5))
		m_isBloom = !m_isBloom;
	if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_F6))
		m_isFog = !m_isFog;
	if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_F7))
		m_isHDR = !m_isHDR;
	if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_F8))
		m_pDepthofField->Set_Active();
	// SSAO 토글.
	if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_F9))
		m_isSSAO = !m_isSSAO;

	m_pRadialBlur->Update(fTimeDelta);
}

HRESULT CRenderer::Ready_RenderTargets()
{
	/* 후처리 쉐이딩을 위한 렌더타겟들을 준비. */
	/* Target_Scene */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Scene"), m_vScreenSize.x, m_vScreenSize.y, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.0f, 0.0f, 1.f, 1.f))))
		return E_FAIL;
	/* Target_BloomScene */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_BloomScene"), m_vScreenSize.x, m_vScreenSize.y, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.0f, 0.0f, 1.f, 1.f))))
		return E_FAIL;
	/* Target_Screen */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Screen"), m_vScreenSize.x, m_vScreenSize.y, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.0f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	
	/* MRT_GameObjects. 들어갈거 생각하면 최소 6개. */
	/* Target_Diffuse */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Diffuse"), m_vScreenSize.x, m_vScreenSize.y, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.0f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	/* Target_Normal */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Normal"), m_vScreenSize.x, m_vScreenSize.y, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.0f, 0.f, 0.f, 1.f))))
		return E_FAIL;
	/* Target_Depth */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Depth"), m_vScreenSize.x, m_vScreenSize.y, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.0f, 1.f, 0.f, 0.f))))
		return E_FAIL;
	/* Target_ORM */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_ORM"), m_vScreenSize.x, m_vScreenSize.y, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	/* Target_Emissive */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Emissive"), m_vScreenSize.x, m_vScreenSize.y, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.0f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	/* Target_SpecDetail */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_SpecDetail"), m_vScreenSize.x, m_vScreenSize.y, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(1.0f, 1.f, 1.f, 1.f))))
		return E_FAIL;

	/* MRT_LightAcc */
	/* Target_Shade */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Shade"), m_vScreenSize.x, m_vScreenSize.y, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.0f, 0.f, 0.f, 1.f))))
		return E_FAIL;
	/* Target_Specular */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Specular"), m_vScreenSize.x, m_vScreenSize.y, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.0f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	/* Target_Shadow. */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Shadow"), m_vShadowMapSize.x, m_vShadowMapSize.y, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;
	if (FAILED(Ready_Shadow_DepthStencilView(m_vShadowMapSize.x, m_vShadowMapSize.y)))
		return E_FAIL;

	/* Target_CascadeShadow */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_CascadeShadow"), m_vCascadeShadowMapSize.x, m_vCascadeShadowMapSize.y, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(1.f, 0.f, 0.f, 0.f), CASCADE_LEVEL)))
		return E_FAIL;
	if (FAILED(Ready_CascadeShadow_DepthStencilView(m_vCascadeShadowMapSize.x, m_vCascadeShadowMapSize.y, CASCADE_LEVEL)))
		return E_FAIL;

	/* Target_ToneMapping */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_ToneMapping"), m_vScreenSize.x, m_vScreenSize.y, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.0f, 0.0f, 1.f, 1.f))))
		return E_FAIL;

	/* Target_Velocity */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Velocity"), m_vScreenSize.x, m_vScreenSize.y, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.0f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Ready_MRTs()
{
	/* MRT_Scene*/
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Scene"), TEXT("Target_Scene"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Scene"), TEXT("Target_BloomScene"))))
		return E_FAIL;

	/* MRT_Screen */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Screen"), TEXT("Target_Screen"))))
		return E_FAIL;
	
	/* MRT_GameObjects */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Diffuse"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Normal"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Depth"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_ORM"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Emissive"))))
		return E_FAIL;

	/* MRT_LightAcc */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_Shade"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_Specular"))))
		return E_FAIL;

	/* MRT_Shadow */
	// 일반 그림자는 일단 꺼준다.
	//if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Shadow"), TEXT("Target_Shadow"))))
	//	return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Shadow"), TEXT("Target_CascadeShadow"))))
		return E_FAIL;

	/* MRT_ToneMapping */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_ToneMapping"), TEXT("Target_ToneMapping"))))
		return E_FAIL;

	/* MRT_Velocity */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Velocity"), TEXT("Target_Velocity"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Ready_Shadow_DepthStencilView(_uint iSizeX, _uint iSizeY)
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

	if (FAILED(m_pDevice->CreateDepthStencilView(pDepthStencilTexture, nullptr, &m_pShadowDSV)))
		return E_FAIL;

	Safe_Release(pDepthStencilTexture);

	return S_OK;
}

HRESULT CRenderer::Ready_CascadeShadow_DepthStencilView(_uint iSizeX, _uint iSizeY, _uint iCSMLevel)
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

void CRenderer::Active_RadialBlur(_float fLifeTime, _uint iSampleCount, _float fSamplePower)
{
	m_pRadialBlur->Set_Active(iSampleCount, fSamplePower, fLifeTime);
}

HRESULT CRenderer::Add_RenderGroup(RENDER eRenderGroup, CGameObject* pRenderObject)
{
	if (nullptr == pRenderObject)
		return E_FAIL;
	
	if (eRenderGroup == RENDER::BLUR)
		m_pBlur->Add_RenderObject(pRenderObject);
	else if (eRenderGroup == RENDER::DISTORTION)
		m_pDistortion->Add_RenderObject(pRenderObject);
	else if (eRenderGroup == RENDER::GLOW)
		m_pGlow->Add_RenderObject(pRenderObject);
	else
	{
		m_RenderObjects[ENUM_CLASS(eRenderGroup)].push_back(pRenderObject);
		Safe_AddRef(pRenderObject);
	}

	return S_OK;
}

void CRenderer::Render()
{
	Bind_WVP_Matrices();

	Render_Priority();
	Render_Shadow();
	Render_NonBlend();
	Render_LightAcc();

	if (FAILED(m_pGameInstance->Load_MRT(TEXT("MRT_Scene"))))
		return;

	Render_Combined();
	Render_NonLight();
	Render_Blend();

	if (FAILED(m_pGameInstance->End_MRT()))
		return;

	Render_MotionBlur();
	//렌더 타겟 내용을 백버퍼로 뱉어내게 하기.
	Render_Deferred();
	Render_ScreenDeferred();

	ToneMapping();

	Render_BackBuffer();
	Render_UI();

#ifdef _DEBUG
	Render_Debug();
#endif
}

void CRenderer::Clear_Render()
{
	for (auto& RenderObjects : m_RenderObjects)
	{
		for (auto& pRenderObject : RenderObjects)
			Safe_Release(pRenderObject);
		RenderObjects.clear();
	}
}

const _float4x4* CRenderer::Get_Renderer_Matrix(D3DTS eType)
{
	switch (eType)
	{
	case D3DTS::VIEW:
		return &m_ViewMatrix;
	case D3DTS::PROJ:
		return &m_ProjMatrix;
	default:
		return &m_WorldMatrix;
	}
}

void* CRenderer::Get_DoF_Desc()
{
	return m_pDepthofField->Get_Desc();
}

void* CRenderer::Get_Bloom_Desc()
{
	return m_pBloom->Get_Desc();
}

void* CRenderer::Get_Fog_Desc()
{
	return m_pFog->Get_Desc();
}

void* CRenderer::Get_SSAO_Desc()
{
	return m_pSSAO->Get_Desc();
}

void* CRenderer::Get_MotionBlur_Desc()
{
	return m_pMotionBlur->Get_Desc();
}

void* CRenderer::Get_Volumetric_Desc()
{
	m_VolumetricDesc.fDensity = &m_fDensity;
	m_VolumetricDesc.fStepSize = &m_fStepSize;
	m_VolumetricDesc.fVolumetricG = &m_fVolumetricG;

	return &m_VolumetricDesc;
}

void* CRenderer::Get_HDR_Desc()
{
	m_HDRDesc.fHDRExposure = &m_fHDRExposure;
	m_HDRDesc.isHDR = &m_isHDR;

	return &m_HDRDesc;
}

void CRenderer::Render_Priority()
{
	/* Diffuse + Normal */
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Scene"))))
		return;

	for (auto& pRenderObject : m_RenderObjects[ENUM_CLASS(RENDER::PRIORITY)])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render();

		Safe_Release(pRenderObject);
	}

	m_RenderObjects[ENUM_CLASS(RENDER::PRIORITY)].clear();

	if (FAILED(m_pGameInstance->End_MRT()))
		return;
}

void CRenderer::Render_Shadow()
{
	BeginMarker(m_pContext, TEXT("SHADOW TEST RENDEr"));

	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Shadow"), m_pCascadeShadowDSV)))
		return;
	
	if(FAILED(m_pGameInstance->Bind_Shader_Resource_Cascade(m_pShader, "g_LightViewMatrix", D3DTS::VIEW)))
		return;

	if (FAILED(m_pGameInstance->Bind_Shader_Resource_Cascade(m_pShader, "g_LightProjMatrix", D3DTS::PROJ)))
		return;

	Set_ScreenSize(m_vCascadeShadowMapSize.x, m_vCascadeShadowMapSize.y);

	for (auto& pRenderObject : m_RenderObjects[ENUM_CLASS(RENDER::SHADOW)])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render_Shadow();

		Safe_Release(pRenderObject);
	}
	
	m_RenderObjects[ENUM_CLASS(RENDER::SHADOW)].clear();

	if (FAILED(m_pGameInstance->End_MRT()))
		return;

	Set_ScreenSize(m_vScreenSize.x, m_vScreenSize.y);

	EndMarker(m_pContext);
}

void CRenderer::Render_MotionBlur()
{
	// 모션블러 pass로는 클라이언트에서 처리
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Velocity"))))
		return;

	for (auto& pRenderObject : m_RenderObjects[ENUM_CLASS(RENDER::MOTIONBLUR)])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render_MotionBlur();

		Safe_Release(pRenderObject);
	}

	m_RenderObjects[ENUM_CLASS(RENDER::MOTIONBLUR)].clear();

	if (FAILED(m_pGameInstance->End_MRT()))
		return;
}

void CRenderer::Render_NonBlend()
{
	/* Diffuse + Normal */
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_GameObjects"))))
		return;

	for (auto& pRenderObject : m_RenderObjects[ENUM_CLASS(RENDER::NONBLEND)])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render();

		Safe_Release(pRenderObject);
	}

	m_RenderObjects[ENUM_CLASS(RENDER::NONBLEND)].clear();

	if (FAILED(m_pGameInstance->End_MRT()))
		return;
}

void CRenderer::Render_LightAcc()
{
	/* Shade + Specular */
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_LightAcc"))))
		return ;

	if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_Normal"), m_pShader, "g_NormalTexture")))
		return;

	if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_Depth"), m_pShader, "g_DepthTexture")))
		return;

	if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_ORM"), m_pShader, "g_ORMTexture")))
		return;

	if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_Diffuse"), m_pShader, "g_DiffuseTexture")))
		return;

	m_pVIBuffer->Bind_Resources();

	if (FAILED(m_pGameInstance->Render_Lights(m_pShader, m_pVIBuffer)))
		return;

	if (FAILED(m_pGameInstance->End_MRT()))
		return;

	//SSAO 차폐 연산.
	m_pSSAO->Render(m_pVIBuffer, TEXT("Target_Depth"), TEXT("Target_Normal"), TEXT("NON_USE"));

	/*
	//볼메는 서비스 종료다...
	//Set_ScreenSize(m_vScreenSize.x / 4.f, m_vScreenSize.y / 4.f);

	//for (_uint i = 0; i < CASCADE_LEVEL; ++i)
	//{
	//	if (FAILED(m_pGameInstance->Bind_Shader_Resource_Cascade(m_pShader, "g_LightViewMatrix", D3DTS::VIEW, i)))
	//		return;
	//	if (FAILED(m_pGameInstance->Bind_Shader_Resource_Cascade(m_pShader, "g_LightProjMatrix", D3DTS::PROJ, i)))
	//		return;
	//}

	//if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_Depth"), m_pShader, "g_DepthTexture")))
	//	return;

	//if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_Shadow"), m_pShader, "g_ShadowTexture")))
	//	return;

	//m_pShader->Bind_RawValue("g_fDensity", &m_fDensity, sizeof(_float));
	//m_pShader->Bind_RawValue("g_fStepSize", &m_fStepSize, sizeof(_float));
	//m_pShader->Bind_RawValue("g_fVolumetricG", &m_fVolumetricG, sizeof(_float));

	//m_pVIBuffer->Bind_Resources();

	//if (FAILED(m_pGameInstance->Render_VolumetricLights(m_pShader, m_pVIBuffer)))
	//	return;

	//if (FAILED(m_pGameInstance->End_MRT()))
	//	return;

	//Set_ScreenSize(m_vScreenSize.x, m_vScreenSize.y);
	*/
}

void CRenderer::Render_Combined()
{
	if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_Diffuse"), m_pShader, "g_DiffuseTexture")))
		return;
	if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_Shade"), m_pShader, "g_ShadeTexture")))
		return;
	if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_Specular"), m_pShader, "g_SpecularTexture")))
		return;
	if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_Depth"), m_pShader, "g_DepthTexture")))
		return;
	if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_CascadeShadow"), m_pShader, "g_CascadeShadowTexture")))
		return;
	if (FAILED(m_pGameInstance->Bind_Cascade_Ends(m_pShader, "g_fCascadeEnds0", "g_fCascadeEnds1")))
		return;
	//if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT(""), m_pShader, "g_ShadowTexture")))
	//	return;


	if (false == m_isSSAO)
		m_pGameInstance->Clear_MRT(TEXT("MRT_SSAO_BlurY"));

	if (FAILED(m_pSSAO->Bind_RenderTarget(m_pShader, "g_SSAOTexture")))
		return;

	BeginMarker(m_pContext, TEXT("########## Combine Render"));

	m_pShader->Begin(ENUM_CLASS(SHADER_DEFERRED_IDX::COMBINED));

	m_pVIBuffer->Bind_Resources();
	m_pVIBuffer->Render();

	EndMarker(m_pContext);
}

void CRenderer::Render_NonLight()
{
	for (auto& pRenderObject : m_RenderObjects[ENUM_CLASS(RENDER::NONLIGHT)])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render();

		Safe_Release(pRenderObject);
	}

	m_RenderObjects[ENUM_CLASS(RENDER::NONLIGHT)].clear();
}

void CRenderer::Render_Blend()
{	
	m_RenderObjects[ENUM_CLASS(RENDER::BLEND)].sort([](CGameObject* pSour, CGameObject* pDest)->_bool {
		return static_cast<CBlendObject*>(pSour)->Get_Depth() > static_cast<CBlendObject*>(pDest)->Get_Depth();
	});

	for (auto& pRenderObject : m_RenderObjects[ENUM_CLASS(RENDER::BLEND)])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render();

		Safe_Release(pRenderObject);
	}

	m_RenderObjects[ENUM_CLASS(RENDER::BLEND)].clear();
}

void CRenderer::Render_Deferred()
{
	HRESULT hr = m_pBlur->Render(m_pVIBuffer);
	hr = m_pEmissive->Render(m_pVIBuffer);
	hr = m_pGlow->Render(m_pVIBuffer);
	hr = m_pDistortion->Render(m_pVIBuffer);
	hr = m_pBloom->Render(m_pVIBuffer, TEXT("Target_BloomScene"), TEXT("MRT_Scene"));
	hr = m_pFog->Render(m_pVIBuffer);

	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Screen"))))
		return;

	if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_Scene"), m_pShader, "g_SceneTexture")))
		return;

	//emissive
	if (FAILED(m_pEmissive->Bind_RenderTarget(m_pShader, "g_EmissiveFinalTexture")))
		return;

	//blur
	if (FAILED(m_pBlur->Bind_RenderTarget(m_pShader, "g_BlurFinalTexture")))
		return;
	if (FAILED(m_pBlur->Bind_RenderTarget(m_pShader, "g_BlurWeightTexture")))
		return;
	//glow
	if (FAILED(m_pGlow->Bind_RenderTarget(m_pShader, "g_GlowFinalTexture")))
		return;
	if (FAILED(m_pGlow->Bind_RenderTarget(m_pShader, "g_GlowWeightTexture")))
		return;

	if (FAILED(m_pDistortion->Bind_RenderTarget(m_pShader, "g_DistortionTexture")))
		return;

	if (false == m_isBloom)
		m_pGameInstance->Clear_MRT(TEXT("MRT_Bloom_Final"));

	if (FAILED(m_pBloom->Bind_RenderTarget(m_pShader, "g_BloomTexture")))
		return;

	//fog 바인딩.
	if (false == m_isFog)
		m_pGameInstance->Clear_MRT(TEXT("MRT_Fog"));

	_float4 vFogColor = m_pFog->Get_FogColor();
	if (FAILED(m_pShader->Bind_RawValue("g_vFogColor", &vFogColor, sizeof(_float4))))
		return;
	if (FAILED(m_pFog->Bind_RenderTarget(m_pShader, "g_FogTexture")))
		return;

	m_pShader->Begin(ENUM_CLASS(SHADER_DEFERRED_IDX::DEFERRED));
	m_pVIBuffer->Bind_Resources();
	m_pVIBuffer->Render();

	if (FAILED(m_pGameInstance->End_MRT()))
		return;
}

void CRenderer::Render_ScreenDeferred()
{
	// DOF 먼저 적용.
	m_pDepthofField->Render(m_pVIBuffer, TEXT("Target_Screen"), TEXT("Target_Depth"), TEXT("MRT_Screen"));
	m_pMotionBlur->Render(m_pVIBuffer, TEXT("Target_Screen"), TEXT("MRT_Screen"));
	m_pRadialBlur->Render(m_pVIBuffer, TEXT("Target_Screen"), TEXT("MRT_Screen"));
}

void CRenderer::ToneMapping()
{
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_ToneMapping"))))
		return;

	if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_Screen"), m_pShader, "g_ScreenTexture")))
		return;

	if (FAILED(m_pShader->Bind_RawValue("g_fHDRExposure", &m_fHDRExposure, sizeof(_float))))
		return;

	if(true == m_isHDR)
		m_pShader->Begin(ENUM_CLASS(SHADER_DEFERRED_IDX::TONE_MAPPING));
	else
		m_pShader->Begin(ENUM_CLASS(SHADER_DEFERRED_IDX::FINAL));

	m_pVIBuffer->Bind_Resources();
	m_pVIBuffer->Render();

	if (FAILED(m_pGameInstance->End_MRT()))
		return;
}

void CRenderer::Render_BackBuffer()
{	
	if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_ToneMapping"), m_pShader, "g_ScreenTexture")))
		return;

	m_pShader->Begin(ENUM_CLASS(SHADER_DEFERRED_IDX::FINAL));
	m_pVIBuffer->Bind_Resources();
	m_pVIBuffer->Render();
}

void CRenderer::Render_UI()
{
	m_RenderObjects[ENUM_CLASS(RENDER::UI)].sort([](CGameObject* pSour, CGameObject* pDest)->_bool {
		return static_cast<CUIObject*>(pSour)->GetZOrder() < static_cast<CUIObject*>(pDest)->GetZOrder();
		});

	for (auto& pRenderObject : m_RenderObjects[ENUM_CLASS(RENDER::UI)])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render();

		Safe_Release(pRenderObject);
	}

	m_RenderObjects[ENUM_CLASS(RENDER::UI)].clear();
}

#ifdef _DEBUG

void CRenderer::Render_Debug()
{
	m_pColliderRenderer->Render(m_pShader);

	if (false == m_isDebugVisible)
		return;

	/* MRT에 포함된 렌더타겟들을 디버그로 직교투영을 통해 그려라. */
	if (FAILED(m_pGameInstance->Render_RT_Debug(TEXT("MRT_GameObjects"), m_pShader, m_pVIBuffer)))
		return;
	//if (FAILED(m_pGameInstance->Render_RT_Debug(TEXT("MRT_Scene"), m_pShader, m_pVIBuffer)))
	//	return;
	//if (FAILED(m_pGameInstance->Render_RT_Debug(TEXT("MRT_LightAcc"), m_pShader, m_pVIBuffer)))
	//	return;
	//if (FAILED(m_pGameInstance->Render_RT_Debug(TEXT("MRT_Shadow"), m_pShader, m_pVIBuffer)))
	//	return;
	//if (FAILED(m_pBlur->Render_Debug(m_pVIBuffer, m_pShader)))
	//	return;
	//if (FAILED(m_pGlow->Render_Debug(m_pVIBuffer, m_pShader)))
	//	return;
	//if (FAILED(m_pDistortion->Render_Debug(m_pVIBuffer, m_pShader)))
	//	return;
	//if (FAILED(m_pBloom->Render_Debug(m_pVIBuffer, m_pShader)))
	//	return;
	//if (FAILED(m_pFog->Render_Debug(m_pVIBuffer, m_pShader)))
	//	return;
	//if (FAILED(m_pMotionBlur->Render_Debug(m_pVIBuffer, m_pShader)))
	//	return;

	if (FAILED(m_pSSAO->Render_Debug(m_pVIBuffer, m_pShader)))
		return;

	if (FAILED(m_pGameInstance->Render_RT_Debug(TEXT("MRT_Velocity"), m_pShader, m_pVIBuffer)))
		return;

}
HRESULT CRenderer::Add_DebugComponent(CComponent* pDebugCom)
{
	return m_pColliderRenderer->Add_DebugComponent(pDebugCom);
}

HRESULT CRenderer::Add_PhysxGeometry(CGameObject* pGameObject, PxRigidActor* pActor, PxShape* pShape)
{
	if (true == pGameObject->isDead())
		return S_OK;

	return m_pColliderRenderer->Add_PhysxGeometry(pActor, pShape);
}

#endif


#ifdef _DEBUG
void CRenderer::Set_DebugColliderVisible(_bool isVisible)
{
	m_pColliderRenderer->Set_DebugColliderVisible(isVisible);
}
#endif

HRESULT CRenderer::Bind_WVP_Matrices()
{
	CAMERA_INFO 	CamInfo = m_pGameInstance->Get_CurrentCamInfo();
	m_pShader->Bind_RawValue("g_fFar", &CamInfo.fFar, sizeof(_float));
	m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix);
	m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix);
	m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix);
	m_pShader->Bind_Matrix("g_ViewMatrixInv", m_pGameInstance->Get_Transform_Float4x4_Inverse(D3DTS::VIEW));
	m_pShader->Bind_Matrix("g_ProjMatrixInv", m_pGameInstance->Get_Transform_Float4x4_Inverse(D3DTS::PROJ));
	m_pShader->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float4));
	m_pShader->Bind_RawValue("g_fDensity", &m_fDensity, sizeof(_float));
	m_pShader->Bind_RawValue("g_fStepSize", &m_fStepSize, sizeof(_float));

	return S_OK;
}

void CRenderer::BeginMarker(ID3D11DeviceContext* pContext, const wchar_t* name)
{
	ID3DUserDefinedAnnotation* pAnnotation = nullptr;
	if (SUCCEEDED(pContext->QueryInterface(__uuidof(ID3DUserDefinedAnnotation), (void**)&pAnnotation)))
	{
		pAnnotation->BeginEvent(name);
		pAnnotation->Release();
	}
}

void CRenderer::EndMarker(ID3D11DeviceContext* pContext)
{
	ID3DUserDefinedAnnotation* pAnnotation = nullptr;
	if (SUCCEEDED(pContext->QueryInterface(__uuidof(ID3DUserDefinedAnnotation), (void**)&pAnnotation)))
	{
		pAnnotation->EndEvent();
		pAnnotation->Release();
	}
}

CRenderer* CRenderer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRenderer* pInstance = new CRenderer(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CRenderer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRenderer::Free()
{
    __super::Free();

	for (auto& RenderObjects : m_RenderObjects)
	{
		for (auto& pRenderObject : RenderObjects)
			Safe_Release(pRenderObject);
		RenderObjects.clear();
	}

	Safe_Release(m_pShadowDSV);
	Safe_Release(m_pCascadeShadowDSV);
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pVIBuffer);
	
	Safe_Release(m_pShader);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	Safe_Release(m_pBlur);
	Safe_Release(m_pDistortion);
	Safe_Release(m_pGlow);
	Safe_Release(m_pBloom);
	Safe_Release(m_pFog);
	Safe_Release(m_pRadialBlur);
	Safe_Release(m_pDepthofField);
	Safe_Release(m_pMotionBlur);
	Safe_Release(m_pSSAO);
	Safe_Release(m_pEmissive);

#ifdef _DEBUG
	Safe_Release(m_pColliderRenderer);
#endif
}