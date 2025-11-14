#include "Percetion.h"

#include "GameObject.h"
#include "SphereCollider.h"

CPerception::CPerception(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) :
    CComponent(pDevice, pContext)
{
}

CPerception::CPerception(const CPerception& Prototype) :
    CComponent(Prototype)
{
}

HRESULT CPerception::Initialize_Prototype()
{



    return S_OK;
}

HRESULT CPerception::Initialize(void* pArg)
{
    if (nullptr == pArg)
    {
        OutputDebugStringW(TEXT("[DEBUG] Log : Not In Percetion Desc"));
        return S_OK;
    }

    PERCETION_COMPONENT_DESC* pDesc = static_cast<PERCETION_COMPONENT_DESC*>(pArg);
    m_fSearchRadius = pDesc->fSearchRadius;
    m_fTargetLostDistance = pDesc->fTargetLostDistance;

    if (FAILED(ADD_Components()))
        return E_FAIL;

    return S_OK;
}

void CPerception::Updat_Component(_float fDeletaTime, const _float4x4* pSearchMatrix)
{
    // 여기서 부모의 위치를 넣을까 아님
    // 메트릭스도 받아서 시야 감지범위를 이동시켜줄지 고민
    if (nullptr == pSearchMatrix)
        m_pTargetSearchCol->UpdateColiision(XMLoadFloat4x4(m_pOwner->GetTransform()->Get_WorldMatrixPtr()));
    else
        m_pTargetSearchCol->UpdateColiision(XMLoadFloat4x4(pSearchMatrix));
}

void CPerception::Bind_TargetSearch(function<void(void*)> Func)
{
    m_SearchFunc = Func;
}

void CPerception::Bind_TargetLost(function<void(void*)> Func)
{
    m_TargetLostFunc = Func;
}

void CPerception::Bind_TargetDetected(function<void(void*)> Func)
{
    m_TargetDetectedFunc = Func;
}

#ifdef _DEBUG
void CPerception::Render_Percetion(_float4 vColor)
{
    m_pTargetSearchCol->Render();
}
#endif

HRESULT CPerception::ADD_Components()
{
    m_pTargetSearchCol = CSphereCollider::Create(m_pDevice, m_pContext);
    if (nullptr == m_pTargetSearchCol)
        return E_FAIL;

    CSphereCollider::SPHERE_COLLIDER_DESC SphereColDesc = {};
    SphereColDesc.fRadius = m_fSearchRadius;

    if (FAILED(m_pTargetSearchCol->Initialize(&SphereColDesc)))
        return E_FAIL;

    return S_OK;
}

void CPerception::TargetSearch(_float3 vHitPoint, _float3 vHitDir, CGameObject* pHitActor)
{
    if (m_TargetDetectedFunc)
    {
        _vector vTargetPoint = pHitActor->GetTransform()->Get_State(STATE::POSITION);

        m_PercetionOutDesc.vDir = vHitDir;
        XMStoreFloat3(&m_PercetionOutDesc.vReturnPoint, vTargetPoint);
        m_PercetionOutDesc.pObject = pHitActor;
        m_pPercetionObjects.insert(pHitActor);

        m_TargetDetectedFunc(&m_PercetionOutDesc);
    }
}

void CPerception::TargetDetected(_float3 vHitPoint, _float3 vHitDir, CGameObject* pHitActor)
{
    if (m_TargetLostFunc)
    {
        _vector vTargetPoint = pHitActor->GetTransform()->Get_State(STATE::POSITION);

        m_PercetionOutDesc.vDir = vHitDir;
        XMStoreFloat3(&m_PercetionOutDesc.vReturnPoint, vTargetPoint);
        m_PercetionOutDesc.pObject = pHitActor;
        m_TargetLostFunc(&m_PercetionOutDesc);
    }
}

void CPerception::TargetLost(_float3 vHitPoint, _float3 vHitDir, CGameObject* pHitActor)
{
    if (m_SearchFunc)
    {
        _vector vTargetPoint = pHitActor->GetTransform()->Get_State(STATE::POSITION);

        m_PercetionOutDesc.vDir = vHitDir;
        XMStoreFloat3(&m_PercetionOutDesc.vReturnPoint, vTargetPoint);
        m_PercetionOutDesc.pObject = pHitActor;

        auto iter = m_pPercetionObjects.find(pHitActor);
        if (iter == m_pPercetionObjects.end())
            m_pPercetionObjects.erase(iter);

        m_SearchFunc(&m_PercetionOutDesc);
    }
}

CPerception* CPerception::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CPerception* pPercetion = new CPerception(pDevice, pContext);
    if (FAILED(pPercetion->Initialize_Prototype()))
    {
        Safe_Release(pPercetion);
        MSG_BOX("Create Fail : Percetion");
    }
    return pPercetion;
}

CComponent* CPerception::Clone(void* pArg)
{
    CPerception* pPercetion = new CPerception(*this);
    if (FAILED(pPercetion->Initialize(pArg)))
    {
        Safe_Release(pPercetion);
        MSG_BOX("Clone Fail : Percetion");
    }
    return pPercetion;
}

void CPerception::Free()
{
    __super::Free();

    Safe_Release(m_pTargetSearchCol);
}
