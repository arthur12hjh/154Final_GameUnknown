#include "RadialBlur.h"

#include "GameInstance.h"
#include "GameObject.h"

CRadialBlur::CRadialBlur(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CDeferred{ pDevice, pContext }
{
}

void CRadialBlur::Set_Active(_uint iSampleCount, _float fSamplePower, _float fLifeTime)
{
    m_iSampleCount = iSampleCount;
    m_fSamplePower = fSamplePower;
    m_fLifeTime = fLifeTime;
    m_fTimeAcc = 0.f;
    m_isActive = true;
}

void CRadialBlur::Update(_float fTimeDelta)
{
    if (false == m_isActive)
        return;

    m_fTimeAcc += fTimeDelta;

    if (m_fTimeAcc >= m_fLifeTime)
    {
        m_isActive = false;
        m_fTimeAcc = 0.f;
    }
}

HRESULT CRadialBlur::Initialize()
{
    /* 셰이더 파일 로딩 */
    m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Deferred_RadialBlur.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements);
    if (nullptr == m_pShader)
        return E_FAIL;

    /* 스크린 사이즈는 미리 바인딩 한다. */
    _uint2 vScreenSize = m_pGameInstance->GetScreenSize();
    if (FAILED(m_pShader->Bind_RawValue("g_iWinSizeX", &vScreenSize.x, sizeof(_int))))
        return E_FAIL;

    if (FAILED(m_pShader->Bind_RawValue("g_iWinSizeY", &vScreenSize.y, sizeof(_int))))
        return E_FAIL;

    /* Target_Blur_Final. Y에 대해서도 블러처리 수행. */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_RadialBlur"), vScreenSize.x, vScreenSize.y, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.0f, 0.0f, 0.0f, 0.0f))))
        return E_FAIL;

    /* MRT_Blur */
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_RadialBlur"), TEXT("Target_RadialBlur"))))
        return E_FAIL;

    return S_OK;
}

HRESULT CRadialBlur::Render(CVIBuffer_Rect* pVIBuffer, const _wstring& strRTTag, const _wstring& strReturnRTTag)
{
    if (false == m_isActive)
        return S_OK;

    //Radial Blur  처리
    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_RadialBlur"))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Bind_RenderTarget(strRTTag, m_pShader, "g_SceneTexture")))
        return E_FAIL;

    m_pShader->Bind_Matrix("g_WorldMatrix", m_pGameInstance->Get_Renderer_Matrix());
    m_pShader->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Renderer_Matrix(D3DTS::VIEW));
    m_pShader->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Renderer_Matrix(D3DTS::PROJ));

    m_pShader->Bind_RawValue("g_fLifeTime", &m_fLifeTime, sizeof(_float));
    m_pShader->Bind_RawValue("g_fTimeAcc", &m_fTimeAcc, sizeof(_float));
    m_pShader->Bind_RawValue("g_iSampleCount", &m_iSampleCount, sizeof(_float));
    m_pShader->Bind_RawValue("g_fSamplePower", &m_fSamplePower, sizeof(_float));


    m_pShader->Begin(0);
    pVIBuffer->Bind_Resources();
    pVIBuffer->Render();

    if (FAILED(m_pGameInstance->End_MRT()))
        return E_FAIL;

    //Radial BLur 처리 반환
    if (FAILED(m_pGameInstance->Begin_MRT(strReturnRTTag)))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_RadialBlur"), m_pShader, "g_SceneTexture")))
        return E_FAIL;

    m_pShader->Begin(1);
    pVIBuffer->Bind_Resources();
    pVIBuffer->Render();
     
    if (FAILED(m_pGameInstance->End_MRT()))
        return E_FAIL;

    return S_OK;
}

CRadialBlur* CRadialBlur::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CRadialBlur* pInstance = new CRadialBlur(pDevice, pContext);

    if (FAILED(pInstance->Initialize()))
    {
        Safe_Release(pInstance);
        MSG_BOX("Create Failed! : Radial Blur");
    }

    return pInstance;
}

void CRadialBlur::Free()
{
    __super::Free();

    Safe_Release(m_pShader);
}
