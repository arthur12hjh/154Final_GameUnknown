#include "pch.h"
#include "Bullet.h"

#include "GameInstance.h"
#include "GameManager.h"

CBullet::CBullet(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) :
    CPartObject(pDevice, pContext)
{
}

CBullet::CBullet(const CBullet& Prototype) :
    CPartObject(Prototype)
{
}

HRESULT CBullet::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CBullet::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    BULLET_DESC* pDesc = static_cast<BULLET_DESC*>(pArg);
    m_pSocketMatrix = pDesc->pSocketMatrix;
    m_pSkillData = CGameManager::GetInstance()->Find_SkillData(pDesc->iSkillID);

    return S_OK;
}

void CBullet::Priority_Update(_float fTimeDelta)
{

}

void CBullet::Update(_float fTimeDelta)
{

}

void CBullet::Late_Update(_float fTimeDelta)
{

}

HRESULT CBullet::Render()
{

    return S_OK;
}

void CBullet::Shoot_Projectile(_vector vDir, _float fSpeed)
{
    _matrix CombinedMatrix = XMLoadFloat4x4(&m_CombinedWorldMatrix);
    m_pTransformCom->Set_State(STATE::POSITION, CombinedMatrix.r[3]);
    XMStoreFloat3(&m_vProjectileDir, vDir);
    m_bIsAttachment = false;
    m_fSpeed = fSpeed;
}

void CBullet::Update_BulletCombinedMatrix()
{
    if (m_bIsAttachment)
    {
        if (nullptr == m_pSocketMatrix || nullptr == m_pParent)
            return;

        _matrix ParentMatrix = XMLoadFloat4x4(m_pParent->GetTransform()->Get_WorldMatrixPtr());
        _matrix SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);

        XMStoreFloat4x4(&m_CombinedWorldMatrix, SocketMatrix * ParentMatrix);
    }
    else
        memcpy(&m_CombinedWorldMatrix, m_pTransformCom->Get_WorldMatrixPtr(), sizeof(_float4x4));
}

CGameObject* CBullet::Clone(void* pArg)
{
    return nullptr;
}

void CBullet::Free()
{
    __super::Free();

    Safe_Release(m_pModelCom);
    Safe_Release(m_pColliderCom);
}
