#include "pch.h"
#include "Actor.h"

#include "GameInstance.h"

CActor::CActor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) :
    CGameObject(pDevice, pContext)
{
}

CActor::CActor(const CActor& Prototype) :
    CGameObject(Prototype)
{
}

HRESULT CActor::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CActor::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    m_pCullingCollider->SetColliderHitType(HIT_TYPE::STATIC);
    return S_OK;
}

void CActor::Priority_Update(_float fTimeDelta)
{
}

void CActor::Update(_float fTimeDelta)
{
}

void CActor::Late_Update(_float fTimeDelta)
{
    __super::Late_Update(fTimeDelta);
}

HRESULT CActor::Render()
{
    return S_OK;
}

CGameObject* CActor::Clone(void* pArg)
{
    return nullptr;
}

void CActor::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pRigidBody);
    Safe_Release(m_pColModelCom);
}
