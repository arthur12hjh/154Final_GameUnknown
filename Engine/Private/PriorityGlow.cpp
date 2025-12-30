#include "PriorityGlow.h"

#include "GameInstance.h"
#include "GameObject.h"

CPriorityGlow::CPriorityGlow(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CDeferred{ pDevice, pContext }
{
}

HRESULT CPriorityGlow::Initialize()
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

    /* Target_Glow_X. X에 대해서 우선 블러처리. */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_PriorityGlow_X"), vScreenSize.x, vScreenSize.y, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.0f, 0.0f, 0.0f, 0.0f))))
        return E_FAIL;
    /* Target_Glow_Final. Y에 대해서도 블러처리 수행. */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_PriorityGlow_Y"), vScreenSize.x, vScreenSize.y, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.0f, 0.0f, 0.0f, 0.0f))))
        return E_FAIL;

    /* MRT_Glow */
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_PriorityGlow_X"), TEXT("Target_PriorityGlow_X"))))
        return E_FAIL;
    /* MRT_Glow_Weight */
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_PriorityGlow_Y"), TEXT("Target_PriorityGlow_Y"))))
        return E_FAIL;

    return S_OK;
}

HRESULT CPriorityGlow::Add_RenderObject(CGameObject* pRenderObject)
{
    return S_OK;
}

HRESULT CPriorityGlow::Render(CVIBuffer_Rect* pVIBuffer)
{
    return S_OK;
}

HRESULT CPriorityGlow::Bind_RenderTarget(CShader* pShader, const _char* pConstantName)
{
    return S_OK;
}

HRESULT CPriorityGlow::Ready_Debug(_float fX, _float fY, _float fSizeX, _float fSizeY)
{
    return S_OK;
}

HRESULT CPriorityGlow::Render_Debug(CVIBuffer_Rect* pVIBuffer, CShader* pShader)
{
    return S_OK;
}

CPriorityGlow* CPriorityGlow::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CPriorityGlow* pInstance = CPriorityGlow::Create(pDevice, pContext);

    if (FAILED(pInstance->Initialize()))
    {
        Safe_Release(pInstance);
        MSG_BOX("Create Failed : Priority Glow");
    }
    return pInstance;
}

void CPriorityGlow::Free()
{
    __super::Free();

    Safe_Release(m_pShader);
}
