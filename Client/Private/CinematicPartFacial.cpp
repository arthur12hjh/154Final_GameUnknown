#include "pch.h"
#include "CinematicPartFacial.h"

#include "GameInstance.h"
#include "Character.h"
#include "NayitbaPartBody.h"

CCinematicPartFacial::CCinematicPartFacial(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) :
    CPartObject(pDevice, pContext)
{
}

CCinematicPartFacial::CCinematicPartFacial(const CCinematicPartFacial& Prototype) :
    CPartObject(Prototype)
{
}

HRESULT CCinematicPartFacial::Mapping_Shader_Material(_uint iIdx)
{// idx 0 (Shader pass : 4)	
    if (strcmp(m_pModelCom->Get_MaterialName(m_pModelCom->Get_Mesh_MaterialIndex(iIdx)), "MI_CH_M_NA_961_Head") == 0)
    {
        if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(FACE_MATERIAL::MI_CH_M_NA_961_Head))))
            return E_FAIL;
    }
    // idx 1 (Shader pass : 5)	
    else if (strcmp(m_pModelCom->Get_MaterialName(m_pModelCom->Get_Mesh_MaterialIndex(iIdx)), "MI_CH_M_NA_961_Eyebrow") == 0)
    {
        if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(FACE_MATERIAL::MI_CH_M_NA_961_Eyebrow))))
            return E_FAIL;
    }
    // idx 2 (Shader pass : 6)	
    else if (strcmp(m_pModelCom->Get_MaterialName(m_pModelCom->Get_Mesh_MaterialIndex(iIdx)), "MI_CH_M_NA_961_Eyes") == 0)
    {
        if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(FACE_MATERIAL::MI_CH_M_NA_961_Eyes))))
            return E_FAIL;
    }
    // idx 3 (Shader pass : 7)	
    else if (strcmp(m_pModelCom->Get_MaterialName(m_pModelCom->Get_Mesh_MaterialIndex(iIdx)), "MI_CH_M_NA_961_Lens") == 0)
    {
        if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(FACE_MATERIAL::MI_CH_M_NA_961_Lens))))
            return E_FAIL;
    }
    // idx 4 (Shader pass : 8)	
    else if (strcmp(m_pModelCom->Get_MaterialName(m_pModelCom->Get_Mesh_MaterialIndex(iIdx)), "MI_CH_M_NA_961_Eyelashes") == 0)
    {
        if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(FACE_MATERIAL::MI_CH_M_NA_961_Eyelashes))))
            return E_FAIL;
    }
    // idx 5 (Shader pass : 9)	
    else if (strcmp(m_pModelCom->Get_MaterialName(m_pModelCom->Get_Mesh_MaterialIndex(iIdx)), "MI_CH_M_NA_961_Tearline") == 0)
    {
        if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(FACE_MATERIAL::MI_CH_M_NA_961_Tearline))))
            return E_FAIL;
    }
    // idx 6 (Shader pass : 10)	
    else if (strcmp(m_pModelCom->Get_MaterialName(m_pModelCom->Get_Mesh_MaterialIndex(iIdx)), "MI_CH_M_NA_961_Eyeshadow") == 0)
    {
        if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(FACE_MATERIAL::MI_CH_M_NA_961_Eyeshadow))))
            return E_FAIL;
    }
    // idx 7 (Shader pass : 11)	
    else if (strcmp(m_pModelCom->Get_MaterialName(m_pModelCom->Get_Mesh_MaterialIndex(iIdx)), "MI_CH_M_NA_961_EyeBlend") == 0)
    {
        if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(FACE_MATERIAL::MI_CH_M_NA_961_EyeBlend))))
            return E_FAIL;
    }
    // idx 8 (Shader pass : 12)	
    else if (strcmp(m_pModelCom->Get_MaterialName(m_pModelCom->Get_Mesh_MaterialIndex(iIdx)), "MI_CH_M_NA_961_NoseShadow") == 0)
    {
        if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(FACE_MATERIAL::MI_CH_M_NA_961_NoseShadow))))
            return E_FAIL;
    }
    // idx 9 (Shader pass : 13)	
    else if (strcmp(m_pModelCom->Get_MaterialName(m_pModelCom->Get_Mesh_MaterialIndex(iIdx)), "MI_CH_M_NA_961_Teeth") == 0)
    {
        if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(FACE_MATERIAL::MI_CH_M_NA_961_Teeth))))
            return E_FAIL;
    }

    return S_OK;
}

HRESULT CCinematicPartFacial::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CCinematicPartFacial::Initialize(void* pArg)
{
    FACIAL_CINEMATIC_DESC* pDesc = static_cast<FACIAL_CINEMATIC_DESC*>(pArg);

    m_szModelTag = pDesc->szModelTag;
    m_pSocketMatrix = const_cast<_float4x4*>(pDesc->pSocketMatrix);

    m_bIsAnim = pDesc->isAnim;

    Bind_BoneToPartBody(pDesc->pBody);

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
    m_pModelCom->Bind_MaterialTag(TEXTURE_TYPE::OPACITY, "g_OpacityTexture");
    m_pModelCom->Bind_MaterialTag(TEXTURE_TYPE::NORMAL, "g_NormalTexture");
    m_pModelCom->Bind_MaterialTag(TEXTURE_TYPE::EMISSIVE, "g_EmissiveTexture");
    m_pModelCom->Bind_MaterialTag(TEXTURE_TYPE::ORM, "g_ORMTexture");
    m_pModelCom->Bind_MaterialTag(TEXTURE_TYPE::ORSS, "g_ORSSTexture");

    m_pModelCom->Set_MorphAnimation("EVE_Idle_Facial_Curves", TRUE);

    return S_OK;
}

void CCinematicPartFacial::Priority_Update(_float fTimeDelta)
{
    //m_pTransformCom->Set_Scale(XMVectorSet(1.f, 1.f, 1.f, 1.f));

    //m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(0.f, 0.f, 0.5f, 1.f));

    Update_PreCombinedMatrix();
}

void CCinematicPartFacial::Update(_float fTimeDelta)
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

void CCinematicPartFacial::Late_Update(_float fTimeDelta)
{
    m_pModelCom->Play_MorphAnimation(fTimeDelta);
}

HRESULT CCinematicPartFacial::Render()
{
    if (m_bIsActive == FALSE)
        return S_OK;

    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (size_t i = 0; i < iNumMeshes; i++)
    {

        //if (FAILED(m_pModelCom->Bind_BoneSRV(i, m_pShaderCom, "g_BoneMatrixBuffer")))
        //	return E_FAIL;

        if (FAILED(m_pModelCom->Bind_ShapeKeys(i, m_pShaderCom, "g_ShapeKeyWeights")))
            return E_FAIL;

        if (FAILED(m_pBodyModelCom->Bind_BoneMatrixSRV(m_pShaderCom, "g_BoneMatrixBuffer")))
            return E_FAIL;

        if (FAILED(m_pBodyModelCom->Bind_PreBoneMatrixSRV(m_pShaderCom)))
            return E_FAIL;

        if (FAILED(m_pModelCom->Bind_GlobalOffsetMatrices(m_pShaderCom)))
            return E_FAIL;

        if (FAILED(m_pModelCom->Bind_AllMaterials(i, m_pShaderCom, 0)))
            return E_FAIL;

        //여기서 머테리얼 인덱스 따라 패스 구분.
        if (FAILED(Mapping_Shader_Material(i)))
            return E_FAIL;

        if (FAILED(m_pModelCom->Render(i)))
            return E_FAIL;
    }

    //m_pGameInstance->Render_Text(TEXT("KoPub"), m_szRotationAngle, _float2(g_iWinSizeX / 2 - 180, 0), XMVectorSet(1.f, 1.f, 1.f, 0.1f));

    return S_OK;
}

HRESULT CCinematicPartFacial::Render_Shadow()
{
    if (m_bIsActive == FALSE)
        return S_OK;

    //Anim 객체든 아니든, 어차피 가까이서 보일거니까 다 캐스케이드로 처리함 수고
    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Bind_Shadow_Resource_Cascade(m_pShaderCom, "g_LightViewMatrix", D3DTS::VIEW)))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Bind_Shadow_Resource_Cascade(m_pShaderCom, "g_LightProjMatrix", D3DTS::PROJ)))
        return E_FAIL;

    _uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (size_t i = 0; i < iNumMeshes; i++)
    {
        if (m_bIsAnim)
        {
            if (FAILED(m_pModelCom->Bind_BoneSRV(i, m_pShaderCom, "g_BoneMatrixBuffer")))
                return E_FAIL;

            if (FAILED(m_pShaderCom->Begin(1)))
                return E_FAIL;
        }
        else
        {
            if (FAILED(m_pShaderCom->Begin(6)))
                return E_FAIL;
        }

        if (FAILED(m_pModelCom->Render(i)))
            return E_FAIL;
    }

    return S_OK;
}

HRESULT CCinematicPartFacial::Render_MotionBlur()
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

void CCinematicPartFacial::Reset_SocketMatrix(_float4x4* pSocketMatrix)
{
    m_pSocketMatrix = pSocketMatrix;
}

HRESULT CCinematicPartFacial::Bind_ShaderResources()
{
    _bool bFlag = static_cast<CCharacter*>(m_pParent)->Get_DepthMaskingB();
    if (FAILED(m_pShaderCom->Bind_RawValue("g_IsMaskingDepthB", &bFlag, sizeof(_bool))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_IsMaskingDepthW", &bFlag, sizeof(_bool))))
        return E_FAIL;

    if (nullptr == m_pShaderCom)
        return E_FAIL;

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

HRESULT CCinematicPartFacial::Ready_Components()
{
    /* Com_Model */
    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), m_szModelTag,
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    /* Com_Shader */
    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Shader_Scarlet_Face"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;


    return S_OK;
}

HRESULT CCinematicPartFacial::Bind_BoneToPartBody(CModel* pArg)
{
    m_pBodyModelCom = pArg;

    return S_OK;
}

CCinematicPartFacial* CCinematicPartFacial::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CCinematicPartFacial* pCinematicPartFacial = new CCinematicPartFacial(pDevice, pContext);
    if (FAILED(pCinematicPartFacial->Initialize_Prototype()))
    {
        Safe_Release(pCinematicPartFacial);
        MSG_BOX("Create Fail : Cinematic Part Body");
    }
    return pCinematicPartFacial;
}

CGameObject* CCinematicPartFacial::Clone(void* pArg)
{
    CCinematicPartFacial* pCinematicPartFacial = new CCinematicPartFacial(*this);
    if (FAILED(pCinematicPartFacial->Initialize(pArg)))
    {
        Safe_Release(pCinematicPartFacial);
        MSG_BOX("Clones Fail : Cinematic Part Body");
    }
    return pCinematicPartFacial;
}

void CCinematicPartFacial::Free()
{
    __super::Free();

}
