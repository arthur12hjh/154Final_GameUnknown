#include "pch.h"
#include "TrailEffect.h"
#include "GameInstance.h"
#include "Trail.h"

CTrailEffect::CTrailEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{ pDevice, pContext }
{
}

CTrailEffect::CTrailEffect(const CTrailEffect& Prototype)
    : CGameObject{ Prototype },
    m_tDatas{Prototype.m_tDatas}
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

    
    TRAIL_DATA* pDesc = static_cast<TRAIL_DATA*>(pArg);
    if (pDesc->bisLine) {
        /* Com_Trail */
        if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Line_Trail"),
            TEXT("Com_Trail"), reinterpret_cast<CComponent**>(&m_pWaveTrail), pArg)))
            return E_FAIL;
    }
    else {
        /* Com_Trail */
        if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Trail"),
            TEXT("Com_Trail"), reinterpret_cast<CComponent**>(&m_pTrail), pArg)))
            return E_FAIL;
    }

    if (FAILED(Ready_Components(pArg)))
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

    CTrailData::TRAIL_DATA TrailDesc;
    _int iTrailCount = ReadInt(fileBinaryStream);
    for (_uint i = 0; i < iTrailCount; ++i) {
        _char* szTemp = ReadString(fileBinaryStream);
        TrailDesc.szMaskTexture = szTemp;
        Safe_Delete(szTemp);
        szTemp = ReadString(fileBinaryStream);
        TrailDesc.szDiffuseTexture = szTemp;
        Safe_Delete(szTemp);
        szTemp = ReadString(fileBinaryStream);
        TrailDesc.szDissolveTexture = szTemp;
        Safe_Delete(szTemp);
        
        TrailDesc.fColor = ReadFloat4(fileBinaryStream);
        TrailDesc.fMaskUV = ReadFloat2(fileBinaryStream);
        TrailDesc.fMaskUVSpeed = ReadFloat2(fileBinaryStream);
        TrailDesc.fMaskUVSize = ReadFloat2(fileBinaryStream);
        TrailDesc.fDiffuseUV = ReadFloat2(fileBinaryStream);
        TrailDesc.fDiffuseUVSpeed = ReadFloat2(fileBinaryStream);
        TrailDesc.fDiffuseUVSize = ReadFloat2(fileBinaryStream);
        TrailDesc.fDissolveUV = ReadFloat2(fileBinaryStream);
        TrailDesc.fDissolveUVSpeed = ReadFloat2(fileBinaryStream);
        TrailDesc.fDissolveUVSize = ReadFloat2(fileBinaryStream);


        TrailDesc.iBegin = ReadInt(fileBinaryStream);
        TrailDesc.iSelectRender = ReadInt(fileBinaryStream);
        m_tDatas.push_back(TrailDesc);
    }
    return S_OK;
}
void    CTrailEffect::Update_Trail(_fmatrix matCurrentWorld, _float fTimeDelta, _bool bMakeTrail) {
    m_fTime += fTimeDelta;
    if(nullptr != m_pWaveTrail)
        m_pWaveTrail->Update_WaveTrail(matCurrentWorld, fTimeDelta, bMakeTrail);
    else
        m_pTrail->Update_Trail(matCurrentWorld, fTimeDelta, bMakeTrail);
    for (auto pData : m_pTrailDatas) {
        pData->Add_RenderGroup();
    }
}

HRESULT CTrailEffect::Render(CTrailData* pTrailData)
{
    if (FAILED(Bind_ShaderResources(pTrailData->Get_Data())))
        return E_FAIL;
    if (FAILED(pTrailData->Bind_Texture(m_pShaderCom)))
        return E_FAIL;
    if (nullptr != m_pWaveTrail) {
        if (FAILED(m_pWaveTrail->Render()))
            return E_FAIL;
    }
    else {
        if (FAILED(m_pTrail->Render()))
            return E_FAIL;
    }
    return S_OK;
}

HRESULT CTrailEffect::Ready_Components(void* pArg)
{
    /* Com_Shader */
    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Shader_VtxTrail"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;


    char pattern[30] = {};


    for (auto tData : m_tDatas) {
        CTrailData* trailData = static_cast<CTrailData*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_TrailData"), this));
        trailData->Set_Data(tData);
        m_pTrailDatas.push_back(trailData);
    }
    m_tDatas.clear();
    return S_OK;
}

HRESULT CTrailEffect::Bind_ShaderResources(CTrailData::TRAIL_DATA tData)
{
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fTime", &m_fTime, sizeof(_float))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &tData.fColor, sizeof(_float4))))
        return E_FAIL;


    if (FAILED(m_pShaderCom->Bind_RawValue("g_fMaskUV", &tData.fMaskUV, sizeof(_float2))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fMaskUVSpeed", &tData.fMaskUVSpeed, sizeof(_float2))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fMaskUVSize", &tData.fMaskUVSize, sizeof(_float2))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fDiffuseUV", &tData.fDiffuseUV, sizeof(_float2))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fDiffuseUVSpeed", &tData.fDiffuseUVSpeed, sizeof(_float2))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fDiffuseUVSize", &tData.fDiffuseUVSize, sizeof(_float2))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fDissolveUV", &tData.fDissolveUV, sizeof(_float2))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fDissolveUVSpeed", &tData.fDissolveUVSpeed, sizeof(_float2))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fDissolveUVSize", &tData.fDissolveUVSize, sizeof(_float2))))
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
    Safe_Release(m_pWaveTrail);
    for (auto pTrailData : m_pTrailDatas) {
        Safe_Release(pTrailData);
    }
    m_pTrailDatas.clear();
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

void CTrailEffect::Set_Color(_float4 fColor)
{
    for (auto pData : m_pTrailDatas) {
        CTrailData::TRAIL_DATA data = pData->Get_Data();
        data.fColor = fColor;
        pData->Set_Data(data);
    }
}
