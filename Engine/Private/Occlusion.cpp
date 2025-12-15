#include "Occlusion.h"

#include "Query.h"

COcclusion::COcclusion(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) :
    m_pDevice(pDevice),
    m_pContext(pContext)
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
}

HRESULT COcclusion::Initialize()
{
   /* m_pOCclusionQuery = CQuery::Create(m_pDevice, m_pContext, Desc);
    if (nullptr == m_pOCclusionQuery)
        return E_FAIL;*/

    D3D11_QUERY_DESC Desc{};
    Desc.Query = D3D11_QUERY_OCCLUSION;
    Desc.MiscFlags = 0;

    if (FAILED(m_pDevice->CreateQuery(&Desc, &m_pQuery)))
        return E_FAIL;

    return S_OK;
}

HRESULT COcclusion::OcclusionBegin()
{
    m_pOCclusionQuery->QueryBegin();
    return S_OK;
}

HRESULT COcclusion::OcclusionEnd()
{
    m_pOCclusionQuery->QueryEnd();
    return S_OK;
}

UINT64 COcclusion::GetNumSamplePassed()
{
    return m_pOCclusionQuery->GetNumSamplePassed();
}

_bool COcclusion::IsDataReady() const
{
    return m_pOCclusionQuery->IsDataReady();
}

void COcclusion::Begin_Query()
{
    if (nullptr == m_pQuery)
        return;

    m_pContext->Begin(m_pQuery);
}

void COcclusion::End_Query()
{
    if (nullptr == m_pQuery)
        return;

    m_pContext->End(m_pQuery);
}

HRESULT COcclusion::Get_Result(_bool* pIsVisible)
{
    if (nullptr == m_pQuery)
        return E_FAIL;
    
    UINT64 iNumSamples = 0;
    HRESULT hr = m_pContext->GetData(m_pQuery, &iNumSamples, sizeof(iNumSamples), D3D11_ASYNC_GETDATA_DONOTFLUSH);

    if (S_FALSE == hr)
        return S_FALSE;

    if (S_OK == hr)
    {
        *pIsVisible = (iNumSamples > 0);
        return S_OK;
    }

    return E_FAIL;
}

COcclusion* COcclusion::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    COcclusion* pOcclusion = new COcclusion(pDevice, pContext);
    if (FAILED(pOcclusion->Initialize()))
    {
        Safe_Release(pOcclusion);
        MSG_BOX("Create Fail : Occlusion");
    }
    return pOcclusion;
}

void COcclusion::Free()
{
    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
    Safe_Release(m_pQuery);
    Safe_Release(m_pOCclusionQuery);
}
