#include "pch.h"
#include "Spawner.h"

#include "GameInstance.h"
#include "GameManager.h"

#include "Nayitba.h"
#include "Player.h"

CSpawner::CSpawner(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) 
    : CCharacter(pDevice, pContext)
{
}

CSpawner::CSpawner(const CSpawner& rhs)
    : CCharacter(rhs)
{
}

HRESULT CSpawner::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CSpawner::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    SPAWNER_DESC* pDesc = static_cast<SPAWNER_DESC*>(pArg);
    if (FAILED(Ready_Spawner(*pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    _matrix WorldMatrix = XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr());
    m_pColliderCom->UpdateColiision(WorldMatrix);

    ADD_SpawnList(7, 10);
    return S_OK;
}

void CSpawner::Priority_Update(_float fTimeDelta)
{
}

void CSpawner::Update(_float fTimeDelta)
{
    m_fTimeDelta = fTimeDelta;
    if (m_pGameInstance->isIn_WorldFrustum(m_pColliderCom))
    {
        m_pGameInstance->ADD_Collider(m_pColliderCom);

#ifdef _DEBUG
        m_pGameInstance->Add_DebugComponent(m_pColliderCom);
#endif
    }
}

void CSpawner::Late_Update(_float fTimeDelta)
{
}

void CSpawner::ADD_SpawnList(_uint iSpawnID, _uint iSpawnCnt)
{
    auto iter = find_if(m_SpawnList.begin(), m_SpawnList.end(), [&](auto& pair)
        {
            return pair.first == iSpawnID ? true : false;
        });

    if (iter == m_SpawnList.end())
    {
        m_SpawnList.emplace_back(iSpawnID, iSpawnCnt);
    }
}

HRESULT CSpawner::Ready_Spawner(SPAWNER_DESC& Desc)
{
    m_eSpawnerType = Desc.eType;
    if(SPAWNER_TYPE::TIME == m_eSpawnerType)
    {
        m_eTimeSpanwerType = m_eTimeSpanwerType;
        m_SpawnTime = { 0.f, Desc.fSpawnTime };

        if (TIME_SPANWER_TYPE::LOOP == m_eTimeSpanwerType)
        {
            m_iLoopCount = Desc.fLoopCount;
        }
    }

    return S_OK;
}

HRESULT CSpawner::Ready_Components()
{
    /* Com_Collider_AABB */
    CBoxCollider::BOX_COLLIDER_DESC		AABBDesc{};
    AABBDesc.vSize = { 1.f,1.f,1.f };
    AABBDesc.vCenter = _float3(0.f, AABBDesc.vSize.y * 0.5f, 0.f);

    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Collider_AABB"),
        TEXT("Com_Collider_AABB"), reinterpret_cast<CComponent**>(&m_pColliderCom), &AABBDesc)))
        return E_FAIL;

    m_pColliderCom->SetColliderHitType(HIT_TYPE::MONSTER);
    m_pColliderCom->SetColliderHitType(HIT_TYPE::PLAYER);
    m_pColliderCom->BindBeginOverlapEvent([&](_float3 vHitPoint, _float3 vHitDir, CGameObject* pHitActor) { Trigger_Event(); });
    return S_OK;
}

void CSpawner::Trigger_Event()
{
    switch (m_eSpawnerType)
    {
    case SPAWNER_TYPE::TRIGGER:
        Trigger_SpawnEvent();
        Set_Dead(true);
        break;
    }
}

void CSpawner::Trigger_SpawnEvent()
{
    CNayitba::NAYITBA_DESC SpawnDesc = {};
    SpawnDesc.bIsApplyTransform = true;
    SpawnDesc.vScale = { 1.f, 1.f, 1.f };

    CBoxCollider* HitBox = static_cast<CBoxCollider*>(m_pColliderCom);
    auto HitBoxBound = HitBox->GetBounding();

    _float3 vHitBoxCenter = HitBoxBound.Center;
    _float3 vHitBoxExtents = HitBoxBound.Extents;
    auto pPlayer = m_pGameManager->GetGameCharacter();
    for (auto& pair : m_SpawnList)
    {
        SpawnDesc.iMonsterID = pair.first;
        for (_uint i = 0; i < pair.second; ++i)
        {
            SpawnDesc.vPosition = {
                m_pGameInstance->Random(vHitBoxCenter.x - vHitBoxExtents.x, vHitBoxCenter.x + vHitBoxExtents.x),
                vHitBoxCenter.y + 0.5f,
                m_pGameInstance->Random(vHitBoxCenter.z - vHitBoxExtents.z, vHitBoxCenter.z + vHitBoxExtents.z),
            };
            SpawnDesc.vRotation = {0.f, XMConvertToRadians(m_pGameInstance->Random(0.f, 360.f)), 0.f, 0.f};
            SpawnDesc.pTarget = pPlayer;

            auto pGameObject = m_pGameManager->SetActivePoolObject(ENUM_CLASS(LEVEL::GAMEPLAY), ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_Monster"), TEXT("Monster_Pool"));
            static_cast<CNayitba*>(pGameObject)->Setting_Data(m_fTimeDelta, SpawnDesc);
        }
    }

    Safe_Release(pPlayer);
}

CSpawner* CSpawner::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CSpawner* pSpawner = new CSpawner(pDevice, pContext);
    if (FAILED(pSpawner->Initialize_Prototype()))
    {
        Safe_Release(pSpawner);
        MSG_BOX("Create Fail : Spawner");
    }
    return pSpawner;
}

CGameObject* CSpawner::Clone(void* pArg)
{
    CSpawner* pSpawner = new CSpawner(*this);
    if (FAILED(pSpawner->Initialize(pArg)))
    {
        Safe_Release(pSpawner);
        MSG_BOX("Create Fail : Spawner");
    }
    return pSpawner;
}

void CSpawner::Free()
{
    __super::Free();

    Safe_Release(m_pColliderCom);
}
