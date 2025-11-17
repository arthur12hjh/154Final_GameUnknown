#include "Fog.h"

#include "GameInstance.h"

CFog::CFog(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CDeferred{ pDevice, pContext }
{
}

HRESULT CFog::Initialize()
{
	/* 셰이더 파일 로딩 */
	m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Deferred_Fog.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements);
	if (nullptr == m_pShader)
		return E_FAIL;

	_uint2 vScreenSize = m_pGameInstance->GetScreenSize();
	/* Target_Fog. */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Fog"), vScreenSize.x, vScreenSize.y, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(1.0f, 1.0f, 1.0f, 1.0f))))
		return E_FAIL;
	/* MRT_Fog */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Fog"), TEXT("Target_Fog"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CFog::Render(CVIBuffer_Rect* pVIBuffer)
{
	/* 블러 X 처리 */
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Fog"))))
		return E_FAIL;

	m_pShader->Bind_Matrix("g_WorldMatrix", m_pGameInstance->Get_Renderer_Matrix());
	m_pShader->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Renderer_Matrix(D3DTS::VIEW));
	m_pShader->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Renderer_Matrix(D3DTS::PROJ));
	m_pShader->Bind_Matrix("g_ViewMatrixInv", m_pGameInstance->Get_Transform_Float4x4_Inverse(D3DTS::VIEW));
	m_pShader->Bind_Matrix("g_ProjMatrixInv", m_pGameInstance->Get_Transform_Float4x4_Inverse(D3DTS::PROJ));

	if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_Depth"), m_pShader, "g_DepthTexture")))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_RawValue("g_fFogStart", &m_fFogStart, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_RawValue("g_fFogEnd", &m_fFogEnd, sizeof(_float))))
		return E_FAIL;

	m_pShader->Begin(0);
	pVIBuffer->Bind_Resources();
	pVIBuffer->Render();

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	return S_OK;
}

HRESULT CFog::Bind_RenderTarget(CShader* pShader, const _char* pConstantName)
{
	if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_Fog"), pShader, pConstantName)))
		return E_FAIL;

	return S_OK;
}

#ifdef _DEBUG
HRESULT CFog::Ready_Debug(_float fX, _float fY, _float fSizeX, _float fSizeY)
{
	if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_Fog"), fX, fY, fSizeX, fSizeY)))
		return E_FAIL;

	return S_OK;
}

HRESULT CFog::Render_Debug(CVIBuffer_Rect* pVIBuffer, CShader* pShader)
{
	if (FAILED(m_pGameInstance->Render_RT_Debug(TEXT("MRT_Fog"), pShader, pVIBuffer)))
		return E_FAIL;

	return S_OK;
}
#endif

CFog* CFog::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CFog* pInstance = new CFog(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Create Failed : Fog");
	}

	return pInstance;
}

void CFog::Free()
{
	__super::Free();
}
