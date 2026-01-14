#include "SSAO.h"

#include "GameInstance.h"

CSSAO::CSSAO(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CDeferred { pDevice, pContext }
{
}

void* CSSAO::Get_Desc()
{
    m_Desc.fBiasMax = &m_fBiasMax;
    m_Desc.fBiasMin = &m_fBiasMin;
    m_Desc.fIntensity = &m_fIntensity;
    m_Desc.fRadiusMax = &m_fRadiusMax;
    m_Desc.fRadiusMin = &m_fRadiusMin;
    m_Desc.isActive = &m_isActive;
   
    return &m_Desc;
}

HRESULT CSSAO::Initialize()
{
    Create_RandomNoise();

    /* 셰이더 파일 로딩 */
    m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Deferred_SSAO.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements);
    if (nullptr == m_pShader)
        return E_FAIL;

    /* 스크린 사이즈는 미리 바인딩 한다. */
    _uint2 vScreenSize = m_pGameInstance->GetScreenSize();
    if (FAILED(m_pShader->Bind_RawValue("g_iWinSizeX", &vScreenSize.x, sizeof(_int))))
        return E_FAIL;

    if (FAILED(m_pShader->Bind_RawValue("g_iWinSizeY", &vScreenSize.y, sizeof(_int))))
        return E_FAIL;

    /* Target_SSAO. Y에 대해서도 블러처리 수행. */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_SSAO"), vScreenSize.x, vScreenSize.y, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(1.0f, 1.0f, 1.0f, 1.0f))))
        return E_FAIL;
    /* MRT_SSAO */
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_SSAO"), TEXT("Target_SSAO"))))
        return E_FAIL;

    /* Target_SSAO_BlurX. Y에 대해서도 블러처리 수행. */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_SSAO_BlurX"), vScreenSize.x, vScreenSize.y, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(1.0f, 1.0f, 1.0f, 1.0f))))
        return E_FAIL;
    /* MRT_SSAO_BlurX */
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_SSAO_BlurX"), TEXT("Target_SSAO_BlurX"))))
        return E_FAIL;

    /* Target_SSAO_BlurY. Y에 대해서도 블러처리 수행. */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_SSAO_BlurY"), vScreenSize.x, vScreenSize.y, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(1.0f, 1.0f, 1.0f, 1.0f))))
        return E_FAIL;
    /* MRT_SSAO_BlurY */
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_SSAO_BlurY"), TEXT("Target_SSAO_BlurY"))))
        return E_FAIL;

    return S_OK;
}

HRESULT CSSAO::Render(CVIBuffer_Rect* pVIBuffer, const _wstring& strDepthRTTag, const _wstring& strNormalRTTag, const _wstring& strReturnRTTag)
{
#ifdef _DEBUG
    if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_F9))
        m_isActive = !m_isActive;
#endif

    if (false == m_isActive)
    {
        if (FAILED(m_pGameInstance->Clear_MRT(TEXT("MRT_SSAO"))))
            return E_FAIL;

        if (FAILED(m_pGameInstance->Clear_MRT(TEXT("MRT_SSAO_BlurY"))))
            return E_FAIL;

        return S_OK;
    }

    /* SSAO 뽑아내오고. */
    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_SSAO"))))
        return E_FAIL;

    CAMERA_INFO 	CamInfo = m_pGameInstance->Get_CurrentCamInfo();
    m_pShader->Bind_RawValue("g_fFar", &CamInfo.fFar, sizeof(_float));
    
    m_pShader->Bind_RawValue("g_NoiseValue", &m_vRandomNoise, sizeof(_float3) * 16);

    m_pShader->Bind_Matrix("g_OrthoWorldMatrix", m_pGameInstance->Get_Renderer_Matrix());
    m_pShader->Bind_Matrix("g_OrthoViewMatrix", m_pGameInstance->Get_Renderer_Matrix(D3DTS::VIEW));
    m_pShader->Bind_Matrix("g_OrthoProjMatrix", m_pGameInstance->Get_Renderer_Matrix(D3DTS::PROJ));

    m_pShader->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW));
    m_pShader->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ));
    m_pShader->Bind_Matrix("g_ProjMatrixInv", m_pGameInstance->Get_Transform_Float4x4_Inverse(D3DTS::PROJ));

    if (FAILED(m_pGameInstance->Bind_RenderTarget(strDepthRTTag, m_pShader, "g_DepthTexture")))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Bind_RenderTarget(strNormalRTTag, m_pShader, "g_NormalTexture")))
        return E_FAIL;

    if (FAILED(m_pShader->Bind_RawValue("g_fRadiusMin", &m_fRadiusMin, sizeof(_float))))
        return E_FAIL;
    if (FAILED(m_pShader->Bind_RawValue("g_fRadiusMax", &m_fRadiusMax, sizeof(_float))))
        return E_FAIL;
    if (FAILED(m_pShader->Bind_RawValue("g_fBiasMin", &m_fBiasMin, sizeof(_float))))
        return E_FAIL;
    if (FAILED(m_pShader->Bind_RawValue("g_fBiasMax", &m_fBiasMax, sizeof(_float))))
        return E_FAIL;
    if (FAILED(m_pShader->Bind_RawValue("g_fIntensity", &m_fIntensity, sizeof(_float))))
        return E_FAIL;


    m_pShader->Begin(0);
    pVIBuffer->Bind_Resources();
    pVIBuffer->Render();

    if (FAILED(m_pGameInstance->End_MRT()))
        return E_FAIL;

    // SSAO Blur X 처리
    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_SSAO_BlurX"))))
        return E_FAIL;

    m_pShader->Bind_Matrix("g_OrthoWorldMatrix", m_pGameInstance->Get_Renderer_Matrix());
    m_pShader->Bind_Matrix("g_OrthoViewMatrix", m_pGameInstance->Get_Renderer_Matrix(D3DTS::VIEW));
    m_pShader->Bind_Matrix("g_OrthoProjMatrix", m_pGameInstance->Get_Renderer_Matrix(D3DTS::PROJ));

    if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_SSAO"), m_pShader, "g_SSAOTexture")))
        return E_FAIL;

    m_pShader->Begin(1);
    pVIBuffer->Bind_Resources();
    pVIBuffer->Render();

    if (FAILED(m_pGameInstance->End_MRT()))
        return E_FAIL;

    // Blur Y 처리하고 합성
    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_SSAO_BlurY"))))
        return E_FAIL;

    m_pShader->Bind_Matrix("g_OrthoWorldMatrix", m_pGameInstance->Get_Renderer_Matrix());
    m_pShader->Bind_Matrix("g_OrthoViewMatrix", m_pGameInstance->Get_Renderer_Matrix(D3DTS::VIEW));
    m_pShader->Bind_Matrix("g_OrthoProjMatrix", m_pGameInstance->Get_Renderer_Matrix(D3DTS::PROJ));

    m_pShader->Begin(2);
    pVIBuffer->Bind_Resources();
    pVIBuffer->Render();

    if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_SSAO_BlurX"), m_pShader, "g_SSAOBlurXTexture")))
        return E_FAIL;

    if (FAILED(m_pGameInstance->End_MRT()))
        return E_FAIL;

    return S_OK;
}

//최종 처리한 녀석 SSAO 텍스쳐 바인딩까지.
HRESULT CSSAO::Bind_RenderTarget(CShader* pShader, const _char* pConstantName)
{
    if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_SSAO_BlurY"), pShader, pConstantName)))
        return E_FAIL;

    return S_OK;
}

void CSSAO::Create_RandomNoise()
{
    for (_int i = 0; i < 16; ++i)
    {
        XMStoreFloat3(&m_vRandomNoise[i] , XMVectorSet(m_pGameInstance->Random(-1.f, 1.f), m_pGameInstance->Random(-1.f, 1.f), 0.f, 0.f));
    }
}

#ifdef _DEBUG
HRESULT CSSAO::Ready_Debug(_float fX, _float fY, _float fSizeX, _float fSizeY)
{
    if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_SSAO_BlurY"), fX, fY, fSizeX, fSizeY)))
        return E_FAIL;

    return S_OK;
}

HRESULT CSSAO::Render_Debug(CVIBuffer_Rect* pVIBuffer, CShader* pShader)
{
    if (FAILED(m_pGameInstance->Render_RT_Debug(TEXT("MRT_SSAO_BlurY"), pShader, pVIBuffer)))
        return E_FAIL;

    return S_OK;
}
#endif

CSSAO* CSSAO::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSSAO* pInstance = new CSSAO(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Create Failed : SSAO");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSSAO::Free()
{
	__super::Free();
}
