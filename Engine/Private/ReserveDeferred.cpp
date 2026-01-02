#include "ReserveDeferred.h"

#include "GameInstance.h"
#include "Shader.h"
#include "VIBuffer.h"

CReserveDeferred::CReserveDeferred(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice { pDevice}
	, m_pContext{ pContext }
	, m_pGameInstance { CGameInstance::GetInstance() }
{
	Safe_AddRef(pDevice);
	Safe_AddRef(pContext);
	Safe_AddRef(m_pGameInstance);
}

void CReserveDeferred::Render(class CVIBuffer* pVIBuffer)
{
	//활성화 되어있지 않다면 렌더링하지 않는다
	if (false == m_isActive)
		return;

	m_pShaderCom->Begin(m_iShaderPassIdx);
	pVIBuffer->Bind_Resources();
	pVIBuffer->Render();
}

void CReserveDeferred::Set_Active(_bool bFlag)
{
	if (!bFlag) {
		if (0 < m_fEndTime)
			m_isActive = bFlag;
		else
			m_fEnd = m_fTime + 1;
	}
	else {
		m_isActive = bFlag;
		m_fEnd = m_fEndTime;
		m_fTime = 0;
	}
}
_bool CReserveDeferred::Update(_float fTimeDelta)
{
	m_fTime += fTimeDelta;
	if (0 < m_fEnd && m_fEnd < m_fTime)
		m_isActive = false;
	return m_isActive;
}

void CReserveDeferred::Free()
{
	__super::Free();

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
