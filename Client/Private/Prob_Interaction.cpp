#include "pch.h"
#include "Prob_Interaction.h"

#include "GameManager.h"
#include "Interaction_Component.h"
#include "UIBase.h"

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

    if(m_iInterID > -1)
        m_tInterDesc = *m_pGameManager->Find_InteractionData(m_iInterID);

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

CGameObject* CProb_Interaction::Clone(void* pArg)
{
    return nullptr;
}

void CProb_Interaction::Free()
{
    __super::Free();

    Safe_Release(m_pGameManager);
    Safe_Release(m_pInteractionCom);
    Safe_Release(m_pInteractionUI);
}
