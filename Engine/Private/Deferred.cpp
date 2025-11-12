#include "Deferred.h"

#include "GameInstance.h"

CDeferred::CDeferred(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pGameInstance { CGameInstance::GetInstance() }
	, m_pDevice  { pDevice }
	, m_pContext { pContext }
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

void CDeferred::Free()
{
	__super::Free();

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
