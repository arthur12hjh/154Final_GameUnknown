#include "Renderer.h"

#include "GameObject.h"

#include "BlendObject.h"
#include "UIObject.h"
#include "GameInstance.h"

#include "ColliderRenderer.h"

#include "CascadeShadow.h"
#include "StaticShadow.h"
#include "RadialBlur.h"
#include "Blur.h"
#include "Distortion.h"
#include "BlackBlend.h"
#include "Glow.h"
#include "Metaball.h"
#include "Bloom.h"
#include "Fog.h"
#include "DepthofField.h"
#include "MotionBlur.h"
#include "SSAO.h"
#include "Emissive.h"
#include "ReserveDeferred.h"
#include "VolumeFog.h"

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

	m_pOcclusionShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_DepthOnly.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements);
	if (nullptr == m_pOcclusionShader)
		return E_FAIL;

	/* 직교용 렉트 하나 생성. */
	m_pVIBuffer = CVIBuffer_Rect::Create(m_pDevice, m_pContext);
	if (nullptr == m_pVIBuffer)
		return E_FAIL;

	m_pOcclusionVIBuffer = CVIBuffer_Cube::Create(m_pDevice, m_pContext);
	if (nullptr == m_pOcclusionVIBuffer)
		return E_FAIL;

	/* 직교용 월드 뷰 투영 세팅 */
	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixScaling((_float)m_vScreenSize.x, (_float)m_vScreenSize.y, 1.f));
	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)m_vScreenSize.x, (_float)m_vScreenSize.y, 0.f, 1.f));

	m_pBlur = CBlur::Create(m_pDevice, m_pContext);
	if (nullptr == m_pBlur)
		return E_FAIL;

	m_pCascadeShadow = CCascadeShadow::Create(m_pDevice, m_pContext);
	if (nullptr == m_pCascadeShadow)
		return E_FAIL;

	m_pStaticShadow = CStaticShadow::Create(m_pDevice, m_pContext);
	if (nullptr == m_pStaticShadow)
		return E_FAIL;

	m_pDistortion = CDistortion::Create(m_pDevice, m_pContext);
	if (nullptr == m_pDistortion)
		return E_FAIL;

	m_pGlow = CGlow::Create(m_pDevice, m_pContext);
	if (nullptr == m_pGlow)
		return E_FAIL;

	m_pMetaball = CMetaball::Create(m_pDevice, m_pContext);
	if (nullptr == m_pMetaball)
		return E_FAIL;

	m_pBlackBlend = CBlackBlend::Create(m_pDevice, m_pContext);
	if (nullptr == m_pBlackBlend)
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

	m_pVolumeFog = CVolumeFog::Create(m_pDevice, m_pContext);
	if (nullptr == m_pVolumeFog)
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
	if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_Velocity"), 750.0f, 150.0f, 300.f, 300.f)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_Bloom"), 750.0f, 150.0f, 300.f, 300.f)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_Distortion"), 150.0f, 150.0f, 300.f, 300.f)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_ShadowBlurX"), 750.0f, 450.0f, 300.f, 300.f)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_Combined"), 750.0f, 750.0f, 300.f, 300.f)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_CombinedBloomScene"), 1050.0f, 450.0f, 300.f, 300.f)))
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
	if (FAILED(m_pStaticShadow->Ready_Debug(450.f, 450.f, 300.f, 300.f)))
		return E_FAIL;
	//if (FAILED(m_pFog->Ready_Debug(750.f, 150.f, 300, 300)))
	//	return E_FAIL;

	m_pColliderRenderer = CColliderRenderer::Create(m_pDevice, m_pContext);
	if (nullptr == m_pColliderRenderer)
		return E_FAIL;

#endif

	D3D11_RASTERIZER_DESC rsDesc = {};
	ZeroMemory(&rsDesc, sizeof(D3D11_RASTERIZER_DESC));
	rsDesc.FillMode = D3D11_FILL_SOLID;
	rsDesc.CullMode = D3D11_CULL_NONE;
	rsDesc.FrontCounterClockwise = FALSE;
	rsDesc.DepthClipEnable = TRUE;
	rsDesc.DepthBias = -1;
	rsDesc.DepthBiasClamp = 0.0f;
	rsDesc.SlopeScaledDepthBias = 1.0f;
	if (FAILED(m_pDevice->CreateRasterizerState(&rsDesc, &m_pRS_OcclusionQuery)))
		return E_FAIL;

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
	// SSAO 토글.
	if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_F9))
		m_isSSAO = !m_isSSAO;

	m_pRadialBlur->Update(fTimeDelta);
	m_pDepthofField->Update(fTimeDelta);
	m_pVolumeFog->Update(fTimeDelta);
	m_pCascadeShadow->Update(fTimeDelta);
}

void CRenderer::Update_Shadow(_float fTimeDelta)
{
	m_pCascadeShadow->Update(fTimeDelta);
}

HRESULT CRenderer::Ready_RenderTargets()
{
	/* 후처리 쉐이딩을 위한 렌더타겟들을 준비. */

	for (_uint i = 0; i < 8; ++i)
	{
		/* Target_ClientDeferred0~7까지 생성. */
		if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_ClientDeferred") + to_wstring(i), m_vScreenSize.x, m_vScreenSize.y, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.0f, 0.f, 0.f, 0.f))))
			return E_FAIL;
		/* MRT_ClientDeferred0~7까지 생성. */
		if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_ClientDeferred") + to_wstring(i), TEXT("Target_ClientDeferred") + to_wstring(i))))
			return E_FAIL;
	}

	/* Target_Scene */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Scene"), m_vScreenSize.x, m_vScreenSize.y, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.0f, 0.0f, 1.f, 1.f))))
		return E_FAIL;
	/* Target_BloomScene */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_BloomScene"), m_vScreenSize.x, m_vScreenSize.y, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.0f, 0.0f, 1.f, 1.f))))
		return E_FAIL;
	/* Target_ShadowArea */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_ShadowArea"), m_vScreenSize.x, m_vScreenSize.y, DXGI_FORMAT_R16_FLOAT, _float4(1.0f, 1.0f, 1.f, 1.f))))
		return E_FAIL;
	////////////////////////

	/* Target_ShadowBlurX */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_ShadowBlurX"), m_vScreenSize.x, m_vScreenSize.y, DXGI_FORMAT_R16_FLOAT, _float4(1.0f, 1.0f, 1.f, 1.f))))
		return E_FAIL;

	/* Target_Combined */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Combined"), m_vScreenSize.x, m_vScreenSize.y, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.0f, 0.0f, 1.f, 1.f))))
		return E_FAIL;
	/* Target_CombinedBloomScene*/
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_CombinedBloomScene"), m_vScreenSize.x, m_vScreenSize.y, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.0f, 0.0f, 1.f, 1.f))))
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
	/* Target_Bloom */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Bloom"), m_vScreenSize.x, m_vScreenSize.y, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	/* MRT_LightAcc */
	/* Target_Shade */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Shade"), m_vScreenSize.x, m_vScreenSize.y, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.0f, 0.f, 0.f, 1.f))))
		return E_FAIL;
	/* Target_Specular */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Specular"), m_vScreenSize.x, m_vScreenSize.y, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.0f, 0.f, 0.f, 0.f))))
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
	/* MRT_Combined */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Combined"), TEXT("Target_Scene"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Combined"), TEXT("Target_BloomScene"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Combined"), TEXT("Target_ShadowArea"))))
		return E_FAIL;

	/* MRT_Scene */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Scene"), TEXT("Target_Combined"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Scene"), TEXT("Target_CombinedBloomScene"))))
		return E_FAIL;

	/* MRT_Screen */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Screen"), TEXT("Target_Screen"))))
		return E_FAIL;

	/* MRT_ShadowBlurX */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_ShadowBlurX"), TEXT("Target_ShadowBlurX"))))
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
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Bloom"))))
		return E_FAIL;

	/* MRT_LightAcc */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_Shade"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_Specular"))))
		return E_FAIL;

	/* MRT_ToneMapping */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_ToneMapping"), TEXT("Target_ToneMapping"))))
		return E_FAIL;

	/* MRT_Velocity */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Velocity"), TEXT("Target_Velocity"))))
		return E_FAIL;

	return S_OK;
}

void CRenderer::Active_RadialBlur(_float fLifeTime, _uint iSampleCount, _float fSamplePower)
{
	m_pRadialBlur->Set_Active(iSampleCount, fSamplePower, fLifeTime);
}

void CRenderer::Active_DoF(_bool bFlag, _float fLerpTime)
{
	m_pDepthofField->Set_Active(bFlag, fLerpTime);
}

void CRenderer::Set_DoFInfo(_float fFocusDistance, _float fMaxRange, _float fIntensity)
{
	m_pDepthofField->Set_DoFInfo(fFocusDistance, fMaxRange, fIntensity);
}

void CRenderer::Set_DoFInfo(_float fFocusDistance)
{
	m_pDepthofField->Set_DoFInfo(fFocusDistance);
}

HRESULT CRenderer::Reserve_Deferred(CReserveDeferred* pReserveDeferred)
{
	m_ClientShaderReserves.push_back(pReserveDeferred);
	Safe_AddRef(pReserveDeferred);

	return S_OK;
}

HRESULT CRenderer::Add_RenderGroup(RENDER eRenderGroup, CGameObject* pRenderObject)
{
	if (nullptr == pRenderObject)
		return E_FAIL;
	
	switch (eRenderGroup)
	{
	case RENDER::BLUR:
		m_pBlur->Add_RenderObject(pRenderObject);
		break;
	case RENDER::DISTORTION:
		m_pDistortion->Add_RenderObject(pRenderObject);
		break;
	case RENDER::GLOW:
		m_pGlow->Add_RenderObject(pRenderObject);
		break;
	case RENDER::BLACKBLEND:
		m_pBlackBlend->Add_RenderObject(pRenderObject);
		break;
	case RENDER::METABALL:
		m_pMetaball->Add_RenderObject(pRenderObject);
		break;
	case RENDER::SHADOW:
		m_pCascadeShadow->Add_RenderObject(pRenderObject);
		break;
	default:
		m_RenderObjects[ENUM_CLASS(eRenderGroup)].push_back(pRenderObject);
		Safe_AddRef(pRenderObject);
		break;
	}
	return S_OK;
}

void CRenderer::Render()
{
	Update_Occlusion_Visibility();
	Bind_PreValues();

	Render_Priority();
	Render_Shadow();
	Render_NonBlend();
	Render_Occlusion();
	Render_LightAcc();
	Render_Combined();

	if (FAILED(m_pGameInstance->Load_MRT(TEXT("MRT_Scene"))))
		return;

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

	m_pGameInstance->SwapFrame();

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

void CRenderer::Set_Fog_Desc(void* pArg)
{
	m_pFog->Set_Desc(pArg);
}

HRESULT CRenderer::Ready_CascadeShadow_Light(const CASCADE_SHADOW_DESC& Desc)
{
	return m_pCascadeShadow->Ready_Shadow_Light(Desc);
}

HRESULT CRenderer::Ready_StaticShadow_Light(const STATIC_SHADOW_DESC& Desc)
{
	return m_pStaticShadow->Ready_Shadow_Light(Desc);
}

HRESULT CRenderer::Bind_Shadow_Resource_Static(CShader* pShader, const _char* pConstantName, D3DTS eType)
{
	return m_pStaticShadow->Bind_Shader_Resource(pShader, pConstantName, eType);
}

HRESULT CRenderer::Bind_Shadow_Resource_Cascade(CShader* pShader, const _char* pConstantName, D3DTS eType)
{
	return m_pCascadeShadow->Bind_Shader_Resource(pShader, pConstantName, eType);
}

HRESULT CRenderer::Bind_CascadeEnds(CShader* pShader, const _char* pConstantName, const _char* pConstantName2)
{
	return m_pCascadeShadow->Bind_CascadeEnds(pShader, pConstantName, pConstantName2);
}

_float* CRenderer::Get_CascadeEnds()
{
	return m_pCascadeShadow->Get_CascadeEnds();
}

void* CRenderer::Get_Cascade_Desc()
{
	return m_pCascadeShadow->Get_Desc();
}

void* CRenderer::Get_VolumeFog_Desc()
{
	return m_pVolumeFog->Get_Desc();
}

HRESULT CRenderer::Add_StaticShadowObject(CGameObject* pGameObject)
{
	return m_pStaticShadow->Add_RenderObject(pGameObject);
}

HRESULT CRenderer::Bake_StaticShadow()
{
	return m_pStaticShadow->Render(m_pVIBuffer);
}

void CRenderer::Render_Priority()
{
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Combined"))))
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
	HRESULT hr = m_pCascadeShadow->Render(m_pVIBuffer);
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

void CRenderer::Render_Occlusion()
{
	const _float4x4* pViewMatrix = m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW);
	if (FAILED(m_pOcclusionShader->Bind_Matrix("g_ViewMatrix", pViewMatrix)))
		return;

	const _float4x4* pProjMatrix = m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ);
	if (FAILED(m_pOcclusionShader->Bind_Matrix("g_ProjMatrix", pProjMatrix)))
		return;

	if (FAILED(m_pOcclusionShader->Begin(0)))
		return;

	ID3D11RasterizerState* pOldRS = nullptr;
	m_pContext->RSGetState(&pOldRS); // 기존 RS 백업
	m_pContext->RSSetState(m_pRS_OcclusionQuery);

	m_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	for (auto& pRenderObject : m_RenderObjects[ENUM_CLASS(RENDER::OCCLUSION)])
	{
		if (nullptr != pRenderObject)
		{
			if (pRenderObject->Get_Depth() < 200.f)
			{
				Safe_Release(pRenderObject);
				continue;
			}

			COBBCollider* pCollider = static_cast<COBBCollider*>(pRenderObject->GetCullingCollider());

			if (nullptr != pCollider)
			{
				if (FAILED(m_pGameInstance->Begin_Object_Query(pRenderObject)))
					continue;

				const _float4x4* pWorldMatrix = pCollider->Get_WorldMatrixPtr();


				_float4x4 WorldMatrix = *pCollider->Get_WorldMatrixPtr();
				_matrix matWorld = XMLoadFloat4x4(&WorldMatrix);

				XMStoreFloat4x4(&WorldMatrix, matWorld);

				if (FAILED(m_pOcclusionShader->Bind_Matrix("g_WorldMatrix", &WorldMatrix)))
					continue;

				pCollider->Render_Face(_float4(0.f, 1.f, 0.f, 1.f));

				m_pGameInstance->End_Obejct_Query(pRenderObject);
			}
		}

		Safe_Release(pRenderObject);
	}
	m_RenderObjects[ENUM_CLASS(RENDER::OCCLUSION)].clear();

	m_pContext->RSSetState(pOldRS);
	Safe_Release(pOldRS);
}

void CRenderer::Render_NonBlend()
{
	_uint iNonBlendCount = m_RenderObjects[ENUM_CLASS(RENDER::NONBLEND)].size();
	_uint iRenderedCount = 0;

	_char szDebugString[256];

	/* Diffuse + Normal */
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_GameObjects"))))
		return;

	for (auto& pRenderObject : m_RenderObjects[ENUM_CLASS(RENDER::NONBLEND_PRIORITY)])
	{
		if (nullptr != pRenderObject)
		{
			if (pRenderObject->GetVisibility() == VISIBILITY::VISIBLE)
			{
				pRenderObject->Render();
				iRenderedCount++;
			}
		}

		Safe_Release(pRenderObject);
	}
	m_RenderObjects[ENUM_CLASS(RENDER::NONBLEND_PRIORITY)].clear();

	for (auto& pRenderObject : m_RenderObjects[ENUM_CLASS(RENDER::NONBLEND)])
	{
		if (nullptr != pRenderObject)
		{
			if (pRenderObject->GetVisibility() == VISIBILITY::VISIBLE)
			{
				pRenderObject->Render();
				iRenderedCount++;
			}
		}

		Safe_Release(pRenderObject);
	}

	m_RenderObjects[ENUM_CLASS(RENDER::NONBLEND)].clear();

	if (FAILED(m_pGameInstance->End_MRT()))
		return;

	snprintf(szDebugString, sizeof(szDebugString),
		"Frame Render Count (NONBLEND Group): Total %u, Rendered %u\n", iNonBlendCount, iRenderedCount);
	OutputDebugStringA(szDebugString);
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
}

void CRenderer::Render_Combined()
{
	if (FAILED(m_pGameInstance->Load_MRT(TEXT("MRT_Combined"))))
		return;

	if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_Diffuse"), m_pShader, "g_DiffuseTexture")))
		return;
	if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_Shade"), m_pShader, "g_ShadeTexture")))
		return;
	if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_Specular"), m_pShader, "g_SpecularTexture")))
		return;
	if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_Depth"), m_pShader, "g_DepthTexture")))
		return;

	//캐스케이드 바인딩
	if (FAILED(m_pCascadeShadow->Bind_Shader_Resource(m_pShader, "g_LightViewMatrix", D3DTS::VIEW)))
		return;
	if (FAILED(m_pCascadeShadow->Bind_Shader_Resource(m_pShader, "g_LightProjMatrix", D3DTS::PROJ)))
		return;
	if (FAILED(m_pCascadeShadow->Bind_RenderTarget(m_pShader, "g_CascadeShadowTexture")))
		return;
	if (FAILED(m_pCascadeShadow->Bind_CascadeEnds(m_pShader, "g_fCascadeEnds0", "g_fCascadeEnds1")))
		return;
	//정적 그림자 바인딩
	if (FAILED(m_pStaticShadow->Bind_Shader_Resource(m_pShader, "g_StaticLightViewMatrix", D3DTS::VIEW)))
		return;
	if (FAILED(m_pStaticShadow->Bind_Shader_Resource(m_pShader, "g_StaticLightProjMatrix", D3DTS::PROJ)))
		return;
	if (FAILED(m_pStaticShadow->Bind_RenderTarget(m_pShader, "g_StaticShadowTexture")))
		return;

	if (false == m_isSSAO)
		m_pGameInstance->Clear_MRT(TEXT("MRT_SSAO_BlurY"));

	if (FAILED(m_pSSAO->Bind_RenderTarget(m_pShader, "g_SSAOTexture")))
		return;

#ifdef _DEBUG
	BeginMarker(m_pContext, TEXT("########## Combine Render"));
#endif 
	m_pShader->Begin(ENUM_CLASS(SHADER_DEFERRED_IDX::COMBINED));

	m_pVIBuffer->Bind_Resources();
	m_pVIBuffer->Render();

#ifdef _DEBUG
	EndMarker(m_pContext);
#endif 

	if (FAILED(m_pGameInstance->End_MRT()))
		return;

	//여기서 그림자맵 블러처리

	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_ShadowBlurX"))))
		return;
	if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_ShadowArea"), m_pShader, "g_ShadowAreaTexture")))
		return;

	m_pShader->Begin(ENUM_CLASS(SHADER_DEFERRED_IDX::SHADOW_BLUR_X));

	m_pVIBuffer->Bind_Resources();
	m_pVIBuffer->Render();

	if (FAILED(m_pGameInstance->End_MRT()))
		return;

	// 최종 합성 ㄱㄱ
#ifdef _DEBUG
	BeginMarker(m_pContext, TEXT("########## SHADOW COMBINE"));
#endif 
	if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_Scene"), m_pShader, "g_SceneTexture")))
		return;
	if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_BloomScene"), m_pShader, "g_BloomSceneTexture")))
		return;
	if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_ShadowBlurX"), m_pShader, "g_ShadowBlurXTexture")))
		return;

	if (FAILED(m_pGameInstance->Load_MRT(TEXT("MRT_Scene"))))
		return;

	m_pShader->Begin(ENUM_CLASS(SHADER_DEFERRED_IDX::COMBINE_SHADOW));

	m_pVIBuffer->Bind_Resources();
	m_pVIBuffer->Render();


	if (FAILED(m_pGameInstance->End_MRT()))
		return;
#ifdef _DEBUG
	EndMarker(m_pContext);
#endif 
}

void CRenderer::Render_NonLight()
{
#ifdef _DEBUG
	BeginMarker(m_pContext, TEXT("########## NONLIGHT"));
#endif 
	for (auto& pRenderObject : m_RenderObjects[ENUM_CLASS(RENDER::NONLIGHT)])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render();

		Safe_Release(pRenderObject);
	}

	m_RenderObjects[ENUM_CLASS(RENDER::NONLIGHT)].clear();
#ifdef _DEBUG
	EndMarker(m_pContext);
#endif 
}

void CRenderer::Render_Blend()
{	
#ifdef _DEBUG
	BeginMarker(m_pContext, TEXT("########## BLEND"));
#endif 
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
#ifdef _DEBUG
	EndMarker(m_pContext);
#endif 
}

void CRenderer::Render_Deferred()
{
	HRESULT hr = m_pBlackBlend->Render(m_pVIBuffer);
	hr = m_pGlow->Render(m_pVIBuffer);
	hr = m_pMetaball->Render(m_pVIBuffer);
	hr = m_pBlur->Render(m_pVIBuffer);
	hr = m_pEmissive->Render(m_pVIBuffer);
	hr = m_pDistortion->Render(m_pVIBuffer);
	hr = m_pBloom->Render(m_pVIBuffer, TEXT("Target_CombinedBloomScene"), TEXT("MRT_Scene"));
	hr = m_pFog->Render(m_pVIBuffer);
	hr = m_pVolumeFog->Render(m_pVIBuffer);

#ifdef _DEBUG
	BeginMarker(m_pContext, TEXT("########## DISTORTION"));
#endif 
	// 여러 문제떄문에.. 디스토션은 따로 분리.
	m_pDistortion->Render(m_pVIBuffer, TEXT("Target_Combined"), TEXT("MRT_Scene"));
#ifdef _DEBUG
	EndMarker(m_pContext);
#endif

#ifdef _DEBUG
	BeginMarker(m_pContext, TEXT("########## Screen Combine"));
#endif 
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Screen"))))
		return;

	if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_Combined"), m_pShader, "g_SceneTexture")))
		return;

	//emissive
	if (FAILED(m_pEmissive->Bind_RenderTarget(m_pShader, "g_EmissiveFinalTexture")))
		return;

	//blackblend
	if (FAILED(m_pBlackBlend->Bind_RenderTarget(m_pShader, "g_BlackBlendTexture")))
		return;
	//blur
	if (FAILED(m_pBlur->Bind_RenderTarget(m_pShader, "g_BlurFinalTexture")))
		return;
	//glow
	if (FAILED(m_pGlow->Bind_RenderTarget(m_pShader, "g_GlowBloomTexture")))
		return;
	if (FAILED(m_pGlow->Bind_RenderTarget(m_pShader, "g_GlowFinalTexture")))
		return;
	//metaball
	if (FAILED(m_pMetaball->Bind_RenderTarget(m_pShader, "g_MetaballTexture")))
		return;

	if (FAILED(m_pVolumeFog->Bind_RenderTarget(m_pShader, "g_VolumeFogTexture")))
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

#ifdef _DEBUG
	EndMarker(m_pContext);
#endif
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
	//클라단에서 예약한 셰이더 처리
	_uint iCount = { 0 };
	_wstring strRTTag = {};

	for (auto& ReserveObject : m_ClientShaderReserves)
	{
		//MRT 자동으로 세팅해주고.
		if (iCount >= 8)
			return;

		if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_ClientDeferred") + to_wstring(iCount))))
			return;

		if (iCount == 0)
			strRTTag = TEXT("Target_ToneMapping");
		else
			strRTTag = TEXT("Target_ClientDeferred") + to_wstring(iCount - 1);

		ReserveObject->Bind_Resources(strRTTag);
		ReserveObject->Render(m_pVIBuffer);

		if (FAILED(m_pGameInstance->End_MRT()))
			return;
	
		iCount++;
		Safe_Release(ReserveObject);
	}

	m_ClientShaderReserves.clear();

	// 최종 백버퍼 합성과정. 여긴 신경 쓰지마
	if (0 == iCount)
	{
		if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_ToneMapping"), m_pShader, "g_ScreenTexture")))
			return;
	}
	else
	{
		if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_ClientDeferred") + to_wstring(iCount - 1), m_pShader, "g_ScreenTexture")))
			return;
	}

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

void CRenderer::Update_Occlusion_Visibility()
{
	for (auto& pRenderObject : m_RenderObjects[ENUM_CLASS(RENDER::NONBLEND)])
	{
		_bool isVisible = true;

		if (FAILED(m_pGameInstance->Get_Result(pRenderObject, &isVisible)))
		{
			isVisible = false;
		}

		pRenderObject->Set_Occlusion_Culling_Result(isVisible);
	}
}

#ifdef _DEBUG

void CRenderer::Render_Debug()
{
	m_pColliderRenderer->Render(m_pShader);

	if (false == m_isDebugVisible)
		return;
	//if (FAILED(m_pGameInstance->Render_RT_Debug(TEXT("MRT_Scene"), m_pShader, m_pVIBuffer)))
	//	return;
	//if (FAILED(m_pGameInstance->Render_RT_Debug(TEXT("MRT_LightAcc"), m_pShader, m_pVIBuffer)))
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
	if (FAILED(m_pStaticShadow->Render_Debug(m_pVIBuffer, m_pShader)))
		return;
	if (FAILED(m_pSSAO->Render_Debug(m_pVIBuffer, m_pShader)))
		return;

	if (FAILED(m_pGameInstance->Render_RT_Debug(TEXT("MRT_Velocity"), m_pShader, m_pVIBuffer)))
		return;
	if (FAILED(m_pGameInstance->Render_RT_Debug(TEXT("MRT_ShadowBlurX"), m_pShader, m_pVIBuffer)))
		return;
	if (FAILED(m_pGameInstance->Render_RT_Debug(TEXT("MRT_Scene"), m_pShader, m_pVIBuffer)))
		return;
	if (FAILED(m_pGameInstance->Render_RT_Debug(TEXT("MRT_Distortion"), m_pShader, m_pVIBuffer)))
		return;
	/* MRT에 포함된 렌더타겟들을 디버그로 직교투영을 통해 그려라. */
	if (FAILED(m_pGameInstance->Render_RT_Debug(TEXT("MRT_GameObjects"), m_pShader, m_pVIBuffer)))
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

HRESULT CRenderer::Bind_PreValues()
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

#ifdef _DEBUG
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
#endif

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

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pShader);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pBlur);
	Safe_Release(m_pBlackBlend);
	Safe_Release(m_pMetaball);
	Safe_Release(m_pDistortion);
	Safe_Release(m_pGlow);
	Safe_Release(m_pBloom);
	Safe_Release(m_pFog);
	Safe_Release(m_pRadialBlur);
	Safe_Release(m_pDepthofField);
	Safe_Release(m_pMotionBlur);
	Safe_Release(m_pSSAO);
	Safe_Release(m_pEmissive);
	Safe_Release(m_pOcclusionShader);
	Safe_Release(m_pOcclusionVIBuffer);
	Safe_Release(m_pRS_OcclusionQuery);
	Safe_Release(m_pStaticShadow);
	Safe_Release(m_pCascadeShadow);
	Safe_Release(m_pVolumeFog);

#ifdef _DEBUG
	Safe_Release(m_pColliderRenderer);
#endif
}