#include "Interaction_Component.h"

#include "GameInstance.h"
#include "GameObject.h"

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
    m_BeginCallBackFunc = pDesc->BeginCallBackFunc;
    m_EndCallBackFunc = pDesc->EndCallBackFunc;
    m_InteractionFunc = pDesc->InteractionEvent;

    if (FAILED(Ready_Components(*pDesc)))
        return E_FAIL;
    
    m_pOBBColiider->SetCollision({}, pDesc->vRoation, pDesc->vSize);
    return S_OK;
}

void CInteraction_Component::SetOwner(CGameObject* pGameObject)
{
    __super::SetOwner(pGameObject);
    m_pOBBColiider->SetOwner(pGameObject);
}

void CInteraction_Component::Update_Com(_matrix WorldMat)
{
    m_pOBBColiider->UpdateColiision(WorldMat);
    m_pGameInstance->ADD_Collider(m_pOBBColiider);
#ifdef _DEBUG
    m_pGameInstance->Add_DebugComponent(m_pOBBColiider);
#endif // _DEBUG
}

void CInteraction_Component::Action_InteractionEvent(_float fTimeDelta, CGameObject* pGameObject)
{
    // 여기서 인터렉션 호출
    // 호출하면 여기서 호출한 녀석과 함께 넘겨준다.
    if(m_InteractionFunc)
        m_InteractionFunc(fTimeDelta, pGameObject);
}

void CInteraction_Component::SetInteractionHitType(HIT_TYPE eHitType)
{
    m_pOBBColiider->SetColliderHitType(eHitType);
}

void CInteraction_Component::ADD_InteractionIgnoreObject(HIT_TYPE eHitType)
{
    m_pOBBColiider->ADD_IgnoreObjectType(eHitType);
}

void CInteraction_Component::ADD_InteractionOnlyHitObject(HIT_TYPE typeID)
{
    m_pOBBColiider->ADD_OnlyHitObjectType(typeID);
}

const _float3& CInteraction_Component::Get_CenterPos()
{
    return m_pOBBColiider->GetBounding().Center;
}

#ifdef _DEBUG
HRESULT CInteraction_Component::Render()
{
    m_pOBBColiider->Render();


    return S_OK;
}
#endif // _DEBUG

_bool CInteraction_Component::Is_Overlap(CCollider* pCollider)
{
    return m_pOBBColiider->Intersect(pCollider->GetCollierType(), pCollider);
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

HRESULT CInteraction_Component::Ready_Components(const INTERACTION_DESC& Desc)
{
    m_pOBBColiider = COBBCollider::Create(m_pDevice, m_pContext);
    if (nullptr == m_pOBBColiider)
        return E_FAIL;

    COBBCollider::OBB_COLLIDER_DESC OBBDesc = { };
    m_pOBBColiider->Initialize(&OBBDesc);

    m_pOBBColiider->BindBeginOverlapEvent([&](_float3 vHitPoint, _float3 vHitDir, CGameObject* pHitActor) { m_BeginCallBackFunc(); });
    m_pOBBColiider->BindEndOverlapEvent([&](_float3 vHitPoint, _float3 vHitDir, CGameObject* pHitActor) { m_EndCallBackFunc(); });

    m_pOBBColiider->SetColliderHitType(HIT_TYPE::INTERACTION);

    m_pOBBColiider->ADD_IgnoreObjectType(HIT_TYPE::INTERACTION);
    m_pOBBColiider->ADD_IgnoreObjectType(HIT_TYPE::SENCE);
    m_pOBBColiider->ADD_IgnoreObjectType(HIT_TYPE::OBJECT);
    m_pOBBColiider->ADD_IgnoreObjectType(HIT_TYPE::STATIC);
    return S_OK;
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
