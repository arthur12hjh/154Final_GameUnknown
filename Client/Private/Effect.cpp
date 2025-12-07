#include "pch.h"
#include "GameObject.h"
#include "Effect.h"
#include "GameInstance.h"
#include "MeshEffect.h"
#include "PointParticle.h"
#include "SpriteParticle.h"
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
    for (auto pSpriteParticle : Prototype.m_pSpriteParticles) {
        m_pSpriteParticles.push_back(dynamic_cast<CSpriteParticle*>(pSpriteParticle->Clone(nullptr)));
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
    if (FAILED(__super::Initialize(nullptr)))
        return E_FAIL;
    m_fStopTime = -10.f;
    if (nullptr != pArg)
    {
        EFFECT_TRANSFORM_DESC* pDesc = static_cast<EFFECT_TRANSFORM_DESC*>(pArg);
        if (nullptr != pDesc->pRootMatrix) {
            m_pParentMat = pDesc->pRootMatrix;
            if (nullptr != pDesc->pWorldMatrix) {
                m_pParentWorldMat = pDesc->pWorldMatrix;
            }
        }
        
        m_pTransformCom->Set_State(STATE::POSITION, pDesc->vPos);
        m_pTransformCom->Rotation(pDesc->fRot.x, pDesc->fRot.y, pDesc->fRot.z);
        m_pTransformCom->Set_Scale(pDesc->fSize, pDesc->fSize, pDesc->fSize);
        m_bisFloor = pDesc->bisFloor;
    }
    if (nullptr != m_pParentMat) {
        if (nullptr != m_pParentWorldMat) {
            XMStoreFloat4x4(&m_CombinedWorldMatrix,
                XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()) * XMLoadFloat4x4(m_pParentMat) * XMLoadFloat4x4(m_pParentWorldMat));
            if (m_bisFloor) {
                m_CombinedWorldMatrix._42 = m_pParentWorldMat->_42;
                XMStoreFloat4x4(&m_CombinedWorldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_CombinedWorldMatrix)));
            }
        }
        else {
            XMStoreFloat4x4(&m_CombinedWorldMatrix,
                XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()) * XMLoadFloat4x4(m_pParentMat));
            if (m_bisFloor) {
                m_CombinedWorldMatrix._42 = m_pParentMat->_42;
                XMStoreFloat4x4(&m_CombinedWorldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_CombinedWorldMatrix)));
            }
        }
    }
    else {
        m_CombinedWorldMatrix = *m_pTransformCom->Get_WorldMatrixPtr();
    }
    for (auto pMeshEffect : m_pMeshEffects) {
        pMeshEffect->Set_ParentMat(&m_CombinedWorldMatrix);
    }
    for (auto pParticle : m_pPointParticles) {
        pParticle->Set_ParentMat(&m_CombinedWorldMatrix);
    }
    for (auto pSpriteParticle : m_pSpriteParticles) {
        pSpriteParticle->Set_ParentMat(&m_CombinedWorldMatrix);
    }
    for (auto pSpriteEffect : m_pSpriteEffects) {
        pSpriteEffect->Set_ParentMat(&m_CombinedWorldMatrix);
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
    for (auto pSpriteParticle : m_pSpriteParticles) {
        pSpriteParticle->Priority_Update(fTimeDelta);
    }
    for (auto pSpriteEffect : m_pSpriteEffects) {
        pSpriteEffect->Priority_Update(fTimeDelta);
    }
}

HRESULT CEffect::Load_Binary(const _char* szFile)
{

    CMeshEffect::MESH_EFFECT_DATA		    MeshDesc{};
    CPointParticle::POINT_PARTICLE_DATA		ParticleDesc{};
    CSpriteParticle::SPRITE_PARTICLE_DATA	SpriteParticleDesc{};
    CSpriteUVEffect::SPRITE_DATA		    SpriteDesc{};
    char szBinModelFilePath[MAX_PATH] = {};
    strcat_s(szBinModelFilePath, MAX_PATH, szFile);
    ifstream fileBinaryStream;
    fileBinaryStream.open(szBinModelFilePath, ios_base::binary);
    _int iMeshCount = ReadInt(fileBinaryStream);
    for (_uint i = 0; i < iMeshCount; ++i) {
        _char* szTemp = ReadString(fileBinaryStream);
        MeshDesc.szModel = szTemp;
        Safe_Delete(szTemp);
        szTemp = ReadString(fileBinaryStream);
        MeshDesc.szMaskTexture = szTemp;
        Safe_Delete(szTemp);
        szTemp = ReadString(fileBinaryStream);
        MeshDesc.szDiffuseTexture = szTemp;
        Safe_Delete(szTemp);
        szTemp = ReadString(fileBinaryStream);
        MeshDesc.szDissolveTexture = szTemp;
        Safe_Delete(szTemp);
        _int iSizeDiagramCount = ReadInt(fileBinaryStream);
        MeshDesc.fSizeDiagrams.clear();
        for (_uint j = 0; j < iSizeDiagramCount; ++j) {
            MeshDesc.fSizeDiagrams.push_back(ReadFloat3(fileBinaryStream));
        }
        MeshDesc.fColor = ReadFloat4(fileBinaryStream);
        MeshDesc.fPosition = ReadFloat4(fileBinaryStream);
        MeshDesc.fScale = ReadFloat3(fileBinaryStream);
        MeshDesc.fRotation = ReadFloat3(fileBinaryStream);




        MeshDesc.fMaskUV = ReadFloat2(fileBinaryStream);
        MeshDesc.fMaskUVSpeed = ReadFloat2(fileBinaryStream);
        MeshDesc.fMaskUVSize = ReadFloat2(fileBinaryStream);
        MeshDesc.fDiffuseUV = ReadFloat2(fileBinaryStream);
        MeshDesc.fDiffuseUVSpeed = ReadFloat2(fileBinaryStream);
        MeshDesc.fDiffuseUVSize = ReadFloat2(fileBinaryStream);
        MeshDesc.fDissolveUV = ReadFloat2(fileBinaryStream);
        MeshDesc.fDissolveUVSpeed = ReadFloat2(fileBinaryStream);
        MeshDesc.fDissolveUVSize = ReadFloat2(fileBinaryStream);
        MeshDesc.fDelayTime = ReadFloat(fileBinaryStream);
        MeshDesc.fEndTime = ReadFloat(fileBinaryStream);



        MeshDesc.iBegin = ReadInt(fileBinaryStream);
        MeshDesc.iSelectRender = ReadInt(fileBinaryStream);

        CMeshEffect* pMeshEffect = CMeshEffect::Create(m_pDevice, m_pContext, &MeshDesc);
        m_pMeshEffects.push_back(pMeshEffect);
    }
    _int iParticleCount = ReadInt(fileBinaryStream);
    for (_uint i = 0; i < iParticleCount; ++i) {
        _char* szTemp = ReadString(fileBinaryStream);
        ParticleDesc.szMaskTexture = szTemp;
        Safe_Delete(szTemp);
        szTemp = ReadString(fileBinaryStream);
        ParticleDesc.szDiffuseTexture = szTemp;
        Safe_Delete(szTemp);
        szTemp = ReadString(fileBinaryStream);
        ParticleDesc.szDissolveTexture = szTemp;
        Safe_Delete(szTemp);
        szTemp = ReadString(fileBinaryStream);
        ParticleDesc.szCS = szTemp;
        Safe_Delete(szTemp);
        _int iSizeDiagramCount = ReadInt(fileBinaryStream);
        ParticleDesc.fSizeDiagrams.clear();
        for (_uint j = 0; j < iSizeDiagramCount; ++j) {
            ParticleDesc.fSizeDiagrams.push_back(ReadFloat3(fileBinaryStream));
        }
        ParticleDesc.fGravityDiagram = ReadFloat4(fileBinaryStream);
        ParticleDesc.fPosition = ReadFloat4(fileBinaryStream);
        ParticleDesc.fColor = ReadFloat4(fileBinaryStream);
        ParticleDesc.fCenter = ReadFloat3(fileBinaryStream);
        ParticleDesc.fPivot = ReadFloat3(fileBinaryStream);
        ParticleDesc.fRange = ReadFloat3(fileBinaryStream);
        ParticleDesc.fRotation = ReadFloat3(fileBinaryStream);
        ParticleDesc.fSize = ReadFloat2(fileBinaryStream);
        ParticleDesc.fLifeTime = ReadFloat2(fileBinaryStream);
        ParticleDesc.fSpeed = ReadFloat2(fileBinaryStream);
        ParticleDesc.fTurnPower = ReadFloat2(fileBinaryStream);

        ParticleDesc.fMaskUV = ReadFloat2(fileBinaryStream);
        ParticleDesc.fMaskUVSpeed = ReadFloat2(fileBinaryStream);
        ParticleDesc.fMaskUVSize = ReadFloat2(fileBinaryStream);
        ParticleDesc.fDiffuseUV = ReadFloat2(fileBinaryStream);
        ParticleDesc.fDiffuseUVSpeed = ReadFloat2(fileBinaryStream);
        ParticleDesc.fDiffuseUVSize = ReadFloat2(fileBinaryStream);
        ParticleDesc.fDissolveUV = ReadFloat2(fileBinaryStream);
        ParticleDesc.fDissolveUVSpeed = ReadFloat2(fileBinaryStream);
        ParticleDesc.fDissolveUVSize = ReadFloat2(fileBinaryStream);
        ParticleDesc.fCircle = ReadFloat2(fileBinaryStream);
        ParticleDesc.fDelayTime = ReadFloat(fileBinaryStream);
        ParticleDesc.fEndTime = ReadFloat(fileBinaryStream);
        ParticleDesc.fSphereSize = ReadFloat(fileBinaryStream);
        ParticleDesc.fCircleSpeed = ReadFloat(fileBinaryStream);

        ParticleDesc.iBegin = ReadInt(fileBinaryStream);
        ParticleDesc.iNumInstance = ReadInt(fileBinaryStream);
        ParticleDesc.iSelectRender = ReadInt(fileBinaryStream);

        ParticleDesc.bisBillboard = ReadBool(fileBinaryStream);
        ParticleDesc.bisLoop = ReadBool(fileBinaryStream);
        ParticleDesc.bisSphere = ReadBool(fileBinaryStream);
        ParticleDesc.bisCircle = ReadBool(fileBinaryStream);
        ParticleDesc.bisSpectrum = ReadBool(fileBinaryStream);

        CPointParticle* pParticle = CPointParticle::Create(m_pDevice, m_pContext, &ParticleDesc);
        m_pPointParticles.push_back(pParticle);
    }
    _int iSpriteParticleCount = ReadInt(fileBinaryStream);
    for (_uint i = 0; i < iSpriteParticleCount; ++i) {
        _char* szTemp = ReadString(fileBinaryStream);
        SpriteParticleDesc.szMaskTexture = szTemp;
        Safe_Delete(szTemp);
        szTemp = ReadString(fileBinaryStream);
        SpriteParticleDesc.szDiffuseTexture = szTemp;
        Safe_Delete(szTemp);
        szTemp = ReadString(fileBinaryStream);
        SpriteParticleDesc.szNormalTexture = szTemp;
        Safe_Delete(szTemp);
        szTemp = ReadString(fileBinaryStream);
        SpriteParticleDesc.szCS = szTemp;
        Safe_Delete(szTemp);
        _int iSizeDiagramCount = ReadInt(fileBinaryStream);
        SpriteParticleDesc.fSizeDiagrams.clear();
        for (_uint j = 0; j < iSizeDiagramCount; ++j) {
            SpriteParticleDesc.fSizeDiagrams.push_back(ReadFloat3(fileBinaryStream));
        }
        SpriteParticleDesc.fGravityDiagram = ReadFloat4(fileBinaryStream);
        SpriteParticleDesc.fPosition = ReadFloat4(fileBinaryStream);
        SpriteParticleDesc.fColor = ReadFloat4(fileBinaryStream);
        SpriteParticleDesc.fCenter = ReadFloat3(fileBinaryStream);
        SpriteParticleDesc.fPivot = ReadFloat3(fileBinaryStream);
        SpriteParticleDesc.fRange = ReadFloat3(fileBinaryStream);
        SpriteParticleDesc.fRotation = ReadFloat3(fileBinaryStream);
        SpriteParticleDesc.fSize = ReadFloat2(fileBinaryStream);
        SpriteParticleDesc.fLifeTime = ReadFloat2(fileBinaryStream);
        SpriteParticleDesc.fSpeed = ReadFloat2(fileBinaryStream);
        SpriteParticleDesc.fTurnPower = ReadFloat2(fileBinaryStream);

        SpriteParticleDesc.fMaskUV = ReadFloat2(fileBinaryStream);
        SpriteParticleDesc.fMaskUVSpeed = ReadFloat2(fileBinaryStream);
        SpriteParticleDesc.fMaskUVSize = ReadFloat2(fileBinaryStream);
        SpriteParticleDesc.fDiffuseUV = ReadFloat2(fileBinaryStream);
        SpriteParticleDesc.fDiffuseUVSpeed = ReadFloat2(fileBinaryStream);
        SpriteParticleDesc.fDiffuseUVSize = ReadFloat2(fileBinaryStream);
        SpriteParticleDesc.fDissolveUV = ReadFloat2(fileBinaryStream);
        SpriteParticleDesc.fDissolveUVSpeed = ReadFloat2(fileBinaryStream);
        SpriteParticleDesc.fDissolveUVSize = ReadFloat2(fileBinaryStream);
        SpriteParticleDesc.fParticleSize = ReadFloat2(fileBinaryStream);
        SpriteParticleDesc.fCircle = ReadFloat2(fileBinaryStream);
        SpriteParticleDesc.iUV = ReadInt2(fileBinaryStream);
        SpriteParticleDesc.fDelayTime = ReadFloat(fileBinaryStream);
        SpriteParticleDesc.fEndTime = ReadFloat(fileBinaryStream);
        SpriteParticleDesc.fSphereSize = ReadFloat(fileBinaryStream);
        SpriteParticleDesc.fAngle = ReadFloat(fileBinaryStream);
        SpriteParticleDesc.fCircleSpeed = ReadFloat(fileBinaryStream);

        SpriteParticleDesc.iBegin = ReadInt(fileBinaryStream);
        SpriteParticleDesc.iNumInstance = ReadInt(fileBinaryStream);
        SpriteParticleDesc.iSelectRender = ReadInt(fileBinaryStream);

        SpriteParticleDesc.bisBillboard = ReadBool(fileBinaryStream);
        SpriteParticleDesc.bisLoop = ReadBool(fileBinaryStream);
        SpriteParticleDesc.bisSphere = ReadBool(fileBinaryStream);
        SpriteParticleDesc.bisCircle = ReadBool(fileBinaryStream);
        SpriteParticleDesc.bisSpectrum = ReadBool(fileBinaryStream);

        CSpriteParticle* pParticle = CSpriteParticle::Create(m_pDevice, m_pContext, &SpriteParticleDesc);
        m_pSpriteParticles.push_back(pParticle);
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
    if (0 <= m_fStopTime) {
        m_fStopTime -= fTimeDelta;
    }
    else if(-10 < m_fStopTime){
        m_fStopTime = -10;
        for (auto pParticle : m_pPointParticles) {
            pParticle->End();
        }
        for (auto pSpriteParticle : m_pSpriteParticles) {
            pSpriteParticle->End();
        }
    }
    for (auto pMeshEffect : m_pMeshEffects) {
        pMeshEffect->Update(fTimeDelta);
    }
    for (auto pParticle : m_pPointParticles) {
        pParticle->Update(fTimeDelta);
    }
    for (auto pSpriteParticle : m_pSpriteParticles) {
        pSpriteParticle->Update(fTimeDelta);
    }
    for (auto pSpriteEffect : m_pSpriteEffects) {
        pSpriteEffect->Update(fTimeDelta);
    }
}

void CEffect::Late_Update(_float fTimeDelta)
{

    if (nullptr != m_pParentMat) {
        if (nullptr != m_pParentWorldMat) {
            XMStoreFloat4x4(&m_CombinedWorldMatrix, 
                XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()) * XMLoadFloat4x4(m_pParentMat) * XMLoadFloat4x4(m_pParentWorldMat));
            if (m_bisFloor) {
                m_CombinedWorldMatrix._42 = m_pParentWorldMat->_42;
                _float  fLength = XMVectorGetX(XMVector3Length(XMVectorSet(m_CombinedWorldMatrix._11, m_CombinedWorldMatrix._12, m_CombinedWorldMatrix._13, m_CombinedWorldMatrix._14)));
                _vector vLook = XMVector3Normalize(XMVectorSet(m_CombinedWorldMatrix._31, 0, m_CombinedWorldMatrix._33, 0));
                _vector vRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0,1,0,0), vLook));
                _vector vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight));
                vRight *= fLength;
                vUp *= fLength;
                vLook *= fLength;

                XMStoreFloat4(reinterpret_cast<_float4*>(&m_CombinedWorldMatrix.m[0]), vRight);
                XMStoreFloat4(reinterpret_cast<_float4*>(&m_CombinedWorldMatrix.m[1]), vUp);
                XMStoreFloat4(reinterpret_cast<_float4*>(&m_CombinedWorldMatrix.m[2]), vLook);
            }
        }
        else {
            XMStoreFloat4x4(&m_CombinedWorldMatrix,
                XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()) * XMLoadFloat4x4(m_pParentMat));
            if (m_bisFloor) {
                m_CombinedWorldMatrix._42 = m_pParentWorldMat->_42;
                _float  fLength = XMVectorGetX(XMVector3Length(XMVectorSet(m_CombinedWorldMatrix._11, m_CombinedWorldMatrix._12, m_CombinedWorldMatrix._13, m_CombinedWorldMatrix._14)));
                _vector vLook = XMVector3Normalize(XMVectorSet(m_CombinedWorldMatrix._31, 0, m_CombinedWorldMatrix._33, 0));
                _vector vRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0, 1, 0, 0), vLook));
                _vector vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight));
                vRight *= fLength;
                vUp *= fLength;
                vLook *= fLength;

                XMStoreFloat4(reinterpret_cast<_float4*>(&m_CombinedWorldMatrix.m[0]), vRight);
                XMStoreFloat4(reinterpret_cast<_float4*>(&m_CombinedWorldMatrix.m[1]), vUp);
                XMStoreFloat4(reinterpret_cast<_float4*>(&m_CombinedWorldMatrix.m[2]), vLook);
            }
        }
    }
    if (1 < m_pMeshEffects.size()) {
        for (auto i = m_pMeshEffects.begin(); i != m_pMeshEffects.end();) {
            if ((*i)->isDead()) {
                Safe_Release((*i));
                i = m_pMeshEffects.erase(i);
            }
            else {
                (*i)->Late_Update(fTimeDelta);
                ++i;
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
                ++i;
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
    if (1 < m_pSpriteParticles.size()) {
        for (auto i = m_pSpriteParticles.begin(); i != m_pSpriteParticles.end();) {
            if ((*i)->isDead()) {
                Safe_Release((*i));
                i = m_pSpriteParticles.erase(i);
            }
            else {
                (*i)->Late_Update(fTimeDelta);
                ++i;
            }
        }
    }
    else if (1 == m_pSpriteParticles.size()) {
        if (m_pSpriteParticles[0]->isDead()) {
            Safe_Release(m_pSpriteParticles[0]);
            m_pSpriteParticles.clear();
        }
        else {
            m_pSpriteParticles[0]->Late_Update(fTimeDelta);
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
                ++i;
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
    if (0 >= m_pMeshEffects.size() + m_pPointParticles.size() + m_pSpriteParticles.size() + m_pSpriteEffects.size())
        m_isDead = true;
}

HRESULT CEffect::Render()
{
    return S_OK;
}

void CEffect::Stop() {
    for (auto pParticle : m_pPointParticles)
        pParticle->Stop();
    for (auto pSpriteParticle : m_pSpriteParticles)
        pSpriteParticle->Stop();
}

void CEffect::Play(_float fTime)
{
    m_fStopTime = fTime;
    for (auto pParticle : m_pPointParticles)
        pParticle->Play();
    for (auto pSpriteParticle : m_pSpriteParticles)
        pSpriteParticle->Play();
}

void CEffect::End()
{
    for (auto pParticle : m_pPointParticles)
        pParticle->End();
    for (auto pSpriteParticle : m_pSpriteParticles)
        pSpriteParticle->End();
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
    for (auto pSpriteParticle : m_pSpriteParticles)
        Safe_Release(pSpriteParticle);
    m_pSpriteParticles.clear();
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

RENDER CEffect::ReadRENDER(ifstream& fileBinaryStream)
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
