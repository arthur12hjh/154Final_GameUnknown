#include "pch.h"
#include "NaytibaBeam_Part.h"

#include "GameInstance.h"
#include "Character.h"

CNaytibaBeam_Part::CNaytibaBeam_Part(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) :
    CPartObject(pDevice, pContext)
{
}

CNaytibaBeam_Part::CNaytibaBeam_Part(const CNaytibaBeam_Part& Prototype) :
    CPartObject(Prototype)
{
}

HRESULT CNaytibaBeam_Part::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CNaytibaBeam_Part::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    BEAM_DESC* pDesc = static_cast<BEAM_DESC*>(pArg);
    m_pSocketMatrix = pDesc->pSocketMatrix;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_eVisibility = VISIBILITY::HIDDEN;
    return S_OK;
}

void CNaytibaBeam_Part::Priority_Update(_float fTimeDelta)
{
}

void CNaytibaBeam_Part::Update(_float fTimeDelta)
{
    _matrix SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);
    for (_uint i = 0; i < 3; ++i)
        SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);

    XMStoreFloat4x4(&m_CombinedWorldMatrix,
         XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()) * SocketMatrix * XMLoadFloat4x4(m_pParentTransformCom->Get_WorldMatrixPtr()));
}

void CNaytibaBeam_Part::Late_Update(_float fTimeDelta)
{
    if (VISIBILITY::VISIBLE == m_eVisibility)
    {
        m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
        m_pGameInstance->Add_RenderGroup(RENDER::BLUR, this);
    }
}

HRESULT CNaytibaBeam_Part::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _uint		iNumMeshes = m_pModelCom->Get_NumMeshes();
    for (size_t i = 0; i < iNumMeshes; i++)
    {
        if (FAILED(m_pModelCom->Bind_AllMaterials(i, m_pShaderCom, 0)))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(0)))
            return E_FAIL;

        if (FAILED(m_pModelCom->Render(i)))
            return E_FAIL;
    }
    return S_OK;
}

HRESULT CNaytibaBeam_Part::Ready_Components()
{
    /* Com_Model */
    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_Cylinder"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    /* Com_Shader */
    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Shader_BeamShader"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CNaytibaBeam_Part::Bind_ShaderResources()
{
    _bool bFlag = static_cast<CCharacter*>(m_pParent)->Get_DepthMaskingB();
    if (FAILED(m_pShaderCom->Bind_RawValue("g_IsMaskingDepthB", &bFlag, sizeof(_bool))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_IsMaskingDepthW", &bFlag, sizeof(_bool))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
        return E_FAIL;

    _float4 vColor = { 1.f, 0.f, 0.f, 1.f };
    if (FAILED(m_pShaderCom->Bind_RawValue("g_vBeamColor", &vColor, sizeof(_float4))))
        return E_FAIL;

    return S_OK;
}

CNaytibaBeam_Part* CNaytibaBeam_Part::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CNaytibaBeam_Part* pBeamPart = new CNaytibaBeam_Part(pDevice, pContext);
    if (FAILED(pBeamPart->Initialize_Prototype()))
    {
        Safe_Release(pBeamPart);
        MSG_BOX("Create Fail : Beam Part");
    }
    return pBeamPart;
}

CGameObject* CNaytibaBeam_Part::Clone(void* pArg)
{
    CNaytibaBeam_Part* pBeamPart = new CNaytibaBeam_Part(*this);
    if (FAILED(pBeamPart->Initialize(pArg)))
    {
        Safe_Release(pBeamPart);
        MSG_BOX("Clone Fail : Beam Part");
    }
    return pBeamPart;
}

void CNaytibaBeam_Part::Free()
{
    __super::Free();
}
