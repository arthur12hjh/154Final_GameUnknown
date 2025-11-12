#include "Renderer.h"

#include "GameObject.h"

#include "BlendObject.h"
#include "UIObject.h"
#include "GameInstance.h"

CRenderer::CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
	, m_pGameInstance { CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
    Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
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

#ifdef _DEBUG
	m_pPhysxDebugShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Physx_Debug.hlsl"), VTXPOSCOLOR::Elements, VTXPOSCOLOR::iNumElements);
	if (nullptr == m_pPhysxDebugShader)
		return E_FAIL;

    m_pEffect =	new BasicEffect(m_pDevice);

	const void* pShaderByteCode = { nullptr };
	size_t		iShaderByteCodeLength = {};

	m_pEffect->GetVertexShaderBytecode(&pShaderByteCode, &iShaderByteCodeLength);

	if (m_pDevice->CreateInputLayout(VertexPositionColor::InputElements, VertexPositionColor::InputElementCount,
		pShaderByteCode, iShaderByteCodeLength, &m_pInputLayout))
		return E_FAIL;
#endif
	/* 스크린 사이즈는 미리 바인딩 한다. */
	if (FAILED(m_pShader->Bind_RawValue("g_iWinSizeX", &m_vScreenSize.x, sizeof(_int))))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_RawValue("g_iWinSizeY", &m_vScreenSize.y, sizeof(_int))))
		return E_FAIL;

	/* 직교용 렉트 하나 생성. */
	m_pVIBuffer = CVIBuffer_Rect::Create(m_pDevice, m_pContext);
	if (nullptr == m_pVIBuffer)
		return E_FAIL;

	/* 직교용 월드 뷰 투영 세팅 */
	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixScaling((_float)m_vScreenSize.x, (_float)m_vScreenSize.y, 1.f));
	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)m_vScreenSize.x, (_float)m_vScreenSize.y, 0.f, 1.f));

	/* 디버그 렌더링 준비 */
#ifdef _DEBUG
	if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_Diffuse"), 150.0f, 150.0f, 300.f, 300.f)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_Normal"), 150.0f, 450.0f, 300.f, 300.f)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_Shade"), 450.0f, 150.0f, 300.f, 300.f)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_Specular"), 450.0f, 450.0f, 300.f, 300.f)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_Shadow"), 150.0f, 750.0f, 300.f, 300.f)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_Distortion"), m_vScreenSize.x - 150.0f, 150.0f, 300.0f, 300.0f)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_Outline"), m_vScreenSize.x - 150.0f, 450.f, 300.f, 300.f)))
		return E_FAIL;
	//if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_Outline"), m_vScreenSize.x /2.f, m_vScreenSize.y /2.f, m_vScreenSize.x, m_vScreenSize.y)))
	//	return E_FAIL;
	if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_RimLight"), m_vScreenSize.x - 450.0f, 150.f, 300.f, 300.f)))
		return E_FAIL;

	//반지름이 1인 객체로 생성.
	m_pSphereShape = GeometricPrimitive::CreateSphere(m_pContext, 1.f, 6);
	m_pBoxShape = GeometricPrimitive::CreateBox(m_pContext, _float3(0.5f, 0.5f, 0.5f));
	m_pCapsuleCylinderShape = GeometricPrimitive::CreateCylinder(m_pContext, 1.f, 1.f, 6);
	m_pCapsuleHemiSphereShape = CreateHemisphere(m_pContext, 0.5f, 6, true);
#endif

    return S_OK;
}

HRESULT CRenderer::Ready_RenderTargets()
{
	/* 후처리 쉐이딩을 위한 렌더타겟들을 준비. */
	/* Target_Scene */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Scene"), m_vScreenSize.x, m_vScreenSize.y, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.0f, 0.f, 1.f, 1.f))))
		return E_FAIL;
	/* Target_Diffuse */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Diffuse"), m_vScreenSize.x, m_vScreenSize.y, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.0f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	/* Target_Normal */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Normal"), m_vScreenSize.x, m_vScreenSize.y, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.0f, 0.f, 0.f, 1.f))))
		return E_FAIL;
	/* Target_Depth */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Depth"), m_vScreenSize.x, m_vScreenSize.y, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.0f, 1.f, 0.f, 0.f))))
		return E_FAIL;
	/* Target_RimLight.*/
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_RimLight"), m_vScreenSize.x, m_vScreenSize.y, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.0f, 0.0f, 0.0f, 0.0f))))
		return E_FAIL;
	/* Target_Shade */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Shade"), m_vScreenSize.x, m_vScreenSize.y, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.0f, 0.f, 0.f, 1.f))))
		return E_FAIL;
	/* Target_Outline. */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Outline"), m_vScreenSize.x * 2.f, m_vScreenSize.y * 2.f, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.0f, 0.f, 0.f, 1.f))))
		return E_FAIL;
	/* Target_Specular */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Specular"), m_vScreenSize.x, m_vScreenSize.y, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.0f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	/* Target_Shadow. */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Shadow"), m_vShadowMapSize.x, m_vShadowMapSize.y, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(1.0f, 1.f, 1.f, 1.f))))
		return E_FAIL;
	if (FAILED(Ready_DepthStencilView(m_vShadowMapSize.x, m_vShadowMapSize.y)))
		return E_FAIL;
	/* Target_Blur. */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Blur"), m_vScreenSize.x, m_vScreenSize.y, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.0f, 0.0f, 0.0f, 0.0f))))
		return E_FAIL;
	/* Target_Blur_X. X에 대해서 우선 블러처리. */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Blur_X"), m_vScreenSize.x, m_vScreenSize.y, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.0f, 0.0f, 0.0f, 0.0f))))
		return E_FAIL;
	/* Target_Blur_Final. Y에 대해서도 블러처리 수행. */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Blur_Final"), m_vScreenSize.x, m_vScreenSize.y, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.0f, 0.0f, 0.0f, 0.0f))))
		return E_FAIL;
	/* Target_Glow. */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Glow"), m_vScreenSize.x, m_vScreenSize.y, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.0f, 0.0f, 0.0f, 0.0f))))
		return E_FAIL;
	/* Target_Glow_X. X에 대해서 우선 블러처리. */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Glow_X"), m_vScreenSize.x, m_vScreenSize.y, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.0f, 0.0f, 0.0f, 0.0f))))
		return E_FAIL;
	/* Target_Glow_Final. Y에 대해서도 블러처리 수행. */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Glow_Final"), m_vScreenSize.x, m_vScreenSize.y, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.0f, 0.0f, 0.0f, 0.0f))))
		return E_FAIL;
	/* Target_Distortion.*/
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Distortion"), m_vScreenSize.x, m_vScreenSize.y, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.0f, 0.0f, 0.0f, 0.0f))))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Ready_MRTs()
{
#pragma region MRT_GAMEOBJECT
	/* MRT_GameObjects */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Diffuse"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Normal"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Depth"))))
		return E_FAIL;
	//림라이트는 GameObject단에서 찍어서 전처리.
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_RimLight"))))
		return E_FAIL;

#pragma endregion

#pragma region MRT_LIGHTACC
	/* MRT_LightAcc */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_Shade"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_Specular"))))
		return E_FAIL;
#pragma endregion
	/* MRT_Outline */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Outline"), TEXT("Target_Outline"))))
		return E_FAIL;
	/* MRT_Scene*/
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Scene"), TEXT("Target_Scene"))))
		return E_FAIL;
	/* MRT_Shadow */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Shadow"), TEXT("Target_Shadow"))))
		return E_FAIL;
	/* MRT_Blur */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Blur"), TEXT("Target_Blur"))))
		return E_FAIL;
	/* MRT_Blur_X */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Blur_X"), TEXT("Target_Blur_X"))))
		return E_FAIL;
	/* MRT_Blur_Final */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Blur_Final"), TEXT("Target_Blur_Final"))))
		return E_FAIL;
	/* MRT_Glow */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Glow"), TEXT("Target_Glow"))))
		return E_FAIL;
	/* MRT_Glow_X */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Glow_X"), TEXT("Target_Glow_X"))))
		return E_FAIL;
	/* MRT_Glow_Final */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Glow_Final"), TEXT("Target_Glow_Final"))))
		return E_FAIL;
	/* MRT_Distortion */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Distortion"), TEXT("Target_Distortion"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Add_RenderGroup(RENDER eRenderGroup, CGameObject* pRenderObject)
{
	if (nullptr == pRenderObject)
		return E_FAIL;

	m_RenderObjects[ENUM_CLASS(eRenderGroup)].push_back(pRenderObject);

	Safe_AddRef(pRenderObject);

	return S_OK;
}

void CRenderer::Render()
{
	/* 토글 */
#ifdef _DEBUG
	if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_F2))
		m_isDebugVisible = !m_isDebugVisible;

	if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_F3))
		m_isColliderVisible = !m_isColliderVisible;
#endif

	Render_Priority();
	Render_Shadow();
	Render_NonBlend();
	Render_LightAcc();
	Render_Blur();
	Render_Glow();
	Render_Distortion();
	Render_Combined();
	Render_NonLight();
	Render_Blend();

	Composite_RT_ToBackBuffer();
	Apply_Deferred();
	
	Render_UI();

#ifdef _DEBUG
	Render_Debug();
	Render_PhysxDebug();
#endif
}

#ifdef _DEBUG
HRESULT CRenderer::Add_DebugComponent(CComponent* pDebugCom)
{
	if (false == m_isColliderVisible)
		return S_OK;

	m_DebugComponents.push_back(pDebugCom);
	Safe_AddRef(pDebugCom);

	return S_OK;
}

HRESULT CRenderer::Add_PhysxGeometry(PxRigidActor* pActor, PxShape* pShape)
{
	if (false == m_isColliderVisible)
		return S_OK;

	m_PxShapes.push_back(make_pair(pActor, pShape));
	
	return S_OK;
}

unique_ptr<GeometricPrimitive> CRenderer::CreateHemisphere(ID3D11DeviceContext* pContext, _float fRadius, _int iTessellation, _bool isTop)
{   
	GeometricPrimitive::VertexCollection Vertices;
	GeometricPrimitive::IndexCollection Indices;

	// 반구 정점 생성
	for (_int i = 0; i <= iTessellation / 2; ++i)
	{
		_float fPhi = (i / _float(iTessellation / 2)) * XM_PIDIV2; // 0 ~ PI/2
		if (!isTop) // 하단
		{
			fPhi = XM_PIDIV2 + fPhi;
		}

		for (_int j = 0; j <= iTessellation; ++j)
		{
			_float fTheta = j / _float(iTessellation) * XM_2PI;
			_float fX = fRadius * sinf(fPhi) * cosf(fTheta);
			_float fY = fRadius * cosf(fPhi);
			_float fZ = fRadius * sinf(fPhi) * sinf(fTheta);

			Vertices.push_back({ XMFLOAT3(fX, fY, fZ), XMFLOAT3(0,1,0), XMFLOAT2(0,0) });
		}
	}

	// Index 생성 (삼각형)
	for (int i = 0; i < iTessellation / 2; ++i)
	{
		for (int j = 0; j < iTessellation; ++j)
		{
			_uint iIdx0 = i * (iTessellation + 1) + j;
			_uint iIdx1 = iIdx0 + iTessellation + 1;
			_uint iIdx2 = iIdx0 + 1;
			_uint iIdx3 = iIdx1 + 1;

			Indices.push_back(iIdx0); Indices.push_back(iIdx1); Indices.push_back(iIdx2);
			Indices.push_back(iIdx0); Indices.push_back(iIdx2); Indices.push_back(iIdx3);
		}
	}

	return GeometricPrimitive::CreateCustom(pContext, Vertices, Indices);
}

#endif

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
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Shadow"), m_pShadowDSV)))
		return;

	D3D11_VIEWPORT			ViewPortDesc;
	ZeroMemory(&ViewPortDesc, sizeof(D3D11_VIEWPORT));
	ViewPortDesc.TopLeftX = 0;
	ViewPortDesc.TopLeftY = 0;
	ViewPortDesc.Width = (_float)m_vShadowMapSize.x;
	ViewPortDesc.Height = (_float)m_vShadowMapSize.y;
	ViewPortDesc.MinDepth = 0.f;
	ViewPortDesc.MaxDepth = 1.f;

	m_pContext->RSSetViewports(1, &ViewPortDesc);

	for (auto& pRenderObject : m_RenderObjects[ENUM_CLASS(RENDER::SHADOW)])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render_Shadow();

		Safe_Release(pRenderObject);
	}

	m_RenderObjects[ENUM_CLASS(RENDER::SHADOW)].clear();

	if (FAILED(m_pGameInstance->End_MRT()))
		return;

	ZeroMemory(&ViewPortDesc, sizeof(D3D11_VIEWPORT));
	ViewPortDesc.TopLeftX = 0;
	ViewPortDesc.TopLeftY = 0;
	ViewPortDesc.Width = (_float)m_vScreenSize.x;
	ViewPortDesc.Height = (_float)m_vScreenSize.y;
	ViewPortDesc.MinDepth = 0.f;
	ViewPortDesc.MaxDepth = 1.f;

	m_pContext->RSSetViewports(1, &ViewPortDesc);
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

	m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix);
	m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix);
	m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix);
	m_pShader->Bind_Matrix("g_ViewMatrixInv", m_pGameInstance->Get_Transform_Float4x4_Inverse(D3DTS::VIEW));
	m_pShader->Bind_Matrix("g_ProjMatrixInv", m_pGameInstance->Get_Transform_Float4x4_Inverse(D3DTS::PROJ));
	m_pShader->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float4));

	if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_Normal"), m_pShader, "g_NormalTexture")))
		return;

	if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_Depth"), m_pShader, "g_DepthTexture")))
		return;

	m_pVIBuffer->Bind_Resources();

	if (FAILED(m_pGameInstance->Render_Lights(m_pShader, m_pVIBuffer)))
		return;

	if (FAILED(m_pGameInstance->End_MRT()))
		return;
}

void CRenderer::Render_Combined()
{
	/* Diffuse + Normal */
	if (FAILED(m_pGameInstance->Load_MRT(TEXT("MRT_Scene"))))
		return;

	m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix);
	m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix);
	m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix);
	m_pShader->Bind_Matrix("g_ViewMatrixInv", m_pGameInstance->Get_Transform_Float4x4_Inverse(D3DTS::VIEW));
	m_pShader->Bind_Matrix("g_ProjMatrixInv", m_pGameInstance->Get_Transform_Float4x4_Inverse(D3DTS::PROJ));

	if (FAILED(m_pGameInstance->Bind_Shadow_Resource(m_pShader, "g_LightViewMatrix", D3DTS::VIEW)))
		return;
	if (FAILED(m_pGameInstance->Bind_Shadow_Resource(m_pShader, "g_LightProjMatrix", D3DTS::PROJ)))
		return;

	if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_Diffuse"), m_pShader, "g_DiffuseTexture")))
		return;
	if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_Shade"), m_pShader, "g_ShadeTexture")))
		return;
	if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_Specular"), m_pShader, "g_SpecularTexture")))
		return;
	if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_Depth"), m_pShader, "g_DepthTexture")))
		return;
	if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_Shadow"), m_pShader, "g_ShadowTexture")))
		return;
	if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_Blur_Final"), m_pShader, "g_BlurFinalTexture")))
		return;
	if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_Glow_Final"), m_pShader, "g_GlowFinalTexture")))
		return;
	if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_RimLight"), m_pShader, "g_RimLightTexture")))
		return;
	//if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_Outline"), m_pShader, "g_OutlineTexture")))
	//	return;

	m_pShader->Begin(ENUM_CLASS(SHADER_DEFERRED_IDX::COMBINED));

	m_pVIBuffer->Bind_Resources();
	m_pVIBuffer->Render();

	if (FAILED(m_pGameInstance->End_MRT()))
		return;
}

void CRenderer::Render_NonLight()
{
	if (FAILED(m_pGameInstance->Load_MRT(TEXT("MRT_Scene"))))
		return;


	for (auto& pRenderObject : m_RenderObjects[ENUM_CLASS(RENDER::NONLIGHT)])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render();

		Safe_Release(pRenderObject);
	}

	m_RenderObjects[ENUM_CLASS(RENDER::NONLIGHT)].clear();

	if (FAILED(m_pGameInstance->End_MRT()))
		return;
}

void CRenderer::Render_Blur()
{
	/* 블러 기록할 물체들만 뺴서 기록 */
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Blur"))))
		return;

	for (auto& pRenderObject : m_RenderObjects[ENUM_CLASS(RENDER::BLUR)])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render();

		Safe_Release(pRenderObject);
	}

	m_RenderObjects[ENUM_CLASS(RENDER::BLUR)].clear();

	if (FAILED(m_pGameInstance->End_MRT()))
		return;

	/* 블러 X 처리 */
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Blur_X"))))
		return;

	m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix);
	m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix);
	m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix);

	if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_Blur"), m_pShader, "g_BlurTexture")))
		return;

	m_pShader->Begin(ENUM_CLASS(SHADER_DEFERRED_IDX::BLUR_X));

	m_pVIBuffer->Bind_Resources();

	m_pVIBuffer->Render();

	if (FAILED(m_pGameInstance->End_MRT()))
		return;

	/* 블러 Y 처리 */
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Blur_Final"))))
		return;

	if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_Blur_X"), m_pShader, "g_BlurXTexture")))
		return;

	m_pShader->Begin(ENUM_CLASS(SHADER_DEFERRED_IDX::BLUR_FINAL));

	m_pVIBuffer->Bind_Resources();

	m_pVIBuffer->Render();

	if (FAILED(m_pGameInstance->End_MRT()))
		return;
}

void CRenderer::Render_Glow()
{
	/* 블러 기록할 물체들만 뺴서 기록 */
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Glow"))))
		return;

	for (auto& pRenderObject : m_RenderObjects[ENUM_CLASS(RENDER::GLOW)])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render();

		Safe_Release(pRenderObject);
	}

	m_RenderObjects[ENUM_CLASS(RENDER::GLOW)].clear();

	if (FAILED(m_pGameInstance->End_MRT()))
		return;

	/* 블러 X 처리 */
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Glow_X"))))
		return;

	m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix);
	m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix);
	m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix);

	if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_Glow"), m_pShader, "g_GlowTexture")))
		return;

	m_pShader->Begin(ENUM_CLASS(SHADER_DEFERRED_IDX::GLOW_X));

	m_pVIBuffer->Bind_Resources();

	m_pVIBuffer->Render();

	if (FAILED(m_pGameInstance->End_MRT()))
		return;

	/* 블러 Y 처리 */
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Glow_Final"))))
		return;

	if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_Glow_X"), m_pShader, "g_GlowXTexture")))
		return;

	m_pShader->Begin(ENUM_CLASS(SHADER_DEFERRED_IDX::GLOW_FINAL));

	m_pVIBuffer->Bind_Resources();

	m_pVIBuffer->Render();

	if (FAILED(m_pGameInstance->End_MRT()))
		return;
}

void CRenderer::Render_Distortion()
{
	/* Diffuse + Normal */
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Distortion"))))
		return;

	for (auto& pRenderObject : m_RenderObjects[ENUM_CLASS(RENDER::DISTORTION)])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render();

		Safe_Release(pRenderObject);
	}

	m_RenderObjects[ENUM_CLASS(RENDER::DISTORTION)].clear();

	if (FAILED(m_pGameInstance->End_MRT()))
		return;
}

void CRenderer::Render_Blend()
{	
	if (FAILED(m_pGameInstance->Load_MRT(TEXT("MRT_Scene"))))
		return;

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

	if (FAILED(m_pGameInstance->End_MRT()))
		return;
}

void CRenderer::Apply_Deferred()
{
	m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix);
	m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix);
	m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix);

	if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_Scene"), m_pShader, "g_SceneTexture")))
		return;
	if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_Distortion"), m_pShader, "g_DistortionTexture")))
		return;

	m_pShader->Begin(ENUM_CLASS(SHADER_DEFERRED_IDX::DISTORTION));
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

void CRenderer::Composite_RT_ToBackBuffer()
{
	if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_Scene"), m_pShader, "g_SceneTexture")))
		return;

	m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix);
	m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix);
	m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix);

	m_pShader->Begin(ENUM_CLASS(SHADER_DEFERRED_IDX::SCENE));
	m_pVIBuffer->Bind_Resources();
	m_pVIBuffer->Render();
}

HRESULT CRenderer::Ready_DepthStencilView(_uint iSizeX, _uint iSizeY)
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

	/* 동적? 정적?  */
	TextureDesc.Usage = D3D11_USAGE_DEFAULT /* 정적 */;
	/* 추후에 어떤 용도로 바인딩 될 수 있는 View타입의 텍스쳐를 만들기위한 Texture2D입니까? */
	TextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL
		/*| D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE*/;
	TextureDesc.CPUAccessFlags = 0;
	TextureDesc.MiscFlags = 0;

	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &pDepthStencilTexture)))
		return E_FAIL;

	if (FAILED(m_pDevice->CreateDepthStencilView(pDepthStencilTexture, nullptr, &m_pShadowDSV)))
		return E_FAIL;

	Safe_Release(pDepthStencilTexture);

	/* 아웃라인 DSV도 생성. */
	TextureDesc.Width = m_vScreenSize.x * 2.f;
	TextureDesc.Height = m_vScreenSize.y * 2.f;

	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &pDepthStencilTexture)))
		return E_FAIL;

	if (FAILED(m_pDevice->CreateDepthStencilView(pDepthStencilTexture, nullptr, &m_pOutlineDSV)))
		return E_FAIL;

	Safe_Release(pDepthStencilTexture);

	return S_OK;
}

#ifdef _DEBUG

void CRenderer::Render_Debug()
{
	if (true == m_isColliderVisible)
	{
		m_pGameInstance->Debug_LightRender();
		for (auto& pDebugCom : m_DebugComponents)
		{
			if (nullptr != pDebugCom)
				pDebugCom->Render();

			Safe_Release(pDebugCom);
		}
		m_DebugComponents.clear();

		if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
			return;
		if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
			return;
	}

	if (false == m_isDebugVisible)
		return;

	/* MRT에 포함된 렌더타겟들을 디버그로 직교투영을 통해 그려라. */
	if (FAILED(m_pGameInstance->Render_RT_Debug(TEXT("MRT_GameObjects"), m_pShader, m_pVIBuffer)))
		return ;
	if (FAILED(m_pGameInstance->Render_RT_Debug(TEXT("MRT_LightAcc"), m_pShader, m_pVIBuffer)))
		return ;
	if (FAILED(m_pGameInstance->Render_RT_Debug(TEXT("MRT_Shadow"), m_pShader, m_pVIBuffer)))
		return;
	if (FAILED(m_pGameInstance->Render_RT_Debug(TEXT("MRT_Distortion"), m_pShader, m_pVIBuffer)))
		return;
	if (FAILED(m_pGameInstance->Render_RT_Debug(TEXT("MRT_Outline"), m_pShader, m_pVIBuffer)))
		return;
}

void CRenderer::Render_PhysxDebug()
{
	if (false == m_isColliderVisible)
		return;

	for (auto& Pair : m_PxShapes)
	{
		PxGeometryHolder Geometry = Pair.second->getGeometry();
 		PxTransform PhysxTransform = PxShapeExt::getGlobalPose(*Pair.second, *Pair.first);

		m_pEffect->SetView(m_pGameInstance->Get_Transform_Matrix(D3DTS::VIEW));
		m_pEffect->SetProjection(m_pGameInstance->Get_Transform_Matrix(D3DTS::PROJ));
		m_pEffect->SetColorAndAlpha(XMVectorSet(0.f, 1.0f, 0.f, 1.f));

		if (PxGeometryType::eBOX == Geometry.getType())
		{
			PxBoxGeometry BoxGeom = Geometry.box();
			_matrix ScalingMatrix = XMMatrixScaling(BoxGeom.halfExtents.x, BoxGeom.halfExtents.y, BoxGeom.halfExtents.z);
			_matrix ConvertMatrix = ScalingMatrix * m_pGameInstance->Convert_PxTransform_ToMatrix(PhysxTransform);

			m_pEffect->SetWorld(ConvertMatrix);
			m_pBoxShape->Draw(m_pEffect, m_pInputLayout, false, true);
		}
		else if (PxGeometryType::eSPHERE == Geometry.getType())
		{
			PxSphereGeometry SphereGeom = Geometry.sphere();
			_matrix ScalingMatrix = XMMatrixScaling(SphereGeom.radius * 2.f, SphereGeom.radius * 2.f, SphereGeom.radius * 2.f);
			_matrix ConvertMatrix = m_pGameInstance->Convert_PxTransform_ToMatrix(PhysxTransform);

			m_pEffect->SetWorld(ScalingMatrix * ConvertMatrix);
			m_pSphereShape->Draw(m_pEffect, m_pInputLayout, false, true);
		}
		else if (PxGeometryType::eCAPSULE == Geometry.getType())
		{
			PxCapsuleGeometry CapsuleGeom = Geometry.capsule();
			_matrix ConvertMatrix = XMMatrixRotationAxis(XMVectorSet(0.f, 0.f, 1.f, 0.f ), XMConvertToRadians(-90.f)) * m_pGameInstance->Convert_PxTransform_ToMatrix(PhysxTransform);
			
			/* 실린더 출력 */
			m_pEffect->SetWorld(ConvertMatrix);
			m_pCapsuleCylinderShape->Draw(m_pEffect, m_pInputLayout, false, true);
			
			/* 상반구 출력 */
			_vector		vQuternion = XMQuaternionRotationRollPitchYaw(XMConvertToRadians(30.f), 0.f, XMConvertToRadians(-90.f));
			_matrix		RotationMatrix = XMMatrixRotationQuaternion(vQuternion);

			PhysxTransform.p.y += CapsuleGeom.halfHeight;
			ConvertMatrix = RotationMatrix * m_pGameInstance->Convert_PxTransform_ToMatrix(PhysxTransform);
			m_pEffect->SetWorld(ConvertMatrix);
			m_pCapsuleHemiSphereShape->Draw(m_pEffect, m_pInputLayout, false, true);
	

			/* 하반구 출력*/
			vQuternion = XMQuaternionRotationRollPitchYaw(XMConvertToRadians(30.f), 0.f, XMConvertToRadians(90.f));
			RotationMatrix = XMMatrixRotationQuaternion(vQuternion);

			PhysxTransform.p.y -= 2.f * CapsuleGeom.halfHeight;
			ConvertMatrix = RotationMatrix * m_pGameInstance->Convert_PxTransform_ToMatrix(PhysxTransform);
			m_pEffect->SetWorld(ConvertMatrix);
			m_pCapsuleHemiSphereShape->Draw(m_pEffect, m_pInputLayout, false, true);
		}
		//box, sphere, capsule만 드로우 지원. 나머진 고려해볼게요 ㅎ..
		else
		{
		}
	}

	m_PxShapes.clear();
}

_float4 CRenderer::Convert_PxColor_ToVector(PxU32 iColor)
{
	_float fAlpha = ((iColor >> 24) & 0xFF) / 255.0f;
	_float fRed   = ((iColor >> 16) & 0xFF) / 255.0f;
	_float fGreen = ((iColor >> 8) & 0xFF) / 255.0f;
	_float fBlue  = (iColor & 0xFF) / 255.0f;

	return _float4(fRed, fGreen, fBlue, fAlpha);
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

#ifdef _DEBUG
	for (auto& pDebugCom : m_DebugComponents)
		Safe_Release(pDebugCom);
	m_DebugComponents.clear();

	Safe_Release(m_pPhysxDebugShader);
	Safe_Delete(m_pEffect);
	Safe_Release(m_pInputLayout);

	/* 혹시모를 null 대입. 원본이 스마트 포인터라 따로 해제 안해도 돼요*/
	m_pSphereShape = nullptr;
	m_pBoxShape = nullptr;
	m_pCapsuleCylinderShape = nullptr;
	m_pCapsuleHemiSphereShape = nullptr;
#endif // _DEBUG

	for (auto& RenderObjects : m_RenderObjects)
	{
		for (auto& pRenderObject : RenderObjects)
			Safe_Release(pRenderObject);
		RenderObjects.clear();
	}

	Safe_Release(m_pShadowDSV);
	Safe_Release(m_pOutlineDSV);
	Safe_Release(m_pGameInstance);

	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pShader);
	
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}