#include "Glow.h"

#include "GameInstance.h"
#include "GameObject.h"

CGlow::CGlow(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CDeferred{ pDevice, pContext }
{
}

HRESULT CGlow::Initialize()
{
    /* 셰이더 파일 로딩 */
    m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Deferred_Glow.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements);
    if (nullptr == m_pShader)
        return E_FAIL;

    /* 스크린 사이즈는 미리 바인딩 한다. */
    _uint2 vScreenSize = m_pGameInstance->GetScreenSize();
    if (FAILED(m_pShader->Bind_RawValue("g_iWinSizeX", &vScreenSize.x, sizeof(_int))))
        return E_FAIL;

    if (FAILED(m_pShader->Bind_RawValue("g_iWinSizeY", &vScreenSize.y, sizeof(_int))))
        return E_FAIL;

    /* Target_Glow. */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Glow"), vScreenSize.x, vScreenSize.y, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.0f, 0.0f, 0.0f, 0.0f))))
        return E_FAIL;
    /* Target_Glow_X. X에 대해서 우선 블러처리. */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Glow_X"), vScreenSize.x, vScreenSize.y, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.0f, 0.0f, 0.0f, 0.0f))))
        return E_FAIL;
    /* Target_Glow_Final. Y에 대해서도 블러처리 수행. */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Glow_Final"), vScreenSize.x, vScreenSize.y, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.0f, 0.0f, 0.0f, 0.0f))))
        return E_FAIL;

    /* MRT_Glow */
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Glow"), TEXT("Target_Glow"))))
        return E_FAIL;
    /* MRT_Glow_X */
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Glow_X"), TEXT("Target_Glow_X"))))
        return E_FAIL;
    /* MRT_Glow_Final */
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Glow_Final"), TEXT("Target_Glow_Final"))))
        return E_FAIL;

    return S_OK;
}

HRESULT CGlow::Add_RenderObject(CGameObject* pRenderObject)
{
    if (nullptr == pRenderObject)
        return E_FAIL;

    m_GlowObjects.push_back(pRenderObject);

    Safe_AddRef(pRenderObject);

    return S_OK;
}

HRESULT CGlow::Render(CVIBuffer_Rect* pVIBuffer)
{
    /* 블러 기록할 물체들만 뺴서 기록 */
    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Glow"))))
        return E_FAIL;

    for (auto& pRenderObject : m_GlowObjects)
    {
        if (nullptr != pRenderObject)
            pRenderObject->Render();

        Safe_Release(pRenderObject);
    }

    m_GlowObjects.clear();

    if (FAILED(m_pGameInstance->End_MRT()))
        return E_FAIL;

    /* 블러 X 처리 */
    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Glow_X"))))
        return E_FAIL;

    m_pShader->Bind_Matrix("g_WorldMatrix", m_pGameInstance->Get_Renderer_Matrix());
    m_pShader->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Renderer_Matrix(D3DTS::VIEW));
    m_pShader->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Renderer_Matrix(D3DTS::PROJ));

    if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_Glow"), m_pShader, "g_GlowTexture")))
        return E_FAIL;

    m_pShader->Begin(0);
    pVIBuffer->Bind_Resources();
    pVIBuffer->Render();

    if (FAILED(m_pGameInstance->End_MRT()))
        return E_FAIL;

    /* 블러 Y 처리 */
    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Glow_Final"))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_Glow_X"), m_pShader, "g_GlowXTexture")))
        return E_FAIL;

    m_pShader->Begin(1);
    pVIBuffer->Bind_Resources();
    pVIBuffer->Render();

    if (FAILED(m_pGameInstance->End_MRT()))
        return E_FAIL;

    return S_OK;
}

HRESULT CGlow::Bind_RenderTarget(CShader* pShader, const _char* pConstantName)
{
    if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_Glow_Final"), pShader, pConstantName)))
        return E_FAIL;

    return S_OK;
}

#ifdef _DEBUG
HRESULT CGlow::Ready_Debug(_float fX, _float fY, _float fSizeX, _float fSizeY)
{
    if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_Glow_Final"), fX, fY, fSizeX, fSizeY)))
        return E_FAIL;

    return S_OK;
}

HRESULT CGlow::Render_Debug(CVIBuffer_Rect* pVIBuffer, CShader* pShader)
{
    if (FAILED(m_pGameInstance->Render_RT_Debug(TEXT("MRT_Glow_Final"), pShader, pVIBuffer)))
        return E_FAIL;

    return S_OK;
}
#endif

CGlow* CGlow::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CGlow* pInstance = new CGlow(pDevice, pContext);

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Create Failed : Glow");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CGlow::Free()
{
    __super::Free();

    for (auto& BlurObject : m_GlowObjects)
    {
        Safe_Release(BlurObject);
    }

    m_GlowObjects.clear();
}
