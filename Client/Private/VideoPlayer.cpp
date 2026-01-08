#include "pch.h"
#include "VideoPlayer.h"
#include "GameInstance.h"

#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#include <mfobjects.h>

#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "mfreadwrite.lib")
#pragma comment(lib, "mfuuid.lib")

static long g_MFRefCount = 0;

CVideoPlayer::CVideoPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CComponent(pDevice, pContext)
{
}

CVideoPlayer::CVideoPlayer(const CVideoPlayer& Prototype)
    : CComponent(Prototype)
{
}

HRESULT CVideoPlayer::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CVideoPlayer::Initialize(void* pArg)
{
    if (InterlockedIncrement(&g_MFRefCount) == 1)
    {
        if (FAILED(MFStartup(MF_VERSION)))
        {
            InterlockedDecrement(&g_MFRefCount);
            return E_FAIL;
        }
    }

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    return S_OK;
}

void CVideoPlayer::Update(_float fTimeDelta)
{
    if (!m_bPlaying)
        return;

    m_fTimeAcc += fTimeDelta;

    const _float frameTime = 1.f / max(m_fFPS, 1.f);
    if (m_fTimeAcc >= frameTime)
    {
        Read_VideoFrame();
        m_fTimeAcc -= frameTime;
    }
}

HRESULT CVideoPlayer::Render()
{
    return S_OK;
}

void CVideoPlayer::Play() { m_bPlaying = true; }
void CVideoPlayer::Pause() { m_bPlaying = false; }

void CVideoPlayer::Stop()
{
    m_bPlaying = false;
    m_fTimeAcc = 0.f;
    if (m_pReader) SeekToStart();
}

HRESULT CVideoPlayer::Set_Source(const _tchar* szPath)
{
    Stop();
    Release_Source();

    HRESULT hr = Create_SourceReader(szPath);
    if (FAILED(hr))
        return hr;

    // 첫 프레임 준비
    Read_VideoFrame();
    return S_OK;
}

HRESULT CVideoPlayer::Create_SourceReader(const _tchar* szPath)
{
    Safe_Release(m_pReader);

    // SourceReader 생성 (가능한 변환/하드웨어 사용 허용)
    IMFAttributes* pAttr = nullptr;
    HRESULT hr = MFCreateAttributes(&pAttr, 2);
    if (FAILED(hr)) return hr;

    pAttr->SetUINT32(MF_READWRITE_ENABLE_HARDWARE_TRANSFORMS, TRUE);
    pAttr->SetUINT32(MF_SOURCE_READER_ENABLE_VIDEO_PROCESSING, TRUE);

    hr = MFCreateSourceReaderFromURL(szPath, pAttr, &m_pReader);
    Safe_Release(pAttr);
    if (FAILED(hr)) return hr;

    // 비디오 스트림만
    m_pReader->SetStreamSelection(MF_SOURCE_READER_ALL_STREAMS, FALSE);
    m_pReader->SetStreamSelection(MF_SOURCE_READER_FIRST_VIDEO_STREAM, TRUE);

    // NV12로 출력 요청 (이건 대부분 성공한다)
    IMFMediaType* pType = nullptr;
    hr = MFCreateMediaType(&pType);
    if (FAILED(hr)) return hr;

    pType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);
    pType->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_NV12);

    // 실패해도 GetCurrentMediaType로 확인할 것
    m_pReader->SetCurrentMediaType(MF_SOURCE_READER_FIRST_VIDEO_STREAM, nullptr, pType);
    Safe_Release(pType);

    IMFMediaType* pOutType = nullptr;
    hr = m_pReader->GetCurrentMediaType(MF_SOURCE_READER_FIRST_VIDEO_STREAM, &pOutType);
    if (FAILED(hr)) return hr;

    GUID subtype{};
    pOutType->GetGUID(MF_MT_SUBTYPE, &subtype);

    if (subtype != MFVideoFormat_NV12)
    {
        // 여기 걸리면 환경이 특이한 것. 일단 실패 처리.
        Safe_Release(pOutType);
        return E_FAIL;
    }

    _uint width = 0, height = 0;
    hr = MFGetAttributeSize(pOutType, MF_MT_FRAME_SIZE, &width, &height);
    if (FAILED(hr))
    {
        Safe_Release(pOutType);
        return hr;
    }

    // FPS
    UINT32 num = 0, den = 0;
    if (SUCCEEDED(MFGetAttributeRatio(pOutType, MF_MT_FRAME_RATE, &num, &den)) && den != 0)
        m_fFPS = (float)num / (float)den;
    else
        m_fFPS = 30.f;

    LONG stride = 0;
    hr = pOutType->GetUINT32(MF_MT_DEFAULT_STRIDE, (UINT32*)&stride);

    // GetUINT32가 실패할 수 있어서 보조로 MFGetStrideForBitmapInfoHeader도 쓸 수 있음
    if (FAILED(hr) || stride == 0)
    {
        // NV12의 기본 stride는 보통 width로 보지만, 실제는 더 클 수 있음
        // 그래도 최소 fallback은 width
        stride = (LONG)width;
    }

    m_iStrideY = (UINT)abs(stride);
    m_iStrideUV = m_iStrideY; // NV12는 보통 Y/UV stride 같음

    Safe_Release(pOutType);

    // NV12 텍스처 2장 생성/재생성
    if (!m_pTexY || !m_pTexUV || width != m_iWidth || height != m_iHeight)
    {
        Safe_Release(m_pSRV_Y);
        Safe_Release(m_pSRV_UV);
        Safe_Release(m_pTexY);
        Safe_Release(m_pTexUV);

        hr = Create_Textures(width, height);
        if (FAILED(hr)) return hr;
    }

    m_iWidth = width;
    m_iHeight = height;

    return S_OK;
}

HRESULT CVideoPlayer::Create_Textures(_uint width, _uint height)
{
    // Y plane: R8_UNORM, WxH
    {
        D3D11_TEXTURE2D_DESC desc{};
        desc.Width = width;
        desc.Height = height;
        desc.MipLevels = 1;
        desc.ArraySize = 1;
        desc.Format = DXGI_FORMAT_R8_UNORM;
        desc.SampleDesc.Count = 1;
        desc.Usage = D3D11_USAGE_DYNAMIC;
        desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

        HRESULT hr = m_pDevice->CreateTexture2D(&desc, nullptr, &m_pTexY);
        if (FAILED(hr)) return hr;

        hr = m_pDevice->CreateShaderResourceView(m_pTexY, nullptr, &m_pSRV_Y);
        if (FAILED(hr)) return hr;
    }

    // UV plane: R8G8_UNORM, (W/2)x(H/2)
    {
        D3D11_TEXTURE2D_DESC desc{};
        desc.Width = width / 2;
        desc.Height = height / 2;
        desc.MipLevels = 1;
        desc.ArraySize = 1;
        desc.Format = DXGI_FORMAT_R8G8_UNORM;
        desc.SampleDesc.Count = 1;
        desc.Usage = D3D11_USAGE_DYNAMIC;
        desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

        HRESULT hr = m_pDevice->CreateTexture2D(&desc, nullptr, &m_pTexUV);
        if (FAILED(hr)) return hr;

        hr = m_pDevice->CreateShaderResourceView(m_pTexUV, nullptr, &m_pSRV_UV);
        if (FAILED(hr)) return hr;
    }

    return S_OK;
}

//HRESULT CVideoPlayer::Read_VideoFrame()
//{
//    if (!m_pReader || !m_pTexY || !m_pTexUV)
//        return E_FAIL;
//
//    IMFSample* pSample = nullptr;
//    IMFMediaBuffer* pBuffer = nullptr;
//    DWORD dwFlags = 0;
//
//    HRESULT hr = m_pReader->ReadSample(
//        MF_SOURCE_READER_FIRST_VIDEO_STREAM,
//        0,
//        nullptr,
//        &dwFlags,
//        nullptr,
//        &pSample
//    );
//
//    if (dwFlags & MF_SOURCE_READERF_ENDOFSTREAM)
//    {
//        Safe_Release(pSample);
//
//        if (m_bLoop)
//        {
//            SeekToStart();
//            return S_FALSE;
//        }
//
//        m_bPlaying = false;
//        return S_FALSE;
//    }
//
//    if (FAILED(hr) || !pSample)
//    {
//        Safe_Release(pSample);
//        return FAILED(hr) ? hr : S_FALSE;
//    }
//
//    hr = pSample->ConvertToContiguousBuffer(&pBuffer);
//    if (FAILED(hr) || !pBuffer)
//    {
//        Safe_Release(pBuffer);
//        Safe_Release(pSample);
//        return FAILED(hr) ? hr : E_FAIL;
//    }
//
//    BYTE* pData = nullptr;
//    DWORD maxLen = 0, curLen = 0;
//    hr = pBuffer->Lock(&pData, &maxLen, &curLen);
//    if (FAILED(hr) || !pData || curLen == 0)
//    {
//        if (pBuffer) pBuffer->Unlock();
//        Safe_Release(pBuffer);
//        Safe_Release(pSample);
//        return FAILED(hr) ? hr : E_FAIL;
//    }
//
//    // NV12 expected size = W*H + W*H/2
//    //const _uint ySize = m_iWidth * m_iHeight;
//    //const _uint uvSize = (m_iWidth * m_iHeight) / 2;
//
//    const _uint srcStride = (m_iStrideY != 0) ? m_iStrideY : m_iWidth;
//    const _uint ySize = srcStride * m_iHeight;
//
//    const _uint srcStrideUV = (m_iStrideUV != 0) ? m_iStrideUV : m_iWidth;
//    const _uint uvSize = srcStrideUV * (m_iHeight / 2);
//
//    if (curLen < ySize + uvSize)
//    {
//        pBuffer->Unlock();
//        Safe_Release(pBuffer);
//        Safe_Release(pSample);
//        return E_FAIL;
//    }
//
//    BYTE* pY = pData;
//    BYTE* pUV = pData + ySize;
//
//    // ---- Update Y texture ----
//    {
//        D3D11_MAPPED_SUBRESOURCE mapped{};
//        hr = m_pContext->Map(m_pTexY, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
//        if (FAILED(hr) || !mapped.pData)
//        {
//            pBuffer->Unlock();
//            Safe_Release(pBuffer);
//            Safe_Release(pSample);
//            return FAILED(hr) ? hr : E_FAIL;
//        }
//
//        //const _uint srcPitch = (m_iStrideY != 0) ? m_iStrideY : m_iWidth;;          // 1 byte per pixel
//        //const _uint dstPitch = mapped.RowPitch;
//
//        //BYTE* src = pY;
//        //BYTE* dst = reinterpret_cast<BYTE*>(mapped.pData);
//
//        //for (_uint row = 0; row < m_iHeight; ++row)
//        //{
//        //    memcpy(dst, src, srcPitch);
//        //    src += srcPitch;
//        //    dst += dstPitch;
//        //}
//
//        const _uint srcStride = (m_iStrideY != 0) ? m_iStrideY : m_iWidth;
//        const _uint dstPitch = mapped.RowPitch;
//
//        BYTE* src = pY;
//        BYTE* dst = (BYTE*)mapped.pData;
//
//        for (_uint row = 0; row < m_iHeight; ++row)
//        {
//            memcpy(dst, src, m_iWidth);   // 복사 크기는 width
//            src += srcStride;             // stride로 이동
//            dst += dstPitch;
//        }
//
//        m_pContext->Unmap(m_pTexY, 0);
//    }
//
//    // ---- Update UV texture ----
//    {
//        D3D11_MAPPED_SUBRESOURCE mapped{};
//        hr = m_pContext->Map(m_pTexUV, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
//        if (FAILED(hr) || !mapped.pData)
//        {
//            pBuffer->Unlock();
//            Safe_Release(pBuffer);
//            Safe_Release(pSample);
//            return FAILED(hr) ? hr : E_FAIL;
//        }
//
//        // UV plane: width bytes per row (U,V interleaved => 2 bytes per 2 pixels)
//        /*const _uint srcPitch = (m_iStrideUV != 0) ? m_iStrideUV : m_iWidth;
//        const _uint dstPitch = mapped.RowPitch;
//
//        const _uint uvH = m_iHeight / 2;
//
//        BYTE* src = pUV;
//        BYTE* dst = reinterpret_cast<BYTE*>(mapped.pData);
//
//        for (_uint row = 0; row < uvH; ++row)
//        {
//            memcpy(dst, src, srcPitch);
//            src += srcPitch;
//            dst += dstPitch;
//        }*/
//
//        const _uint srcStride = (m_iStrideUV != 0) ? m_iStrideUV : m_iWidth;
//        const _uint dstPitch = mapped.RowPitch;
//        const _uint uvH = m_iHeight / 2;
//
//        BYTE* src = pUV;
//        BYTE* dst = (BYTE*)mapped.pData;
//
//        for (_uint row = 0; row < uvH; ++row)
//        {
//            memcpy(dst, src, m_iWidth);   // UV도 width 바이트
//            src += srcStride;             // stride
//            dst += dstPitch;
//        }
//
//
//        m_pContext->Unmap(m_pTexUV, 0);
//    }
//
//    pBuffer->Unlock();
//    Safe_Release(pBuffer);
//    Safe_Release(pSample);
//
//    return S_OK;
//}

HRESULT CVideoPlayer::Read_VideoFrame()
{
    if (!m_pReader || !m_pTexY || !m_pTexUV)
        return E_FAIL;

    IMFSample* pSample = nullptr;
    IMFMediaBuffer* pBuffer = nullptr;
    DWORD dwFlags = 0;

    HRESULT hr = m_pReader->ReadSample(
        MF_SOURCE_READER_FIRST_VIDEO_STREAM,
        0, nullptr, &dwFlags, nullptr, &pSample);

    if (dwFlags & MF_SOURCE_READERF_ENDOFSTREAM)
    {
        Safe_Release(pSample);
        if (m_bLoop) { SeekToStart(); return S_FALSE; }
        m_bPlaying = false;
        return S_FALSE;
    }

    if (FAILED(hr) || !pSample)
    {
        Safe_Release(pSample);
        return FAILED(hr) ? hr : S_FALSE;
    }

    hr = pSample->ConvertToContiguousBuffer(&pBuffer);
    if (FAILED(hr) || !pBuffer)
    {
        Safe_Release(pBuffer);
        Safe_Release(pSample);
        return FAILED(hr) ? hr : E_FAIL;
    }

    // 여기부터가 핵심: IMF2DBuffer로 Lock2D
    IMF2DBuffer* p2D = nullptr;
    hr = pBuffer->QueryInterface(IID_PPV_ARGS(&p2D));
    if (FAILED(hr) || !p2D)
    {
        // 2D 버퍼가 아니면 기존 Lock 방식 fallback 가능(하지만 너 케이스는 2D로 가야 해결됨)
        Safe_Release(pBuffer);
        Safe_Release(pSample);
        return E_FAIL;
    }

    BYTE* pY = nullptr;
    LONG strideY = 0;
    hr = p2D->Lock2D(&pY, &strideY);
    if (FAILED(hr) || !pY)
    {
        Safe_Release(p2D);
        Safe_Release(pBuffer);
        Safe_Release(pSample);
        return E_FAIL;
    }

    // NV12: UV plane은 Y plane 바로 뒤에 "height * strideY" 만큼 떨어진 곳
    // 이 계산은 Lock2D로 얻은 strideY 기준이므로 신뢰 가능
    BYTE* pUV = pY + (size_t)abs(strideY) * (size_t)m_iHeight;
    LONG strideUV = strideY; // 대부분 동일

    // ---- Update Y texture ----
    {
        D3D11_MAPPED_SUBRESOURCE mapped{};
        hr = m_pContext->Map(m_pTexY, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
        if (FAILED(hr) || !mapped.pData)
        {
            p2D->Unlock2D();
            Safe_Release(p2D);
            Safe_Release(pBuffer);
            Safe_Release(pSample);
            return E_FAIL;
        }

        BYTE* src = pY;
        BYTE* dst = (BYTE*)mapped.pData;

        const _uint srcStride = (_uint)abs(strideY);
        const _uint dstPitch = mapped.RowPitch;

        for (UINT row = 0; row < m_iHeight; ++row)
        {
            memcpy(dst, src, m_iWidth);   // width만 복사
            src += srcStride;
            dst += dstPitch;
        }

        m_pContext->Unmap(m_pTexY, 0);
    }

    // ---- Update UV texture ----
    {
        D3D11_MAPPED_SUBRESOURCE mapped{};
        hr = m_pContext->Map(m_pTexUV, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
        if (FAILED(hr) || !mapped.pData)
        {
            p2D->Unlock2D();
            Safe_Release(p2D);
            Safe_Release(pBuffer);
            Safe_Release(pSample);
            return E_FAIL;
        }

        BYTE* src = pUV;
        BYTE* dst = (BYTE*)mapped.pData;

        const UINT srcStride = (UINT)abs(strideUV);
        const UINT dstPitch = mapped.RowPitch;
        const UINT uvH = m_iHeight / 2;

        for (UINT row = 0; row < uvH; ++row)
        {
            memcpy(dst, src, m_iWidth);   // UV도 width 바이트
            src += srcStride;
            dst += dstPitch;
        }

        m_pContext->Unmap(m_pTexUV, 0);
    }

    p2D->Unlock2D();
    Safe_Release(p2D);
    Safe_Release(pBuffer);
    Safe_Release(pSample);

    return S_OK;
}


HRESULT CVideoPlayer::SeekToStart()
{
    if (!m_pReader)
        return E_FAIL;

    PROPVARIANT var;
    PropVariantInit(&var);
    var.vt = VT_I8;
    var.hVal.QuadPart = 0;

    HRESULT hr = m_pReader->SetCurrentPosition(GUID_NULL, var);
    PropVariantClear(&var);
    return hr;
}

void CVideoPlayer::Release_Source()
{
    Safe_Release(m_pReader);

    m_fTimeAcc = 0.f;
    m_bPlaying = false;
}

CVideoPlayer* CVideoPlayer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CVideoPlayer* pInstance = new CVideoPlayer(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        Safe_Release(pInstance);
        MSG_BOX("Create Fail : VideoPlayer");
    }
    return pInstance;
}

CComponent* CVideoPlayer::Clone(void* pArg)
{
    CVideoPlayer* pInstance = new CVideoPlayer(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        Safe_Release(pInstance);
        MSG_BOX("Clone Fail : VideoPlayer");
    }
    return pInstance;
}

void CVideoPlayer::Free()
{
    __super::Free();

    Release_Source();

    Safe_Release(m_pSRV_Y);
    Safe_Release(m_pSRV_UV);
    Safe_Release(m_pTexY);
    Safe_Release(m_pTexUV);

    if (InterlockedDecrement(&g_MFRefCount) == 0)
        MFShutdown();
}
