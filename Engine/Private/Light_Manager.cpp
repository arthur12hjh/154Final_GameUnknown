#include "Light_Manager.h"
#include "Light.h"

#ifdef _DEBUG
#include "GameInstance.h"
#include "VIBuffer_Point.h"
#include "Shader.h"
#include "Texture.h"
#include "Model.h"
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
#pragma region VIBuffer
    m_pVIBuffer[0] = CVIBuffer_Point::Create(m_pDevice, m_pContext);
    if (nullptr == m_pVIBuffer[0])
        return E_FAIL;

    _matrix PreMatrix = XMMatrixScaling(0.01f, 0.005f, 0.01f) * XMMatrixRotationRollPitchYaw(XMConvertToRadians(90.f), 0.f, 0.f);
    m_pVIBuffer[1] = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/EngineResource/Model/Light/arrow.fbx", PreMatrix);
    if (nullptr == m_pVIBuffer[1])
        return E_FAIL;
#pragma endregion


#pragma region Shader
    m_pShader[0] = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/EngineShader_VIPoint.hlsl"), VTXPOS::Elements, VTXPOS::iNumElements);
    if (nullptr == m_pShader[0])
        return E_FAIL;

    m_pShader[1] = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/EngineShader_VtxMesh.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements);
    if (nullptr == m_pShader[1])
        return E_FAIL;
#pragma endregion

    m_pTexture = CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/EngineResource/Texture/Light.png"), 1);
    if (nullptr == m_pTexture)
        return E_FAIL;

    return S_OK;
}

void CLight_Manager::Debug_LightRender()
{
    _uint iIndex = 0;
   
    for (auto& pLight : m_Lights)
    {
        m_pTexture->Bind_ShaderResource(m_pShader[0], "g_Texture", 0);
        m_pShader[0]->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW));
        m_pShader[0]->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ));
        m_pShader[0]->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float4));

        m_pShader[0]->Bind_Matrix("g_WorldMatrix", pLight->GetWorldMatrix());
        m_pShader[0]->Begin(0);

        CVIBuffer_Point* pVIPointBuffer = static_cast<CVIBuffer_Point*>(m_pVIBuffer[0]);
        pVIPointBuffer->Bind_Resources();
        pVIPointBuffer->Render();

        if (LIGHT_TYPE::DIRECTIONAL == pLight->Get_LightDesc()->eType)
        {
            m_pShader[1]->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW));
            m_pShader[1]->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ));
            m_pShader[1]->Bind_Matrix("g_WorldMatrix", pLight->GetWorldMatrix());

            CModel* pModel = static_cast<CModel*>(m_pVIBuffer[1]);
            _uint iNumMeshes = pModel->Get_NumMeshes();
            m_pShader[1]->Begin(0);

            for (_uint i = 0; i < iNumMeshes; ++i)
            {
                pModel->Render(i);
            }
        }
        else if (LIGHT_TYPE::POINT == pLight->Get_LightDesc()->eType)
        {
            pLight->Debug_Render();
        }
    }
}
#else
CLight_Manager::CLight_Manager()
{
}
#endif // _DEBUG

HRESULT CLight_Manager::Add_Light(const LIGHT_DESC& LightDesc, CLight* pOutLight)
{
    CLight* pLight = nullptr;
#ifdef _DEBUG
    pLight = CLight::Create(m_pDevice, m_pContext, LightDesc);
#else
    pLight = CLight::Create(LightDesc);
#endif // _DEBUG
    if (nullptr == pLight)
        return E_FAIL;

    if (pOutLight)
    {
        pOutLight = pLight;
        Safe_AddRef(pOutLight);
    }

    m_Lights.push_back(pLight);
    return S_OK;
}

CLight* CLight_Manager::Find_Light(_uint iIndex)
{
    if (0 > iIndex || m_Lights.size() <= iIndex)
        return nullptr;

    auto iter = m_Lights.begin();
    advance(iter, iIndex);

    return *iter;
}

void CLight_Manager::Clear_DeadLight()
{
    for (auto iter = m_Lights.begin(); iter != m_Lights.end();)
    {
        if ((*iter)->IsDead())
        {
            Safe_Release(*iter);
            iter = m_Lights.erase(iter);
        }
        else
            iter++;
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
#else
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
    for (_uint i = 0; i < 2; ++i)
    {
        Safe_Release(m_pVIBuffer[i]);
        Safe_Release(m_pShader[i]);
    }

    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
    Safe_Release(m_pGameInstance);
#endif // _DEBUG

}
