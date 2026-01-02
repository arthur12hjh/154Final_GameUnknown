#include "pch.h"
#include "Note.h"

#include "GameInstance.h"
#include "RimLight.h"

CNote::CNote(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) :
    CGameObject(pDevice, pContext)
{
}

CNote::CNote(const CNote& Prototype) :
    CGameObject({Prototype})
{
}

HRESULT CNote::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CNote::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    NOTE_DESC* pDsec = static_cast<NOTE_DESC*>(pArg);
    m_vTargetPoint = pDsec->vTargetPoint;
    if (FAILED(Ready_Components()))
        return E_FAIL;

    // 여기서 램덤으로 방향을 6방향중에서 선택
    SettingNoteDirection();
    return S_OK;
}

void CNote::Priority_Update(_float fTimeDelta)
{
    __super::Priority_Update(fTimeDelta);
}

void CNote::Update(_float fTimeDelta)
{
    m_pColliderCom->UpdateColiision(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));

    _vector vPos = m_pTransformCom->Get_State(STATE::POSITION);
    _vector vTargetPos = XMLoadFloat3(&m_vTargetPoint);

    _float fDistance = XMVectorGetX(XMVector3Length(vTargetPos - vPos));
    if (1.f < fDistance)
        m_pTransformCom->Set_State(STATE::POSITION ,XMVectorLerp(vPos, vTargetPos, fTimeDelta * m_fNoteSpeed));

    //m_pModelCom->Set_Animation("N_Dororong_Idle");
    m_pModelCom->Play_Animation(fTimeDelta);
    __super::Update(fTimeDelta);
}

void CNote::Late_Update(_float fTimeDelta)
{
    if (m_bIsActive == TRUE)
    {
        m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
    }

    m_pGameInstance->ADD_Collider(m_pColliderCom);
    __super::Late_Update(fTimeDelta);
}

HRESULT CNote::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;
    _uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (size_t i = 0; i < iNumMeshes; i++)
    {
        if (FAILED(m_pModelCom->Bind_BoneSRV(i, m_pShaderCom, "g_BoneMatrixBuffer")))
            return E_FAIL;

        if (FAILED(m_pModelCom->Bind_Material(i, m_pShaderCom, "g_DiffuseTexture", aiTextureType_DIFFUSE, 0)))
            return E_FAIL;

        if (FAILED(m_pModelCom->Bind_Material(i, m_pShaderCom, "g_ORMTexture", aiTextureType_METALNESS, 0)))
            return E_FAIL;

        if (FAILED(m_pModelCom->Bind_Material(i, m_pShaderCom, "g_NormalTexture", aiTextureType_NORMALS, 0)))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(0)))
            return E_FAIL;

        if (FAILED(m_pModelCom->Render(i)))
            return E_FAIL;
    }

    return S_OK;
}

HRESULT CNote::Ready_Components()
{
    /* Com_Model */
    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::BEATSABER_GAME), TEXT("Prototype_Component_Model_Dororong"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    /* Com_Shader */
    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* Com_ColliderBox */
    CBoxCollider::BOX_COLLIDER_DESC BoxColliderDesc = {};
    BoxColliderDesc.vCenter = {};
    BoxColliderDesc.vSize = { 1.f,1.f,1.f };

    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Collider_AABB"),
        TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &BoxColliderDesc)))
        return E_FAIL;

    m_pColliderCom->SetColliderHitType(HIT_TYPE::INTERACTION);
    m_pColliderCom->ADD_IgnoreObjectType(HIT_TYPE::INTERACTION);
    m_pColliderCom->ADD_IgnoreObjectType(HIT_TYPE::SENCE);

    /* Com_RimLight */
    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_RimLight"),
        TEXT("Com_RimLight"), reinterpret_cast<CComponent**>(&m_pRimLight), &m_RimLightDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CNote::Bind_ShaderResources()
{
    if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float4))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldMatrixPtr())))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
        return E_FAIL;

    return S_OK;
}

void CNote::SettingNoteDirection()
{
    _uint iIndexRandom = _uint(m_pGameInstance->Random(0.f, 120.f) / 20.f);

    switch (iIndexRandom)
    {
    case 0 :
        m_NoteData.eDirection = DIRECTION::LEFT;
        m_pModelCom->Set_Animation("N_Dororong_Evade", false, 1.f, 0.12f, false, 10.f, 10.f);
        m_NoteData.vBoundAnimFrame = { 8.f, 10.f };
        break;
    case 1:
        m_NoteData.eDirection = DIRECTION::RIGHT;
        m_pModelCom->Set_Animation("N_Dororong_Evade", false, 1.f, 0.12f, false, 30.f, 30.f);
        m_NoteData.vBoundAnimFrame = { 28.f, 30.f };
        break;
    case 2:
        m_NoteData.eDirection = DIRECTION::RIGHT_FRONT;
        m_pModelCom->Set_Animation("N_Dororong_Exhaust_Evade", false, 1.f, 0.12f, false, 40.f, 40.f);
        m_NoteData.vBoundAnimFrame = { 38.f, 40.f };
        break;
    case 3:
        m_NoteData.eDirection = DIRECTION::LEFT_FRONT;
        m_pModelCom->Set_Animation("N_Dororong_Evade", false, 1.f, 0.12f, false, 40.f, 40.f);
        m_NoteData.vBoundAnimFrame = { 38.f, 40.f };
        break;
    case 4:
        m_NoteData.eDirection = DIRECTION::LEFT_BACK;
        m_pModelCom->Set_Animation("N_Dororong_Exhaust_Evade", false, 1.f, 0.12f, false, 10.f, 10.f);
        m_NoteData.vBoundAnimFrame = { 8.f, 10.f };
        break;
    case 5:
        m_NoteData.eDirection = DIRECTION::RIGHT_BACK;
        m_pModelCom->Set_Animation("N_Dororong_Exhaust_Evade", false, 1.f, 0.12f, false, 30.f, 30.f);
        m_NoteData.vBoundAnimFrame = { 28.f, 30.f };
        break;
    }
}

CNote* CNote::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CNote* pNote = new CNote(pDevice, pContext);
    if (FAILED(pNote->Initialize_Prototype()))
    {
        Safe_Release(pNote);
        MSG_BOX("Create FAil : Note");
    }
    return pNote;
}

CGameObject* CNote::Clone(void* pArg)
{
    CNote* pNote = new CNote(*this);
    if (FAILED(pNote->Initialize(pArg)))
    {
        Safe_Release(pNote);
        MSG_BOX("Clone FAil : Note");
    }
    return pNote;
}

void CNote::Free()
{
    __super::Free();

    Safe_Release(m_pModelCom);
    Safe_Release(m_pColliderCom);
    Safe_Release(m_pRimLight);
    Safe_Release(m_pShaderCom);
}
