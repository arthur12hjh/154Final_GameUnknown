#include "pch.h"
#include "InteractionBinder.h"

#include "GameInstance.h"

CInteractionBinder::CInteractionBinder(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) :
	CInteraction_Component(pDevice, pContext)
{
}

CInteractionBinder::CInteractionBinder(const CInteractionBinder& Prototype)
    : CInteraction_Component{ Prototype }
{
}

HRESULT CInteractionBinder::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CInteractionBinder::Initialize(void* pArg)
{
    //INTERACTION_DATA Desc = *static_cast<INTERACTION_DATA*>(pArg);

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    //m_InteractionDesc = &Desc;

    return S_OK;
}

void CInteractionBinder::Finished_Interaction()
{
    if (INTERACTION_STATE::ACTIVE == m_eInterState)
        m_eInterState = INTERACTION_STATE::DEFAULT;
}

_bool CInteractionBinder::IsInteractionEnable()
{
    /*if (m_fInteractionDuration <= 0.f && m_InteractionDesc->fInteractionTime > 0.f)
        m_pGameInstance->Manager_PlaySound(TEXT("UI_GaugeFX.wav"), CHANNELID::EFFECT2, 1.f, 1.f);*/

    return m_fInteractionDuration >= m_InteractionDesc->fInteractionTime ? true : false;
}

CInteractionBinder* CInteractionBinder::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CInteractionBinder* pInteractionBinder = new CInteractionBinder(pDevice, pContext);
    if (FAILED(pInteractionBinder->Initialize_Prototype()))
    {
        Safe_Release(pInteractionBinder);
        MSG_BOX("Create Fail : InteractionBinder");
    }
    return pInteractionBinder;
}

CComponent* CInteractionBinder::Clone(void* pArg)
{
    CInteractionBinder* pInteractionBinder = new CInteractionBinder(*this);
    if (FAILED(pInteractionBinder->Initialize(pArg)))
    {
        Safe_Release(pInteractionBinder);
        MSG_BOX("Clone Fail : InteractionBinder");
    }
    return pInteractionBinder;
}

void CInteractionBinder::Free()
{
    __super::Free();
}
