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

void* CMotionBlur::Get_Desc()
{
    m_Desc.fObjectBlurScale = &m_fObjectBlurScale;
    m_Desc.fBias = &m_fBias;
    m_Desc.fCamBlurScale = &m_fCamBlurScale;
    m_Desc.iSampleCount = &m_iSampleCount;

    return &m_Desc;
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

    /* Target_CamMotionBlur. */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_CamVelocity"), vScreenSize.x, vScreenSize.y, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.0f, 0.0f, 0.0f, 0.0f))))
        return E_FAIL;

    /* MRT_CamMotionBlur */
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_CamVelocity"), TEXT("Target_CamVelocity"))))
        return E_FAIL;

	return S_OK;
}

HRESULT CMotionBlur::Render(CVIBuffer_Rect* pVIBuffer, const _wstring& strSceneRTTag, const _wstring& strReturnRTTag)
{
    /* 블러 X 처리 */
    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_MotionBlur"))))
        return E_FAIL;

    CAMERA_INFO 	CamInfo = m_pGameInstance->Get_CurrentCamInfo();
    m_pShader->Bind_RawValue("g_fFar", &CamInfo.fFar, sizeof(_float));
    m_pShader->Bind_Matrix("g_WorldMatrix", m_pGameInstance->Get_Renderer_Matrix());
    m_pShader->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Renderer_Matrix(D3DTS::VIEW));
    m_pShader->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Renderer_Matrix(D3DTS::PROJ));
    
    m_pShader->Bind_RawValue("g_fObjectBlurScale", &m_fObjectBlurScale, sizeof(_float));
    m_pShader->Bind_RawValue("g_fCamBlurScale", &m_fCamBlurScale, sizeof(_float));
    m_pShader->Bind_RawValue("g_iSampleCount", &m_iSampleCount, sizeof(_uint));
    m_pShader->Bind_RawValue("g_fBias", &m_fBias, sizeof(_float));

    if (FAILED(m_pGameInstance->Bind_RenderTarget(strSceneRTTag, m_pShader, "g_SceneTexture")))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_Velocity"), m_pShader, "g_VelocityTexture")))
        return E_FAIL;

    m_pShader->Begin(1);
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

    m_pShader->Begin(2);
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

_float2 CMotionBlur::Calc_CamVelocity()
{
    _matrix CurView = XMLoadFloat4x4(m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW));
    _matrix CurProj = XMLoadFloat4x4(m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ));
    _matrix PreView = XMLoadFloat4x4(m_pGameInstance->Get_PreTransform_Float4x4(D3DTS::VIEW));
    _matrix PreProj = XMLoadFloat4x4(m_pGameInstance->Get_PreTransform_Float4x4(D3DTS::PROJ));

    _vector vCamPos, vCamLook;

    //이동값 날리려고 Position 사용안함.
    vCamLook = XMLoadFloat4(m_pGameInstance->Get_CamLook());
    // 현재/이전 프레임에서의 클립 좌표
    
    _vector vCurrClip = XMVector4Transform(XMVector4Transform(vCamLook, CurView), CurProj);
    _vector vPrevClip = XMVector4Transform(XMVector4Transform(vCamLook, PreView), PreProj);

    _float fCurrW = XMVectorGetW(vCurrClip);
    _float fPrevW = XMVectorGetW(vPrevClip);

    // NDC 기준 velocity
    _float fCurrX = XMVectorGetX(vCurrClip) / fCurrW;
    _float fCurrY = XMVectorGetY(vCurrClip) / fCurrW;
    _float fPrevX = XMVectorGetX(vPrevClip) / fPrevW;
    _float fPrevY = XMVectorGetY(vPrevClip) / fPrevW;

    _float fStrength = 1.0f;
    _float2 fVelocity = _float2((fCurrX - fPrevX) * fStrength, -1.f * (fCurrY - fPrevY) * fStrength);

    return fVelocity;
}

#ifdef _DEBUG

HRESULT CMotionBlur::Ready_Debug(_float fX, _float fY, _float fSizeX, _float fSizeY)
{
    if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_Velocity"), fX, fY, fSizeX, fSizeY)))
        return E_FAIL;

	return S_OK;
}

HRESULT CMotionBlur::Render_Debug(CVIBuffer_Rect* pVIBuffer, CShader* pShader)
{
    if (FAILED(m_pGameInstance->Render_RT_Debug(TEXT("MRT_Velocity"), pShader, pVIBuffer)))
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
