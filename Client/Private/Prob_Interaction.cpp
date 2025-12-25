#include "pch.h"
#include "Prob_Interaction.h"

#include "GameInstance.h"
#include "GameManager.h"
#include "Interaction_Component.h"


CProb_Interaction::CProb_Interaction(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) :
    CActor(pDevice, pContext),
    m_pGameManager(CGameManager::GetInstance())
{
    Safe_AddRef(m_pGameManager);
}

CProb_Interaction::CProb_Interaction(const CProb_Interaction& Prototype) :
    CActor(Prototype),
    m_pGameManager(CGameManager::GetInstance())
{
    Safe_AddRef(m_pGameManager);
}

HRESULT CProb_Interaction::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CProb_Interaction::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    PROB_INTERACTION_DESC* pDesc = static_cast<PROB_INTERACTION_DESC*>(pArg);
    m_InteractionDesc = m_pGameManager->Find_InteractionData(pDesc->iInteractionID);

    return S_OK;
}

void CProb_Interaction::Priority_Update(_float fTimeDelta)
{
}

void CProb_Interaction::Update(_float fTimeDelta)
{
}

void CProb_Interaction::Late_Update(_float fTimeDelta)
{
}

HRESULT CProb_Interaction::Render()
{
    return S_OK;
}

void CProb_Interaction::Finished_Interaction()
{
    if (INTERACTION_STATE::ACTIVE == m_eInterState)
        m_eInterState = INTERACTION_STATE::DEFAULT;
}

void CProb_Interaction::Reset_Interaction()
{
    m_fInteractionDuration = false;
}

HRESULT CProb_Interaction::Begin_OverlapCallBack()
{
    if(m_eInterState != INTERACTION_STATE::END)
        m_pGameInstance->ADD_Interaction(m_pInteractionCom);

    return S_OK;
}

HRESULT CProb_Interaction::End_OverlapCallBack()
{
    m_pGameInstance->Remove_Interaction(m_pInteractionCom);

    return S_OK;
}

void CProb_Interaction::Excute_CallBack(_float fTimeDelta, CGameObject* pActionObject)
{
}

_bool CProb_Interaction::IsInteractionEnable()
{
    return m_fInteractionDuration >= m_InteractionDesc->fInteractionTime ? true : false;
}

CGameObject* CProb_Interaction::Clone(void* pArg)
{
    return nullptr;
}

void CProb_Interaction::Free()
{
    __super::Free();

    Safe_Release(m_pGameManager);
    Safe_Release(m_pInteractionCom);
}
