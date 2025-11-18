#include "Distortion.h"

#include "GameInstance.h"
#include "GameObject.h"

CDistortion::CDistortion(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CDeferred{ pDevice, pContext }
{
}

HRESULT CDistortion::Initialize()
{    
    /* 디스토션은 셰이더 따로 필요 없음. */
    _uint2 vScreenSize = m_pGameInstance->GetScreenSize();

    /* Target_Distortion.*/
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Distortion"), vScreenSize.x, vScreenSize.y, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.0f, 0.0f, 0.0f, 0.0f))))
        return E_FAIL;

    /* MRT_Distortion */
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Distortion"), TEXT("Target_Distortion"))))
        return E_FAIL;

    return S_OK;
}

HRESULT CDistortion::Add_RenderObject(CGameObject* pRenderObject)
{
    if (nullptr == pRenderObject)
        return E_FAIL;

    m_DistortionObjects.push_back(pRenderObject);

    Safe_AddRef(pRenderObject);

    return S_OK;
}

HRESULT CDistortion::Render(CVIBuffer_Rect* pVIBuffer)
{
    /* Diffuse + Normal */
    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Distortion"))))
        return E_FAIL;

    for (auto& pRenderObject : m_DistortionObjects)
    {
        if (nullptr != pRenderObject)
            pRenderObject->Render();

        Safe_Release(pRenderObject);
    }

    m_DistortionObjects.clear();

    if (FAILED(m_pGameInstance->End_MRT()))
        return E_FAIL;

    return S_OK;
}

HRESULT CDistortion::Bind_RenderTarget(CShader* pShader, const _char* pConstantName)
{
    if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_Distortion"), pShader, pConstantName)))
        return E_FAIL;

    return S_OK;
}

#ifdef _DEBUG
HRESULT CDistortion::Ready_Debug(_float fX, _float fY, _float fSizeX, _float fSizeY)
{
    if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_Distortion"), fX, fY, fSizeX, fSizeY)))
        return E_FAIL;

    return S_OK;
}

HRESULT CDistortion::Render_Debug(CVIBuffer_Rect* pVIBuffer, CShader* pShader)
{
    if (FAILED(m_pGameInstance->Render_RT_Debug(TEXT("MRT_Distortion"), pShader, pVIBuffer)))
        return E_FAIL;

    return S_OK;
}
#endif

CDistortion* CDistortion::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CDistortion* pInstance = new CDistortion(pDevice, pContext);

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Create Failed : CDistortion");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CDistortion::Free()
{
    __super::Free();

    for (auto& BlurObject : m_DistortionObjects)
    {
        Safe_Release(BlurObject);
    }

    m_DistortionObjects.clear();
}
