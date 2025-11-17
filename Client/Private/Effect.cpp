#include "pch.h"
#include "GameObject.h"
#include "Effect.h"
#include "GameInstance.h"
#include "MeshEffect.h"
#include "PointParticle.h"
#include "SpriteUVEffect.h"

CEffect::CEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{ pDevice, pContext }
{
}

CEffect::CEffect(const CEffect& Prototype)
    : CGameObject{ Prototype }
{
    for (auto pMeshEffect : Prototype.m_pMeshEffects) {
        m_pMeshEffects.push_back(dynamic_cast<CMeshEffect*>(pMeshEffect->Clone(nullptr)));
    }
    for (auto pParticle : Prototype.m_pPointParticles) {
        m_pPointParticles.push_back(dynamic_cast<CPointParticle*>(pParticle->Clone(nullptr)));
    }
    for (auto pSpriteEffect : Prototype.m_pSpriteEffects) {
        m_pSpriteEffects.push_back(dynamic_cast<CSpriteUVEffect*>(pSpriteEffect->Clone(nullptr)));
    }
}

HRESULT CEffect::Initialize_Prototype(const _char* szFile)
{
    Load_Binary(szFile);
    return S_OK;
}

HRESULT CEffect::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (nullptr != pArg)
    {
        EFFECT_TRANSFORM_DESC* pDesc = static_cast<EFFECT_TRANSFORM_DESC*>(pArg);
        m_pTransformCom->Set_State(STATE::POSITION, pDesc->vPos);
    }
    m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(5, 5, 5, 1));
    for (auto pMeshEffect : m_pMeshEffects) {
        pMeshEffect->Set_ParentMat(m_pTransformCom->Get_WorldMatrixPtr());
    }
    for (auto pParticle : m_pPointParticles) {
        pParticle->Set_ParentMat(m_pTransformCom->Get_WorldMatrixPtr());
    }
    for (auto pSpriteEffect : m_pSpriteEffects) {
        pSpriteEffect->Set_ParentMat(m_pTransformCom->Get_WorldMatrixPtr());
    }
    return S_OK;
}

void CEffect::Priority_Update(_float fTimeDelta)
{
    for (auto pMeshEffect : m_pMeshEffects) {
        pMeshEffect->Priority_Update(fTimeDelta);
    }
    for (auto pParticle : m_pPointParticles) {
        pParticle->Priority_Update(fTimeDelta);
    }
    for (auto pSpriteEffect : m_pSpriteEffects) {
        pSpriteEffect->Priority_Update(fTimeDelta);
    }
}

HRESULT CEffect::Load_Binary(const _char* szFile)
{
    CMeshEffect::MESH_EFFECT_DATA	MeshEffectDesc{};
    CPointParticle::POINT_PARTICLE_DATA    PointParticleDesc{};
    CSpriteUVEffect::SPRITE_DATA    SpriteDesc{};
    ifstream fileBinaryStream;
    fileBinaryStream.open(szFile, ios_base::binary);
    _int iMeshCount = ReadInt(fileBinaryStream);
    for (_uint i = 0; i < iMeshCount; ++i) {
        _char* szTemp = ReadString(fileBinaryStream);
        MeshEffectDesc.szModel = szTemp;
        Safe_Delete(szTemp);
        szTemp = ReadString(fileBinaryStream);
        MeshEffectDesc.szMaskTexture = szTemp;
        Safe_Delete(szTemp);
        szTemp = ReadString(fileBinaryStream);
        MeshEffectDesc.szDiffuseTexture = szTemp;
        Safe_Delete(szTemp);
        szTemp = ReadString(fileBinaryStream);
        MeshEffectDesc.szDissolveTexture = szTemp;
        Safe_Delete(szTemp);
        MeshEffectDesc.fColor = ReadFloat4(fileBinaryStream);
        MeshEffectDesc.fPosition = ReadFloat4(fileBinaryStream);
        MeshEffectDesc.fScale = ReadFloat3(fileBinaryStream);
        MeshEffectDesc.fRotation = ReadFloat3(fileBinaryStream);




        MeshEffectDesc.fMaskUV = ReadFloat2(fileBinaryStream);
        MeshEffectDesc.fMaskUVSpeed = ReadFloat2(fileBinaryStream);
        MeshEffectDesc.fMaskUVSize = ReadFloat2(fileBinaryStream);
        MeshEffectDesc.fDiffuseUV = ReadFloat2(fileBinaryStream);
        MeshEffectDesc.fDiffuseUVSpeed = ReadFloat2(fileBinaryStream);
        MeshEffectDesc.fDiffuseUVSize = ReadFloat2(fileBinaryStream);
        MeshEffectDesc.fDissolveUV = ReadFloat2(fileBinaryStream);
        MeshEffectDesc.fDissolveUVSpeed = ReadFloat2(fileBinaryStream);
        MeshEffectDesc.fDissolveUVSize = ReadFloat2(fileBinaryStream);
        MeshEffectDesc.fDelayTime = ReadFloat(fileBinaryStream);
        MeshEffectDesc.fEndTime = ReadFloat(fileBinaryStream);



        MeshEffectDesc.iBegin = ReadInt(fileBinaryStream);
        MeshEffectDesc.iSelectRender = ReadInt(fileBinaryStream);

        CMeshEffect* pMeshEffect = CMeshEffect::Create(m_pDevice, m_pContext, &MeshEffectDesc);
        m_pMeshEffects.push_back(pMeshEffect);
    }

    _int iParticleCount = ReadInt(fileBinaryStream);
    for (_uint i = 0; i < iParticleCount; ++i) {
        _char* szTemp = ReadString(fileBinaryStream);
        PointParticleDesc.szMaskTexture = szTemp;
        Safe_Delete(szTemp);
        szTemp = ReadString(fileBinaryStream);
        PointParticleDesc.szDiffuseTexture = szTemp;
        Safe_Delete(szTemp);
        szTemp = ReadString(fileBinaryStream);
        PointParticleDesc.szDissolveTexture = szTemp;
        Safe_Delete(szTemp);
        szTemp = ReadString(fileBinaryStream);
        PointParticleDesc.szCS = szTemp;
        Safe_Delete(szTemp);
        _int iSizeDiagramCount = ReadInt(fileBinaryStream);
        PointParticleDesc.fSizeDiagrams.clear();
        for (_uint j = 0; j < iSizeDiagramCount; ++j) {
            PointParticleDesc.fSizeDiagrams.push_back(ReadFloat3(fileBinaryStream));
        }
        PointParticleDesc.fGravityDiagram = ReadFloat4(fileBinaryStream);
        PointParticleDesc.fPosition = ReadFloat4(fileBinaryStream);
        PointParticleDesc.fColor = ReadFloat4(fileBinaryStream);
        PointParticleDesc.fCenter = ReadFloat3(fileBinaryStream);
        PointParticleDesc.fPivot = ReadFloat3(fileBinaryStream);
        PointParticleDesc.fRange = ReadFloat3(fileBinaryStream);
        PointParticleDesc.fRotation = ReadFloat3(fileBinaryStream);
        PointParticleDesc.fSize = ReadFloat2(fileBinaryStream);
        PointParticleDesc.fLifeTime = ReadFloat2(fileBinaryStream);
        PointParticleDesc.fSpeed = ReadFloat2(fileBinaryStream);


        PointParticleDesc.fMaskUV = ReadFloat2(fileBinaryStream);
        PointParticleDesc.fMaskUVSpeed = ReadFloat2(fileBinaryStream);
        PointParticleDesc.fMaskUVSize = ReadFloat2(fileBinaryStream);
        PointParticleDesc.fDiffuseUV = ReadFloat2(fileBinaryStream);
        PointParticleDesc.fDiffuseUVSpeed = ReadFloat2(fileBinaryStream);
        PointParticleDesc.fDiffuseUVSize = ReadFloat2(fileBinaryStream);
        PointParticleDesc.fDissolveUV = ReadFloat2(fileBinaryStream);
        PointParticleDesc.fDissolveUVSpeed = ReadFloat2(fileBinaryStream);
        PointParticleDesc.fDissolveUVSize = ReadFloat2(fileBinaryStream);
        PointParticleDesc.fDelayTime = ReadFloat(fileBinaryStream);
        PointParticleDesc.fEndTime = ReadFloat(fileBinaryStream);


        PointParticleDesc.iBegin = ReadInt(fileBinaryStream);
        PointParticleDesc.iNumInstance = ReadInt(fileBinaryStream);
        PointParticleDesc.iSelectRender = ReadInt(fileBinaryStream);

        PointParticleDesc.bisBillboard = ReadBool(fileBinaryStream);
        PointParticleDesc.bisLoop = ReadBool(fileBinaryStream);

        CPointParticle* pParticle = CPointParticle::Create(m_pDevice, m_pContext, &PointParticleDesc);
        m_pPointParticles.push_back(pParticle);
    }

    _int iSpriteCount = ReadInt(fileBinaryStream);
    for (_uint i = 0; i < iSpriteCount; ++i) {
        _char* szTemp = ReadString(fileBinaryStream);
        SpriteDesc.szMaskTexture = szTemp;
        Safe_Delete(szTemp);
        szTemp = ReadString(fileBinaryStream);
        SpriteDesc.szDiffuseTexture = szTemp;
        Safe_Delete(szTemp);
        szTemp = ReadString(fileBinaryStream);
        SpriteDesc.szNormalTexture = szTemp;
        Safe_Delete(szTemp);

        SpriteDesc.fPosition = ReadFloat4(fileBinaryStream);
        SpriteDesc.fColor = ReadFloat4(fileBinaryStream);
        SpriteDesc.fSize = ReadFloat2(fileBinaryStream);
        SpriteDesc.iUV = ReadInt2(fileBinaryStream);
        SpriteDesc.fFPS = ReadFloat(fileBinaryStream);
        SpriteDesc.iBegin = ReadInt(fileBinaryStream);
        SpriteDesc.iSelectRender = ReadInt(fileBinaryStream);
        SpriteDesc.bisLoop = ReadBool(fileBinaryStream);


        CSpriteUVEffect* pSprite = CSpriteUVEffect::Create(m_pDevice, m_pContext, &SpriteDesc);
        m_pSpriteEffects.push_back(pSprite);
    }
    return S_OK;
}

void CEffect::Update(_float fTimeDelta)
{
    for (auto pMeshEffect : m_pMeshEffects) {
        pMeshEffect->Update(fTimeDelta);
    }
    for (auto pParticle : m_pPointParticles) {
        pParticle->Update(fTimeDelta);
    }
    for (auto pSpriteEffect : m_pSpriteEffects) {
        pSpriteEffect->Update(fTimeDelta);
    }
}

void CEffect::Late_Update(_float fTimeDelta)
{

    if (1 < m_pMeshEffects.size()) {
        for (auto i = m_pMeshEffects.begin(); i != m_pMeshEffects.end();) {
            if ((*i)->isDead()) {
                Safe_Release((*i));
                i = m_pMeshEffects.erase(i);
            }
            else {
                (*i)->Late_Update(fTimeDelta);
            }
        }
    }
    else if(1 == m_pMeshEffects.size()){
        if (m_pMeshEffects[0]->isDead()) {
            Safe_Release(m_pMeshEffects[0]);
            m_pMeshEffects.clear();
        }
        else {
            m_pMeshEffects[0]->Late_Update(fTimeDelta);
        }
    }
    if (1 < m_pPointParticles.size()) {
        for (auto i = m_pPointParticles.begin(); i != m_pPointParticles.end();) {
            if ((*i)->isDead()) {
                Safe_Release((*i));
                i = m_pPointParticles.erase(i);
            }
            else {
                (*i)->Late_Update(fTimeDelta);
            }
        }
    }
    else if (1 == m_pPointParticles.size()) {
        if (m_pPointParticles[0]->isDead()) {
            Safe_Release(m_pPointParticles[0]);
            m_pPointParticles.clear();
        }
        else {
            m_pPointParticles[0]->Late_Update(fTimeDelta);
        }
    }
    if (1 < m_pSpriteEffects.size()) {
        for (auto i = m_pSpriteEffects.begin(); i != m_pSpriteEffects.end();) {
            if ((*i)->isDead()) {
                Safe_Release((*i));
                i = m_pSpriteEffects.erase(i);
            }
            else {
                (*i)->Late_Update(fTimeDelta);
            }
        }
    }
    else if (1 == m_pSpriteEffects.size()) {
        if (m_pSpriteEffects[0]->isDead()) {
            Safe_Release(m_pSpriteEffects[0]);
            m_pSpriteEffects.clear();
        }
        else {
            m_pSpriteEffects[0]->Late_Update(fTimeDelta);
        }
    }
}

HRESULT CEffect::Render()
{
    return S_OK;
}

CEffect* CEffect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _char* szFile)
{
    auto p = new CEffect(pDevice, pContext);
    if (FAILED(p->Initialize_Prototype(szFile)))
    {
        Safe_Release(p);
        MSG_BOX("CREATE FAIL : CEffect");
        return nullptr;
    }
    return p;
}

CGameObject* CEffect::Clone(void* pArg)
{
    CEffect* pInstance = new CEffect(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CParticle");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CEffect::Free()
{
    __super::Free();
    for (auto pMeshEffect : m_pMeshEffects)
        Safe_Release(pMeshEffect);
    m_pMeshEffects.clear();
    for (auto pParticle : m_pPointParticles)
        Safe_Release(pParticle);
    m_pPointParticles.clear();
    for (auto pSpriteEffect : m_pSpriteEffects)
        Safe_Release(pSpriteEffect);
    m_pSpriteEffects.clear();
}


_char* CEffect::ReadString(ifstream& fileBinaryStream)
{
    _uint iStringLength;
    fileBinaryStream.read((_char*)&iStringLength, sizeof(iStringLength));

    _char* sz = new _char[iStringLength + 1];
    fileBinaryStream.read(&sz[0], iStringLength);

    sz[iStringLength] = '\0';
    return sz;
}

_int CEffect::ReadInt(ifstream& fileBinaryStream)
{
    _int iValue;
    fileBinaryStream.read((_char*)&iValue, sizeof(_int));
    return iValue;
}

_float4 CEffect::ReadFloat4(ifstream& fileBinaryStream)
{
    _float4 fValue;
    fileBinaryStream.read((_char*)&fValue, sizeof(_float4));
    return fValue;
}

_float3 CEffect::ReadFloat3(ifstream& fileBinaryStream)
{
    _float3 fValue;
    fileBinaryStream.read((_char*)&fValue, sizeof(_float3));
    return fValue;
}

_float2 CEffect::ReadFloat2(ifstream& fileBinaryStream)
{
    _float2 fValue;
    fileBinaryStream.read((_char*)&fValue, sizeof(_float2));
    return fValue;
}

_float CEffect::ReadFloat(ifstream& fileBinaryStream)
{
    _float fValue;
    fileBinaryStream.read((_char*)&fValue, sizeof(_float));
    return fValue;
}

_int2 CEffect::ReadInt2(ifstream& fileBinaryStream)
{
    _int2 iValue;
    fileBinaryStream.read((_char*)&iValue, sizeof(_int2));
    return iValue;
}

_bool CEffect::ReadBool(ifstream& fileBinaryStream)
{
    _bool bisFlag;
    fileBinaryStream.read((_char*)&bisFlag, sizeof(_bool));
    return bisFlag;
}
