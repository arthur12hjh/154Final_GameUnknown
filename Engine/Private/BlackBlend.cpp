#include "BlackBlend.h"

#include "GameInstance.h"
#include "BlendObject.h"

CBlackBlend::CBlackBlend(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CDeferred{ pDevice, pContext }
{
}

HRESULT CBlackBlend::Initialize()
{
    /* 셰이더 파일 로딩 */
    m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Deferred_BlackBlend.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements);
    if (nullptr == m_pShader)
        return E_FAIL;

    /* 스크린 사이즈는 미리 바인딩 한다. */
    _uint2 vScreenSize = m_pGameInstance->GetScreenSize();
    if (FAILED(m_pShader->Bind_RawValue("g_iWinSizeX", &vScreenSize.x, sizeof(_int))))
        return E_FAIL;

    if (FAILED(m_pShader->Bind_RawValue("g_iWinSizeY", &vScreenSize.y, sizeof(_int))))
        return E_FAIL;

    /* Target_BlackBlend. */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_BlackBlend"), vScreenSize.x, vScreenSize.y, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.0f, 0.0f, 0.0f, 0.0f))))
        return E_FAIL;
    /* Target_BlackBlend_Final. Y에 대해서도 블러처리 수행. */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_BlackBlend_Final"), vScreenSize.x, vScreenSize.y, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.0f, 0.0f, 0.0f, 0.0f))))
        return E_FAIL;

    /* MRT_BlackBlend */
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_BlackBlend"), TEXT("Target_BlackBlend"))))
        return E_FAIL;
    /* MRT_BlackBlend_Final */
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_BlackBlend_Final"), TEXT("Target_BlackBlend_Final"))))
        return E_FAIL;

    return S_OK;
}

HRESULT CBlackBlend::Add_RenderObject(CGameObject* pRenderObject)
{
    if (nullptr == pRenderObject)
        return E_FAIL;
    m_BlackBlendObjects.push_back(pRenderObject);
    Safe_AddRef(pRenderObject);

    return S_OK;
}

HRESULT CBlackBlend::Render(CVIBuffer_Rect* pVIBuffer)
{
    /* 블러 기록할 물체들만 뺴서 기록 */
    if (0 < m_BlackBlendObjects.size()) {
        if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_BlackBlend"))))
            return E_FAIL;

        m_BlackBlendObjects.sort([](CGameObject* pSour, CGameObject* pDest)->_bool {
            return static_cast<CBlendObject*>(pSour)->Get_Depth() > static_cast<CBlendObject*>(pDest)->Get_Depth();
            });
        for (auto& pRenderObject : m_BlackBlendObjects)
        {
            if (nullptr != pRenderObject)
                pRenderObject->Render();
            Safe_Release(pRenderObject);
        }

        if (FAILED(m_pGameInstance->End_MRT()))
            return E_FAIL;

        // 웨이트 처리
        if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_BlackBlend_Final"))))
            return E_FAIL;

        m_pShader->Bind_Matrix("g_WorldMatrix", m_pGameInstance->Get_Renderer_Matrix());
        m_pShader->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Renderer_Matrix(D3DTS::VIEW));
        m_pShader->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Renderer_Matrix(D3DTS::PROJ));

        if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_BlackBlend"), m_pShader, "g_BlendTexture")))
            return E_FAIL;

        m_pShader->Begin(0);
        pVIBuffer->Bind_Resources();
        pVIBuffer->Render();

        if (FAILED(m_pGameInstance->End_MRT()))
            return E_FAIL;
        m_BlackBlendObjects.clear();
    }
    else {
        m_pGameInstance->Clear_MRT(TEXT("MRT_BlackBlend_Final"));
    }

    m_bisTexture = true;
    return S_OK;
}

HRESULT CBlackBlend::Bind_RenderTarget(CShader* pShader, const _char* pConstantName)
{
        if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_BlackBlend_Final"), pShader, pConstantName)))
            return E_FAIL;

    return S_OK;
}

#ifdef _DEBUG
HRESULT CBlackBlend::Ready_Debug(_float fX, _float fY, _float fSizeX, _float fSizeY)
{
    if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_BlackBlend_Final"), fX, fY, fSizeX, fSizeY)))
        return E_FAIL;

    return S_OK;
}

HRESULT CBlackBlend::Render_Debug(CVIBuffer_Rect* pVIBuffer, CShader* pShader)
{
    if (FAILED(m_pGameInstance->Render_RT_Debug(TEXT("MRT_BlackBlend_Final"), pShader, pVIBuffer)))
        return E_FAIL;

    return S_OK;
}
#endif

CBlackBlend* CBlackBlend::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CBlackBlend* pInstance = new CBlackBlend(pDevice, pContext);

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Create Failed : BlackBlend");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CBlackBlend::Free()
{
    __super::Free();

    for (auto& BlurObject : m_BlackBlendObjects)
    {
        Safe_Release(BlurObject);
    }
    m_BlackBlendObjects.clear();
}
