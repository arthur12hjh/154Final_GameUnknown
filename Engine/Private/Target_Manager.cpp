#include "Target_Manager.h"
#include "RenderTarget.h"

CTarget_Manager::CTarget_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : m_pDevice { pDevice }
    , m_pContext { pContext }
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);

}

HRESULT CTarget_Manager::Add_RenderTarget(const _wstring& strTargetTag, _uint iSizeX, _uint iSizeY, DXGI_FORMAT ePixelFormat, const _float4& vClearColor)
{
    if (nullptr != Find_RenderTarget(strTargetTag))
        return E_FAIL;

    CRenderTarget* pRenderTarget = CRenderTarget::Create(m_pDevice, m_pContext, iSizeX, iSizeY, ePixelFormat, vClearColor);
    if (nullptr == pRenderTarget)
        return E_FAIL;

    m_RenderTargets.emplace(strTargetTag, pRenderTarget);

    return S_OK;
}

HRESULT CTarget_Manager::Add_MRT(const _wstring& strMRTTag, const _wstring& strTargetTag)
{
    CRenderTarget* pRenderTarget = Find_RenderTarget(strTargetTag);
    if (nullptr == pRenderTarget)
        return E_FAIL;

    list<CRenderTarget*>*       pMRTList = Find_MRT(strMRTTag);
    if (nullptr == pMRTList)
    {
        list<CRenderTarget*>       MRTList;
        MRTList.push_back(pRenderTarget);
        m_MRTs.emplace(strMRTTag, MRTList);
    }
    else
        pMRTList->push_back(pRenderTarget);

    Safe_AddRef(pRenderTarget);

    return S_OK;
}

/* 내가 원하는 그룹에 속해있는 렌더타겟들을 모두 다 장치에 바인딩한다. */
HRESULT CTarget_Manager::Begin_MRT(const _wstring& strMRTTag, ID3D11DepthStencilView* pDSV)
{
    m_pContext->OMGetRenderTargets(1, &m_pBackBufferRTV, &m_pOriginalDSV);

    list<CRenderTarget*>* pMRTList = Find_MRT(strMRTTag);

    if (nullptr == pMRTList)
        return E_FAIL;

    _uint iNumRenderTargets = {};
    ID3D11RenderTargetView* pRenderTargets[8] = {};

    for (auto& pRenderTarget : *pMRTList)
    {
        pRenderTarget->Clear();
        pRenderTargets[iNumRenderTargets++] = pRenderTarget->Get_RTV();
    }

    
    if (nullptr != pDSV)
        m_pContext->ClearDepthStencilView(pDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
    

    m_pContext->OMSetRenderTargets(iNumRenderTargets, pRenderTargets, nullptr == pDSV ? m_pOriginalDSV : pDSV);

    return S_OK;
}

HRESULT CTarget_Manager::Load_MRT(const _wstring& strMRTTag, ID3D11DepthStencilView* pDSV)
{
    m_pContext->OMGetRenderTargets(1, &m_pBackBufferRTV, &m_pOriginalDSV);

    list<CRenderTarget*>* pMRTList = Find_MRT(strMRTTag);

    if (nullptr == pMRTList)
        return E_FAIL;

    _uint iNumRenderTargets = {};
    ID3D11RenderTargetView* pRenderTargets[8] = {};

    for (auto& pRenderTarget : *pMRTList)
    {
        pRenderTargets[iNumRenderTargets++] = pRenderTarget->Get_RTV();
    }


    if (nullptr != pDSV)
        m_pContext->ClearDepthStencilView(pDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);


    m_pContext->OMSetRenderTargets(iNumRenderTargets, pRenderTargets, nullptr == pDSV ? m_pOriginalDSV : pDSV);

    return S_OK;
}

/* 백버퍼가 바인딩된 상태로 돌려주낟. */
HRESULT CTarget_Manager::End_MRT()
{
    ID3D11RenderTargetView* pRenderTargets[8] = { m_pBackBufferRTV };

    m_pContext->OMSetRenderTargets(8, pRenderTargets, m_pOriginalDSV);

    Safe_Release(m_pBackBufferRTV);
    Safe_Release(m_pOriginalDSV);

    return S_OK;
}

HRESULT CTarget_Manager::Copy_RenderTarget(const _wstring& strTargetTag, ID3D11Texture2D* pTexture2D)
{
    CRenderTarget* pRenderTarget = Find_RenderTarget(strTargetTag);
    if (nullptr == pRenderTarget)
        return E_FAIL;

    pRenderTarget->Copy_Resource(pTexture2D);

    return S_OK;
}

HRESULT CTarget_Manager::Bind_RenderTarget(const _wstring& strTargetTag, CShader* pShader, const _char* pConstantName)
{
    CRenderTarget* pRenderTarget = Find_RenderTarget(strTargetTag);
    if (nullptr == pRenderTarget)
        return E_FAIL;

    return pRenderTarget->Bind_ShaderResource(pShader, pConstantName);    
}

HRESULT CTarget_Manager::Clear_MRT(const _wstring& strMRTTag)
{
    _uint iNumRenderTargets = {};

    list<CRenderTarget*>* pMRTList = Find_MRT(strMRTTag);

    for (auto& pRenderTarget : *pMRTList)
    {
        pRenderTarget->Clear();
    }

    return S_OK;
}

#ifdef _DEBUG

HRESULT CTarget_Manager::Ready_Debug(const _wstring& strTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY)
{
    CRenderTarget* pRenderTarget = Find_RenderTarget(strTargetTag);
    if (nullptr == pRenderTarget)
        return E_FAIL;
    return pRenderTarget->Ready_Debug(fX, fY, fSizeX, fSizeY);
}

HRESULT CTarget_Manager::Render_Debug(const _wstring& strMRTTag, CShader* pShader, CVIBuffer_Rect* pVIBuffer)
{
    list<CRenderTarget*>*       pMRTList = Find_MRT(strMRTTag);

    if (nullptr == pMRTList)
        return E_FAIL;

    for (auto& pRenderTarget : *pMRTList)
    {
        pRenderTarget->Render_Debug(pShader, pVIBuffer);
    }

    return S_OK;
}

#endif

CRenderTarget* CTarget_Manager::Find_RenderTarget(const _wstring& strTargetTag)
{
    auto    iter = m_RenderTargets.find(strTargetTag);

    if(iter == m_RenderTargets.end())    
        return nullptr;

    return iter->second;
}

list<class CRenderTarget*>* CTarget_Manager::Find_MRT(const _wstring& strMRTTag)
{
    auto    iter = m_MRTs.find(strMRTTag);

    if (iter == m_MRTs.end())
        return nullptr;

    return &iter->second;
}

CTarget_Manager* CTarget_Manager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    return new CTarget_Manager(pDevice, pContext);
}

void CTarget_Manager::Free()
{
    __super::Free();

    for (auto& Pair : m_MRTs)
    {
        for (auto& pRenderTarget : Pair.second)
            Safe_Release(pRenderTarget);
        Pair.second.clear();
    }
    m_MRTs.clear();

    for (auto& Pair : m_RenderTargets)    
        Safe_Release(Pair.second);    

    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
}
