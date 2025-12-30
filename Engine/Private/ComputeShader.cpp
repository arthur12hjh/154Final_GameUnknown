#include "ComputeShader.h"

#include "GameInstance.h"

CComputeShader::CComputeShader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) :
    CComponent(pDevice, pContext)
{
}

CComputeShader::CComputeShader(const CComputeShader& Prototype) :
    CComponent(Prototype),
    m_pComputeShaderCom(Prototype.m_pComputeShaderCom),
    m_iNumData(Prototype.m_iNumData)
{
    Safe_AddRef(m_pComputeShaderCom);
}

HRESULT CComputeShader::Initialize_Prototype(const WCHAR* szShaderFilePath, const char* szStartFunctionName, _uint iNumData)
{
    m_iNumData = iNumData;
    if (FAILED(LoadShader(szShaderFilePath, szStartFunctionName)))
        return E_FAIL;

    return S_OK;
}

HRESULT CComputeShader::Initialize(void* pArg)
{
    return S_OK;
}

HRESULT CComputeShader::Render()
{
    

    return S_OK;
}

void CComputeShader::Update_Shader(_float3 iGroupCount)
{
    m_pContext->CSSetShader(m_pComputeShaderCom, nullptr, 0);
    m_pContext->Dispatch(iGroupCount.x, iGroupCount.y, iGroupCount.z);
}

void CComputeShader::SetInputData(_uint iIndex, void* pData)
{
    if (0 > iIndex || m_pInputBuffer.size() <= iIndex)
        return;

    m_pContext->UpdateSubresource(m_pInputBuffer[iIndex], 0, nullptr, pData, 0, 0);
}

void CComputeShader::SetConstantData(_uint iIndex, void* pData)
{
    if (0 > iIndex || m_pCBuffer.size() <= iIndex)
        return;

    m_pContext->UpdateSubresource(m_pCBuffer[iIndex], 0, nullptr, pData, 0, 0);
}

void CComputeShader::GetBufferResource(BUFFER_TYPE eBufferType, _uint iBufferIndex, ID3D11Buffer* pOutBuffer)
{
    switch (eBufferType)
    {
    case CComputeShader::BUFFER_TYPE::INPUT:
    {
        if (0 > iBufferIndex || m_pInputBuffer.size() <= iBufferIndex)
            return;

        m_pContext->CopyResource(pOutBuffer, m_pInputBuffer[iBufferIndex]);
    }
    break;

    case CComputeShader::BUFFER_TYPE::OUTPUT:
    {
        if (0 > iBufferIndex || m_pOutputBuffer.size() <= iBufferIndex)
            return;

        m_pContext->CopyResource(pOutBuffer, m_pOutputBuffer[iBufferIndex]);
    }
    break;
    case CComputeShader::BUFFER_TYPE::CONSTATNT:
    {
        if (0 > iBufferIndex || m_pCBuffer.size() <= iBufferIndex)
            return;

        m_pContext->CopyResource(pOutBuffer, m_pCBuffer[iBufferIndex]);
    }
    break;
    }

    return;
}

HRESULT CComputeShader::ADD_Buffer(BUFFER_TYPE eBufferType, ID3D11Buffer* pBuffer, _uint iNumData)
{
    if (nullptr == pBuffer)
        return E_FAIL;

    if (0 == iNumData)
        iNumData = m_iNumData;

    switch (eBufferType)
    {
    case CComputeShader::BUFFER_TYPE::INPUT:
    {
        ID3D11ShaderResourceView* pSRV = nullptr;
        D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
        SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
        SRVDesc.Format = DXGI_FORMAT_UNKNOWN;
        SRVDesc.Buffer.NumElements = iNumData;

        if (FAILED(m_pDevice->CreateShaderResourceView(pBuffer, &SRVDesc, &pSRV)))
            return E_FAIL;

        m_pInputBuffer.push_back(pBuffer);
        m_pSRVs.push_back(pSRV);
    }
        break;

    case CComputeShader::BUFFER_TYPE::OUTPUT:
    {
        ID3D11UnorderedAccessView* pUAV = nullptr;
        D3D11_UNORDERED_ACCESS_VIEW_DESC UAVDesc = {};
        UAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
        UAVDesc.Format = DXGI_FORMAT_UNKNOWN;
        UAVDesc.Buffer.NumElements = iNumData;

        if (FAILED(m_pDevice->CreateUnorderedAccessView(pBuffer, &UAVDesc, &pUAV)))
            return E_FAIL;

        m_pOutputBuffer.push_back(pBuffer);
        m_pUAVs.push_back(pUAV);
    }
        break;
    case CComputeShader::BUFFER_TYPE::CONSTATNT:
    {
        D3D11_BUFFER_DESC ContantDesc = {};
        pBuffer->GetDesc(&ContantDesc);

        if (0 != ContantDesc.ByteWidth % 16)
        {
            MSG_BOX("Constant Buffer Not Match 16 sqrt");
            return E_FAIL;
        }

        m_pCBuffer.push_back(pBuffer);
    }
        break;
    }
    return S_OK;
}

HRESULT CComputeShader::ADD_AppendOutBuffer(ID3D11Buffer* pBuffer)
{
    ID3D11UnorderedAccessView* pUAV = nullptr;
    D3D11_UNORDERED_ACCESS_VIEW_DESC UAVDesc = {};
    UAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
    UAVDesc.Format = DXGI_FORMAT_UNKNOWN;
    UAVDesc.Buffer.NumElements = m_iNumData;
    UAVDesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_APPEND;

    if (FAILED(m_pDevice->CreateUnorderedAccessView(pBuffer, &UAVDesc, &pUAV)))
        return E_FAIL;

    m_pOutputBuffer.push_back(pBuffer);
    m_pUAVs.push_back(pUAV);
    return S_OK;
}

HRESULT CComputeShader::Update_BufferResource(BUFFER_TYPE eBufferType, _uint iBufferIndex, void* pData)
{
    switch (eBufferType)
    {
    case CComputeShader::BUFFER_TYPE::INPUT:
    {
        if (0 > iBufferIndex || m_pInputBuffer.size() <= iBufferIndex)
            return E_FAIL;

        m_pContext->UpdateSubresource(m_pInputBuffer[iBufferIndex], 0, nullptr, pData, 0, 0);
    }
    break;

    case CComputeShader::BUFFER_TYPE::OUTPUT:
    {
        if (0 > iBufferIndex || m_pOutputBuffer.size() <= iBufferIndex)
            return E_FAIL;

        m_pContext->UpdateSubresource(m_pOutputBuffer[iBufferIndex], 0, nullptr, pData, 0, 0);
    }
    break;
    case CComputeShader::BUFFER_TYPE::CONSTATNT:
    {
        if (0 > iBufferIndex || m_pCBuffer.size() <= iBufferIndex)
            return E_FAIL;

        m_pContext->UpdateSubresource(m_pCBuffer[iBufferIndex], 0, nullptr, pData, 0, 0);
    }
    break;
    }

    return S_OK;
}

HRESULT CComputeShader::Update_BufferResource(BUFFER_TYPE eBufferType, _uint iBufferIndex, ID3D11Buffer* pBuffer)
{
    switch (eBufferType)
    {
    case CComputeShader::BUFFER_TYPE::INPUT:
    {
        if (0 > iBufferIndex || m_pInputBuffer.size() <= iBufferIndex)
            return E_FAIL;

        m_pContext->CopyResource(m_pInputBuffer[iBufferIndex], pBuffer);
    }
    break;

    case CComputeShader::BUFFER_TYPE::OUTPUT:
    {
        if (0 > iBufferIndex || m_pOutputBuffer.size() <= iBufferIndex)
            return E_FAIL;

        m_pContext->CopyResource(m_pOutputBuffer[iBufferIndex], pBuffer);
    }
    break;
    case CComputeShader::BUFFER_TYPE::CONSTATNT:
    {
        if (0 > iBufferIndex || m_pCBuffer.size() <= iBufferIndex)
            return E_FAIL;

        m_pContext->CopyResource(m_pCBuffer[iBufferIndex], pBuffer);
    }
    break;
    }

    return S_OK;
}

HRESULT CComputeShader::Bind_ConstBuffer(_uint iCBNum, _uint* iCBIndex)
{
    if (0 > iCBNum || 8 <= iCBNum)
        return E_FAIL;

    ID3D11Buffer* BindCBs[8] = { nullptr, };
    for (_uint i = 0; i < iCBNum; ++i)
    {
        if (0 > iCBIndex[i] || m_pCBuffer.size() <= iCBIndex[i])
            return E_FAIL;

        BindCBs[i] = m_pCBuffer[iCBIndex[i]];
    }
    m_pContext->CSSetConstantBuffers(0, iCBNum, BindCBs);
    return S_OK;
}

HRESULT CComputeShader::Bind_ConstBuffer_Slot(_uint iSlot, ID3D11Buffer* pCB)
{
    if (nullptr == pCB) 
        return E_FAIL;
    
    m_pContext->CSSetConstantBuffers(iSlot, 1, &pCB);

    return S_OK;
}

HRESULT CComputeShader::Bind_InputBuffer(_uint iBufferNum, _uint* iInputIndex)
{
    if (0 > iBufferNum || 8 <= iBufferNum)
        return E_FAIL;

    ID3D11ShaderResourceView* BindSRVs[8] = { nullptr, };
    for (_uint i = 0; i < iBufferNum; ++i)
    {
        if (0 > iInputIndex[i] || m_pSRVs.size() <= iInputIndex[i])
            return E_FAIL;

        BindSRVs[i] = m_pSRVs[iInputIndex[i]];
    }
    m_pContext->CSSetShaderResources(0, iBufferNum, BindSRVs);
    return S_OK;
}

HRESULT CComputeShader::Bind_OutputBuffer(_uint iBufferNum, _uint* iOutputIndex)
{
    if (0 > iBufferNum || 8 <= iBufferNum)
        return E_FAIL;

    ID3D11UnorderedAccessView* BindUAVs[8] = { nullptr, };
    for (_uint i = 0; i < iBufferNum; ++i)
    {
        if (0 > iOutputIndex[i] || m_pUAVs.size() <= iOutputIndex[i])
            return E_FAIL;

        BindUAVs[i] = m_pUAVs[iOutputIndex[i]];
    }
    m_pContext->CSSetUnorderedAccessViews(0, iBufferNum, BindUAVs, nullptr);
    return S_OK;
}

HRESULT CComputeShader::Bind_UAV(ID3D11UnorderedAccessView** pUAV)
{
    m_pContext->CSSetUnorderedAccessViews(0, 1, pUAV, nullptr);

    return S_OK;
}

HRESULT CComputeShader::Unbind_UAV()
{
    ID3D11UnorderedAccessView* pUAV[1] = { nullptr };
    m_pContext->CSSetUnorderedAccessViews(0, 1, pUAV, nullptr);
    m_pContext->CSSetShader(nullptr, nullptr, 0);

    return S_OK;
}

HRESULT CComputeShader::Bind_SRV(ID3D11ShaderResourceView** pSRV)
{
    m_pContext->CSSetShaderResources(0, 1, pSRV);

    return S_OK;
}

HRESULT CComputeShader::Unbind_SRV()
{
    ID3D11UnorderedAccessView* pSRV[1] = { nullptr };
    m_pContext->CSSetUnorderedAccessViews(0, 1, pSRV, nullptr);
    m_pContext->CSSetShader(nullptr, nullptr, 0);

    return S_OK;
}

HRESULT CComputeShader::Bind_Sampler(UINT iSlot, ID3D11SamplerState* pSamp)
{
    m_pContext->CSSetSamplers(iSlot, 1, &pSamp);
    
    return S_OK;
}

HRESULT CComputeShader::Reset()
{
    for (auto& pInputBuffer : m_pInputBuffer)
        Safe_Release(pInputBuffer);
    m_pInputBuffer.clear();

    for (auto& pOutputBuffer : m_pOutputBuffer)
        Safe_Release(pOutputBuffer);
    m_pOutputBuffer.clear();

    for (auto& pCBBuffer : m_pCBuffer)
        Safe_Release(pCBBuffer);
    m_pCBuffer.clear();

    for (auto& pSRV : m_pSRVs)
        Safe_Release(pSRV);
    m_pSRVs.clear();

    for (auto& pUAV : m_pUAVs)
        Safe_Release(pUAV);
    m_pUAVs.clear();
    return S_OK;
}


HRESULT		CComputeShader::LoadShader(const WCHAR* szShaderFilePath, const char* szStartFunName)
{
    ID3DBlob* pBlobVS = nullptr;
    ID3DBlob* pBlobCode = nullptr;

    // Debug 모드와 release 모드의 플레그를 다르게 줘야함
    UINT HLSLFlags = {};
#ifdef _DEBUG
    HLSLFlags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
    HLSLFlags = D3DCOMPILE_OPTIMIZATION_LEVEL1;
#endif // _DEBUG

    if (FAILED(D3DCompileFromFile(
        szShaderFilePath, // HLSL 파일 명
        nullptr,											 // 매크로 정의 배열 포인터
        D3D_COMPILE_STANDARD_FILE_INCLUDE,					 // 셰이더에 #inlcude 가 있을경우 설정
        szStartFunName,
        "cs_5_0",
        HLSLFlags,
        0,
        &pBlobCode,
        &pBlobVS)))		// 컴파일 중에 발생한 오류를 기록하는 포인터 
    {
        if (pBlobVS) {
            OutputDebugStringA((char*)pBlobVS->GetBufferPointer());
            pBlobVS->Release();
        }
        return E_FAIL;
    }

    if (FAILED(m_pDevice->CreateComputeShader(
        pBlobCode->GetBufferPointer(),          // 컴파일된 쉐이더 코드
        pBlobCode->GetBufferSize(),             // 코드 길이
        nullptr,                                // 클래스 인스턴스 (없음)
        &m_pComputeShaderCom)))                 // 컴퓨트 쉐이더 객체
        return E_FAIL;

    return S_OK;
}

CComputeShader* CComputeShader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const WCHAR* szShaderFilePath, const char* szStartFunctionName, _uint iNumData)
{
    CComputeShader* pComputeShader = new CComputeShader(pDevice, pContext);
    if (FAILED(pComputeShader->Initialize_Prototype(szShaderFilePath, szStartFunctionName, iNumData)))
    {
        Safe_Release(pComputeShader);
        MSG_BOX("Create Fail : Compute Shader");
    }
    return pComputeShader;
}

CComponent* CComputeShader::Clone(void* pArg)
{
    CComponent* pComputeShader = new CComputeShader(*this);
    if (FAILED(pComputeShader->Initialize(pArg)))
    {
        Safe_Release(pComputeShader);
        MSG_BOX("Create Fail : Compute Shader");
    }
    return pComputeShader;
}

void CComputeShader::Free()
{
    __super::Free();

    for (auto& pInputBuffer : m_pInputBuffer)
        Safe_Release(pInputBuffer);
    m_pInputBuffer.clear();

    for (auto& pOutputBuffer : m_pOutputBuffer)
        Safe_Release(pOutputBuffer);
    m_pOutputBuffer.clear();

    for (auto& pCBBuffer : m_pCBuffer)
        Safe_Release(pCBBuffer);
    m_pCBuffer.clear();

    for (auto& pSRV : m_pSRVs)
        Safe_Release(pSRV);
    m_pSRVs.clear();

    for (auto& pUAV : m_pUAVs)
        Safe_Release(pUAV);
    m_pUAVs.clear();

    Safe_Release(m_pComputeShaderCom);
}
