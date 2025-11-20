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

    _vector vObjectPos = m_pBaseObject->GetTransform()->Get_State(STATE::POSITION);
    sort(m_pInteractionList.begin(), m_pInteractionList.end(), [&](CInteraction_Component* pSrc, CInteraction_Component* pDest)
        {
            _vector vSrcPos = pSrc->GetOwner()->GetTransform()->Get_State(STATE::POSITION);
            _vector vDestPos = pDest->GetOwner()->GetTransform()->Get_State(STATE::POSITION);

            _float fSrcDist = XMVectorGetX(XMVector3Length(vObjectPos - vSrcPos));
            _float fDestDist = XMVectorGetX(XMVector3Length(vObjectPos - vDestPos));

            return fSrcDist < fDestDist;
        });

    if (!m_pInteractionList.empty())
        m_pNearInteraction = m_pInteractionList.front();
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
