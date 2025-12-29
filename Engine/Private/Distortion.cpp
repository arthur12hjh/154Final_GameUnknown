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

    m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Deferred_Distortion.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements);
    if (nullptr == m_pShader)
        return E_FAIL;

    /* Target_Distortion.*/
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Distortion"), vScreenSize.x, vScreenSize.y, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.0f, 0.0f, 0.0f, 0.0f))))
        return E_FAIL;

    /* Target_DistortionResult */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_DistortionResult"), vScreenSize.x, vScreenSize.y, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.0f, 0.0f, 0.0f, 0.0f))))
        return E_FAIL;

    /* MRT_Distortion */
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Distortion"), TEXT("Target_Distortion"))))
        return E_FAIL;

    /* MRT_DistortionResult */
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_DistortionResult"), TEXT("Target_DistortionResult"))))
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

HRESULT CDistortion::Render(CVIBuffer_Rect* pVIBuffer, const _wstring& strRTTag, const _wstring& strReturnRTTag)
{
    m_pShader->Bind_Matrix("g_WorldMatrix", m_pGameInstance->Get_Renderer_Matrix());
    m_pShader->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Renderer_Matrix(D3DTS::VIEW));
    m_pShader->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Renderer_Matrix(D3DTS::PROJ));

#pragma region DISTORTION OBJECT RECORD
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
#pragma endregion

#ifdef _DEBUG
    m_pGameInstance->BeginMarker(m_pContext, TEXT("##############DISTORTION_COMBINE"));
#endif

#pragma region DISTORTION
    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_DistortionResult"))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_Distortion"), m_pShader, "g_DistortionTexture")))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Bind_RenderTarget(strRTTag, m_pShader, "g_SceneTexture")))
        return E_FAIL;

    m_pShader->Begin(0);
    pVIBuffer->Bind_Resources();
    pVIBuffer->Render();

    if (FAILED(m_pGameInstance->End_MRT()))
        return E_FAIL;
#pragma endregion 

#pragma region DISTIORTION RETURN TO SCENE
    if (FAILED(m_pGameInstance->Begin_MRT(strReturnRTTag)))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_DistortionResult"), m_pShader, "g_SceneTexture")))
        return E_FAIL;

    m_pShader->Begin(1);
    pVIBuffer->Bind_Resources();
    pVIBuffer->Render();

    if (FAILED(m_pGameInstance->End_MRT()))
        return E_FAIL;
#pragma endregion


#ifdef _DEBUG
    m_pGameInstance->EndMarker(m_pContext);
#endif
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

    for (auto& DistortionObject : m_DistortionObjects)
    {
        Safe_Release(DistortionObject);
    }

    m_DistortionObjects.clear();
}
