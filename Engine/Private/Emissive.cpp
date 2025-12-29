#include "Emissive.h"

#include "GameInstance.h"
#include "GameObject.h"

CEmissive::CEmissive(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CDeferred { pDevice, pContext }
{
}

HRESULT CEmissive::Initialize()
{
    /* 셰이더 파일 로딩 */
    m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Deferred_Blur.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements);
    if (nullptr == m_pShader)
        return E_FAIL;

    /* 스크린 사이즈는 미리 바인딩 한다. */
    _uint2 vScreenSize = m_pGameInstance->GetScreenSize();

    CAMERA_INFO 	CamInfo = m_pGameInstance->Get_CurrentCamInfo();
    m_pShader->Bind_RawValue("g_fFar", &CamInfo.fFar, sizeof(_float));

    if (FAILED(m_pShader->Bind_RawValue("g_iWinSizeX", &vScreenSize.x, sizeof(_int))))
        return E_FAIL;

    if (FAILED(m_pShader->Bind_RawValue("g_iWinSizeY", &vScreenSize.y, sizeof(_int))))
        return E_FAIL;

    /* X에 대해서 우선 블러처리. */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Emissive_Blur_X"), vScreenSize.x, vScreenSize.y, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.0f, 0.0f, 0.0f, 0.0f))))
        return E_FAIL;
    /* Y에 대해서도 블러처리 수행. */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Emissive_Blur_Final"), vScreenSize.x, vScreenSize.y, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.0f, 0.0f, 0.0f, 0.0f))))
        return E_FAIL;
    /* MRT_Emissive_Blur_X */
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Emissive_Blur_X"), TEXT("Target_Emissive_Blur_X"))))
        return E_FAIL;
    /* MRT_Emissive_Blur_Final */
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Emissive_Blur_Final"), TEXT("Target_Emissive_Blur_Final"))))
        return E_FAIL;

    return S_OK;
}

HRESULT CEmissive::Render(CVIBuffer_Rect* pVIBuffer)
{
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Emissive_Blur_X"))))
		return E_FAIL;

	m_pShader->Bind_Matrix("g_WorldMatrix", m_pGameInstance->Get_Renderer_Matrix());
	m_pShader->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Renderer_Matrix(D3DTS::VIEW));
	m_pShader->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Renderer_Matrix(D3DTS::PROJ));

	if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_Emissive"), m_pShader, "g_BlurTexture")))
		return E_FAIL;

	m_pShader->Begin(3);
	pVIBuffer->Bind_Resources();
	pVIBuffer->Render();

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	/* 블러 Y 처리 */
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Emissive_Blur_Final"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_Emissive_Blur_X"), m_pShader, "g_BlurTexture")))
		return E_FAIL;

	m_pShader->Begin(4);
	pVIBuffer->Bind_Resources();
	pVIBuffer->Render();

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	return S_OK;
}

HRESULT CEmissive::Bind_RenderTarget(CShader* pShader, const _char* pConstantName)
{
    //렌더타겟 이름 바꿔줘야 함.
    if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_Emissive_Blur_Final"), pShader, pConstantName)))
        return E_FAIL;

    return S_OK;
}

#ifdef _DEBUG
HRESULT CEmissive::Ready_Debug(_float fX, _float fY, _float fSizeX, _float fSizeY)
{
    return S_OK;
}

HRESULT CEmissive::Render_Debug(CVIBuffer_Rect* pVIBuffer, CShader* pShader)
{
    return S_OK;
}
#endif

CEmissive* CEmissive::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CEmissive* pInstance = new CEmissive(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Create Failed : CEmissive");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEmissive::Free()
{
    __super::Free();
}
