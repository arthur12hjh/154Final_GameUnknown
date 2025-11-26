#include "pch.h"
#include "TrailEffect.h"
#include "GameInstance.h"
#include "Trail.h"

CTrailEffect::CTrailEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CPartObject{ pDevice, pContext }
{
}

CTrailEffect::CTrailEffect(const CTrailEffect& Prototype)
    : CPartObject{ Prototype },
    m_tData{ Prototype.m_tData }
{
}

HRESULT CTrailEffect::Initialize_Prototype(const _char* szFile)
{
    Load_Binary(szFile);
    return S_OK;
}

HRESULT CTrailEffect::Initialize(void* pArg)
{
    if(nullptr == pArg)
        return E_FAIL;
    if (FAILED(__super::Initialize(nullptr)))
        return E_FAIL;

    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Trail"),
        TEXT("Com_Trail"), reinterpret_cast<CComponent**>(&m_pTrail), pArg)))
        return E_FAIL;
    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_fTime = 0.f;

    return S_OK;
}

HRESULT CTrailEffect::Load_Binary(const _char* szFile)
{
    char szBinModelFilePath[MAX_PATH] = {};
    strcat_s(szBinModelFilePath, MAX_PATH, szFile);
    ifstream fileBinaryStream;
    fileBinaryStream.open(szBinModelFilePath, ios_base::binary);

    _char* szTemp = ReadString(fileBinaryStream);
    m_tData.szMaskTexture = szTemp;
    Safe_Delete(szTemp);
    szTemp = ReadString(fileBinaryStream);
    m_tData.szDiffuseTexture = szTemp;
    Safe_Delete(szTemp);
    szTemp = ReadString(fileBinaryStream);
    m_tData.szDissolveTexture = szTemp;
    Safe_Delete(szTemp);
    m_tData.fColor = ReadFloat4(fileBinaryStream);
    m_tData.fMaskUV = ReadFloat2(fileBinaryStream);
    m_tData.fMaskUVSpeed = ReadFloat2(fileBinaryStream);
    m_tData.fMaskUVSize = ReadFloat2(fileBinaryStream);
    m_tData.fDiffuseUV = ReadFloat2(fileBinaryStream);
    m_tData.fDiffuseUVSpeed = ReadFloat2(fileBinaryStream);
    m_tData.fDiffuseUVSize = ReadFloat2(fileBinaryStream);
    m_tData.fDissolveUV = ReadFloat2(fileBinaryStream);
    m_tData.fDissolveUVSpeed = ReadFloat2(fileBinaryStream);
    m_tData.fDissolveUVSize = ReadFloat2(fileBinaryStream);
    m_tData.iBegin = ReadInt(fileBinaryStream);
    m_tData.eSelectRender = ReadRENDER(fileBinaryStream);
    return S_OK;
}
void    CTrailEffect::Update_Trail(_fmatrix matCurrentWorld, _float fTimeDelta, _bool bMakeTrail) {
    m_fTime += fTimeDelta;
    m_pTrail->Update_Trail(matCurrentWorld, fTimeDelta, bMakeTrail);
    m_pGameInstance->Add_RenderGroup(m_tData.eSelectRender, this);
}

HRESULT CTrailEffect::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;


    if (FAILED(m_pShaderCom->Begin(m_tData.iBegin)))
        return E_FAIL;

    m_pTrail->Render();
    return S_OK;
}

HRESULT CTrailEffect::Ready_Components()
{
    _tchar sztPrototype[256] = { 0, };
    MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, m_tData.szMaskTexture.c_str(), strlen(m_tData.szMaskTexture.c_str()), sztPrototype, 256);
    /* Com_Texture */
    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), sztPrototype,
        TEXT("Com_MaskTexture"), reinterpret_cast<CComponent**>(&m_pTexture[0]))))
        return E_FAIL;
    memset(sztPrototype, 0, sizeof(sztPrototype));

    MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, m_tData.szDiffuseTexture.c_str(), strlen(m_tData.szDiffuseTexture.c_str()), sztPrototype, 256);
    /* Com_Texture */
    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), sztPrototype,
        TEXT("Com_DiffuseTexture"), reinterpret_cast<CComponent**>(&m_pTexture[1]))))
        return E_FAIL;
    memset(sztPrototype, 0, sizeof(sztPrototype));

    MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, m_tData.szDissolveTexture.c_str(), strlen(m_tData.szDissolveTexture.c_str()), sztPrototype, 256);
    /* Com_Texture */
    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), sztPrototype,
        TEXT("Com_DissolveTexture"), reinterpret_cast<CComponent**>(&m_pTexture[2]))))
        return E_FAIL;

    /* Com_Shader */
    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Shader_VtxTrail"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;
    return S_OK;
}

HRESULT CTrailEffect::Bind_ShaderResources()
{
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float3))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_fTime", &m_fTime, sizeof(_float))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_tData.fColor, sizeof(_float4))))
        return E_FAIL;


    if (FAILED(m_pShaderCom->Bind_RawValue("g_fMaskUV", &m_tData.fMaskUV, sizeof(_float2))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fMaskUVSpeed", &m_tData.fMaskUVSpeed, sizeof(_float2))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fMaskUVSize", &m_tData.fMaskUVSize, sizeof(_float2))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fDiffuseUV", &m_tData.fDiffuseUV, sizeof(_float2))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fDiffuseUVSpeed", &m_tData.fDiffuseUVSpeed, sizeof(_float2))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fDiffuseUVSize", &m_tData.fDiffuseUVSize, sizeof(_float2))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fDissolveUV", &m_tData.fDissolveUV, sizeof(_float2))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fDissolveUVSpeed", &m_tData.fDissolveUVSpeed, sizeof(_float2))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fDissolveUVSize", &m_tData.fDissolveUVSize, sizeof(_float2))))
        return E_FAIL;

    if (FAILED(m_pTexture[0]->Bind_ShaderResource(m_pShaderCom, "g_MaskTexture", 0)))
        return E_FAIL;

    if (FAILED(m_pTexture[1]->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", 0)))
        return E_FAIL;

    if (FAILED(m_pTexture[2]->Bind_ShaderResource(m_pShaderCom, "g_DissolveTexture", 0)))
        return E_FAIL;
    return S_OK;
}


CTrailEffect* CTrailEffect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _char* szFile)
{
    auto p = new CTrailEffect(pDevice, pContext);
    if (FAILED(p->Initialize_Prototype(szFile)))
    {
        Safe_Release(p);
        MSG_BOX("CREATE FAIL : CTrailEffect");
        return nullptr;
    }
    return p;
}

CGameObject* CTrailEffect::Clone(void* pArg)
{
    CTrailEffect* pInstance = new CTrailEffect(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CParticle");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CTrailEffect::Free()
{
    __super::Free();
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pTrail);
    for (_uint i = 0; i < 3; ++i) {
        Safe_Release(m_pTexture[i]);
    }
}


_char* CTrailEffect::ReadString(ifstream& fileBinaryStream)
{
    _uint iStringLength;
    fileBinaryStream.read((_char*)&iStringLength, sizeof(iStringLength));

    _char* sz = new _char[iStringLength + 1];
    fileBinaryStream.read(&sz[0], iStringLength);

    sz[iStringLength] = '\0';
    return sz;
}

_int CTrailEffect::ReadInt(ifstream& fileBinaryStream)
{
    _int iValue;
    fileBinaryStream.read((_char*)&iValue, sizeof(_int));
    return iValue;
}

RENDER CTrailEffect::ReadRENDER(ifstream& fileBinaryStream)
{
    _int iValue;
    fileBinaryStream.read((_char*)&iValue, sizeof(_int));
    switch (iValue)
    {
    case 0:
        return RENDER::NONBLEND;
    case 1:
        return RENDER::NONLIGHT;
    case 2:
        return RENDER::BLUR;
    case 3:
        return RENDER::GLOW;
    case 4:
        return RENDER::DISTORTION;
    case 5:
        return RENDER::BLEND;
    }
    return RENDER::UI;
}

_float4 CTrailEffect::ReadFloat4(ifstream& fileBinaryStream)
{
    _float4 fValue;
    fileBinaryStream.read((_char*)&fValue, sizeof(_float4));
    return fValue;
}

_float2 CTrailEffect::ReadFloat2(ifstream& fileBinaryStream)
{
    _float2 fValue;
    fileBinaryStream.read((_char*)&fValue, sizeof(_float2));
    return fValue;
}