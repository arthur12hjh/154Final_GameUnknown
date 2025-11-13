#include "Bloom.h"

#include "GameInstance.h"
#include "GameObject.h"

CBloom::CBloom(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CDeferred{ pDevice, pContext }
{
}

HRESULT CBloom::Initialize()
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

    /* MRT_Glow */
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Glow"), TEXT("Target_Glow"))))
        return E_FAIL;


    return S_OK;
}

HRESULT CBloom::Add_RenderObject(CGameObject* pRenderObject)
{
    return S_OK;
}

HRESULT CBloom::Render(CVIBuffer_Rect* pVIBuffer)
{
    return S_OK;
}

HRESULT CBloom::Bind_RenderTarget(CShader* pShader, const _char* pConstantName)
{
    return S_OK;
}
#ifdef _DEBUG
HRESULT CBloom::Ready_Debug(_float fX, _float fY, _float fSizeX, _float fSizeY)
{
    return S_OK;
}

HRESULT CBloom::Render_Debug(CVIBuffer_Rect* pVIBuffer, CShader* pShader)
{
    return S_OK;
}
#endif
CBloom* CBloom::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CBloom* pInstance = new CBloom(pDevice, pContext);

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Create Failed : CBloom");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CBloom::Free()
{
    __super::Free();
}
