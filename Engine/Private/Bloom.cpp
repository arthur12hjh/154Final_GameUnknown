#include "Bloom.h"

#include "GameInstance.h"
#include "GameObject.h"

CBloom::CBloom(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CDeferred{ pDevice, pContext }
{
}

void* CBloom::Get_Desc()
{
    m_Desc.iBloomLevel = &m_iBloomLevel;
    m_Desc.iSampleLevel = &m_iSampleLevel;

    return &m_Desc;
}

HRESULT CBloom::Initialize()
{
    m_vOriginScreenSize = m_pGameInstance->GetScreenSize();

    /* 셰이더 파일 로딩 */
    m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Deferred_Bloom.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements);
    if (nullptr == m_pShader)
        return E_FAIL;

    if (FAILED(Ready_RenderTargets()))
        return E_FAIL;

    if (FAILED(Ready_DSVs()))
        return E_FAIL;

    return S_OK;
}

HRESULT CBloom::Render(CVIBuffer_Rect* pVIBuffer, const _wstring& strSceneRenderTargetTag, const _wstring& strCombineRTTag)
{
    /* 과정 1. 전체 다운 샘플링 수행먼저해주고..*/
    /* 과정 2. 업 샘플링 할떄마다 블러처리 해서 누적 가산한 뒤 처리*/
    m_pShader->Bind_Matrix("g_WorldMatrix", m_pGameInstance->Get_Renderer_Matrix());
    m_pShader->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Renderer_Matrix(D3DTS::VIEW));
    m_pShader->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Renderer_Matrix(D3DTS::PROJ));
#pragma region DOWN_SAMPLE
    DownSampling(pVIBuffer, strSceneRenderTargetTag);
#pragma endregion

#pragma region BLUR
    MiddleBlur(pVIBuffer);
#pragma endregion

#pragma region UP_SAMPLE
    UpSampling(pVIBuffer);
#pragma endregion

#pragma region FINAL
    // 최종적으로 원본 렌더타겟과 같은 사이즈로 샘플링 
    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Bloom_Final"))))
        return E_FAIL;

    m_pGameInstance->Set_ScreenSize(m_vOriginScreenSize.x, m_vOriginScreenSize.y);
    if (FAILED(m_pShader->Bind_RawValue("g_iWinSizeX", &m_vOriginScreenSize.x, sizeof(_int))))
        return E_FAIL;
    if (FAILED(m_pShader->Bind_RawValue("g_iWinSizeY", &m_vOriginScreenSize.y, sizeof(_int))))
        return E_FAIL;

    m_pShader->Bind_Matrix("g_WorldMatrix", m_pGameInstance->Get_Renderer_Matrix());
    m_pShader->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Renderer_Matrix(D3DTS::VIEW));
    m_pShader->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Renderer_Matrix(D3DTS::PROJ));

    if (FAILED(m_pGameInstance->Bind_RenderTarget(m_strPreRenderTargetTag, m_pShader, "g_SceneTexture")))
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
    if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_Bloom_Final"), 750.f, 150.f, 300, 300)))
        return E_FAIL;

    return S_OK;
}

HRESULT CBloom::Render_Debug(CVIBuffer_Rect* pVIBuffer, CShader* pShader)
{
    if (FAILED(m_pGameInstance->Render_RT_Debug(TEXT("MRT_Bloom_Final"), pShader, pVIBuffer)))
        return E_FAIL;


    return S_OK;
}

#endif

HRESULT CBloom::Ready_RenderTargets()
{
    //블룸 레벨이 3이고, 샘플레벨이 3이라면,
    //3x3, 6x6, 9x9 렌더타겟들이 생성된다.
    for (_uint i = 0; i < m_iBloomLevel; i++)
    {
        for (_uint j = 0; j < 8; j+=2)
        {
            _wstring strRenderTargetTag = m_strRenderTargetTags[j] + to_wstring(m_iSampleLevel * (i + 1)) + TEXT("x") + to_wstring(m_iSampleLevel * (i + 1));
            _wstring strMRTTag = m_strRenderTargetTags[j+1] + to_wstring(m_iSampleLevel * (i + 1)) + TEXT("x") + to_wstring(m_iSampleLevel * (i + 1));
            
            // 다운 샘플
            /* Target_BloomDownSample4x4. */
            if (FAILED(m_pGameInstance->Add_RenderTarget(strRenderTargetTag, m_vOriginScreenSize.x / pow(m_iSampleLevel,(i + 1)), m_vOriginScreenSize.y / pow(m_iSampleLevel, (i + 1)), DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.0f, 0.0f, 0.0f, 0.0f))))
                return E_FAIL;
            if (FAILED(m_pGameInstance->Add_MRT(strMRTTag, strRenderTargetTag)))
                return E_FAIL;
        }
    }

    // 최종 렌더타겟. 
    /* Target_Bloom_Final */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Bloom_Final"), m_vOriginScreenSize.x, m_vOriginScreenSize.y, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.0f, 0.0f, 0.0f, 0.0f))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Bloom_Final"), TEXT("Target_Bloom_Final"))))
        return E_FAIL;

    return S_OK;
}

HRESULT CBloom::Ready_DSVs()
{
    if (nullptr == m_pDevice)
        return E_FAIL;

    ID3D11DepthStencilView* pDSV = { nullptr };
    ID3D11Texture2D* pDepthStencilTexture = { nullptr };
    D3D11_TEXTURE2D_DESC	TextureDesc;
    ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

    /* 깊이 버퍼의 픽셀은 백버퍼의 픽셀과 갯수가 동일해야만 깊이 텍스트가 가능해진다. */
    /* 픽셀의 수가 다르면 아에 렌더링을 못함. */
    _uint2 vScreenSize = m_pGameInstance->GetScreenSize();
    TextureDesc.Width = vScreenSize.x;
    TextureDesc.Height = vScreenSize.y;
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

    for (_uint i = 0; i < m_iBloomLevel; ++i)
    {
        TextureDesc.Width = vScreenSize.x / pow(m_iSampleLevel, (i + 1));
        TextureDesc.Height = vScreenSize.y / pow(m_iSampleLevel, (i + 1));

        if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &pDepthStencilTexture)))
            return E_FAIL;
        if (FAILED(m_pDevice->CreateDepthStencilView(pDepthStencilTexture, nullptr, &pDSV)))
            return E_FAIL;
        Safe_Release(pDepthStencilTexture);

        m_pDSVs.push_back(pDSV);
    }

    return S_OK;
}

HRESULT CBloom::DownSampling(CVIBuffer* pVIBuffer, const _wstring& strSceneRenderTargetTag)
{
#ifdef _DEBUG
    m_pGameInstance->BeginMarker(m_pContext, TEXT("############### BLOOOM"));
#endif

    // i가 0일때도 0x0 계산하는것만 방지해주자.
    for (_uint i = 0; i < m_iBloomLevel; ++i)
    {
        _wstring strRenderTargetTag = m_strRenderTargetTags[0] + to_wstring(m_iSampleLevel * (i + 1)) + TEXT("x") + to_wstring(m_iSampleLevel * (i + 1));
        _wstring strMRTTag = m_strRenderTargetTags[1] + to_wstring(m_iSampleLevel * (i + 1)) + TEXT("x") + to_wstring(m_iSampleLevel * (i + 1));

        if (FAILED(m_pGameInstance->Begin_MRT(strMRTTag, m_pDSVs[i])))
            return E_FAIL;

        m_pGameInstance->Set_ScreenSize(m_vOriginScreenSize.x / pow(m_iSampleLevel, (i + 1)),
            m_vOriginScreenSize.y / pow(m_iSampleLevel, (i + 1)));

        if (i == 0)
        {

            if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_Bloom"), m_pShader, "g_SceneTexture")))
                return E_FAIL;
            m_pShader->Begin(ENUM_CLASS(SHADER_BLOOM_IDX::CURVE));

        }
        else
        {
            _uint2 inSize = _uint2(
                m_vOriginScreenSize.x / pow(m_iSampleLevel, i),   // i=1이면 /2
                m_vOriginScreenSize.y / pow(m_iSampleLevel, i)
            );

            m_pShader->Bind_RawValue("g_iWinSizeX", &inSize.x, sizeof(_int));
            m_pShader->Bind_RawValue("g_iWinSizeY", &inSize.y, sizeof(_int));

            _wstring strPreRenderTargetTag = m_strRenderTargetTags[0] + to_wstring(m_iSampleLevel * i) + TEXT("x") + to_wstring(m_iSampleLevel * i);
            if (FAILED(m_pGameInstance->Bind_RenderTarget(strPreRenderTargetTag, m_pShader, "g_SceneTexture")))
                return E_FAIL;
            m_pShader->Begin(ENUM_CLASS(SHADER_BLOOM_IDX::DOWNSAMPLE));
        }

        pVIBuffer->Bind_Resources();
        pVIBuffer->Render();

        if (FAILED(m_pGameInstance->End_MRT()))
            return E_FAIL;
    }

#ifdef _DEBUG
    m_pGameInstance->EndMarker(m_pContext);
#endif

    return S_OK;
}

HRESULT CBloom::MiddleBlur(CVIBuffer* pVIBuffer)
{
    //블룸 레벨이 3이고, 샘플레벨이 3이라면,
    //9x9에 대해 블러 처리만 수행해준다.

    //샘플링. 
    _wstring strRenderTargetTag = m_strRenderTargetTags[0] + to_wstring(m_iBloomLevel * m_iSampleLevel) + TEXT("x") + to_wstring(m_iBloomLevel * m_iSampleLevel);
    _wstring strMRTTag = m_strRenderTargetTags[1] + to_wstring(m_iBloomLevel * m_iSampleLevel) + TEXT("x") + to_wstring(m_iBloomLevel * m_iSampleLevel);
    _wstring strBlurXRenderTargetTag = m_strRenderTargetTags[4] + to_wstring(m_iBloomLevel * m_iSampleLevel) + TEXT("x") + to_wstring(m_iBloomLevel * m_iSampleLevel);
    _wstring strBlurXMRTTag = m_strRenderTargetTags[5] + to_wstring(m_iBloomLevel * m_iSampleLevel) + TEXT("x") + to_wstring(m_iBloomLevel * m_iSampleLevel);
    _wstring strBlurYRenderTargetTag = m_strRenderTargetTags[6] + to_wstring(m_iBloomLevel * m_iSampleLevel) + TEXT("x") + to_wstring(m_iBloomLevel * m_iSampleLevel);
    _wstring strBlurYMRTTag = m_strRenderTargetTags[7] + to_wstring(m_iBloomLevel * m_iSampleLevel) + TEXT("x") + to_wstring(m_iBloomLevel * m_iSampleLevel);

    /* 마지막에 샘플링 된 녀석 블러 처리. */
    if (FAILED(m_pGameInstance->Begin_MRT(strBlurXMRTTag, m_pDSVs[m_iBloomLevel - 1])))
        return E_FAIL;

    m_pGameInstance->Set_ScreenSize(m_vOriginScreenSize.x / pow(m_iSampleLevel, m_iBloomLevel), m_vOriginScreenSize.y / pow(m_iSampleLevel, m_iBloomLevel));
    _uint2 vNewScreenSize = _uint2(m_vOriginScreenSize.x / pow(m_iSampleLevel, m_iBloomLevel), m_vOriginScreenSize.y / pow(m_iSampleLevel, m_iBloomLevel));
    
    if (FAILED(m_pShader->Bind_RawValue("g_iWinSizeX", &vNewScreenSize.x, sizeof(_int))))
        return E_FAIL;
    if (FAILED(m_pShader->Bind_RawValue("g_iWinSizeY", &vNewScreenSize.y, sizeof(_int))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Bind_RenderTarget(strRenderTargetTag, m_pShader, "g_SceneTexture")))
        return E_FAIL;

    m_pShader->Begin(ENUM_CLASS(SHADER_BLOOM_IDX::SAMPLING_BLUR_X));
    pVIBuffer->Bind_Resources();
    pVIBuffer->Render();

    if (FAILED(m_pGameInstance->End_MRT()))
        return E_FAIL;

    // 블러 Y 처리 
    if (FAILED(m_pGameInstance->Begin_MRT(strBlurYMRTTag, m_pDSVs[m_iBloomLevel - 1])))
        return E_FAIL;

    m_pGameInstance->Set_ScreenSize(m_vOriginScreenSize.x / pow(m_iSampleLevel, m_iBloomLevel), m_vOriginScreenSize.y / pow(m_iSampleLevel, m_iBloomLevel));

    vNewScreenSize = _uint2(m_vOriginScreenSize.x / pow(m_iSampleLevel, m_iBloomLevel), m_vOriginScreenSize.y / pow(m_iSampleLevel, m_iBloomLevel));
    if (FAILED(m_pShader->Bind_RawValue("g_iWinSizeX", &vNewScreenSize.x, sizeof(_int))))
        return E_FAIL;
    if (FAILED(m_pShader->Bind_RawValue("g_iWinSizeY", &vNewScreenSize.y, sizeof(_int))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Bind_RenderTarget(strBlurXRenderTargetTag, m_pShader, "g_SceneTexture")))
        return E_FAIL;

    m_pShader->Begin(ENUM_CLASS(SHADER_BLOOM_IDX::SAMPLING_BLUR_Y));
    pVIBuffer->Bind_Resources();
    pVIBuffer->Render();

    if (FAILED(m_pGameInstance->End_MRT()))
        return E_FAIL;
        
    m_strPreRenderTargetTag = strBlurYRenderTargetTag;

    return S_OK;
}

HRESULT CBloom::UpSampling(CVIBuffer* pVIBuffer)
{
    //블룸 레벨이 3이고, 샘플레벨이 3이라면,
    //9x9를 가산하여 6x6 업샘플링,
    //6x6을 가산하여 3x3 업샘플링을 수행,
    //
    //레벨이 2이라면 1번 돌아야함.

    for (_uint i = 0; i < m_iBloomLevel - 1; ++i)
    {
        // 현재 처리할 레벨에 해당하는 텍스처 태그들 계산
        // 예를 들어, m_iBloomLevel이 3이고 m_iSampleLevel이 3일 때
        // i=0이면 (m_iBloomLevel - (i + 1)) = 2. 즉 6x6 스케일 텍스처를 처리.
        // i=1이면 (m_iBloomLevel - (i + 1)) = 1. 즉 3x3 스케일 텍스처를 처리.
        _uint  iCurrentSampleLevelMultiplier = m_iBloomLevel - (i + 1);
        _wstring strRenderTargetTag = m_strRenderTargetTags[2] + to_wstring(m_iSampleLevel * iCurrentSampleLevelMultiplier) + TEXT("x") + to_wstring(m_iSampleLevel * iCurrentSampleLevelMultiplier); // Target_BloomUpSample
        _wstring strMRTTag = m_strRenderTargetTags[3] + to_wstring(m_iSampleLevel * iCurrentSampleLevelMultiplier) + TEXT("x") + to_wstring(m_iSampleLevel * iCurrentSampleLevelMultiplier); // MRT_BloomUpSample
        _wstring strBlurXRenderTargetTag = m_strRenderTargetTags[4] + to_wstring(m_iSampleLevel * iCurrentSampleLevelMultiplier) + TEXT("x") + to_wstring(m_iSampleLevel * iCurrentSampleLevelMultiplier); // Target_BloomUpSample_BlurX
        _wstring strBlurXMRTTag = m_strRenderTargetTags[5] + to_wstring(m_iSampleLevel * iCurrentSampleLevelMultiplier) + TEXT("x") + to_wstring(m_iSampleLevel * iCurrentSampleLevelMultiplier); // MRT_BloomUpSample_BlurX
        _wstring strBlurYRenderTargetTag = m_strRenderTargetTags[6] + to_wstring(m_iSampleLevel * iCurrentSampleLevelMultiplier) + TEXT("x") + to_wstring(m_iSampleLevel * iCurrentSampleLevelMultiplier); // Target_BloomUpSample_BlurY
        _wstring strBlurYMRTTag = m_strRenderTargetTags[7] + to_wstring(m_iSampleLevel * iCurrentSampleLevelMultiplier) + TEXT("x") + to_wstring(m_iSampleLevel * iCurrentSampleLevelMultiplier); // MRT_BloomUpSample_BlurY

        // 이 레벨에 해당하는 다운샘플링 원본 텍스처 가져옴.
        // Additive Blend 시 이 텍스처를 g_SourTexture에 바인딩.
        _wstring strCurrentDownSampleOriginalTag = m_strRenderTargetTags[0] + to_wstring(m_iSampleLevel * iCurrentSampleLevelMultiplier) + TEXT("x") + to_wstring(m_iSampleLevel * iCurrentSampleLevelMultiplier); // Target_BloomDownSample

        // 업 샘플링 수행 (이전 레벨의 블러 결과를 현재 레벨 크기로 업샘플)
        if (FAILED(m_pGameInstance->Begin_MRT(strMRTTag, m_pDSVs[iCurrentSampleLevelMultiplier - 1])))
            return E_FAIL;

        m_pGameInstance->Set_ScreenSize(m_vOriginScreenSize.x / pow(m_iSampleLevel, iCurrentSampleLevelMultiplier), m_vOriginScreenSize.y / pow(m_iSampleLevel, iCurrentSampleLevelMultiplier));

        if (FAILED(m_pGameInstance->Bind_RenderTarget(m_strPreRenderTargetTag, m_pShader, "g_SceneTexture")))
            return E_FAIL;

        m_pShader->Begin(ENUM_CLASS(SHADER_BLOOM_IDX::SAMPLING));
        pVIBuffer->Bind_Resources();
        pVIBuffer->Render();

        if (FAILED(m_pGameInstance->End_MRT()))
            return E_FAIL;
        /* 블러 X 처리 (가산 블러) */
        if (FAILED(m_pGameInstance->Begin_MRT(strBlurXMRTTag, m_pDSVs[iCurrentSampleLevelMultiplier - 1]))) // DSV 인덱스 조정
            return E_FAIL;

        m_pGameInstance->Set_ScreenSize(m_vOriginScreenSize.x / pow(m_iSampleLevel, iCurrentSampleLevelMultiplier), m_vOriginScreenSize.y / pow(m_iSampleLevel, iCurrentSampleLevelMultiplier));
        _uint2 vNewScreenSize = _uint2(m_vOriginScreenSize.x / pow(m_iSampleLevel, iCurrentSampleLevelMultiplier), m_vOriginScreenSize.y / pow(m_iSampleLevel, iCurrentSampleLevelMultiplier));

        if (FAILED(m_pShader->Bind_RawValue("g_iWinSizeX", &vNewScreenSize.x, sizeof(_int))))
            return E_FAIL;
        if (FAILED(m_pShader->Bind_RawValue("g_iWinSizeY", &vNewScreenSize.y, sizeof(_int))))
            return E_FAIL;

        if (FAILED(m_pGameInstance->Bind_RenderTarget(strCurrentDownSampleOriginalTag, m_pShader, "g_SourTexture")))
            return E_FAIL;
    
        /* 이전 블러 결과 (m_strPreRenderTargetTag)를 g_SceneTexture에 바인딩하여 현재 레벨 크기로 업샘플링 */
        if (FAILED(m_pGameInstance->Bind_RenderTarget(m_strPreRenderTargetTag, m_pShader, "g_SceneTexture")))
            return E_FAIL;

        m_pShader->Begin(ENUM_CLASS(SHADER_BLOOM_IDX::ADDITIVE_BLUR_X));
        pVIBuffer->Bind_Resources();
        pVIBuffer->Render();

        if (FAILED(m_pGameInstance->End_MRT()))
            return E_FAIL;

        /* 블러 Y 처리 (가산 블러) */
        if (FAILED(m_pGameInstance->Begin_MRT(strBlurYMRTTag, m_pDSVs[iCurrentSampleLevelMultiplier - 1]))) // DSV 인덱스 조정
            return E_FAIL;

        m_pGameInstance->Set_ScreenSize(m_vOriginScreenSize.x / pow(m_iSampleLevel, iCurrentSampleLevelMultiplier), m_vOriginScreenSize.y / pow(m_iSampleLevel, iCurrentSampleLevelMultiplier));
        vNewScreenSize = _uint2(m_vOriginScreenSize.x / pow(m_iSampleLevel, iCurrentSampleLevelMultiplier), m_vOriginScreenSize.y / pow(m_iSampleLevel, iCurrentSampleLevelMultiplier));

        if (FAILED(m_pShader->Bind_RawValue("g_iWinSizeX", &vNewScreenSize.x, sizeof(_int))))
            return E_FAIL;
        if (FAILED(m_pShader->Bind_RawValue("g_iWinSizeY", &vNewScreenSize.y, sizeof(_int))))
            return E_FAIL;

        if (FAILED(m_pGameInstance->Bind_RenderTarget(strCurrentDownSampleOriginalTag, m_pShader, "g_SourTexture")))
            return E_FAIL;

        // g_SceneTexture: 이전 블러 X 결과
        if (FAILED(m_pGameInstance->Bind_RenderTarget(strBlurXRenderTargetTag, m_pShader, "g_SceneTexture")))
            return E_FAIL;

        m_pShader->Begin(ENUM_CLASS(SHADER_BLOOM_IDX::ADDITIVE_BLUR_Y));
        pVIBuffer->Bind_Resources();
        pVIBuffer->Render();

        if (FAILED(m_pGameInstance->End_MRT()))
            return E_FAIL;

        m_strPreRenderTargetTag = strBlurYRenderTargetTag; // 다음 루프의 입력으로 현재 y블러 결과를 설정
    }

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

    for (auto& iter : m_pDSVs)
    {
        Safe_Release(iter); 
    }
    m_pDSVs.clear();
}
