#include "pch.h"
#include "ShaderDebugger.h"

#include "GameInstance.h"

CShaderDebugger::CShaderDebugger(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject { pDevice, pContext }
{
}

HRESULT CShaderDebugger::Initialize()
{
    return S_OK;
}

void CShaderDebugger::Update(_float fTimeDeleta)
{
    _bool bIsOpen = 1 - ENUM_CLASS(m_eVisibility);

    ImGui::Begin("Shader Debugger", &bIsOpen);

    ImGui::End();
}

HRESULT CShaderDebugger::Render()
{
    return S_OK;
}

CShaderDebugger* CShaderDebugger::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CShaderDebugger* pInstance = new CShaderDebugger(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        Safe_Release(pInstance);
        MSG_BOX("Create Failed : Shader Debugger");
    }

    return pInstance;
}

CGameObject* CShaderDebugger::Clone(void* pArg)
{
    CShaderDebugger* pInstance = new CShaderDebugger(*this);

    if (FAILED(pInstance->Initialize()))
    {
        Safe_Release(pInstance);
        MSG_BOX("Create Failed : Shader Debugger");
    }

    return pInstance;
}

void CShaderDebugger::Free()
{
}
