#include "pch.h"
#include "DebugCheatUI.h"

#include "GameInstance.h"
#include "GameManager.h"

CDebugCheatUI::CDebugCheatUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) :
    CGameObject(pDevice, pContext)
{
}

HRESULT CDebugCheatUI::Initialize()
{
    m_pGameManager = CGameManager::GetInstance();
    Safe_AddRef(m_pGameManager);

    return S_OK;
}

void CDebugCheatUI::Update(_float fTimeDeleta)
{
    _bool bIsOpen = 1 - ENUM_CLASS(m_eVisibility);
    ImGui::Begin("Cheat List", &bIsOpen);
    ImGui::Checkbox("Object Teleport Picking", &m_bIsTeleport);
    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::Text("Button Select");
        ImGui::Text("Mesh or Terrian Picking Move Object");
        ImGui::EndTooltip();
    }
    ImGui::End();





    if (m_bIsTeleport)
    {
        _float3 vPickingPoint = {};
        if (m_pGameInstance->KeyDown(KEY_INPUT::MOUSE, 0) &&
            m_pGameInstance->isPicking(&vPickingPoint))
        {
            auto pCharacter = m_pGameManager->GetGameCharacter();
            _vector vPos = XMLoadFloat3(&vPickingPoint);
            vPos.m128_f32[3] = 1.f;
            pCharacter->GetTransform()->Set_State(STATE::POSITION, vPos);
            Safe_Release(pCharacter);
        }
    }

    if (!bIsOpen)
        m_eVisibility = VISIBILITY::HIDDEN;
}

HRESULT CDebugCheatUI::Render()
{
    return S_OK;
}

CDebugCheatUI* CDebugCheatUI::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CDebugCheatUI* pDebugCheatUI = new CDebugCheatUI(pDevice, pContext);
    if (FAILED(pDebugCheatUI->Initialize()))
    {
        Safe_Release(pDebugCheatUI);
        MSG_BOX("Create Fail : Debug Cheat UI");
    }
    return pDebugCheatUI;
}

CGameObject* CDebugCheatUI::Clone(void* pArg)
{
    return nullptr;
}

void CDebugCheatUI::Free()
{
    __super::Free();

    Safe_Release(m_pGameManager);
}
