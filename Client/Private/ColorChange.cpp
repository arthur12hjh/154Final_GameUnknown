#include "pch.h"
#include "ColorChange.h"

#include "GameInstance.h"

CColorChange::CColorChange(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CReserveDeferred { pDevice, pContext }
{
}

HRESULT CColorChange::Initialize(void* pArg)
{
    //if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EDITOR), TEXT("Prototype_Component_Shader_VtxNorTex"),
    //    CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX::Elements, VTXNORTEX::iNumElements))))
    //    return E_FAIL;

    //셰이더 파일은 STATIC에 넣어둘 것
    m_pShaderCom = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_ColorChange.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements);
    if (nullptr == m_pShaderCom)
        return E_FAIL;

    return S_OK;
}

/* strRTTag 어디서 받아옴? -> 렌더러에서 알아서 넣어주니까 신경 ㄴㄴ */
void CColorChange::Bind_Resources(const _wstring& strRTTag)
{
    //여기서 필요한 리소스들 바인딩해주고..

    //END로 바인딩해야 월드매트릭스 반환해줌.
    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pGameInstance->Get_Renderer_Matrix(D3DTS::END))))
        return;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Renderer_Matrix(D3DTS::VIEW))))
        return;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Renderer_Matrix(D3DTS::PROJ))))
        return;

    //컬러만 바인딩하자.
    if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vColor, sizeof(_float4))))
        return;
    // 씬 텍스쳐 바인딩.
    if (FAILED(m_pGameInstance->Bind_RenderTarget(strRTTag, m_pShaderCom, "g_SceneTexture")))
        return;
}

void CColorChange::Set_Desc(void* pArg)
{
    COLORCHANGE_DEFERRED_DESC* pDesc = static_cast<COLORCHANGE_DEFERRED_DESC*>(pArg);

    m_vColor = pDesc->vColor;
    m_fIntensity = pDesc->fIntensity;
}

CColorChange* CColorChange::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
    CColorChange* pInstance = new CColorChange(pDevice, pContext);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Create Failed : ColorChange");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CColorChange::Free()
{
    __super::Free();
}
