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
                DrawObjectInfo(pGameObject);


                // ♥─────────────────────────────────────┐
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

    if (1 == m_pSelectList.size())
    {
        ImGui::Begin("Collision Editor");
        CGameObject* pObject = m_pSelectList.front();

        _uint iIndex = {};
        _char szComponentName[MAX_PATH] = {};
        for (auto& Pair : *pObject->GetAllComponents())
        {
            //노드 표현했으면 부모 래퍼런스 감소 그다음 다그리면 클리어
            m_TreeNodeFlag = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
            if (Pair.second == m_pSelectComponent)
                m_TreeNodeFlag |= ImGuiTreeNodeFlags_Selected;

            CStringHelper::ConvertWideToUTF(Pair.first.c_str(), szComponentName);
            sprintf_s(m_szObjectTag, "%s%d##Component%d", szComponentName, iIndex, iIndex);
            bool opened = ImGui::TreeNodeEx(m_szObjectTag, m_TreeNodeFlag);

            // 선택 감지 (펼치기와 별개로) 
            if (ImGui::IsItemClicked())
            {
                if (m_TreeNodeFlag & ImGuiTreeNodeFlags_Selected)
                {
                    if (Pair.second == m_pSelectComponent)
                        m_pSelectComponent = nullptr;
                }
                else
                    m_pSelectComponent = Pair.second;
            }

            iIndex++;

            if (opened)
            {
                auto pCollider = dynamic_cast<CCollider*>(Pair.second);
                if (pCollider)
                {
                    //DrawEditorCollider(pCollider);
                }

                ImGui::TreePop();
            }
        }

        ImGui::End();
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

void CDebugHierarchy::DrawObjectInfo(CGameObject* pDrawObject)
{
    auto pTransform = pDrawObject->GetTransform();
    _float3 vPosition{}, vLook{}, vRight{}, vUp{};
    _float3 vScale = pTransform->Get_Scale();
    XMStoreFloat3(&vPosition, pTransform->Get_State(STATE::POSITION));
    XMStoreFloat3(&vRight, pTransform->Get_State(STATE::RIGHT));
    XMStoreFloat3(&vUp, pTransform->Get_State(STATE::UP));
    XMStoreFloat3(&vLook, pTransform->Get_State(STATE::LOOK));

    ImGui::Text("Transform Info");
    ImGui::Separator();
    ImGui::Text("Right -    X : %.2f Y : %.2f Z :%.2f", vRight.x, vRight.y, vRight.z);
    ImGui::Text("Up -       X : %.2f Y : %.2f Z :%.2f", vUp.x, vUp.y, vUp.z);
    ImGui::Text("Look -     X : %.2f Y : %.2f Z :%.2f", vLook.x, vLook.y, vLook.z);
    ImGui::Text("Position - X : %.2f Y : %.2f Z :%.2f", vPosition.x, vPosition.y, vPosition.z);
    ImGui::Text("Rotation - 보류");
    ImGui::Text("Scale -    X : %.2f Y : %.2f Z :%.2f", vScale.x, vScale.y, vScale.z);
}

void CDebugHierarchy::DrawEditorCollider(CCollider* pCollider)
{
    // 1 : Collider Size & Collider Radius
    // 2 : Collider Tag Change
    // 3 : Collider Ignore Change
    // 4 : Collider Only Hit Change
    _bool bIsShowFlag[4] = { false, true, true, true};
    _bool bIsApplySize = { false };
    switch (pCollider->GetCollierType())
    {
    case COLLIDER::OBB :
    {
        auto pObbCol = static_cast<COBBCollider*>(pCollider);
        memcpy(m_vColliderSize, &pObbCol->GetOrizinBounding().Extents, sizeof(_float3));
        memcpy(m_vColliderRotation, &pObbCol->GetOrizinBounding().Orientation, sizeof(_float4));

        if (ImGui::InputFloat3("Collider Size", m_vColliderSize, "%.2f"))
            bIsApplySize = true;

        if (ImGui::InputFloat4("Collider Rotation", m_vColliderRotation, "%.2f"))
            bIsApplySize = true;

        if (bIsApplySize = true)
        {
            _float3 vSize = { m_vColliderSize[0], m_vColliderSize[1], m_vColliderSize[2]};
            _float3 vCenter = { 0.f, vSize.y * 0.5f, 0.f };
            _float4 vRotation = { m_vColliderRotation[0], m_vColliderRotation[1], m_vColliderRotation[2], m_vColliderRotation[3]};
            pObbCol->SetCollision(vCenter, vRotation, vSize);
        }
    }
        break;

    case COLLIDER::AABB:
    {
        auto pAABBCol = static_cast<CBoxCollider*>(pCollider);
        memcpy(m_vColliderSize, &pAABBCol->GetOrizinBounding().Extents, sizeof(_float3));
        if(ImGui::InputFloat3("Collider Size", m_vColliderSize, "%.2f"))
            bIsApplySize = true;

        if (bIsApplySize = true)
        {
            _float3 vSize = { m_vColliderSize[0], m_vColliderSize[1], m_vColliderSize[2] };
            _float3 vCenter = { 0.f, vSize.y, 0.f };
            pAABBCol->SetCollision(vCenter, vSize);
        }
    }
        break;

    case COLLIDER::SPHERE :
    {
        auto pSphereCol = static_cast<CSphereCollider*>(pCollider);
        m_vColliderSize[0] = pSphereCol->GetOrizinBounding().Radius;
        if (ImGui::InputFloat("Collider Radius", &m_vColliderSize[0]))
            bIsApplySize = true;

        if (bIsApplySize = true)
        {
            _float3 vCenter = { 0.f, m_vColliderSize[0] * 0.5f, 0.f};
            static_cast<CSphereCollider*>(pCollider)->SetCollision(vCenter, m_vColliderSize[0]);
        }
    }
        break;
    }

    switch (pCollider->GetCollierHitType())
    {
    case HIT_TYPE::ALL :
        ImGui::Text("Hit Type : ALL");
        break;
    case HIT_TYPE::INTERACTION:
        ImGui::Text("Hit Type : InterRaction");
        break;
    case HIT_TYPE::MONSTER:
        ImGui::Text("Hit Type : Monster");
        break;
    case HIT_TYPE::OBJECT:
        ImGui::Text("Hit Type : Object");
        break;
    case HIT_TYPE::PLAYER:
        ImGui::Text("Hit Type : Player");
        break;
    case HIT_TYPE::SENCE:
        ImGui::Text("Hit Type : Sence");
        break;
    case HIT_TYPE::STATIC:
        ImGui::Text("Hit Type : Static");
        break;
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
