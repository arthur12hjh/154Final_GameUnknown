#include "Interaction_Manager.h"
#include "GameInstance.h"

#include "Interaction_Component.h"
#include "GameObject.h"

CInteraction_Manager::CInteraction_Manager() :
    m_pGameInstance(CGameInstance::GetInstance())
{
    Safe_AddRef(m_pGameInstance);
}

HRESULT CInteraction_Manager::Initalize()
{
    m_pInteractionList.reserve(100);
    return S_OK;
}

void CInteraction_Manager::Update()
{
    if (nullptr == m_pBaseObject)
        return;

    _matrix vCamMatrix = XMLoadFloat4x4(m_pGameInstance->GetMainCameraWorldMatrixPtr());
    _vector vBaseObjectPos = m_pBaseObject->GetTransform()->Get_State(STATE::POSITION);

    m_pCandidates.clear();
    for (auto& iter : m_pInteractionList)
    {
        CCollider::DEFAULT_HIT_DESC Desc = {};
        if (iter->Is_RayHit(vCamMatrix.r[3], vCamMatrix.r[2], &Desc))
            m_pCandidates.push_back(iter);
    }

    if (!m_pCandidates.empty())
    {
        m_pCandidates.sort([&](CInteraction_Component* pSrc, CInteraction_Component* pDest)
            {
                _vector vSrcPos = pSrc->GetOwner()->GetTransform()->Get_State(STATE::POSITION);
                _vector vDestPos = pDest->GetOwner()->GetTransform()->Get_State(STATE::POSITION);

                _float fSrcDist = XMVectorGetX(XMVector3Length(vBaseObjectPos - vSrcPos));
                _float fDestDist = XMVectorGetX(XMVector3Length(vBaseObjectPos - vDestPos));

                return fSrcDist < fDestDist;
            });

        m_pNearInteraction = m_pCandidates.front();
    }
    else
        m_pNearInteraction = nullptr;

}

void CInteraction_Manager::SetInteractionBaseObject(CGameObject* pObject)
{
    m_pBaseObject = pObject;
}

void CInteraction_Manager::ADD_Interaction(CInteraction_Component* pInteraction_Com)
{
    auto iter = find(m_pInteractionList.begin(), m_pInteractionList.end(), pInteraction_Com);
    if (iter == m_pInteractionList.end())
        m_pInteractionList.push_back(pInteraction_Com);
}

void CInteraction_Manager::Remove_Interaction(CInteraction_Component* pInteraction_Com)
{
    auto iter = find(m_pInteractionList.begin(), m_pInteractionList.end(), pInteraction_Com);
    if (iter != m_pInteractionList.end())
        m_pInteractionList.erase(iter);
}

CInteraction_Manager* CInteraction_Manager::Create()
{
    CInteraction_Manager* pInteractionMgr = new CInteraction_Manager();
    if (FAILED(pInteractionMgr->Initalize()))
    {
        Safe_Release(pInteractionMgr);
        MSG_BOX("Create Fail : Interaction Mgr");
    }
    return pInteractionMgr;
}

void CInteraction_Manager::Free()
{
    Safe_Release(m_pGameInstance);
}
