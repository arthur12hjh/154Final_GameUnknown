#include "pch.h"
#include "InteractionUIBinder.h"

#include "GameInstance.h"

CInteractionUIBinder::CInteractionUIBinder(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) :
	CInteraction_Component(pDevice, pContext)
{
}

CInteractionUIBinder::CInteractionUIBinder(const CInteractionUIBinder& Prototype)
    : CInteraction_Component{ Prototype }
{
}

HRESULT CInteractionUIBinder::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CInteractionUIBinder::Initialize(void* pArg)
{
    //INTERACTION_DATA Desc = *static_cast<INTERACTION_DATA*>(pArg);

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    //m_InteractionDesc = &Desc;

    return S_OK;
}

void CInteractionUIBinder::Finished_Interaction()
{
    if (INTERACTION_STATE::ACTIVE == m_eInterState)
        m_eInterState = INTERACTION_STATE::DEFAULT;
}

_bool CInteractionUIBinder::IsInteractionEnable()
{
    return m_fInteractionDuration >= m_InteractionDesc->fInteractionTime ? true : false;
}

CInteractionUIBinder* CInteractionUIBinder::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CInteractionUIBinder* pInteractionUIBinder = new CInteractionUIBinder(pDevice, pContext);
    if (FAILED(pInteractionUIBinder->Initialize_Prototype()))
    {
        Safe_Release(pInteractionUIBinder);
        MSG_BOX("Create Fail : InteractionUIBinder");
    }
    return pInteractionUIBinder;
}

CComponent* CInteractionUIBinder::Clone(void* pArg)
{
    CInteractionUIBinder* pInteractionUIBinder = new CInteractionUIBinder(*this);
    if (FAILED(pInteractionUIBinder->Initialize(pArg)))
    {
        Safe_Release(pInteractionUIBinder);
        MSG_BOX("Clone Fail : InteractionUIBinder");
    }
    return pInteractionUIBinder;
}

void CInteractionUIBinder::Free()
{
    __super::Free();
}
