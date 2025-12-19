#include "Query.h"

#include "GameInstance.h"

CQuery::CQuery(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) :
	m_pDevice(pDevice),
	m_pContext(pContext),
	m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pGameInstance);
}

HRESULT CQuery::Initialize(const D3D11_QUERY_DESC& Desc)
{
	if(FAILED(m_pDevice->CreateQuery(&Desc, &m_pQuery)))
		return E_FAIL;

	return S_OK;
}

void CQuery::QueryBegin()
{
	m_pContext->Begin(m_pQuery);
}

void CQuery::QueryEnd()
{
	m_pContext->End(m_pQuery);
}

UINT64 CQuery::GetNumSamplePassed()
{
	UINT64  numSamplePassed = (numeric_limits<UINT64>::max)();
	while (m_pContext->GetData(m_pQuery, &numSamplePassed, sizeof(UINT64), 0) != S_OK);

	return numSamplePassed;
}

_bool CQuery::IsDataReady() const
{
	return true;
}

CQuery* CQuery::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const D3D11_QUERY_DESC& Desc)
{
	CQuery* pQuery = new CQuery(pDevice, pContext);
	if (FAILED(pQuery->Initialize(Desc)))
	{
		Safe_Release(pQuery);
		MSG_BOX("Create Fail : Query");
	}
	return pQuery;
}

void CQuery::Free()
{
	__super::Free();
	Safe_Release(m_pQuery);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pGameInstance);
}
