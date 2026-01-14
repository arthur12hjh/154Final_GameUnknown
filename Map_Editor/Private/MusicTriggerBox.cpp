#include "pch.h"
#include "MusicTriggerBox.h"
#include "GameInstance.h"

CMusicTriggerBox::CMusicTriggerBox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) :
    CGameObject(pDevice, pContext)
{
}

CMusicTriggerBox::CMusicTriggerBox(const CMusicTriggerBox& Prototype) :
    CGameObject(Prototype)
{
}

HRESULT CMusicTriggerBox::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CMusicTriggerBox::Initialize(void* pArg)
{

    MUSIC_TRIGGER_BOX_DESC* pDesc = static_cast<MUSIC_TRIGGER_BOX_DESC*>(pArg);

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    return S_OK;
}

void CMusicTriggerBox::Priority_Update(_float fTimeDelta)
{
}

void CMusicTriggerBox::Update(_float fTimeDelta)
{
    //_matrix WorldMat = XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr());
    ////m_pCullingCollider->UpdateColiision(WorldMat);
    m_pCollider->UpdateColiision(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));

    if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_L))
    {
        m_bIsRender = !m_bIsRender;
    }
}

void CMusicTriggerBox::Late_Update(_float fTimeDelta)
{
    /*if (m_pGameInstance->isIn_WorldFrustum(m_pCullingCollider))
    {*/
    m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
#ifdef _DEBUG
    m_pGameInstance->Add_DebugComponent(m_pCollider);
#endif
    //m_pGameInstance->Add_DebugComponent(m_pCullingCollider);
//}
}

HRESULT CMusicTriggerBox::Render()
{
    if (m_bIsRender == true)
    {
        COBBCollider* pObbCollider = static_cast<COBBCollider*>(m_pCollider);
        pObbCollider->Render_Face(_float4(1.f, 1.f, 0.f, 1.f));
    }

    return S_OK;
}

HRESULT CMusicTriggerBox::Ready_Components()
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

CMusicTriggerBox* CMusicTriggerBox::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CMusicTriggerBox* pMusicTriggerBox = new CMusicTriggerBox(pDevice, pContext);
    if (FAILED(pMusicTriggerBox->Initialize_Prototype()))
    {
        Safe_Release(pMusicTriggerBox);
        MSG_BOX("Create Fail : MusicTriggerBox");
    }
    return pMusicTriggerBox;
}

CGameObject* CMusicTriggerBox::Clone(void* pArg)
{
    CMusicTriggerBox* pMusicTriggerBox = new CMusicTriggerBox(*this);
    if (FAILED(pMusicTriggerBox->Initialize(pArg)))
    {
        Safe_Release(pMusicTriggerBox);
        MSG_BOX("Create Fail : MusicTriggerBox");
    }
    return pMusicTriggerBox;
}

void CMusicTriggerBox::Free()
{
    __super::Free();

    Safe_Release(m_pCollider);
}
