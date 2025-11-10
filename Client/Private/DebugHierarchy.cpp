#include "pch.h"
#include "DebugHierarchy.h"

#include "GameInstance.h"
#include "GameObject.h"
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

    if (nullptr != m_pSelectLayerObject)
    {
        _uint iIndex = {};
        for (auto& pGameObject : *m_pSelectLayerObject)
        {
            //노드 표현했으면 부모 래퍼런스 감소 그다음 다그리면 클리어
            m_TreeNodeFlag = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;

            auto iter = find(m_pSelectList.begin(), m_pSelectList.end(), pGameObject);
            if (iter != m_pSelectList.end())
                m_TreeNodeFlag |= ImGuiTreeNodeFlags_Selected;

            sprintf_s(m_szObjectTag, "%s%d##%d", typeid(*pGameObject).name(), iIndex, iIndex);
            bool opened = ImGui::TreeNodeEx(m_szObjectTag, m_TreeNodeFlag);

            // 선택 감지 (펼치기와 별개로) 
            if (ImGui::IsItemClicked())
            {
                if (m_TreeNodeFlag & ImGuiTreeNodeFlags_Selected)
                {
                    if (iter != m_pSelectList.end())
                        m_pSelectList.erase(iter);
                }
                else
                    m_pSelectList.push_back(pGameObject);
            }

            iIndex++;

            if (opened)
            {
                // ┌─────────────────────────────────────┐
                // │  class Logic                        │
                // │  1. ImGui TreeNode Debug            │
                // │  2. JH Here Code                    │
                // │  3. JH Here Code                    │
                // │  4. JH Here Code                    │
                // │  5. JH Here Code                    │
                // └─────────────────────────────────────┘

                ImGui::TreePop();
            }
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

void CDebugHierarchy::UpdateTransform(_float3 vPosition)
{
    for (auto& iter : m_pSelectList)
    {
        iter->GetTransform()->Set_State(STATE::POSITION, XMLoadFloat3(&vPosition));
    }
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
