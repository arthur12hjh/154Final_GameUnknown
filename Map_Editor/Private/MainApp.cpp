#include "pch.h"
#include "MainApp.h"

#include "GameInstance.h"
#include "Level_Loading.h"
#include "Camera_Free.h"
#include "LoadingBG.h"


CMainApp::CMainApp()
	: m_pGameInstance{ CGameInstance::GetInstance() }
{
	// D3D11_SAMPLER_DESC
	//ID3D11RasterizerState* pRSState = { nullptr };

	//D3D11_RASTERIZER_DESC		RSDesc{};
	//m_pDevice->CreateRasterizerState(&RSDesc, &pRSState);
	//m_pContext->RSSetState(pRSState);

	// D3D11_DEPTH_STENCIL_DESC
	//m_pContext->OMSetDepthStencilState();

	//D3D11_BLEND_DESC

	//m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

	//ID3D11RasterizerState* pRSState = { nullptr };
	//ID3D11DepthStencilState*;
	//ID3D11BlendState*

	//m_pContext->RSSetState();
	//m_pContext->OMSetDepthStencilState();
	//m_pContext->OMSetBlendState();



	Safe_AddRef(m_pGameInstance);
}

HRESULT CMainApp::Initialize()
{
	ENGINE_DESC				EngineDesc{};
	EngineDesc.hInstance = g_hInstance;
	EngineDesc.hWnd = g_hWnd;
	EngineDesc.eWindowMode = WINMODE::WIN;
	EngineDesc.iWinSizeX = g_iWinSizeX;
	EngineDesc.iWinSizeY = g_iWinSizeY;
	EngineDesc.iNumLevels = ENUM_CLASS(LEVEL::END);

	if (FAILED(m_pGameInstance->Initialize_Engine(EngineDesc, &m_pDevice, &m_pContext)))
		return E_FAIL;

	/*if (FAILED(Ready_Gara()))
		return E_FAIL;*/

	/*if (FAILED(Ready_Default_Setting()))
		return E_FAIL;*/

	if (FAILED(Ready_Prototypes()))
		return E_FAIL;

	if (FAILED(Start_Level(LEVEL::VILLAGE)))
		return E_FAIL;

	return S_OK;
}

void CMainApp::Update(_float fTimeDelta)
{
	m_pGameInstance->Update_Engine(fTimeDelta);

#ifdef _DEBUG
	m_fTimeAcc += fTimeDelta;
#endif
}

HRESULT CMainApp::Render()
{
	_float4			vClearColor = _float4(0.f, 0.f, 1.f, 1.f);

	m_pGameInstance->Render_Begin(&vClearColor);

	m_pGameInstance->Draw();

//#ifdef _DEBUG
//	++m_iDrawCnt;
//
//	if (m_fTimeAcc >= 1.f)
//	{
//		wsprintf(m_szFPS, TEXT("FPS : %d"), m_iDrawCnt);
//
//		m_iDrawCnt = 0;
//		m_fTimeAcc = 0.f;
//	}
//
//	//m_pGameInstance->Render_Text(TEXT("Font_154"), m_szFPS, _float2(0.f, 0.f), XMVectorSet(1.f, 0.f, 0.f, 1.f));
//
//#endif


	m_pGameInstance->Render_End();

	return S_OK;
}

HRESULT CMainApp::Ready_Default_Setting()
{
	/*MakeSpriteFont "³Ø½¼Lv1°íµñ Bold" /FontSize:20 /FastPack /CharacterRegion:0x0020-0x00FF /CharacterRegion:0x3131-0x3163 /CharacterRegion:0xAC00-0xD800 /DefaultCharacter:0xAC00 155ex.spritefont */
	/*if (FAILED(m_pGameInstance->Add_Font(TEXT("Font_154"), TEXT("../Bin/Resources/Fonts/154ex.spritefont"))))
		return E_FAIL;*/

	return S_OK;
}

HRESULT CMainApp::Start_Level(LEVEL eLevelID)
{
	if (FAILED(m_pGameInstance->Change_Level(CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL::LOADING, eLevelID))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_Gara()
{
	//ID3D11Texture2D* pTexture2D = { nullptr };

	//D3D11_TEXTURE2D_DESC		TextureDesc{};
	//TextureDesc.Width = 256;
	//TextureDesc.Height = 256;
	//TextureDesc.MipLevels = 1;
	//TextureDesc.ArraySize = 1;
	//TextureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	//TextureDesc.SampleDesc.Quality = 0;
	//TextureDesc.SampleDesc.Count = 1;
	//TextureDesc.Usage = D3D11_USAGE_STAGING;
	//TextureDesc.BindFlags = 0;
	//TextureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
	//TextureDesc.MiscFlags = 0;

	//_uint* pPixel = new _uint[256 * 256];

	//for (size_t i = 0; i < 256; i++)
	//{
	//	for (size_t j = 0; j < 256; j++)
	//	{
	//		_uint iIndex = i * 256 + j;
	//		pPixel[iIndex] = D3DCOLOR_ARGB(255, 0, 0, 0);
	//	}
	//}

	//D3D11_SUBRESOURCE_DATA		InitialDesc{};
	//InitialDesc.pSysMem = pPixel;
	//InitialDesc.SysMemPitch = 256 * 4;

	//if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, &InitialDesc, &pTexture2D)))
	//	return E_FAIL;

	//D3D11_MAPPED_SUBRESOURCE		SubResource{};

	//m_pContext->Map(pTexture2D, 0, D3D11_MAP_READ_WRITE, 0, &SubResource);

	//_uint* pPixels = static_cast<_uint*>(SubResource.pData);

	//for (size_t i = 0; i < 256; i++)
	//{
	//	for (size_t j = 0; j < 256; j++)
	//	{
	//		_uint iIndex = i * 256 + j;

	//		if (j < 128)
	//			pPixels[iIndex] = D3DCOLOR_ARGB(255, 0, 0, 0);/*0xff000000*//*0b00000000000000000000000011111111*/
	//		else
	//			pPixels[iIndex] = D3DCOLOR_ARGB(255, 255, 255, 255);/*0xff000000*//*0b00000000000000000000000011111111*/;
	//	}
	//}

	//m_pContext->Unmap(pTexture2D, 0);

	///*if (FAILED(DirectX::SaveDDSTextureToFile(m_pContext, pTexture2D, TEXT("../Bin/Resources/Textures/Terrain/Mask.dds"))))
	//	return E_FAIL;*/

	//Safe_Delete_Array(pPixel);

	//Safe_Release(pTexture2D);

	_ulong			dwByte = {};

	HANDLE			hFile = CreateFile(TEXT("../Bin/DataFiles/Navigation.dat"), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	_float3			vPoints[3] = {};

	vPoints[0] = _float3(0.0f, 0.f, 10.f);
	vPoints[1] = _float3(10.f, 0.f, 0.f);
	vPoints[2] = _float3(0.f, 3.f, 0.f);
	WriteFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);

	vPoints[0] = _float3(0.0f, 0.f, 10.f);
	vPoints[1] = _float3(10.f, 0.f, 10.f);
	vPoints[2] = _float3(10.f, 0.f, 0.f);
	WriteFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);

	vPoints[0] = _float3(0.0f, 0.f, 20.f);
	vPoints[1] = _float3(10.f, 0.f, 10.f);
	vPoints[2] = _float3(0.f, 0.f, 10.f);
	WriteFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);

	vPoints[0] = _float3(10.f, 0.f, 10.f);
	vPoints[1] = _float3(20.f, 0.f, 0.f);
	vPoints[2] = _float3(10.f, 0.f, 0.f);
	WriteFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);

	CloseHandle(hFile);

	return S_OK;
}

HRESULT CMainApp::Ready_Prototypes()
{
	///* For.Prototype_Component_Transform*/
	//if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Transform"),
	//	CTransform::Create(m_pGraphic_Device))))
	//	return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Rect */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxPosTex */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPosTex.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_BackGround */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_LoadingBG"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Texture/Loading/Loading_BG%d.png"), 6))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_LoadingBG"),
		CLoadingBG::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Camera_Free */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Camera_Free"),
		CCamera_Free::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	return S_OK;
}

CMainApp* CMainApp::Create()
{
	CMainApp* pInstance = new CMainApp();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CMainApp");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMainApp::Free()
{
	__super::Free();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	m_pGameInstance->Release_Engine();

	Safe_Release(m_pGameInstance);
}
