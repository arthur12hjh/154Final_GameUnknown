#include "Collider.h"

#include "GameInstance.h"
#include "GameObject.h"

CCollider::CCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) :
    CComponent(pDevice, pContext)
{
}

CCollider::CCollider(const CCollider& rhs) :
    CComponent(rhs),
    m_CollisionType(rhs.m_CollisionType)
#ifdef _DEBUG
    , m_pBatch{ rhs.m_pBatch }
    , m_pEffect{ rhs.m_pEffect }
    , m_pInputLayout{ rhs.m_pInputLayout }
#endif
{
#ifdef _DEBUG
    Safe_AddRef(m_pInputLayout);
#endif
}

HRESULT CCollider::Initialize_Prototype()
{
#ifdef _DEBUG
    m_pBatch = new PrimitiveBatch<VertexPositionColor>(m_pContext);
    m_pEffect = new BasicEffect(m_pDevice);
    m_pEffect->SetVertexColorEnabled(true);

    const void* pShaderByteCode = { nullptr };
    size_t		iShaderByteCodeLength = {};

    m_pEffect->GetVertexShaderBytecode(&pShaderByteCode, &iShaderByteCodeLength);

    if (m_pDevice->CreateInputLayout(VertexPositionColor::InputElements, VertexPositionColor::InputElementCount,
        pShaderByteCode, iShaderByteCodeLength, &m_pInputLayout))
        return E_FAIL;
#endif

    return S_OK;
}

HRESULT CCollider::Initialize(void* pArg)
{

    return S_OK;
}

void CCollider::UpdateColiision(_matrix WorldMatrix)
{
  
}

_bool CCollider::RayHit(_vector vOrizin, _vector vDiraction, DEFAULT_HIT_DESC& OutDesc)
{
    return _bool();
}

_bool CCollider::FrustomIntersect(const BoundingFrustum& Frustom)
{
    return _bool();
}

ContainmentType CCollider::Contains(_vector Point)
{
    return ContainmentType();
}

#ifdef _DEBUG
HRESULT CCollider::Render()
{
    m_pEffect->SetWorld(XMMatrixIdentity());
    m_pEffect->SetView(XMLoadFloat4x4(m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW)));
    m_pEffect->SetProjection(XMLoadFloat4x4(m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ)));

    m_pContext->IASetInputLayout(m_pInputLayout);
    m_pEffect->Apply(m_pContext);
    return S_OK;
}

HRESULT CCollider::Render(_float4 vColor)
{
    return Render();
}
#endif // _DEBUG

void CCollider::BindBeginOverlapEvent(function<void(_float3 vHitPoint, _float3 vHitDir, CGameObject* pHitActor)> BeginEvent)
{
    m_BeginHitFunc = BeginEvent;
}

void CCollider::BindOverlappingEvent(function<void(_float3 vHitPoint, _float3 vHitDir, CGameObject* pHitActor)> OverlappingEvent)
{
    m_OverlapHitFunc = OverlappingEvent;
}

void CCollider::BindEndOverlapEvent(function<void(_float3 vHitPoint, _float3 vHitDir, CGameObject* pHitActor)> EndEvent)
{
    m_EndHitFunc = m_EndHitFunc;
}

void CCollider::ADD_HitObejct(CGameObject* pObject)
{
    auto iter = find(m_HitList.begin(), m_HitList.end(), pObject);
    if (iter == m_HitList.end())
    {
        m_HitList.push_back(pObject);
        m_bIsHit = true;
    }
}

void CCollider::ADD_IgnoreObejct(HIT_TYPE typeID)
{
    m_IgnoreObject.insert(typeID);
}

void CCollider::ADD_OnlyHitObject(HIT_TYPE typeID)
{
    m_eOnlyHitType = typeID;
}

void CCollider::CallFunction()
{
    list<CGameObject*> ExitObject;
    for (auto& HitObject : m_HitList)
    {
        auto iter = find(m_OldHitList.begin(), m_OldHitList.end(), HitObject);
        if (iter == m_OldHitList.end())
        {
            if (m_BeginHitFunc)
                m_BeginHitFunc(m_HitDesc.vHitPoint, m_HitDesc.vDireaction, HitObject);
        }
        else
        {
            if (m_OverlapHitFunc)
                m_OverlapHitFunc(m_HitDesc.vHitPoint, m_HitDesc.vDireaction, HitObject);
        }
    }

    for (auto& OldHitObject : m_OldHitList)
    {
        auto iter = find(m_HitList.begin(), m_HitList.end(), OldHitObject);
        if (iter == m_HitList.end())
        {
            if (m_EndHitFunc)
                m_EndHitFunc(m_HitDesc.vHitPoint, m_HitDesc.vDireaction, OldHitObject);
        }
    }

    m_OldHitList = m_HitList;
    m_HitList.clear();
}

void CCollider::ResetCollision()
{
    m_OldHitList.clear();
    m_HitList.clear();
}

_bool CCollider::IntersectAble(HIT_TYPE TagetTypeHashCode)
{
    auto iter = m_IgnoreObject.find(TagetTypeHashCode);
    if (iter == m_IgnoreObject.end())
        return true;

    return false;
}

CComponent* CCollider::Clone(void* pArg)
{
    return nullptr;
}

void CCollider::Free()
{
    __super::Free();

#ifdef _DEBUG
    if (false == m_isCloned)
    {
        Safe_Delete(m_pBatch);
        Safe_Delete(m_pEffect);
    }

    Safe_Release(m_pInputLayout);
#endif // _DEBUG

    m_OldHitList.clear();
    m_HitList.clear();
}