#include "pch.h"
#include "SoundTriggerBox.h"
#include "GameInstance.h"

CSoundTriggerBox::CSoundTriggerBox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) :
    CGameObject(pDevice, pContext)
{
}

CSoundTriggerBox::CSoundTriggerBox(const CSoundTriggerBox& Prototype) :
    CGameObject(Prototype)
{
}

HRESULT CSoundTriggerBox::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CSoundTriggerBox::Initialize(void* pArg)
{

    SOUNDTRIGGER_BOX_DESC* pDesc = static_cast<SOUNDTRIGGER_BOX_DESC*>(pArg);

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_eGroundSoundType = pDesc->eSoundBoxType;

    return S_OK;
}

void CSoundTriggerBox::Priority_Update(_float fTimeDelta)
{
}

void CSoundTriggerBox::Update(_float fTimeDelta)
{
    _matrix WorldMat = XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr());
    //m_pCullingCollider->UpdateColiision(WorldMat);
    m_pCollider->UpdateColiision(WorldMat);
}

void CSoundTriggerBox::Late_Update(_float fTimeDelta)
{
    /*if (m_pGameInstance->isIn_WorldFrustum(m_pCullingCollider))
    {*/
#ifdef _DEBUG
    m_pGameInstance->Add_DebugComponent(m_pCollider);
#endif
    //m_pGameInstance->Add_DebugComponent(m_pCullingCollider);
//}
}

HRESULT CSoundTriggerBox::Render()
{
    return S_OK;
}

HRESULT CSoundTriggerBox::Ready_Components()
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

    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_Component_Collider_OBB"),
        TEXT("Com_Collider_OBB"), reinterpret_cast<CComponent**>(&m_pCollider), &OBBDesc)))
        return E_FAIL;

    return S_OK;
}

CSoundTriggerBox* CSoundTriggerBox::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CSoundTriggerBox* pSoundTriggerBox = new CSoundTriggerBox(pDevice, pContext);
    if (FAILED(pSoundTriggerBox->Initialize_Prototype()))
    {
        Safe_Release(pSoundTriggerBox);
        MSG_BOX("Create Fail : SoundTriggerBox");
    }
    return pSoundTriggerBox;
}

CGameObject* CSoundTriggerBox::Clone(void* pArg)
{
    CSoundTriggerBox* pSoundTriggerBox = new CSoundTriggerBox(*this);
    if (FAILED(pSoundTriggerBox->Initialize(pArg)))
    {
        Safe_Release(pSoundTriggerBox);
        MSG_BOX("Create Fail : SoundTriggerBox");
    }
    return pSoundTriggerBox;
}

void CSoundTriggerBox::Free()
{
    __super::Free();

    Safe_Release(m_pCollider);
}
