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

    return S_OK;
}

HRESULT COcclusion::OccluseionBegin()
{
    m_pOCclusionQuery->QueryBegin();
    return S_OK;
}

HRESULT COcclusion::OccluseionEnd()
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
}
