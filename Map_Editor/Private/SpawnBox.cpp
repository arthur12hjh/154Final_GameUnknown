#include "pch.h"
#include "SpawnBox.h"
#include "MapTool_Desert.h"
#include "GameInstance.h"

CSpawnBox::CSpawnBox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) :
    CGameObject(pDevice, pContext)
{
}

CSpawnBox::CSpawnBox(const CSpawnBox& Prototype) :
    CGameObject(Prototype)
{
}

HRESULT CSpawnBox::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CSpawnBox::Initialize(void* pArg)
{
    
    CMapTool_Desert::NAYITBA_DESC* Desc = nullptr;

    if (pArg != nullptr)
    {
        Desc = reinterpret_cast<CMapTool_Desert::NAYITBA_DESC*>(pArg);
    }

    if(FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if(FAILED(Ready_Components()))
        return E_FAIL;

	m_iMonsterID = Desc->iMonsterID;


    return S_OK;
}

void CSpawnBox::Priority_Update(_float fTimeDelta)
{
}

void CSpawnBox::Update(_float fTimeDelta)
{
    _matrix WorldMat = XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr());
    //m_pCullingCollider->UpdateColiision(WorldMat);
    m_pCollider->UpdateColiision(WorldMat);
}

void CSpawnBox::Late_Update(_float fTimeDelta)
{
    /*if (m_pGameInstance->isIn_WorldFrustum(m_pCullingCollider))
    {*/
        m_pGameInstance->Add_DebugComponent(m_pCollider);
        //m_pGameInstance->Add_DebugComponent(m_pCullingCollider);
    //}
}

HRESULT CSpawnBox::Render()
{
    return S_OK;
}

void CSpawnBox::SetSpawnPrototypeName(const WCHAR* szPrototypeName)
{
    m_szPrototypeName = szPrototypeName;
}

const WCHAR* CSpawnBox::GetSpawnObjectName()
{
    return m_szPrototypeName;
}

HRESULT CSpawnBox::Ready_Components()
{
    //COBBCollider::OBB_COLLIDER_DESC ObbDesc = {};
    //ObbDesc.vSize = {1.f, 1.f, 1.f};

    ///* Com_Shader */
    //if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::DESERT), TEXT("Prototype_Component_Collider_OBB"),
    //    TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pCollider))))
    //    return E_FAIL;

    //static_cast<COBBCollider*>(m_pCullingCollider)->SetCollision({}, {}, ObbDesc.vSize);

    /* Com_Collider_Sphere */
    /* Com_Collider_OBB */
    COBBCollider::OBB_COLLIDER_DESC		OBBDesc{};
    OBBDesc.vSize = _float3(1.f, 1.f, 1.f);
    OBBDesc.vCenter = _float3(0.f, OBBDesc.vSize.y, 0.f);

    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::DESERT), TEXT("Prototype_Component_Collider_OBB"),
        TEXT("Com_Collider_OBB"), reinterpret_cast<CComponent**>(&m_pCollider), &OBBDesc)))
        return E_FAIL;

    return S_OK;
}

CSpawnBox* CSpawnBox::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CSpawnBox* pSpawnBox = new CSpawnBox(pDevice, pContext);
    if (FAILED(pSpawnBox->Initialize_Prototype()))
    {
        Safe_Release(pSpawnBox);
        MSG_BOX("Create Fail : Spawn Box");
    }
    return pSpawnBox;
}

CGameObject* CSpawnBox::Clone(void* pArg)
{
    CSpawnBox* pSpawnBox = new CSpawnBox(*this);
    if (FAILED(pSpawnBox->Initialize(pArg)))
    {
        Safe_Release(pSpawnBox);
        MSG_BOX("Create Fail : Spawn Box");
    }
    return pSpawnBox;
}

void CSpawnBox::Free()
{
    __super::Free();

    Safe_Release(m_pCollider);
}
