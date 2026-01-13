#include "pch.h"
#include "CameraTriggerBox.h"
#include "GameInstance.h"

CCameraTriggerBox::CCameraTriggerBox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) :
    CGameObject(pDevice, pContext)
{
}

CCameraTriggerBox::CCameraTriggerBox(const CCameraTriggerBox& Prototype) :
    CGameObject(Prototype)
{
}

HRESULT CCameraTriggerBox::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CCameraTriggerBox::Initialize(void* pArg)
{

    CAMERA_TRIGGER_BOX_DESC* pDesc = static_cast<CAMERA_TRIGGER_BOX_DESC*>(pArg);

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_eCameraType = pDesc->eCameraType;

    return S_OK;
}

void CCameraTriggerBox::Priority_Update(_float fTimeDelta)
{
}

void CCameraTriggerBox::Update(_float fTimeDelta)
{
    //_matrix WorldMat = XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr());
    ////m_pCullingCollider->UpdateColiision(WorldMat);
    m_pCollider->UpdateColiision(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));

    if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_L))
    {
        m_bIsRender = !m_bIsRender;
    }
}

void CCameraTriggerBox::Late_Update(_float fTimeDelta)
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

HRESULT CCameraTriggerBox::Render()
{
    if (m_bIsRender == true)
    {  
        COBBCollider* pObbCollider = static_cast<COBBCollider*>(m_pCollider);
        pObbCollider->Render_Face(_float4(1.f, 1.f, 0.f, 1.f));
    }

    return S_OK;
}

HRESULT CCameraTriggerBox::Ready_Components()
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

CCameraTriggerBox* CCameraTriggerBox::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CCameraTriggerBox* pCameraTriggerBox = new CCameraTriggerBox(pDevice, pContext);
    if (FAILED(pCameraTriggerBox->Initialize_Prototype()))
    {
        Safe_Release(pCameraTriggerBox);
        MSG_BOX("Create Fail : CameraTriggerBox");
    }
    return pCameraTriggerBox;
}

CGameObject* CCameraTriggerBox::Clone(void* pArg)
{
    CCameraTriggerBox* pCameraTriggerBox = new CCameraTriggerBox(*this);
    if (FAILED(pCameraTriggerBox->Initialize(pArg)))
    {
        Safe_Release(pCameraTriggerBox);
        MSG_BOX("Create Fail : CameraTriggerBox");
    }
    return pCameraTriggerBox;
}

void CCameraTriggerBox::Free()
{
    __super::Free();

    Safe_Release(m_pCollider);
}
