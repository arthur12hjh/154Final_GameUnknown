#include "Interaction_Component.h"

#include "GameInstance.h"

CInteraction_Component::CInteraction_Component(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) :
    CComponent(pDevice, pContext)
{
}

CInteraction_Component::CInteraction_Component(const CInteraction_Component& rhs) :
    CComponent(rhs)
{
}

HRESULT CInteraction_Component::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CInteraction_Component::Initialize(void* pArg)
{
    INTERACTION_DESC* pDesc = static_cast<INTERACTION_DESC*>(pArg);
    m_pOBBColiider->SetCollision({}, pDesc->vRoation, pDesc->vSize);

    m_UpdateFunction = pDesc->CallBackFunc;
    return S_OK;
}

void CInteraction_Component::Update_Intraction(_float fTimeDeleta)
{
    if (m_UpdateFunction)
        m_UpdateFunction(fTimeDeleta);
}

_bool CInteraction_Component::Is_Overlap(CCollider* pCollider)
{
    return m_pOBBColiider->Intersect(pCollider->GetCollisionType(), pCollider);
}

_bool CInteraction_Component::Is_RayHit(_vector vTargetPos, _vector vDir, void* OutDesc)
{
    if (nullptr == m_pOBBColiider)
        return false;

    CCollider::DEFAULT_HIT_DESC Desc = {};
    _bool bFlag = m_pOBBColiider->RayHit(vTargetPos, vDir, Desc);
    if(OutDesc)
        OutDesc = &Desc;

    return bFlag;
}

CInteraction_Component* CInteraction_Component::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CInteraction_Component* pInteraction_Com = new CInteraction_Component(pDevice, pContext);
    if (FAILED(pInteraction_Com->Initialize_Prototype()))
    {
        Safe_Release(pInteraction_Com);
        MSG_BOX("Create Fail : Interaction Component");
    }
    return pInteraction_Com;
}

CComponent* CInteraction_Component::Clone(void* pArg)
{
    CInteraction_Component* pInteraction_Com = new CInteraction_Component(*this);
    if (FAILED(pInteraction_Com->Initialize(pArg)))
    {
        Safe_Release(pInteraction_Com);
        MSG_BOX("Clone Fail : Interaction Component");
    }
    return pInteraction_Com;
}

void CInteraction_Component::Free()
{
    __super::Free();

    Safe_Release(m_pOBBColiider);
}
