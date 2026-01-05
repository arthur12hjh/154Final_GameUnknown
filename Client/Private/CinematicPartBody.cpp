#include "pch.h"
#include "CinematicPartBody.h"

#include "GameInstance.h"
#include "RimLight.h"

CCinematicPartBody::CCinematicPartBody(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) :
    CPartObject(pDevice, pContext)
{
}

CCinematicPartBody::CCinematicPartBody(const CCinematicPartBody& Prototype) :
    CPartObject(Prototype)
{
}

HRESULT CCinematicPartBody::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CCinematicPartBody::Initialize(void* pArg)
{
    BODY_CINEMATIC_DESC* pDesc = static_cast<BODY_CINEMATIC_DESC*>(pArg);

    m_szModelTag = pDesc->szModelTag;
    m_pSocketMatrix = const_cast<_float4x4*>(pDesc->pSocketMatrix);

    m_bIsAnim = pDesc->isAnim;

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    if (pDesc->isSetTransform)
    {
        m_pTransformCom->Set_State(STATE::POSITION, XMVectorSetW(XMLoadFloat3(&pDesc->vPartPosition), 1.f));
        m_pTransformCom->Rotation(XMConvertToRadians(pDesc->vPartRotation.x),
            XMConvertToRadians(pDesc->vPartRotation.y),
            XMConvertToRadians(pDesc->vPartRotation.z));
        m_pTransformCom->Set_Scale(XMVectorSetW(XMLoadFloat3(&pDesc->vPartScale), 1.f));
	}

    m_pModelCom->Bind_MaterialTag(TEXTURE_TYPE::DIFFUSE, "g_DiffuseTexture");

    if(TEXT("Prototype_Component_Model_Cinematic_Dororong") != m_szModelTag)
        m_pModelCom->Bind_MaterialTag(TEXTURE_TYPE::NORMAL, "g_NormalTexture");

    //m_pModelCom->Bind_MaterialTag(TEXTURE_TYPE::EMISSIVE, "g_EmissiveTexture");
    m_pModelCom->Bind_MaterialTag(TEXTURE_TYPE::ORM, "g_ORMTexture");

    return S_OK;
}

void CCinematicPartBody::Priority_Update(_float fTimeDelta)
{
    //m_pTransformCom->Set_Scale(XMVectorSet(1.f, 1.f, 1.f, 1.f));

    //m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(0.f, 0.f, 0.5f, 1.f));

    Update_PreCombinedMatrix();
}

void CCinematicPartBody::Update(_float fTimeDelta)
{
    if (m_pSocketMatrix == nullptr)
    {
        XMStoreFloat4x4(&m_CombinedWorldMatrix,
            XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()) * XMLoadFloat4x4(m_pParentTransformCom->Get_WorldMatrixPtr()));
    }
    else
    {
        _matrix		SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);

        for (size_t i = 0; i < 3; i++)
            SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);

        XMStoreFloat4x4(&m_CombinedWorldMatrix,
            XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()) * SocketMatrix * XMLoadFloat4x4(m_pParentTransformCom->Get_WorldMatrixPtr()));
    }
}

void CCinematicPartBody::Late_Update(_float fTimeDelta)
{
}

HRESULT CCinematicPartBody::Render()
{
    if (m_bIsActive == FALSE)
        return S_OK;

    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _uint		iNumMeshes = m_pModelCom->Get_NumMeshes();
    
    if (m_bIsAnim)
    {
        for (size_t i = 0; i < iNumMeshes; i++)
        {
            if (FAILED(m_pModelCom->Bind_BoneSRV(i, m_pShaderCom, "g_BoneMatrixBuffer")))
                return E_FAIL;

            if (FAILED(m_pModelCom->Bind_AllMaterials(i, m_pShaderCom, 0)))
                return E_FAIL;

            if (FAILED(m_pShaderCom->Begin(9)))
                return E_FAIL;

            if (FAILED(m_pModelCom->Render(i)))
                return E_FAIL;
        }
    }
    else
    {
        for (size_t i = 0; i < iNumMeshes; i++)
        {
            if (FAILED(m_pModelCom->Bind_AllMaterials(i, m_pShaderCom, 0)))
                return E_FAIL;

            if (FAILED(m_pShaderCom->Begin(0)))
                return E_FAIL;

            if (FAILED(m_pModelCom->Render(i)))
                return E_FAIL;
        }
    }

    //m_pGameInstance->Render_Text(TEXT("KoPub"), m_szRotationAngle, _float2(g_iWinSizeX / 2 - 180, 0), XMVectorSet(1.f, 1.f, 1.f, 0.1f));

    return S_OK;
}

HRESULT CCinematicPartBody::Render_Shadow()
{
    if (m_bIsActive == FALSE)
        return S_OK;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Bind_Shadow_Resource_Cascade(m_pShaderCom, "g_LightViewMatrix", D3DTS::VIEW)))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Bind_Shadow_Resource_Cascade(m_pShaderCom, "g_LightProjMatrix", D3DTS::PROJ)))
        return E_FAIL;

    _uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

    if (m_bIsAnim)
    {
        for (size_t i = 0; i < iNumMeshes; i++)
        {
            if (FAILED(m_pModelCom->Bind_BoneSRV(i, m_pShaderCom, "g_BoneMatrixBuffer")))
                return E_FAIL;

            if (FAILED(m_pShaderCom->Begin(1)))
                return E_FAIL;

            if (FAILED(m_pModelCom->Render(i)))
                return E_FAIL;
        }
    }
    else
    {
        for (size_t i = 0; i < iNumMeshes; i++)
        {
            if (FAILED(m_pShaderCom->Begin(6)))
                return E_FAIL;

            if (FAILED(m_pModelCom->Render(i)))
                return E_FAIL;
        }
    }


    return S_OK;
}

HRESULT CCinematicPartBody::Render_MotionBlur()
{
    /* 이전 프레임 월드매트릭스도 바인딩 */
    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_PreWorldMatrix", &m_PreCombinedWorldMatrix)))
        return E_FAIL;

    /* 이전 뷰 매트릭스도 바인딩 */
    if (FAILED(m_pShaderCom->Bind_Matrix("g_PreViewMatrix", m_pGameInstance->Get_PreTransform_Float4x4(D3DTS::VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
        return E_FAIL;

    _uint		iNumMeshes = m_pModelCom->Get_NumMeshes();


    if (m_bIsAnim)
    {
        for (size_t i = 0; i < iNumMeshes; i++)
        {
            if (FAILED(m_pModelCom->Bind_BoneSRV(i, m_pShaderCom, "g_BoneMatrixBuffer")))
                return E_FAIL;

            if (FAILED(m_pShaderCom->Begin(4)))
                return E_FAIL;

            if (FAILED(m_pModelCom->Render(i)))
                return E_FAIL;
        }
    }
    else
    {

    }

    return S_OK;
}

void CCinematicPartBody::Reset_SocketMatrix(_float4x4* pSocketMatrix)
{
	m_pSocketMatrix = pSocketMatrix;
}

HRESULT CCinematicPartBody::Bind_ShaderResources()
{
    if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float4))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
        return E_FAIL;

    return S_OK;
}

HRESULT CCinematicPartBody::Ready_Components()
{
    /* Com_Model */
    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), m_szModelTag,
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    if (m_bIsAnim)
    {
        /* Com_Shader */
        if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
            TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
            return E_FAIL;
    }
    else
    {
        /* Com_Shader */
        if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Shader_VtxMesh"),
            TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
            return E_FAIL;
    }

    return S_OK;
}

CCinematicPartBody* CCinematicPartBody::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CCinematicPartBody* pCinematicPartBody = new CCinematicPartBody(pDevice, pContext);
    if (FAILED(pCinematicPartBody->Initialize_Prototype()))
    {
        Safe_Release(pCinematicPartBody);
        MSG_BOX("Create Fail : Cinematic Part Body");
    }
    return pCinematicPartBody;
}

CGameObject* CCinematicPartBody::Clone(void* pArg)
{
    CCinematicPartBody* pCinematicPartBody = new CCinematicPartBody(*this);
    if (FAILED(pCinematicPartBody->Initialize(pArg)))
    {
        Safe_Release(pCinematicPartBody);
        MSG_BOX("Clones Fail : Cinematic Part Body");
    }
    return pCinematicPartBody;
}

void CCinematicPartBody::Free()
{
    __super::Free();
    
}
