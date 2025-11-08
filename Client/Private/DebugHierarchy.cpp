#include "pch.h"
#include "DebugHierarchy.h"

#include "GameInstance.h"
#include "StringHelper.h"

CDebugHierarchy::CDebugHierarchy(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) :
	CGameObject(pDevice, pContext)
{
}

HRESULT CDebugHierarchy::Initialize()
{


	return S_OK;
}

void CDebugHierarchy::Update(_float fTimeDeleta)
{
	_bool bIsOpen = 1 - ENUM_CLASS(m_eVisibility);
	ImGui::Begin("Hierarchy", &bIsOpen);

    DarwLayerSelect();

    //노드 표현했으면 부모 래퍼런스 감소 그다음 다그리면 클리어
    m_TreeNodeFlag = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
    if (nullptr != m_pSelectLayerObject)
    {
        for (auto& iter : *m_pSelectLayerObject)
        {

        }
    }

	ImGui::End();
	if (!bIsOpen)
		m_eVisibility = VISIBILITY::HIDDEN;
}

HRESULT CDebugHierarchy::Render()
{
	return S_OK;
}

void CDebugHierarchy::DarwLayerSelect()
{
    auto Layer = m_pGameInstance->GetCurrentLevelLayer();
    if (ImGui::BeginCombo("LayerSelect##HierarchyLayerView", m_szLayerPreview))
    {
        for (auto& iter : *Layer)
        {
            CStringHelper::ConvertWideToUTF(iter.first.c_str(), m_szLayerView);
            if (ImGui::Selectable(m_szLayerView, false))
            {
                strcpy_s(m_szLayerPreview, m_szLayerView);
                m_pSelectLayerObject = m_pGameInstance->GetAllObejctToLayer(m_pGameInstance->GetCurrentLevelID(), iter.first.c_str());
            }
        }

        ImGui::EndCombo();
    }
}

CDebugHierarchy* CDebugHierarchy::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDebugHierarchy* pDebugHierarchy = new CDebugHierarchy(pDevice, pContext);
	if (FAILED(pDebugHierarchy->Initialize()))
	{
		Safe_Release(pDebugHierarchy);
		MSG_BOX("Create Fail : Deubg Hierarchy");
	}
	return pDebugHierarchy;
}

CGameObject* CDebugHierarchy::Clone(void* pArg)
{
	return nullptr;
}

void CDebugHierarchy::Free()
{
	__super::Free();
}
