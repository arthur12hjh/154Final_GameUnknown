#include "Bloom.h"

#include "GameInstance.h"
#include "GameObject.h"

CBloom::CBloom(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CDeferred{ pDevice, pContext }
{
}

HRESULT CBloom::Initialize()
{
    /* 셰이더 파일 로딩 */
    m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Deferred_Bloom.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements);
    if (nullptr == m_pShader)
        return E_FAIL;

    /* 스크린 사이즈는 미리 바인딩 한다. */
    _uint2 vScreenSize = m_pGameInstance->GetScreenSize();
    if (FAILED(m_pShader->Bind_RawValue("g_iWinSizeX", &vScreenSize.x, sizeof(_int))))
        return E_FAIL;
    if (FAILED(m_pShader->Bind_RawValue("g_iWinSizeY", &vScreenSize.y, sizeof(_int))))
        return E_FAIL;

    // 다운 샘플 4x4 
    /* Target_BloomDownSample4x4. */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_BloomDownSample4x4"), vScreenSize.x / 4, vScreenSize.y / 4, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(1.0f, 0.0f, 0.0f, 1.0f))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_BloomDownSample4x4"), TEXT("Target_BloomDownSample4x4"))))
        return E_FAIL;

    // 다운 샘플 5x5 (최종 20x20)
    /* Target_BloomDownSample20x20. */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_BloomDownSample20x20"), vScreenSize.x / 20, vScreenSize.y / 20, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.0f, 0.0f, 0.0f, 0.0f))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_BloomDownSample20x20"), TEXT("Target_BloomDownSample20x20"))))
        return E_FAIL;

    // 다운 샘플 5x5 (최종 20x20)
    /* Target_BloomDownSample20x20. */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_BloomDownSample20x20_Final"), vScreenSize.x / 20, vScreenSize.y / 20, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.0f, 0.0f, 0.0f, 0.0f))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_BloomDownSample20x20_Final"), TEXT("Target_BloomDownSample20x20_Final"))))
        return E_FAIL;


    // 업 샘플 5x5 1번 더. (최종 4x4)
    /* Target_BloomUpSample4x4. */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_BloomUpSample4x4"), vScreenSize.x / 4, vScreenSize.y / 4, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.0f, 0.0f, 0.0f, 0.0f))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_BloomUpSample4x4"), TEXT("Target_BloomUpSample4x4"))))
        return E_FAIL;

    // 업 샘플에 블러 먹여.
    /* Target_BloomUpSample4x4_BlurX */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_BloomUpSample4x4_BlurX"), vScreenSize.x / 4, vScreenSize.y / 4, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.0f, 0.0f, 0.0f, 0.0f))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_BloomUpSample4x4_BlurX"), TEXT("Target_BloomUpSample4x4_BlurX"))))
        return E_FAIL;

    /* Target_BloomUpSample4x4_BlurY */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_BloomUpSample4x4_BlurY"), vScreenSize.x / 4, vScreenSize.y / 4, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.0f, 0.0f, 0.0f, 0.0f))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_BloomUpSample4x4_BlurY"), TEXT("Target_BloomUpSample4x4_BlurY"))))
        return E_FAIL;

    // 최종 렌더타겟. 
    /* Target_Bloom_Final */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Bloom_Final"), vScreenSize.x, vScreenSize.y, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.0f, 1.0f, 0.0f, 1.0f))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Bloom_Final"), TEXT("Target_Bloom_Final"))))
        return E_FAIL;

    if (FAILED(Ready_DSVs()))
        return E_FAIL;

    return S_OK;
}

HRESULT CBloom::Render(CVIBuffer_Rect* pVIBuffer, const _wstring& strRenderTargetTag)
{
    _uint2 vScreenSize = m_pGameInstance->GetScreenSize();

#pragma region DOWN_SAMPLE_4x4
    /* 다운 샘플링 4x4 수행.*/
    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_BloomDownSample4x4"), m_p4x4SampleDSV)))
        return E_FAIL;

    m_pGameInstance->Set_ScreenSize(vScreenSize.x / 4, vScreenSize.y / 4);
    m_pShader->Bind_Matrix("g_WorldMatrix", m_pGameInstance->Get_Renderer_Matrix());
    m_pShader->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Renderer_Matrix(D3DTS::VIEW));
    m_pShader->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Renderer_Matrix(D3DTS::PROJ));

    /* 캡쳐된 화면을 바인딩. */
    if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_Scene"), m_pShader, "g_SceneTexture")))
        return E_FAIL;

    m_pShader->Begin(ENUM_CLASS(SHADER_BLOOM_IDX::CURVE));	
    pVIBuffer->Bind_Resources();
    pVIBuffer->Render();

    if (FAILED(m_pGameInstance->End_MRT()))
        return E_FAIL;
#pragma endregion

#pragma region DOWN_SAMPLE_20x20
    /* 다운 샘플링 6x6 수행. (최종 24x24) */
    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_BloomDownSample20x20"), m_p20x20SampleDSV)))
        return E_FAIL;

    m_pGameInstance->Set_ScreenSize(vScreenSize.x / 20, vScreenSize.y / 20);
    m_pShader->Bind_Matrix("g_WorldMatrix", m_pGameInstance->Get_Renderer_Matrix());
    m_pShader->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Renderer_Matrix(D3DTS::VIEW));
    m_pShader->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Renderer_Matrix(D3DTS::PROJ));

    /* 4x4 다운 샘플링된 화면을 바인딩. */
    if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_BloomDownSample4x4"), m_pShader, "g_SceneTexture")))
        return E_FAIL;

    m_pShader->Begin(ENUM_CLASS(SHADER_BLOOM_IDX::SAMPLING_BLUR_X));
    pVIBuffer->Bind_Resources();
    pVIBuffer->Render();

    if (FAILED(m_pGameInstance->End_MRT()))
        return E_FAIL;
#pragma endregion

#pragma region DOWN_SAMPLE_20x20_FINAL
    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_BloomDownSample20x20_Final"), m_p20x20SampleDSV)))
        return E_FAIL;

    m_pGameInstance->Set_ScreenSize(vScreenSize.x / 20, vScreenSize.y / 20);
    m_pShader->Bind_Matrix("g_WorldMatrix", m_pGameInstance->Get_Renderer_Matrix());
    m_pShader->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Renderer_Matrix(D3DTS::VIEW));
    m_pShader->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Renderer_Matrix(D3DTS::PROJ));

    if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_BloomDownSample20x20"), m_pShader, "g_SceneTexture")))
        return E_FAIL;

    m_pShader->Begin(ENUM_CLASS(SHADER_BLOOM_IDX::SAMPLING_BLUR_Y));
    pVIBuffer->Bind_Resources();
    pVIBuffer->Render();

    if (FAILED(m_pGameInstance->End_MRT()))
        return E_FAIL;
#pragma endregion

#pragma region UP_SAMPLE_4x4
    /* 다운 샘플링 24x24 에 대해 블러처리. */
    /* 이전 단계 텍스쳐도 누적 (additive) 해서 블러처리 해야됨. */
    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_BloomUpSample4x4"), m_p4x4SampleDSV)))
        return E_FAIL;

    m_pGameInstance->Set_ScreenSize(vScreenSize.x / 4, vScreenSize.y / 4);
    m_pShader->Bind_Matrix("g_WorldMatrix", m_pGameInstance->Get_Renderer_Matrix());
    m_pShader->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Renderer_Matrix(D3DTS::VIEW));
    m_pShader->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Renderer_Matrix(D3DTS::PROJ));

    m_pShader->Begin(ENUM_CLASS(SHADER_BLOOM_IDX::SAMPLING));
    pVIBuffer->Bind_Resources();
    pVIBuffer->Render();

    if (FAILED(m_pGameInstance->End_MRT()))
        return E_FAIL;
#pragma endregion

#pragma region UP_SAMPLE_4x4_BLUR_X
    /* 다운 샘플링 24x24 에 대해 블러 X 처리. */
    /* 이전 단계 텍스쳐도 누적 (additive) 해서 블러처리 해야됨. */
    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_BloomUpSample4x4_BlurX"), m_p4x4SampleDSV)))
        return E_FAIL;

    m_pGameInstance->Set_ScreenSize(vScreenSize.x / 4, vScreenSize.y / 4);

    m_pShader->Bind_Matrix("g_WorldMatrix", m_pGameInstance->Get_Renderer_Matrix());
    m_pShader->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Renderer_Matrix(D3DTS::VIEW));
    m_pShader->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Renderer_Matrix(D3DTS::PROJ));

    _uint2 vNewScreenSize = _uint2(vScreenSize.x / 4, vScreenSize.y / 4);
    if (FAILED(m_pShader->Bind_RawValue("g_iWinSizeX", &vNewScreenSize.x, sizeof(_int))))
        return E_FAIL;
    if (FAILED(m_pShader->Bind_RawValue("g_iWinSizeY", &vNewScreenSize.y, sizeof(_int))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_BloomUpSample4x4"), m_pShader, "g_SceneTexture")))
        return E_FAIL;


    if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_BloomDownSample20x20_Final"), m_pShader, "g_SourTexture")))
        return E_FAIL;


    m_pShader->Begin(ENUM_CLASS(SHADER_BLOOM_IDX::ADDITIVE_BLUR_X));
    pVIBuffer->Bind_Resources();
    pVIBuffer->Render();

    if (FAILED(m_pGameInstance->End_MRT()))
        return E_FAIL;
#pragma endregion

#pragma region UP_SAMPLE_4x4_BLUR_Y
    /* 다운 샘플링 24x24 에 대해 블러 Y 처리. */
    /* 이전 단계 텍스쳐도 누적 (additive) 해서 블러처리 해야됨. */
    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_BloomUpSample4x4_BlurY"), m_p4x4SampleDSV)))
        return E_FAIL;

    m_pGameInstance->Set_ScreenSize(vScreenSize.x / 4, vScreenSize.y / 4);
    m_pShader->Bind_Matrix("g_WorldMatrix", m_pGameInstance->Get_Renderer_Matrix());
    m_pShader->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Renderer_Matrix(D3DTS::VIEW));
    m_pShader->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Renderer_Matrix(D3DTS::PROJ));
    vNewScreenSize = _uint2(vScreenSize.x / 4, vScreenSize.y / 4);
    if (FAILED(m_pShader->Bind_RawValue("g_iWinSizeX", &vNewScreenSize.x, sizeof(_int))))
        return E_FAIL;
    if (FAILED(m_pShader->Bind_RawValue("g_iWinSizeY", &vNewScreenSize.y, sizeof(_int))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_BloomUpSample4x4_BlurX"), m_pShader, "g_SceneTexture")))
        return E_FAIL;

    m_pShader->Begin(ENUM_CLASS(SHADER_BLOOM_IDX::ADDITIVE_BLUR_Y));
    pVIBuffer->Bind_Resources();
    pVIBuffer->Render();

    if (FAILED(m_pGameInstance->End_MRT()))
        return E_FAIL;
#pragma endregion

#pragma region FINAL
    // 최종적으로 원본 렌더타겟과 같은 사이즈로 샘플링 
    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Bloom_Final"))))
        return E_FAIL;

    m_pGameInstance->Set_ScreenSize(vScreenSize.x, vScreenSize.y);
    vNewScreenSize = _uint2(vScreenSize.x, vScreenSize.y);
    if (FAILED(m_pShader->Bind_RawValue("g_iWinSizeX", &vNewScreenSize.x, sizeof(_int))))
        return E_FAIL;
    if (FAILED(m_pShader->Bind_RawValue("g_iWinSizeY", &vNewScreenSize.y, sizeof(_int))))
        return E_FAIL;

    m_pShader->Bind_Matrix("g_WorldMatrix", m_pGameInstance->Get_Renderer_Matrix());
    m_pShader->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Renderer_Matrix(D3DTS::VIEW));
    m_pShader->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Renderer_Matrix(D3DTS::PROJ));

    if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_BloomUpSample4x4_BlurY"), m_pShader, "g_SceneTexture")))
        return E_FAIL;

    m_pShader->Begin(ENUM_CLASS(SHADER_BLOOM_IDX::SAMPLING));
    pVIBuffer->Bind_Resources();
    pVIBuffer->Render();

    if (FAILED(m_pGameInstance->End_MRT()))
        return E_FAIL;
#pragma endregion

    return S_OK;
}

HRESULT CBloom::Bind_RenderTarget(CShader* pShader, const _char* pConstantName)
{
    /* 업샘플 4x4가 끝난게 최종 블룸 처리 렌더타겟. */
    if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_Bloom_Final"), pShader, pConstantName)))
        return E_FAIL;

    return S_OK;
}

#ifdef _DEBUG
HRESULT CBloom::Ready_Debug(_float fX, _float fY, _float fSizeX, _float fSizeY)
{
    if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_BloomUpSample4x4_BlurX"), 150.f, 150.f, 300, 300)))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_BloomUpSample4x4_BlurY"), 450.f, 150.f, 300, 300)))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_Bloom_Final"), 750.f, 150.f, 300, 300)))
        return E_FAIL;

    return S_OK;
}

HRESULT CBloom::Render_Debug(CVIBuffer_Rect* pVIBuffer, CShader* pShader)
{
    if (FAILED(m_pGameInstance->Render_RT_Debug(TEXT("MRT_BloomUpSample4x4_BlurX"), pShader, pVIBuffer)))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Render_RT_Debug(TEXT("MRT_BloomUpSample4x4_BlurY"), pShader, pVIBuffer)))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Render_RT_Debug(TEXT("MRT_Bloom_Final"), pShader, pVIBuffer)))
        return E_FAIL;


    return S_OK;
}
#endif

HRESULT CBloom::Ready_DSVs()
{
    if (nullptr == m_pDevice)
        return E_FAIL;

    ID3D11Texture2D* pDepthStencilTexture = nullptr;
    D3D11_TEXTURE2D_DESC	TextureDesc;
    ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

    /* 깊이 버퍼의 픽셀은 백버퍼의 픽셀과 갯수가 동일해야만 깊이 텍스트가 가능해진다. */
    /* 픽셀의 수가 다르면 아에 렌더링을 못함. */
    _uint2 vScreenSize = m_pGameInstance->GetScreenSize();
    TextureDesc.Width = vScreenSize.x / 4;
    TextureDesc.Height = vScreenSize.y / 4 ;
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
    if (FAILED(m_pDevice->CreateDepthStencilView(pDepthStencilTexture, nullptr, &m_p4x4SampleDSV)))
        return E_FAIL;
    Safe_Release(pDepthStencilTexture);

    TextureDesc.Width = vScreenSize.x / 20;
    TextureDesc.Height = vScreenSize.y / 20;
    if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &pDepthStencilTexture)))
        return E_FAIL;
    if (FAILED(m_pDevice->CreateDepthStencilView(pDepthStencilTexture, nullptr, &m_p20x20SampleDSV)))
        return E_FAIL;
    Safe_Release(pDepthStencilTexture);

    return S_OK;
}

CBloom* CBloom::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CBloom* pInstance = new CBloom(pDevice, pContext);

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Create Failed : CBloom");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CBloom::Free()
{
    __super::Free();

    Safe_Release(m_p4x4SampleDSV);
    Safe_Release(m_p20x20SampleDSV);
}
