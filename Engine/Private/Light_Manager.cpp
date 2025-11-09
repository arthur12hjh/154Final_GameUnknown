#include "Light_Manager.h"
#include "Light.h"

#ifdef _DEBUG
#include "GameInstance.h"
#include "VIBuffer_Point.h"
#include "Shader.h"
#include "Texture.h"
#endif

#ifdef _DEBUG
CLight_Manager::CLight_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) :
    m_pDevice(pDevice),
    m_pContext(pContext),
    m_pGameInstance(CGameInstance::GetInstance())
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
    Safe_AddRef(m_pGameInstance);
}

HRESULT CLight_Manager::Initialize()
{
    m_pVIBuffer = CVIBuffer_Point::Create(m_pDevice, m_pContext);
    if (nullptr == m_pVIBuffer)
        return E_FAIL;

    m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VIPoint.hlsl"), VTXPOS::Elements, VTXPOS::iNumElements);
    if (nullptr == m_pShader)
        return E_FAIL;

    m_pTexture = CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/EngineResource/Texture/Light.png"), 1);
    if (nullptr == m_pTexture)
        return E_FAIL;

    return S_OK;
}

void CLight_Manager::Debug_LightRender()
{
    m_pTexture->Bind_ShaderResource(m_pShader, "g_Texture", 0);
    m_pShader->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW));
    m_pShader->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ));
    m_pShader->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float4));

    for (auto& pLight : m_Lights)
    {
        m_pShader->Bind_Matrix("g_WorldMatrix", pLight->GetWorldMatrix());
        m_pShader->Begin(0);

        m_pVIBuffer->Bind_Resources();
        m_pVIBuffer->Render();
        pLight->Debug_Render();
        
    }
}
#elif
CLight_Manager::CLight_Manager()
{
}
#endif // _DEBUG

const LIGHT_DESC* CLight_Manager::Get_LightDesc(_uint iIndex) const
{
    auto    iter = m_Lights.begin();

    for (size_t i = 0; i < iIndex; i++)
        ++iter;

    return (*iter)->Get_LightDesc();    
}

HRESULT CLight_Manager::Add_Light(const LIGHT_DESC& LightDesc)
{
    CLight* pLight = nullptr;
#ifdef _DEBUG
    pLight = CLight::Create(m_pDevice, m_pContext, LightDesc);
#elif
    pLight = CLight::Create(LightDesc);
#endif // _DEBUG
    if (nullptr == pLight)
        return E_FAIL;

    m_Lights.push_back(pLight);
    return S_OK;
}

void CLight_Manager::Remove_Light(CLight* pLight)
{
    auto iter = find(m_Lights.begin(), m_Lights.end(), pLight);
    if (iter != m_Lights.end())
    {
        m_Lights.erase(iter);
    }
}

HRESULT CLight_Manager::Render_Lights(CShader* pShader, CVIBuffer* pVIBuffer)
{
    for (auto& pLight : m_Lights)
    {
        pLight->Render(pShader, pVIBuffer);
    }

    return S_OK;
}

#ifdef _DEBUG
CLight_Manager* CLight_Manager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CLight_Manager* pLight_Manager = new CLight_Manager(pDevice, pContext);
    if (FAILED(pLight_Manager->Initialize()))
    {
        Safe_Release(pLight_Manager);
        MSG_BOX("Create Fail : Light Manager");
    }
    return pLight_Manager;
}
#elif
CLight_Manager* CLight_Manager::Create()
{
    return new CLight_Manager();
}
#endif // _DEBUG

void CLight_Manager::Free()
{
    __super::Free();
    
    for (auto& pLight : m_Lights)
        Safe_Release(pLight);

    m_Lights.clear();

#ifdef _DEBUG
    Safe_Release(m_pTexture);
    Safe_Release(m_pVIBuffer);
    Safe_Release(m_pShader);

    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
    Safe_Release(m_pGameInstance);
#endif // _DEBUG

}
