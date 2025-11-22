#include "MotionBlur.h"

#include "GameInstance.h"

CMotionBlur::CMotionBlur(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CDeferred { pDevice, pContext }
{
}

CMotionBlur::CMotionBlur(const CMotionBlur& rhs)
	: CDeferred{ rhs }
{
}

HRESULT CMotionBlur::Initialize()
{
    /* 셰이더 파일 로딩 */
    m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Deferred_MotionBlur.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements);
    if (nullptr == m_pShader)
        return E_FAIL;

    /* 스크린 사이즈는 미리 바인딩 한다. */
    _uint2 vScreenSize = m_pGameInstance->GetScreenSize();
    if (FAILED(m_pShader->Bind_RawValue("g_iWinSizeX", &vScreenSize.x, sizeof(_int))))
        return E_FAIL;

    if (FAILED(m_pShader->Bind_RawValue("g_iWinSizeY", &vScreenSize.y, sizeof(_int))))
        return E_FAIL;

    /* Target_MotionBlur. */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_MotionBlur"), vScreenSize.x, vScreenSize.y, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.0f, 0.0f, 0.0f, 0.0f))))
        return E_FAIL;

	/* MRT_MotionBlur */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_MotionBlur"), TEXT("Target_MotionBlur"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMotionBlur::Render(CVIBuffer_Rect* pVIBuffer, const _wstring& strSceneRTTag, const _wstring& strReturnRTTag)
{
    /* 블러 X 처리 */
    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_MotionBlur"))))
        return E_FAIL;

    m_pShader->Bind_Matrix("g_WorldMatrix", m_pGameInstance->Get_Renderer_Matrix());
    m_pShader->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Renderer_Matrix(D3DTS::VIEW));
    m_pShader->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Renderer_Matrix(D3DTS::PROJ));

    if (FAILED(m_pGameInstance->Bind_RenderTarget(strSceneRTTag, m_pShader, "g_SceneTexture")))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_Velocity"), m_pShader, "g_VelocityTexture")))
        return E_FAIL;

    m_pShader->Begin(0);
    pVIBuffer->Bind_Resources();
    pVIBuffer->Render();

    if (FAILED(m_pGameInstance->End_MRT()))
        return E_FAIL;

    // MRT_Scene에 다시 반환... 
    if (FAILED(m_pGameInstance->Begin_MRT(strReturnRTTag)))
        return E_FAIL;

    m_pShader->Bind_Matrix("g_WorldMatrix", m_pGameInstance->Get_Renderer_Matrix());
    m_pShader->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Renderer_Matrix(D3DTS::VIEW));
    m_pShader->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Renderer_Matrix(D3DTS::PROJ));

    if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_MotionBlur"), m_pShader, "g_SceneTexture")))
        return E_FAIL;

    m_pShader->Begin(1);
    pVIBuffer->Bind_Resources();
    pVIBuffer->Render();

    if (FAILED(m_pGameInstance->End_MRT()))
        return E_FAIL;


    return S_OK;
}

HRESULT CMotionBlur::Bind_RenderTarget(CShader* pShader, const _char* pConstantName)
{
	if(FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_MotionBlur"), pShader, pConstantName)))
		return E_FAIL;

	return S_OK;
}

#ifdef _DEBUG

HRESULT CMotionBlur::Ready_Debug(_float fX, _float fY, _float fSizeX, _float fSizeY)
{
    if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_MotionBlur"), fX, fY, fSizeX, fSizeY)))
        return E_FAIL;

	return S_OK;
}

HRESULT CMotionBlur::Render_Debug(CVIBuffer_Rect* pVIBuffer, CShader* pShader)
{
    if (FAILED(m_pGameInstance->Render_RT_Debug(TEXT("MRT_MotionBlur"), pShader, pVIBuffer)))
        return E_FAIL;

	return S_OK;
}

#endif

CMotionBlur* CMotionBlur::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMotionBlur* pInstance = new CMotionBlur(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Create Failed : MotionBlur");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMotionBlur::Free()
{
	__super::Free();
}
