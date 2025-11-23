#include "pch.h"
#include "ShaderDebugger.h"

#include "GameInstance.h"

CShaderDebugger::CShaderDebugger(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject { pDevice, pContext }
{
}

HRESULT CShaderDebugger::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CShaderDebugger::Initialize()
{
    m_pFogDesc = static_cast<FOG_DESC*>(m_pGameInstance->Get_Fog_Desc());
    m_pBloomDesc = static_cast<BLOOM_DESC*>(m_pGameInstance->Get_Bloom_Desc());
    m_pDoFDesc = static_cast<DOF_DESC*>(m_pGameInstance->Get_DoF_Desc());
    m_pSSAODesc = static_cast<SSAO_DESC*>(m_pGameInstance->Get_SSAO_Desc());

    return S_OK;
}

void CShaderDebugger::Update(_float fTimeDeleta)
{
    _bool bIsOpen = 1 - ENUM_CLASS(m_eVisibility);

    ImGui::Begin("Shader Debugger", &bIsOpen);

    ImGui::Text("FOG INFO");

    ImGui::DragFloat("Fog Start", m_pFogDesc->fFogStart, 1.f, 0.0f, 500.0f);
    ImGui::DragFloat("Fog End", m_pFogDesc->fFogEnd, 1.f, 0.f, 500.0f);
    ImGui::DragFloat("Fog Power Max", m_pFogDesc->fFogPowerMax, 0.01f, 0.f, 1.0f);
    ImGui::DragFloat("Fog Power Min", m_pFogDesc->fFogPowerMin, 0.01f, 0.f, 1.0f);
    ImGui::ColorEdit4("Fog Color", &m_pFogDesc->vFogColor->x);

    ImGui::Separator();

    ImGui::Text("DoF INFO");

    ImGui::DragFloat("DoF Focus Dist", m_pDoFDesc->fFocusDistance, 1.f, 0.0f, 500.0f);
    ImGui::DragFloat("DoF Max Range", m_pDoFDesc->fMaxRange, 1.f, 0.0f, 500.0f);
    ImGui::DragFloat("DoF Intensity", m_pDoFDesc->fIntensity, 0.01f, 0.0f, 1.0f);

    ImGui::Separator();

    ImGui::Text("SSAO INFO");

    ImGui::DragFloat("SSAO Bias Max", m_pSSAODesc->fBiasMax, 0.001f, 0.0f, 2.0f);
    ImGui::DragFloat("SSAO Bias Min", m_pSSAODesc->fBiasMin, 0.001f, 0.0f, 2.0f);
    ImGui::DragFloat("SSAO Radius Max", m_pSSAODesc->fRadiusMax, 0.01f, 0.1f, 5.0f);
    ImGui::DragFloat("SSAO Radius Min", m_pSSAODesc->fRadiusMin, 0.01f, 0.1f, 5.0f);
    ImGui::DragFloat("SSAO Intensity", m_pSSAODesc->fIntensity, 0.01f, 0.0f, 5.0f);

    ImGui::Separator();

    ImGui::Text("Bloom INFO (CANNOT EDIT. JUST SHOW)");
    
    _uint iMax = 10, iMin = 0;
    
    ImGui::BeginDisabled();
    ImGui::DragScalar("Bloom SampleNum", ImGuiDataType_U32, m_pBloomDesc->iBloomLevel, 1.0f, &iMin, &iMax, "%u", 1);
    ImGui::DragScalar("Bloom SampleLevel", ImGuiDataType_U32, m_pBloomDesc->iSampleLevel, 1.0f, &iMin, &iMax, "%u", 1);
    ImGui::EndDisabled();

    ImGui::Separator();

    ImGui::End();
}

HRESULT CShaderDebugger::Render()
{
    return S_OK;
}

CShaderDebugger* CShaderDebugger::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CShaderDebugger* pInstance = new CShaderDebugger(pDevice, pContext);

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Create Failed : Shader Debugger");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CShaderDebugger::Clone(void* pArg)
{
    return nullptr;
}

void CShaderDebugger::Free()
{
    __super::Free();
}
