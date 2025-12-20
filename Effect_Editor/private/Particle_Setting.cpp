#include "pch.h"
#include "GameObject.h"
#include "Particle_Setting.h"
#include "GameInstance.h"
#include "fstream"

CParticle_Setting::CParticle_Setting(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CImgBase{ pDevice, pContext } {}

CParticle_Setting* CParticle_Setting::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    auto p = new CParticle_Setting(pDevice, pContext);
    if (FAILED(p->Initialize()))
    {
        Safe_Release(p);
        MSG_BOX("CREATE FAIL : CParticle_Setting");
        return nullptr;
    }
    return p;
}

HRESULT CParticle_Setting::Initialize()
{
    m_fTime = 0.f;
    m_tParticleData.iNumInstance = 100;
    m_tParticleData.iBegin = 0;
    m_tParticleData.fCenter = _float3(0.0f, 0.f, 0.f);
    m_tParticleData.fPivot = _float3(0.0f, 0.f, 0.0f);
    m_tParticleData.fRange = _float3(1.f, 1.f, 1.f);
    m_tParticleData.fSize = _float2(0.5f, 1.f);
    m_tParticleData.fLifeTime = _float2(0.5f, 1.f);
    m_tParticleData.fSpeed = _float2(1.f, 0.5f);
    m_tParticleData.bisLoop = true;
    m_tParticleData.fGravityDiagram = _float4(0, 0, 0, 0);
    m_tParticleData.iSelectRender = 3;
    m_tParticleData.fColor = { 0,0,0,1 };
    m_tParticleData.szCS = "CS";
    m_tParticleData.fSizeDiagrams.clear();
    m_tParticleData.fSizeDiagrams.push_back(_float3(0, 1, 0));
    m_tParticleData.fSizeDiagrams.push_back(_float3(1, 1, 0));
    m_tParticleData.fPosition = _float4(0, 0, 0, 1);
    m_tParticleData.fRotation = _float3(0, 0, 0);
    m_szCS = "CS";

    m_pTransform = CTransform::Create(m_pDevice, m_pContext);
    m_fPosition = { 0,0,0,1 };
    m_fScale = { 1,1,1 };
    m_fRotation = { 0,0,0 };
    m_pTransform->Set_State(STATE::POSITION, XMLoadFloat4(&m_fPosition));
    m_pTransform->Set_Scale(m_fScale.x, m_fScale.y, m_fScale.z);
    m_pTransform->Rotation(XMConvertToRadians(m_fRotation.x), XMConvertToRadians(m_fRotation.y), XMConvertToRadians(m_fRotation.z));


    char pattern[MAX_PATH] = {};
    strcpy_s(pattern, MAX_PATH, "../Bin/Resources/Textures/MaskTexture/*.dds");
    m_ImageFiles[0].clear();
    m_ImageFiles[1].clear();
    m_ImageFiles[2].clear();
    m_SRVs[0].clear();
    m_SRVs[1].clear();
    m_SRVs[2].clear();
    WIN32_FIND_DATAA fd{};
    HANDLE h = FindFirstFileA(pattern, &fd);
    if (h != INVALID_HANDLE_VALUE) {
        do {
            if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
                char szProtoName[MAX_PATH] = {};
                strcpy_s(szProtoName, MAX_PATH, "Prototype_Component_Texture_Mask_");
                strcat_s(szProtoName, MAX_PATH, fd.cFileName);
                m_ImageFiles[0].emplace_back(szProtoName);

                char szFilePath[MAX_PATH] = {};
                strcpy_s(szFilePath, MAX_PATH, "../Bin/Resources/Textures/MaskTexture/");
                strcat_s(szFilePath, MAX_PATH, fd.cFileName);

                ID3D11ShaderResourceView* pSRV = { nullptr };
                _tchar szPath[256] = { 0, };
                MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, szFilePath, strlen(szFilePath), szPath, 256);
                HRESULT			hr = CreateDDSTextureFromFile(m_pDevice, szPath, nullptr, &pSRV);
                if (FAILED(hr))
                    return E_FAIL;
                m_SRVs[0].push_back(pSRV);

            }
        } while (FindNextFileA(h, &fd));
        FindClose(h);
    }

    memset(pattern, 0, sizeof(pattern));
    strcpy_s(pattern, MAX_PATH, "../Bin/Resources/Textures/DiffuseTexture/*.dds");

    h = FindFirstFileA(pattern, &fd);
    if (h != INVALID_HANDLE_VALUE) {
        do {
            if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
                char szProtoName[MAX_PATH] = {};
                strcpy_s(szProtoName, MAX_PATH, "Prototype_Component_Texture_Diffuse_");
                strcat_s(szProtoName, MAX_PATH, fd.cFileName);
                m_ImageFiles[1].emplace_back(szProtoName);

                char szFilePath[MAX_PATH] = {};
                strcpy_s(szFilePath, MAX_PATH, "../Bin/Resources/Textures/DiffuseTexture/");
                strcat_s(szFilePath, MAX_PATH, fd.cFileName);

                ID3D11ShaderResourceView* pSRV = { nullptr };
                _tchar szPath[256] = { 0, };
                MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, szFilePath, strlen(szFilePath), szPath, 256);
                HRESULT			hr = CreateDDSTextureFromFile(m_pDevice, szPath, nullptr, &pSRV);
                if (FAILED(hr))
                    return E_FAIL;
                m_SRVs[1].push_back(pSRV);
            }
        } while (FindNextFileA(h, &fd));
        FindClose(h);
    }

    memset(pattern, 0, sizeof(pattern));
    strcpy_s(pattern, MAX_PATH, "../Bin/Resources/Textures/DissolveTexture/*.dds");

    h = FindFirstFileA(pattern, &fd);
    if (h != INVALID_HANDLE_VALUE) {
        do {
            if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
                char szProtoName[MAX_PATH] = {};
                strcpy_s(szProtoName, MAX_PATH, "Prototype_Component_Texture_Dissolve_");
                strcat_s(szProtoName, MAX_PATH, fd.cFileName);
                m_ImageFiles[2].emplace_back(szProtoName);

                char szFilePath[MAX_PATH] = {};
                strcpy_s(szFilePath, MAX_PATH, "../Bin/Resources/Textures/DissolveTexture/");
                strcat_s(szFilePath, MAX_PATH, fd.cFileName);

                ID3D11ShaderResourceView* pSRV = { nullptr };
                _tchar szPath[256] = { 0, };
                MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, szFilePath, strlen(szFilePath), szPath, 256);
                HRESULT			hr = CreateDDSTextureFromFile(m_pDevice, szPath, nullptr, &pSRV);
                if (FAILED(hr))
                    return E_FAIL;
                m_SRVs[2].push_back(pSRV);
            }
        } while (FindNextFileA(h, &fd));
        FindClose(h);
    }

    memset(pattern, 0, sizeof(pattern));
    strcpy_s(pattern, MAX_PATH, "../Bin/Resources/Textures/NormalTexture/*.dds");

    h = FindFirstFileA(pattern, &fd);
    if (h != INVALID_HANDLE_VALUE) {
        do {
            if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
                char szProtoName[MAX_PATH] = {};
                strcpy_s(szProtoName, MAX_PATH, "Prototype_Component_Texture_Normal_");
                strcat_s(szProtoName, MAX_PATH, fd.cFileName);
                m_ImageFiles[3].emplace_back(szProtoName);

                char szFilePath[MAX_PATH] = {};
                strcpy_s(szFilePath, MAX_PATH, "../Bin/Resources/Textures/NormalTexture/");
                strcat_s(szFilePath, MAX_PATH, fd.cFileName);

                ID3D11ShaderResourceView* pSRV = { nullptr };
                _tchar szPath[256] = { 0, };
                MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, szFilePath, strlen(szFilePath), szPath, 256);
                HRESULT			hr = CreateDDSTextureFromFile(m_pDevice, szPath, nullptr, &pSRV);
                if (FAILED(hr))
                    return E_FAIL;
                m_SRVs[3].push_back(pSRV);
            }
        } while (FindNextFileA(h, &fd));
        FindClose(h);
    }

    memset(pattern, 0, sizeof(pattern));
    strcpy_s(pattern, MAX_PATH, "../Bin/Resources/Models/EffectMesh/*.binx");

    h = FindFirstFileA(pattern, &fd);
    if (h != INVALID_HANDLE_VALUE) {
        do {
            if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {

                char szFilePath[MAX_PATH] = {};
                strcpy_s(szFilePath, MAX_PATH, "../Bin/Resources/Models/EffectMesh/");
                strcat_s(szFilePath, MAX_PATH, fd.cFileName);
                m_ModelFilePaths.push_back(fd.cFileName);
                m_ModelFiles.push_back(szFilePath);
            }
        } while (FindNextFileA(h, &fd));
        FindClose(h);
    }

    memset(pattern, 0, sizeof(pattern));
    strcpy_s(pattern, MAX_PATH, "../Bin/Resources/Effect/*.binx");

    h = FindFirstFileA(pattern, &fd);
    if (h != INVALID_HANDLE_VALUE) {
        do {
            if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
                m_Effects.push_back(fd.cFileName);
            }
        } while (FindNextFileA(h, &fd));
        FindClose(h);
    }

    memset(pattern, 0, sizeof(pattern));
    strcpy_s(pattern, MAX_PATH, "../Bin/Resources/TrailEffect/*.binx");

    h = FindFirstFileA(pattern, &fd);
    if (h != INVALID_HANDLE_VALUE) {
        do {
            if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
                m_TrailEffects.push_back(fd.cFileName);
            }
        } while (FindNextFileA(h, &fd));
        FindClose(h);
    }
    m_iSelectEffect = 0;
    m_iSelectParticle = 0;
    m_iSelectMesh = 0;

    CTrailData::TRAIL_DATA tTrailData;

    for (_uint i = 0; i < 3; ++i) {
        switch (i) {
        case 0:
            tTrailData.szMaskTexture = m_ImageFiles[i][0];
            break;
        case 1:
            tTrailData.szDiffuseTexture = m_ImageFiles[i][0];
            break;
        case 2:
            tTrailData.szDissolveTexture = m_ImageFiles[i][0];
            break;
        }
    }
    tTrailData.fColor = _float4(0, 0, 0, 1);

    tTrailData.fMaskUV = _float2(0, 0);
    tTrailData.fMaskUVSpeed = _float2(0, 0);
    tTrailData.fMaskUVSize = _float2(1, 1);
    tTrailData.fDiffuseUV = _float2(0, 0);
    tTrailData.fDiffuseUVSpeed = _float2(0, 0);
    tTrailData.fDiffuseUVSize = _float2(1, 1);
    tTrailData.fDissolveUV = _float2(0, 0);
    tTrailData.fDissolveUVSpeed = _float2(0, 0);
    tTrailData.fDissolveUVSize = _float2(1, 1);
    tTrailData.iBegin = 0;
    tTrailData.iSelectRender = 3;

    m_pTrailEffect = CTrailEffect::Create(m_pDevice, m_pContext);
    m_pTrailEffect->Initialize(nullptr);

    CTrailData* pData = CTrailData::Create(m_pDevice, m_pContext);
    pData->Initialize(m_pTrailEffect);
    pData->Set_Components(tTrailData);
    m_pTrailDatas.push_back(pData);
    m_iSelectTrailData = 0;
    m_tTrailData = m_pTrailDatas[m_iSelectTrailData]->Get_Data();

    return S_OK;
}

void CParticle_Setting::Add_Particle()
{
    CParticle::PARTICLE_DATA tParticleData;

    char szModelPath[MAX_PATH] = {};
    strncpy_s(szModelPath, sizeof(szModelPath), m_ModelFilePaths[0].c_str(), _TRUNCATE);
    tParticleData.szModel = szModelPath;
    tParticleData.szMaskTexture = m_ImageFiles[0][0];
    tParticleData.szDiffuseTexture = m_ImageFiles[1][0];
    tParticleData.szDissolveTexture = m_ImageFiles[2][0];
    tParticleData.szCS = "CS";
    tParticleData.fSizeDiagrams.clear();
    tParticleData.fSizeDiagrams.push_back(_float3(0, 1, 0));
    tParticleData.fSizeDiagrams.push_back(_float3(1, 1, 0));
    tParticleData.fGravityDiagram = _float4(0, 0, 0, 0);
    tParticleData.fPosition = _float4(0, 0, 0, 1);
    tParticleData.fColor = { 0,0,0,1 };
    tParticleData.fCenter = _float3(0.0f, 0.f, 0.f);
    tParticleData.fPivot = _float3(0.0f, 0.f, 0.0f);
    tParticleData.fRange = _float3(1.f, 1.f, 1.f);
    tParticleData.fRotation = _float3(0, 0, 0);
    tParticleData.fMeshRotation = _float3(0, 0, 0);
    tParticleData.fSize = _float2(0.5f, 1.f);
    tParticleData.fLifeTime = _float2(0.5f, 1.f);
    tParticleData.fSpeed = _float2(1.f, 0.5f);
    tParticleData.fTurnPower = _float2(1.f, 1.f);
    tParticleData.fMaskUV = _float2(0, 0);
    tParticleData.fMaskUVSpeed = _float2(0, 0);
    tParticleData.fMaskUVSize = _float2(1, 1);
    tParticleData.fDiffuseUV = _float2(0, 0);
    tParticleData.fDiffuseUVSpeed = _float2(0, 0);
    tParticleData.fDiffuseUVSize = _float2(1, 1);
    tParticleData.fDissolveUV = _float2(0, 0);
    tParticleData.fDissolveUVSpeed = _float2(0, 0);
    tParticleData.fDissolveUVSize = _float2(1, 1);
    tParticleData.fCircle = _float2(1, 5);
    tParticleData.fDelayTime = 0.f;
    tParticleData.fEndTime = 0.f;
    tParticleData.fSphereSize = 1.f;
    tParticleData.fCircleSpeed = 1.f;
    tParticleData.iBegin = 0;
    tParticleData.iNumInstance = 100;
    tParticleData.iSelectRender = 3;
    tParticleData.bisLoop = true;
    tParticleData.bisSphere = false;
    tParticleData.bisCircle = false;
    tParticleData.bisSpectrum = true;

    CParticle* pParticle = CParticle::Create(m_pDevice, m_pContext);
    pParticle->Initialize(nullptr);

    pParticle->Set_Components(tParticleData);
    pParticle->Set_ParentMat(m_pTransform->Get_WorldMatrixPtr());

    m_iSelectParticle = m_pParticles.size();
    m_pParticles.push_back(pParticle);
    m_tParticleData = m_pParticles[m_iSelectParticle]->Get_Data();
}

void CParticle_Setting::Delete_Particle()
{
    if (1 < m_pParticles.size()) {
        _uint i = 0;
        for (auto j = m_pParticles.begin(); j != m_pParticles.end();) {
            if (m_iSelectParticle == i) {
                Safe_Release((*j));
                j = m_pParticles.erase(j);
                m_iSelectParticle = max(0, m_iSelectParticle - 1);
                m_tParticleData = m_pParticles[m_iSelectParticle]->Get_Data();
                break;
            }
            ++i;
            ++j;
        }
    }
    else {
        Safe_Release(m_pParticles[0]);
        m_pParticles.clear();
    }
}

void CParticle_Setting::Add_SpriteParticle()
{
    CSpriteParticle::SPRITE_PARTICLE_DATA tSpriteParticleData;
    tSpriteParticleData.szMaskTexture = m_ImageFiles[0][0];
    tSpriteParticleData.szDiffuseTexture = m_ImageFiles[1][0];
    tSpriteParticleData.szNormalTexture = m_ImageFiles[3][0];
    tSpriteParticleData.szCS = "CS";
    tSpriteParticleData.fSizeDiagrams.clear();
    tSpriteParticleData.fSizeDiagrams.push_back(_float3(0, 1, 0));
    tSpriteParticleData.fSizeDiagrams.push_back(_float3(1, 1, 0));
    tSpriteParticleData.fGravityDiagram = _float4(0, 0, 0, 0);
    tSpriteParticleData.fPosition = _float4(0, 0, 0, 1);
    tSpriteParticleData.fColor = { 0,0,0,1 };
    tSpriteParticleData.fCenter = _float3(0.0f, 0.f, 0.f);
    tSpriteParticleData.fPivot = _float3(0.0f, 0.f, 0.0f);
    tSpriteParticleData.fRange = _float3(1.f, 1.f, 1.f);
    tSpriteParticleData.fRotation = _float3(0, 0, 0);
    tSpriteParticleData.fSize = _float2(0.5f, 1.f);
    tSpriteParticleData.fLifeTime = _float2(0.5f, 1.f);
    tSpriteParticleData.fSpeed = _float2(1.f, 0.5f);
    tSpriteParticleData.fTurnPower = _float2(1.f, 1.f);
    tSpriteParticleData.fMaskUV = _float2(0, 0);
    tSpriteParticleData.fMaskUVSpeed = _float2(0, 0);
    tSpriteParticleData.fMaskUVSize = _float2(1, 1);
    tSpriteParticleData.fDiffuseUV = _float2(0, 0);
    tSpriteParticleData.fDiffuseUVSpeed = _float2(0, 0);
    tSpriteParticleData.fDiffuseUVSize = _float2(1, 1);
    tSpriteParticleData.fDissolveUV = _float2(0, 0);
    tSpriteParticleData.fDissolveUVSpeed = _float2(0, 0);
    tSpriteParticleData.fDissolveUVSize = _float2(1, 1);
    tSpriteParticleData.fParticleSize = _float2(1, 1);
    tSpriteParticleData.fCircle = _float2(1, 5);
    tSpriteParticleData.iUV = _int2(1, 1);
    tSpriteParticleData.fDelayTime = 0.f;
    tSpriteParticleData.fEndTime = 0.f;
    tSpriteParticleData.fSphereSize = 1.f;
    tSpriteParticleData.fAngle = 0.f;
    tSpriteParticleData.fCircleSpeed = 1.f;
    tSpriteParticleData.iBegin = 0;
    tSpriteParticleData.iNumInstance = 100;
    tSpriteParticleData.iSelectRender = 3;
    tSpriteParticleData.bisBillboard = true;
    tSpriteParticleData.bisAngleBillboard = false;
    tSpriteParticleData.bisStart = false;
    tSpriteParticleData.bisLoop = true;
    tSpriteParticleData.bisSphere = false;
    tSpriteParticleData.bisCircle = false;
    tSpriteParticleData.bisSpectrum = true;

    CSpriteParticle* pParticle = CSpriteParticle::Create(m_pDevice, m_pContext);
    pParticle->Initialize(nullptr);

    pParticle->Set_Components(tSpriteParticleData);
    pParticle->Set_ParentMat(m_pTransform->Get_WorldMatrixPtr());

    m_iSelectSpriteParticle = m_pSpriteParticles.size();
    m_pSpriteParticles.push_back(pParticle);
    m_tSpriteParticleData = m_pSpriteParticles[m_iSelectSpriteParticle]->Get_Data();
}

void CParticle_Setting::Delete_SpriteParticle()
{
    if (1 < m_pSpriteParticles.size()) {
        _uint i = 0;
        for (auto j = m_pSpriteParticles.begin(); j != m_pSpriteParticles.end();) {
            if (m_iSelectSpriteParticle == i) {
                Safe_Release((*j));
                j = m_pSpriteParticles.erase(j);
                m_iSelectSpriteParticle = max(0, m_iSelectSpriteParticle - 1);
                m_tSpriteParticleData = m_pSpriteParticles[m_iSelectSpriteParticle]->Get_Data();
                break;
            }
            ++i;
            ++j;
        }
    }
    else {
        Safe_Release(m_pSpriteParticles[0]);
        m_pSpriteParticles.clear();
    }
}

void CParticle_Setting::Add_MeshEffect()
{
    CMeshEffect::MeshEffectData		MeshDesc{};

    char szModelPath[MAX_PATH] = {};
    strncpy_s(szModelPath, sizeof(szModelPath), m_ModelFilePaths[0].c_str(), _TRUNCATE);
    MeshDesc.szModel = szModelPath;
    MeshDesc.szMaskTexture = m_ImageFiles[0][0];
    MeshDesc.szDiffuseTexture = m_ImageFiles[1][0];
    MeshDesc.szDissolveTexture = m_ImageFiles[2][0];
    MeshDesc.fSizeDiagrams.clear();
    MeshDesc.fSizeDiagrams.push_back(_float3(0, 1, 0));
    MeshDesc.fSizeDiagrams.push_back(_float3(1, 1, 0));
    MeshDesc.fColor = { 0,0,0,1 };
    MeshDesc.fPosition = _float4(0, 0, 0, 1);
    MeshDesc.fScale = { 1,1,1 };
    MeshDesc.fRotation = _float3(0, 0, 0);
    MeshDesc.fMaskUV = _float2(0, 0);
    MeshDesc.fMaskUVSpeed = _float2(0, 0);
    MeshDesc.fMaskUVSize = _float2(1, 1);
    MeshDesc.fDiffuseUV = _float2(0, 0);
    MeshDesc.fDiffuseUVSpeed = _float2(0, 0);
    MeshDesc.fDiffuseUVSize = _float2(1, 1);
    MeshDesc.fDissolveUV = _float2(0, 0);
    MeshDesc.fDissolveUVSpeed = _float2(0, 0);
    MeshDesc.fDissolveUVSize = _float2(1, 1);
    MeshDesc.fDelayTime = 0.f;
    MeshDesc.fEndTime = 0.f;
    MeshDesc.iSelectRender = 3;
    MeshDesc.iBegin = 0;

    CMeshEffect* pMeshEffect = CMeshEffect::Create(m_pDevice, m_pContext);
    pMeshEffect->Initialize(nullptr);

    pMeshEffect->Set_Components(MeshDesc);
    pMeshEffect->Set_ParentMat(m_pTransform->Get_WorldMatrixPtr());

    m_iSelectMesh = m_pMeshs.size();
    m_pMeshs.push_back(pMeshEffect);
    m_tMeshData = m_pMeshs[m_iSelectMesh]->Get_Data();
}

void CParticle_Setting::Delete_MeshEffect()
{
    if (1 < m_pMeshs.size()) {
        _uint i = 0;
        for (auto j = m_pMeshs.begin(); j != m_pMeshs.end();) {
            if (m_iSelectMesh == i) {
                Safe_Release((*j));
                m_pMeshs.erase(j);
                m_iSelectMesh = max(0, m_iSelectMesh - 1);
                m_tMeshData = m_pMeshs[m_iSelectMesh]->Get_Data();
                break;
            }
            ++i;
            ++j;
        }
    }
    else {
        Safe_Release(m_pMeshs[0]);
        m_pMeshs.clear();
    }
}

void CParticle_Setting::Add_TrailEffectData()
{

    CTrailData::TRAIL_DATA tTrailData;

    for (_uint i = 0; i < 3; ++i) {
        switch (i) {
        case 0:
            tTrailData.szMaskTexture = m_ImageFiles[i][0];
            break;
        case 1:
            tTrailData.szDiffuseTexture = m_ImageFiles[i][0];
            break;
        case 2:
            tTrailData.szDissolveTexture = m_ImageFiles[i][0];
            break;
        }
    }
    tTrailData.fColor = _float4(0, 0, 0, 1);

    tTrailData.fMaskUV = _float2(0, 0);
    tTrailData.fMaskUVSpeed = _float2(0, 0);
    tTrailData.fMaskUVSize = _float2(1, 1);
    tTrailData.fDiffuseUV = _float2(0, 0);
    tTrailData.fDiffuseUVSpeed = _float2(0, 0);
    tTrailData.fDiffuseUVSize = _float2(1, 1);
    tTrailData.fDissolveUV = _float2(0, 0);
    tTrailData.fDissolveUVSpeed = _float2(0, 0);
    tTrailData.fDissolveUVSize = _float2(1, 1);
    tTrailData.iBegin = 0;
    tTrailData.iSelectRender = 3;

    CTrailData* pData = CTrailData::Create(m_pDevice, m_pContext);
    pData->Initialize(m_pTrailEffect);
    pData->Set_Components(tTrailData);
    m_pTrailDatas.push_back(pData);
    m_iSelectTrailData = m_pTrailDatas.size() - 1;
    m_tTrailData = m_pTrailDatas[m_iSelectTrailData]->Get_Data();
}

void CParticle_Setting::Delete_TrailEffectData()
{
    m_pTrailDatas[m_iSelectTrailData]->Get_Data();
    if (1 < m_pTrailDatas.size()) {
        _uint i = 0;
        for (auto j = m_pTrailDatas.begin(); j != m_pTrailDatas.end();) {
            if (m_iSelectTrailData == i) {
                Safe_Release((*j));
                m_pTrailDatas.erase(j);
                m_iSelectTrailData = max(0, m_iSelectTrailData - 1);
                m_tTrailData = m_pTrailDatas[m_iSelectTrailData]->Get_Data();
                break;
            }
            ++i;
            ++j;
        }
    }
    else {
        Safe_Release(m_pTrailDatas[0]);
        m_pTrailDatas.clear();
    }
}

HRESULT CParticle_Setting::Save_Binary(const _char* szFile)
{
    char szBinModelFilePath[MAX_PATH] = "../Bin/Resources/Effect/";
    strcat_s(szBinModelFilePath, MAX_PATH, szFile);
    strcat_s(szBinModelFilePath, MAX_PATH, ".binx");
    ofstream fileBinaryStream;
    fileBinaryStream.open(szBinModelFilePath, ios_base::binary);
    WriteInt(fileBinaryStream, m_pMeshs.size());
    for (auto pMesh : m_pMeshs) {
        char szImageFile[MAX_PATH] = {};
        strncpy_s(szImageFile, sizeof(szImageFile), pMesh->Get_Data().szModel.c_str(), _TRUNCATE);
        WriteString(fileBinaryStream, szImageFile);
        memset(szImageFile, 0, sizeof(szImageFile));

        strncpy_s(szImageFile, sizeof(szImageFile), pMesh->Get_Data().szMaskTexture.c_str(), _TRUNCATE);
        WriteString(fileBinaryStream, szImageFile);
        memset(szImageFile, 0, sizeof(szImageFile));

        strncpy_s(szImageFile, sizeof(szImageFile), pMesh->Get_Data().szDiffuseTexture.c_str(), _TRUNCATE);
        WriteString(fileBinaryStream, szImageFile);
        memset(szImageFile, 0, sizeof(szImageFile));

        strncpy_s(szImageFile, sizeof(szImageFile), pMesh->Get_Data().szDissolveTexture.c_str(), _TRUNCATE);
        WriteString(fileBinaryStream, szImageFile);

        WriteInt(fileBinaryStream, pMesh->Get_Data().fSizeDiagrams.size());
        for (auto fSizeDiagram : pMesh->Get_Data().fSizeDiagrams) {
            WriteFloat3(fileBinaryStream, fSizeDiagram);
        }

        WriteFloat4(fileBinaryStream, pMesh->Get_Data().fColor);
        WriteFloat4(fileBinaryStream, pMesh->Get_Data().fPosition);
        WriteFloat3(fileBinaryStream, pMesh->Get_Data().fScale);
        WriteFloat3(fileBinaryStream, pMesh->Get_Data().fRotation);


        WriteFloat2(fileBinaryStream, pMesh->Get_Data().fMaskUV);
        WriteFloat2(fileBinaryStream, pMesh->Get_Data().fMaskUVSpeed);
        WriteFloat2(fileBinaryStream, pMesh->Get_Data().fMaskUVSize);
        WriteFloat2(fileBinaryStream, pMesh->Get_Data().fDiffuseUV);
        WriteFloat2(fileBinaryStream, pMesh->Get_Data().fDiffuseUVSpeed);
        WriteFloat2(fileBinaryStream, pMesh->Get_Data().fDiffuseUVSize);
        WriteFloat2(fileBinaryStream, pMesh->Get_Data().fDissolveUV);
        WriteFloat2(fileBinaryStream, pMesh->Get_Data().fDissolveUVSpeed);
        WriteFloat2(fileBinaryStream, pMesh->Get_Data().fDissolveUVSize);
        WriteFloat(fileBinaryStream, pMesh->Get_Data().fDelayTime);
        WriteFloat(fileBinaryStream, pMesh->Get_Data().fEndTime);


        WriteInt(fileBinaryStream, pMesh->Get_Data().iBegin);
        WriteInt(fileBinaryStream, pMesh->Get_Data().iSelectRender);
    }
    WriteInt(fileBinaryStream, m_pSpriteParticles.size());
    for (auto pSpriteParticle : m_pSpriteParticles) {


        char szImageFile[MAX_PATH] = {};
        strncpy_s(szImageFile, sizeof(szImageFile), pSpriteParticle->Get_Data().szMaskTexture.c_str(), _TRUNCATE);
        WriteString(fileBinaryStream, szImageFile);
        memset(szImageFile, 0, sizeof(szImageFile));

        strncpy_s(szImageFile, sizeof(szImageFile), pSpriteParticle->Get_Data().szDiffuseTexture.c_str(), _TRUNCATE);
        WriteString(fileBinaryStream, szImageFile);
        memset(szImageFile, 0, sizeof(szImageFile));

        strncpy_s(szImageFile, sizeof(szImageFile), pSpriteParticle->Get_Data().szNormalTexture.c_str(), _TRUNCATE);
        WriteString(fileBinaryStream, szImageFile);
        memset(szImageFile, 0, sizeof(szImageFile));

        strncpy_s(szImageFile, sizeof(szImageFile), pSpriteParticle->Get_Data().szCS.c_str(), _TRUNCATE);
        WriteString(fileBinaryStream, szImageFile);

        WriteInt(fileBinaryStream, pSpriteParticle->Get_Data().fSizeDiagrams.size());
        for (auto fSizeDiagram : pSpriteParticle->Get_Data().fSizeDiagrams) {
            WriteFloat3(fileBinaryStream, fSizeDiagram);
        }
        WriteFloat4(fileBinaryStream, pSpriteParticle->Get_Data().fGravityDiagram);
        WriteFloat4(fileBinaryStream, pSpriteParticle->Get_Data().fPosition);
        WriteFloat4(fileBinaryStream, pSpriteParticle->Get_Data().fColor);
        WriteFloat3(fileBinaryStream, pSpriteParticle->Get_Data().fCenter);
        WriteFloat3(fileBinaryStream, pSpriteParticle->Get_Data().fPivot);
        WriteFloat3(fileBinaryStream, pSpriteParticle->Get_Data().fRange);
        WriteFloat3(fileBinaryStream, pSpriteParticle->Get_Data().fRotation);
        WriteFloat2(fileBinaryStream, pSpriteParticle->Get_Data().fSize);
        WriteFloat2(fileBinaryStream, pSpriteParticle->Get_Data().fLifeTime);
        WriteFloat2(fileBinaryStream, pSpriteParticle->Get_Data().fSpeed);
        WriteFloat2(fileBinaryStream, pSpriteParticle->Get_Data().fTurnPower);

        WriteFloat2(fileBinaryStream, pSpriteParticle->Get_Data().fMaskUV);
        WriteFloat2(fileBinaryStream, pSpriteParticle->Get_Data().fMaskUVSpeed);
        WriteFloat2(fileBinaryStream, pSpriteParticle->Get_Data().fMaskUVSize);
        WriteFloat2(fileBinaryStream, pSpriteParticle->Get_Data().fDiffuseUV);
        WriteFloat2(fileBinaryStream, pSpriteParticle->Get_Data().fDiffuseUVSpeed);
        WriteFloat2(fileBinaryStream, pSpriteParticle->Get_Data().fDiffuseUVSize);
        WriteFloat2(fileBinaryStream, pSpriteParticle->Get_Data().fDissolveUV);
        WriteFloat2(fileBinaryStream, pSpriteParticle->Get_Data().fDissolveUVSpeed);
        WriteFloat2(fileBinaryStream, pSpriteParticle->Get_Data().fDissolveUVSize);
        WriteFloat2(fileBinaryStream, pSpriteParticle->Get_Data().fParticleSize);
        WriteFloat2(fileBinaryStream, pSpriteParticle->Get_Data().fCircle);
        WriteInt2(fileBinaryStream, pSpriteParticle->Get_Data().iUV);
        WriteFloat(fileBinaryStream, pSpriteParticle->Get_Data().fDelayTime);
        WriteFloat(fileBinaryStream, pSpriteParticle->Get_Data().fEndTime);
        WriteFloat(fileBinaryStream, pSpriteParticle->Get_Data().fSphereSize);
        WriteFloat(fileBinaryStream, pSpriteParticle->Get_Data().fAngle);
        WriteFloat(fileBinaryStream, pSpriteParticle->Get_Data().fCircleSpeed);
        WriteInt(fileBinaryStream, pSpriteParticle->Get_Data().iBegin);
        WriteInt(fileBinaryStream, pSpriteParticle->Get_Data().iNumInstance);
        WriteInt(fileBinaryStream, pSpriteParticle->Get_Data().iSelectRender);
        WriteBool(fileBinaryStream, pSpriteParticle->Get_Data().bisBillboard);
        WriteBool(fileBinaryStream, pSpriteParticle->Get_Data().bisAngleBillboard);
        WriteBool(fileBinaryStream, pSpriteParticle->Get_Data().bisStart);
        WriteBool(fileBinaryStream, pSpriteParticle->Get_Data().bisAnimation);
        WriteBool(fileBinaryStream, pSpriteParticle->Get_Data().bisLoop);
        WriteBool(fileBinaryStream, pSpriteParticle->Get_Data().bisSphere);
        WriteBool(fileBinaryStream, pSpriteParticle->Get_Data().bisCircle);
        WriteBool(fileBinaryStream, pSpriteParticle->Get_Data().bisSpectrum);
    }
    WriteInt(fileBinaryStream, m_pParticles.size());
    for (auto pParticle : m_pParticles) {
    
    
        char szImageFile[MAX_PATH] = {};
        strncpy_s(szImageFile, sizeof(szImageFile), pParticle->Get_Data().szModel.c_str(), _TRUNCATE);
        WriteString(fileBinaryStream, szImageFile);
        memset(szImageFile, 0, sizeof(szImageFile));

        strncpy_s(szImageFile, sizeof(szImageFile), pParticle->Get_Data().szMaskTexture.c_str(), _TRUNCATE);
        WriteString(fileBinaryStream, szImageFile);
        memset(szImageFile, 0, sizeof(szImageFile));
    
        strncpy_s(szImageFile, sizeof(szImageFile), pParticle->Get_Data().szDiffuseTexture.c_str(), _TRUNCATE);
        WriteString(fileBinaryStream, szImageFile);
        memset(szImageFile, 0, sizeof(szImageFile));
    
        strncpy_s(szImageFile, sizeof(szImageFile), pParticle->Get_Data().szDissolveTexture.c_str(), _TRUNCATE);
        WriteString(fileBinaryStream, szImageFile);
        memset(szImageFile, 0, sizeof(szImageFile));
    
        strncpy_s(szImageFile, sizeof(szImageFile), pParticle->Get_Data().szCS.c_str(), _TRUNCATE);
        WriteString(fileBinaryStream, szImageFile);
    
        WriteInt(fileBinaryStream, pParticle->Get_Data().fSizeDiagrams.size());
        for (auto fSizeDiagram : pParticle->Get_Data().fSizeDiagrams) {
            WriteFloat3(fileBinaryStream, fSizeDiagram);
        }
        WriteFloat4(fileBinaryStream, pParticle->Get_Data().fGravityDiagram);
        WriteFloat4(fileBinaryStream, pParticle->Get_Data().fPosition);
        WriteFloat4(fileBinaryStream, pParticle->Get_Data().fColor);
        WriteFloat3(fileBinaryStream, pParticle->Get_Data().fCenter);
        WriteFloat3(fileBinaryStream, pParticle->Get_Data().fPivot);
        WriteFloat3(fileBinaryStream, pParticle->Get_Data().fRange);
        WriteFloat3(fileBinaryStream, pParticle->Get_Data().fRotation);
        WriteFloat3(fileBinaryStream, pParticle->Get_Data().fMeshRotation);
        WriteFloat2(fileBinaryStream, pParticle->Get_Data().fSize);
        WriteFloat2(fileBinaryStream, pParticle->Get_Data().fLifeTime);
        WriteFloat2(fileBinaryStream, pParticle->Get_Data().fSpeed);
        WriteFloat2(fileBinaryStream, pParticle->Get_Data().fTurnPower);
        
        WriteFloat2(fileBinaryStream, pParticle->Get_Data().fMaskUV);
        WriteFloat2(fileBinaryStream, pParticle->Get_Data().fMaskUVSpeed);
        WriteFloat2(fileBinaryStream, pParticle->Get_Data().fMaskUVSize);
        WriteFloat2(fileBinaryStream, pParticle->Get_Data().fDiffuseUV);
        WriteFloat2(fileBinaryStream, pParticle->Get_Data().fDiffuseUVSpeed);
        WriteFloat2(fileBinaryStream, pParticle->Get_Data().fDiffuseUVSize);
        WriteFloat2(fileBinaryStream, pParticle->Get_Data().fDissolveUV);
        WriteFloat2(fileBinaryStream, pParticle->Get_Data().fDissolveUVSpeed);
        WriteFloat2(fileBinaryStream, pParticle->Get_Data().fDissolveUVSize);
        WriteFloat2(fileBinaryStream, pParticle->Get_Data().fCircle);
        WriteFloat(fileBinaryStream, pParticle->Get_Data().fDelayTime);
        WriteFloat(fileBinaryStream, pParticle->Get_Data().fEndTime);
        WriteFloat(fileBinaryStream, pParticle->Get_Data().fSphereSize);
        WriteFloat(fileBinaryStream, pParticle->Get_Data().fCircleSpeed);
        WriteInt(fileBinaryStream, pParticle->Get_Data().iBegin);
        WriteInt(fileBinaryStream, pParticle->Get_Data().iNumInstance);
        WriteInt(fileBinaryStream, pParticle->Get_Data().iSelectRender);
        WriteBool(fileBinaryStream, pParticle->Get_Data().bisLoop);
        WriteBool(fileBinaryStream, pParticle->Get_Data().bisSphere);
        WriteBool(fileBinaryStream, pParticle->Get_Data().bisCircle);
        WriteBool(fileBinaryStream, pParticle->Get_Data().bisSpectrum);
    }
    return S_OK;
}

HRESULT CParticle_Setting::Load_Binary(const _char* szFile)
{
    if (0 < m_pMeshs.size()) {
        _uint iMeshCount = m_pMeshs.size();
        for (_uint i = 0; i < iMeshCount; ++i) {
            Delete_MeshEffect();
        }
    }
    if (0 < m_pSpriteParticles.size()) {
        _uint iSpriteParticleCount = m_pSpriteParticles.size();
        for (_uint i = 0; i < iSpriteParticleCount; ++i) {
            Delete_SpriteParticle();
        }
    }
    if (0 < m_pParticles.size()) {
        _uint iParticleCount = m_pParticles.size();
        for (_uint i = 0; i < iParticleCount; ++i) {
            Delete_Particle();
        }
    }
    CMeshEffect::MeshEffectData		MeshDesc{};
    CParticle::ParticleData		    ParticleDesc{};
    CSpriteParticle::SpriteParticleData		    SpriteParticleDesc{};
    CSpriteEffect::SPRITE_DATA		    SpriteDesc{};
    char szBinModelFilePath[MAX_PATH] = "../Bin/Resources/Effect/";
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

        CMeshEffect* pMeshEffect = CMeshEffect::Create(m_pDevice, m_pContext);
        pMeshEffect->Initialize(nullptr);
        pMeshEffect->Set_Components(MeshDesc);
        pMeshEffect->Set_ParentMat(m_pTransform->Get_WorldMatrixPtr());

        m_iSelectMesh = m_pMeshs.size();
        m_pMeshs.push_back(pMeshEffect);
        m_tMeshData = m_pMeshs[m_iSelectMesh]->Get_Data();
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
       SpriteParticleDesc.bisAngleBillboard = ReadBool(fileBinaryStream);
       SpriteParticleDesc.bisStart = ReadBool(fileBinaryStream);
       SpriteParticleDesc.bisAnimation = ReadBool(fileBinaryStream);
       SpriteParticleDesc.bisLoop = ReadBool(fileBinaryStream);
       SpriteParticleDesc.bisSphere = ReadBool(fileBinaryStream);
       SpriteParticleDesc.bisCircle = ReadBool(fileBinaryStream);
       SpriteParticleDesc.bisSpectrum = ReadBool(fileBinaryStream);

        CSpriteParticle* pParticle = CSpriteParticle::Create(m_pDevice, m_pContext);
        pParticle->Initialize(nullptr);

        pParticle->Set_Components(SpriteParticleDesc);
        pParticle->Set_ParentMat(m_pTransform->Get_WorldMatrixPtr());
        m_iSelectSpriteParticle = m_pSpriteParticles.size();
        m_pSpriteParticles.push_back(pParticle);
        m_tSpriteParticleData = m_pSpriteParticles[m_iSelectSpriteParticle]->Get_Data();
    }
    _int iParticleCount = ReadInt(fileBinaryStream);
    for (_uint i = 0; i < iParticleCount; ++i) {
        _char* szTemp = ReadString(fileBinaryStream);
        ParticleDesc.szModel = szTemp;
        Safe_Delete(szTemp);
        szTemp = ReadString(fileBinaryStream);
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
        ParticleDesc.fMeshRotation = ReadFloat3(fileBinaryStream);
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

        ParticleDesc.bisLoop = ReadBool(fileBinaryStream);
        ParticleDesc.bisSphere = ReadBool(fileBinaryStream);
        ParticleDesc.bisCircle = ReadBool(fileBinaryStream);
        ParticleDesc.bisSpectrum = ReadBool(fileBinaryStream);
        CParticle* pParticle = CParticle::Create(m_pDevice, m_pContext);
        pParticle->Initialize(nullptr);

        pParticle->Set_Components(ParticleDesc);
        pParticle->Set_ParentMat(m_pTransform->Get_WorldMatrixPtr());
        m_iSelectParticle = m_pParticles.size();
        m_pParticles.push_back(pParticle);
        m_tParticleData = m_pParticles[m_iSelectParticle]->Get_Data();
    }
    return S_OK;
}

HRESULT CParticle_Setting::Save_TrailBinary(const _char* szFile)
{
    char szBinModelFilePath[MAX_PATH] = "../Bin/Resources/TrailEffect/";
    strcat_s(szBinModelFilePath, MAX_PATH, szFile);
    strcat_s(szBinModelFilePath, MAX_PATH, ".binx");
    ofstream fileBinaryStream;
    fileBinaryStream.open(szBinModelFilePath, ios_base::binary);

    WriteInt(fileBinaryStream, m_pTrailDatas.size());
    for (auto pTrailData : m_pTrailDatas) {
        char szImageFile[MAX_PATH] = {};
        strncpy_s(szImageFile, sizeof(szImageFile), pTrailData->Get_Data().szMaskTexture.c_str(), _TRUNCATE);
        WriteString(fileBinaryStream, szImageFile);
        memset(szImageFile, 0, sizeof(szImageFile));

        strncpy_s(szImageFile, sizeof(szImageFile), pTrailData->Get_Data().szDiffuseTexture.c_str(), _TRUNCATE);
        WriteString(fileBinaryStream, szImageFile);
        memset(szImageFile, 0, sizeof(szImageFile));

        strncpy_s(szImageFile, sizeof(szImageFile), pTrailData->Get_Data().szDissolveTexture.c_str(), _TRUNCATE);
        WriteString(fileBinaryStream, szImageFile);
        memset(szImageFile, 0, sizeof(szImageFile));

        WriteFloat4(fileBinaryStream, pTrailData->Get_Data().fColor);
        WriteFloat2(fileBinaryStream, pTrailData->Get_Data().fMaskUV);
        WriteFloat2(fileBinaryStream, pTrailData->Get_Data().fMaskUVSpeed);
        WriteFloat2(fileBinaryStream, pTrailData->Get_Data().fMaskUVSize);
        WriteFloat2(fileBinaryStream, pTrailData->Get_Data().fDiffuseUV);
        WriteFloat2(fileBinaryStream, pTrailData->Get_Data().fDiffuseUVSpeed);
        WriteFloat2(fileBinaryStream, pTrailData->Get_Data().fDiffuseUVSize);
        WriteFloat2(fileBinaryStream, pTrailData->Get_Data().fDissolveUV);
        WriteFloat2(fileBinaryStream, pTrailData->Get_Data().fDissolveUVSpeed);
        WriteFloat2(fileBinaryStream, pTrailData->Get_Data().fDissolveUVSize);

        WriteInt(fileBinaryStream, pTrailData->Get_Data().iBegin);
        WriteInt(fileBinaryStream, pTrailData->Get_Data().iSelectRender);
    }
    return S_OK;
}

HRESULT CParticle_Setting::Load_TrailBinary(const _char* szFile)
{
    if (0 < m_pTrailDatas.size()) {
        _uint iTrailCount = m_pTrailDatas.size();
        for (_uint i = 0; i < iTrailCount; ++i) {
            Delete_TrailEffectData();
        }
    }
    char szBinModelFilePath[MAX_PATH] = "../Bin/Resources/TrailEffect/";
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

        CTrailData* pData = CTrailData::Create(m_pDevice, m_pContext);
        pData->Initialize(m_pTrailEffect);
        pData->Set_Components(TrailDesc);
        m_pTrailDatas.push_back(pData);
        m_iSelectTrailData = m_pTrailDatas.size() - 1;
        m_tTrailData = m_pTrailDatas[m_iSelectTrailData]->Get_Data();
    }
    return S_OK;
}

void CParticle_Setting::Update(_float fTimeDelta)
{
    ImGui::SetNextWindowSizeConstraints(
        ImVec2(100, 100),
        ImVec2(400, 600)
    );


    ImVec2 btn = { 120, ImGui::GetFrameHeight() };
    char pattern[MAX_PATH] = {};
    snprintf(pattern, sizeof(pattern), "Time : %.3f", m_fTime);
    if(m_bisPause)
        m_fTime += fTimeDelta;
    ImGui::Begin("Tools", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::Text(pattern);
    if (ImGui::Button("Pause", btn)) {
        m_bisPause = !m_bisPause;
    }
    ImGui::SameLine();
    ImGui::DragFloat("Speed", &m_fSpeed, 0.1f, 0.1f, 100.f);
    if (ImGui::BeginCombo("Effect Type", m_iSelectMeshParticle == 0 ? "SpriteParticle" : m_iSelectMeshParticle == 1 ? "MeshParticle" : m_iSelectMeshParticle == 2 ? "MeshEffect" : "TrailEffect"))
    {
        for (_uint i = 0; i < 4; ++i) {
            _bool sel = i == m_iSelectMeshParticle;
            if (ImGui::Selectable(i == 0 ? "SpriteParticle" : i == 1 ? "MeshParticle" : i == 2 ? "MeshEffect" : "TrailEffect", sel)) {
                m_iSelectMeshParticle = i;
            }
            if (sel)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }
    if (3 == m_iSelectMeshParticle) {

        m_pTransform->Set_State(STATE::RIGHT, m_pTrailEffect->GetTransform()->Get_State(STATE::RIGHT));
        m_pTransform->Set_State(STATE::UP, m_pTrailEffect->GetTransform()->Get_State(STATE::UP));
        m_pTransform->Set_State(STATE::LOOK, m_pTrailEffect->GetTransform()->Get_State(STATE::LOOK));
        m_pTransform->Set_State(STATE::POSITION, m_pTrailEffect->GetTransform()->Get_State(STATE::POSITION));
        ImGui::InputText("File", m_SaveFile, IM_ARRAYSIZE(m_SaveFile));
        if (ImGui::Button("Save", btn)) {
            Save_TrailBinary(m_SaveFile);
        }
        if (ImGui::Button("Refresh", btn)) {
            m_TrailEffects.clear();
            char pattern[MAX_PATH] = {};
            strcpy_s(pattern, MAX_PATH, "../Bin/Resources/TrailEffect/*.binx");

            WIN32_FIND_DATAA fd{};
            HANDLE h = FindFirstFileA(pattern, &fd);
            if (h != INVALID_HANDLE_VALUE) {
                do {
                    if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
                        m_TrailEffects.push_back(fd.cFileName);
                    }
                } while (FindNextFileA(h, &fd));
                FindClose(h);
            }
        }


        if (ImGui::BeginCombo("Save Effect", m_TrailEffects[m_iSelectTrailEffect].c_str()))
        {
            for (_uint i = 0; i < m_TrailEffects.size(); ++i) {
                _bool sel = i == m_iSelectTrailEffect;
                if (ImGui::Selectable(m_TrailEffects[i].c_str(), sel)) {
                    m_iSelectTrailEffect = i;
                    _tchar szPath[256] = { 0, };
                    MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, m_TrailEffects[m_iSelectTrailEffect].c_str(), strlen(m_TrailEffects[m_iSelectTrailEffect].c_str()), szPath, 256);

                }
                if (sel)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }

        if (ImGui::Button("Load", btn)) {
            
            Load_TrailBinary(m_TrailEffects[m_iSelectTrailEffect].c_str());
            m_pTrailEffect->Refresh();
        }
    }
    else {

        if (ImGui::TreeNode("Root Transform"))
        {
            ImGui::DragFloat3("Position", reinterpret_cast<_float*>(&m_fPosition), 0.1f, -1000.f, 1000.f);
            ImGui::DragFloat3("Scale", reinterpret_cast<_float*>(&m_fScale), 1.f, 0.f, 100.f);
            ImGui::DragFloat3("Rotation", reinterpret_cast<_float*>(&m_fRotation), 1.f, 0.f, 360.f);
            m_pTransform->Set_State(STATE::POSITION, XMLoadFloat4(&m_fPosition));
            m_pTransform->Set_Scale(m_fScale.x, m_fScale.y, m_fScale.z);
            m_pTransform->Rotation(XMConvertToRadians(m_fRotation.x), XMConvertToRadians(m_fRotation.y), XMConvertToRadians(m_fRotation.z));
            ImGui::TreePop();
        }

        ImGui::InputText("File", m_SaveFile, IM_ARRAYSIZE(m_SaveFile));
        if (ImGui::Button("Save", btn)) {
            Save_Binary(m_SaveFile);
        }
        if (ImGui::Button("Refresh", btn)) {
            m_Effects.clear();
            char pattern[MAX_PATH] = {};
            strcpy_s(pattern, MAX_PATH, "../Bin/Resources/Effect/*.binx");

            WIN32_FIND_DATAA fd{};
            HANDLE h = FindFirstFileA(pattern, &fd);
            if (h != INVALID_HANDLE_VALUE) {
                do {
                    if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
                        m_Effects.push_back(fd.cFileName);
                    }
                } while (FindNextFileA(h, &fd));
                FindClose(h);
            }
        }


        if (ImGui::BeginCombo("Save Effect", m_Effects[m_iSelectEffect].c_str()))
        {
            for (_uint i = 0; i < m_Effects.size(); ++i) {
                _bool sel = i == m_iSelectEffect;
                if (ImGui::Selectable(m_Effects[i].c_str(), sel)) {
                    m_iSelectEffect = i;
                    _tchar szPath[256] = { 0, };
                    MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, m_Effects[m_iSelectEffect].c_str(), strlen(m_Effects[m_iSelectEffect].c_str()), szPath, 256);

                }
                if (sel)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }

        if (ImGui::Button("Load", btn)) {
            Load_Binary(m_Effects[m_iSelectEffect].c_str());
        }

        if (ImGui::Button("ReSave", btn)) {
            for (_uint i = 0; i < m_Effects.size(); ++i) {
                Load_Binary(m_Effects[i].c_str());
                Save_Binary(m_Effects[i].c_str());
            }
        }
    }
    
    switch (m_iSelectMeshParticle)
    {
    case 0:
        if (ImGui::Button("Add SpriteParticle", btn)) {
            Add_SpriteParticle();
        }
        if (0 < m_pSpriteParticles.size())
        {
            ImGui::SameLine();
            if (ImGui::Button("Delete SpriteParticle", btn)) {
                Delete_SpriteParticle();
                if (0 >= m_pSpriteParticles.size()) {
                    ImGui::End();
                    return;
                }
            }
            if (ImGui::Button("Effect", btn)) {
                m_iLevel = 0;
            }
            ImGui::SameLine();
            if (ImGui::Button("Shader", btn)) {
                m_iLevel = 1;
            }
            char str[3];
            snprintf(str, sizeof(str), "%d", m_iSelectSpriteParticle);
            if (ImGui::BeginCombo("SpriteParticles", str))
            {
                for (_uint i = 0; i < m_pSpriteParticles.size(); ++i) {
                    _bool sel = i == m_iSelectSpriteParticle;
                    snprintf(str, sizeof(str), "%d", i);
                    if (ImGui::Selectable(str, sel)) {
                        m_iSelectSpriteParticle = i;
                        m_tSpriteParticleData = m_pSpriteParticles[m_iSelectSpriteParticle]->Get_Data();
                    }
                    if (sel)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }
            if (ImGui::Button("All Replay", btn)) {
                m_bisReplay = true;
                m_fTime = 0.f;
                for (_uint i = 0; i < m_pParticles.size(); ++i) {
                    m_pParticles[i]->Set_Components(m_pParticles[i]->Get_Data());
                }
                for (_uint i = 0; i < m_pSpriteParticles.size(); ++i) {
                    m_pSpriteParticles[i]->Set_Components(m_pSpriteParticles[i]->Get_Data());
                }
                for (_uint i = 0; i < m_pMeshs.size(); ++i) {
                    m_pMeshs[i]->Set_Components(m_pMeshs[i]->Get_Data());
                }
                ImGui::End();
                return;
            }

            ImGui::SameLine();
            if (ImGui::Button("Replay", btn)) {
                m_bisReplay = true;
                m_fTime = 0.f;
                m_pSpriteParticles[m_iSelectSpriteParticle]->Set_Components(m_pSpriteParticles[m_iSelectSpriteParticle]->Get_Data());
                ImGui::End();
                return;
            }
            if (ImGui::Button("Stop", btn)) {
                m_pSpriteParticles[m_iSelectParticle]->Stop();
            }
            if (ImGui::Button("Play", btn)) {
                m_pSpriteParticles[m_iSelectParticle]->Play();
            }

            m_pSpriteParticles[m_iSelectSpriteParticle]->Update(m_tSpriteParticleData);


            ImGui::BeginChild("Menu", ImVec2(300, 500), true);
            switch (m_iLevel)
            {
            case 0:
            {
                _float4 fGravity = m_tSpriteParticleData.fGravityDiagram;
                vector<_float3> fSizeDiagrams = m_tSpriteParticleData.fSizeDiagrams;
                _float fDelayTime = m_tSpriteParticleData.fDelayTime;
                _float fEndTime = m_tSpriteParticleData.fEndTime;

                _float time = 1.f / 100;
                _float fMax = 5;
                float values[100] = {};

                if (ImGui::TreeNode("SpriteParticle"))
                {
                    if (ImGui::TreeNode("SpriteParticle Transform"))
                    {
                        ImGui::DragFloat3("SpriteParticle Position", reinterpret_cast<_float*>(&m_tSpriteParticleData.fPosition), 0.1f, -1000.f, 1000.f);
                        ImGui::DragFloat3("SpriteParticle Rotation", reinterpret_cast<_float*>(&m_tSpriteParticleData.fRotation), 1.f, 0.f, 360.f);
                        ImGui::TreePop();
                    }
                    if (ImGui::TreeNode("SpriteParticle Data"))
                    {
                        ImGui::DragInt("NumInstance", &m_tSpriteParticleData.iNumInstance, 1, 1, 0);
                        ImGui::DragFloat3("Center", reinterpret_cast<_float*>(&m_tSpriteParticleData.fCenter), 0.1f, -100.f, 100.f);
                        ImGui::DragFloat3("Pivot", reinterpret_cast<_float*>(&m_tSpriteParticleData.fPivot), 0.1f, -100.f, 100.f);
                        ImGui::DragFloat3("Range", reinterpret_cast<_float*>(&m_tSpriteParticleData.fRange), 0.1f, -100.f, 100.f);
                        ImGui::DragFloat2("Size", reinterpret_cast<_float*>(&m_tSpriteParticleData.fSize), 0.1f, 0.f, 100.f);
                        ImGui::DragFloat2("LifeTime", reinterpret_cast<_float*>(&m_tSpriteParticleData.fLifeTime), 0.1f, 0.f, 100.f);
                        ImGui::DragFloat2("Speed", reinterpret_cast<_float*>(&m_tSpriteParticleData.fSpeed), 0.1f, 0.f, 100.f);
                        ImGui::DragFloat("Delay Time", reinterpret_cast<_float*>(&m_tSpriteParticleData.fDelayTime), 0.1f, 0.f, 100.f);
                        ImGui::DragFloat("End Time", reinterpret_cast<_float*>(&m_tSpriteParticleData.fEndTime), 0.1f, 0.f, 100.f);
                        ImGui::Checkbox("Loop", &m_tSpriteParticleData.bisLoop);


                        ImGui::TreePop();
                    }

                    ImGui::TreePop();
                }

                ImGui::SetNextItemWidth(180);

                for (_uint i = 0; i < 100; ++i) {
                    _float t = time * i;
                    if (fabsf(m_tSpriteParticleData.fGravityDiagram.y) >= 90.f || fabsf(m_tSpriteParticleData.fGravityDiagram.w) >= 90.f) {
                        values[i] = m_tSpriteParticleData.fGravityDiagram.x;
                    }
                    else {

                        values[i] = (2 * powf(t, 3) - 3 * powf(t, 2) + 1) * m_tSpriteParticleData.fGravityDiagram.x
                            + (powf(t, 3) - 2 * powf(t, 2) + t) * tanf(XMConvertToRadians(m_tSpriteParticleData.fGravityDiagram.y)) * 100
                            + (-2 * powf(t, 3) + 3 * powf(t, 2)) * m_tSpriteParticleData.fGravityDiagram.z
                            + (powf(t, 3) - powf(t, 2)) * tanf(XMConvertToRadians(m_tSpriteParticleData.fGravityDiagram.w)) * 100;
                    }
                    fMax = max(fMax, fabsf(values[i]));
                }


                if (ImGui::TreeNode("Size Diagram")) {
                    m_iSelectSize = min(m_iSelectSize, (_uint)m_tSpriteParticleData.fSizeDiagrams.size() - 1);
                    if (m_iSelectSize < m_tSpriteParticleData.fSizeDiagrams.size()) {
                        _float time = 1.f / 100;
                        _float fMax = 5;
                        for (_uint i = 0; i < 100; ++i) {
                            _float3* in = {};
                            _float3* out = {};
                            for (_uint j = 0; j < (_uint)m_tSpriteParticleData.fSizeDiagrams.size(); ++j) {
                                if (m_tSpriteParticleData.fSizeDiagrams[j].x <= time * i) {
                                    in = &m_tSpriteParticleData.fSizeDiagrams[j];
                                }
                                if (m_tSpriteParticleData.fSizeDiagrams[j].x > time * i) {
                                    out = &m_tSpriteParticleData.fSizeDiagrams[j];
                                    break;
                                }
                            }
                            if (nullptr == out)
                                values[i] = in->y;
                            else {
                                _float t = (time * i - in->x) / (out->x - in->x);
                                if (fabsf(in->z) >= 90.f || fabsf(out->z) >= 90.f) {
                                    values[i] = in->y;
                                }
                                else {
                                    values[i] = (2 * powf(t, 3) - 3 * powf(t, 2) + 1) * in->y
                                        + (powf(t, 3) - 2 * powf(t, 2) + t) * (tanf(XMConvertToRadians(in->z)) * (out->x - in->x) * 100)
                                        + (-2 * powf(t, 3) + 3 * powf(t, 2)) * out->y
                                        + (powf(t, 3) - powf(t, 2)) * (tanf(XMConvertToRadians(out->z)) * (out->x - in->x) * 100);
                                }
                            }
                            fMax = max(fMax, fabsf(values[i]));
                        }
                        ImGui::PlotLines("Size Wave", values, IM_ARRAYSIZE(values), 0,
                            "Size data", -fMax, fMax, ImVec2(0, 100));


                        char str[10];
                        snprintf(str, sizeof(str), "%d. %.2f", m_iSelectSize + 1, m_tSpriteParticleData.fSizeDiagrams[m_iSelectSize].x);

                        if (ImGui::BeginCombo("TimeValue", str))
                        {
                            for (_uint i = 0; i < (_uint)m_tSpriteParticleData.fSizeDiagrams.size(); ++i) {
                                _bool sel = i == m_iSelectSize;
                                snprintf(str, sizeof(str), "%d. %.2f", i + 1, m_tSpriteParticleData.fSizeDiagrams[i].x);
                                if (ImGui::Selectable(str, sel))
                                    m_iSelectSize = i;
                                if (sel)
                                    ImGui::SetItemDefaultFocus();
                            }
                            ImGui::EndCombo();
                        }
                    }
                    if (ImGui::Button("AddTime", btn)) {
                        if (0 < m_tSpriteParticleData.fSizeDiagrams.size()) {
                            _float3 val = m_tSpriteParticleData.fSizeDiagrams.back();
                            val.y = 0;
                            m_tSpriteParticleData.fSizeDiagrams.push_back(val);
                        }
                        else {
                            _float3 val = { 0,0,0 };
                            m_tSpriteParticleData.fSizeDiagrams.push_back(val);
                        }
                        m_iSelectSize = m_tSpriteParticleData.fSizeDiagrams.size() - 1;
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("DeleteTime", btn)) {
                        _uint iCount = 0;
                        for (auto i = m_tSpriteParticleData.fSizeDiagrams.begin(); i != m_tSpriteParticleData.fSizeDiagrams.end();) {
                            if (iCount == m_iSelectSize) {
                                m_tSpriteParticleData.fSizeDiagrams.erase(i);
                                break;
                            }
                            ++i;
                            ++iCount;
                        }
                        m_iSelectSize = max(m_iSelectSize - 1, 0);
                    }

                    if (m_iSelectSize < m_tSpriteParticleData.fSizeDiagrams.size()) {
                        if (0 < m_iSelectSize)
                            ImGui::DragFloat("Size Time", &m_tSpriteParticleData.fSizeDiagrams[m_iSelectSize].x, 0.01f, m_tSpriteParticleData.fSizeDiagrams[m_iSelectSize - 1].x, m_iSelectSize != m_tSpriteParticleData.fSizeDiagrams.size() - 1 ? m_tSpriteParticleData.fSizeDiagrams[m_iSelectSize + 1].x : 1);
                        ImGui::DragFloat("Size fx", &m_tSpriteParticleData.fSizeDiagrams[m_iSelectSize].z, 1.f, -90.f, 90.f);
                        ImGui::DragFloat("Size", &m_tSpriteParticleData.fSizeDiagrams[m_iSelectSize].y, 0.01f, -100.f, 100.f);
                    }
                    ImGui::TreePop();
                }

                if (ImGui::TreeNode("Gravity Diagram")) {
                    ImGui::PlotLines("Gravity Wave", values, IM_ARRAYSIZE(values), 0,
                        "Gravity Data", -fMax, fMax, ImVec2(0, 100));

                    ImGui::DragFloat("Start Gravity", &m_tSpriteParticleData.fGravityDiagram.x, 0.01f, -100.f, 100.f);
                    ImGui::DragFloat("Start Gravity fx", &m_tSpriteParticleData.fGravityDiagram.y, 1.f, -90.f, 90.f);
                    ImGui::DragFloat("End Gravity", &m_tSpriteParticleData.fGravityDiagram.z, 0.01f, -100.f, 100.f);
                    ImGui::DragFloat("End Gravity fx", &m_tSpriteParticleData.fGravityDiagram.w, 1.f, -90.f, 90.f);
                    ImGui::TreePop();
                }

                //if (Desc.iNumInstance != m_tSpriteParticleData.iNumInstance ||
                //    Desc.vCenter.x != m_tSpriteParticleData.fCenter.x ||
                //    Desc.vCenter.y != m_tSpriteParticleData.fCenter.y ||
                //    Desc.vCenter.z != m_tSpriteParticleData.fCenter.z ||
                //    Desc.vPivot.x != m_tSpriteParticleData.fPivot.x ||
                //    Desc.vPivot.y != m_tSpriteParticleData.fPivot.y ||
                //    Desc.vPivot.z != m_tSpriteParticleData.fPivot.z ||
                //    Desc.vRange.x != m_tSpriteParticleData.fRange.x ||
                //    Desc.vRange.y != m_tSpriteParticleData.fRange.y ||
                //    Desc.vRange.z != m_tSpriteParticleData.fRange.z ||
                //    Desc.vSize.x != m_tSpriteParticleData.fSize.x ||
                //    Desc.vSize.y != m_tSpriteParticleData.fSize.y ||
                //    Desc.vLifeTime.x != m_tSpriteParticleData.fLifeTime.x ||
                //    Desc.vLifeTime.y != m_tSpriteParticleData.fLifeTime.y ||
                //    Desc.vSpeed.x != m_tSpriteParticleData.fSpeed.x ||
                //    Desc.vSpeed.y != m_tSpriteParticleData.fSpeed.y ||
                //    Desc.isLoop != m_tSpriteParticleData.bisLoop ||
                //    fGravity.x != m_tSpriteParticleData.fGravityDiagram.x ||
                //    fGravity.y != m_tSpriteParticleData.fGravityDiagram.y ||
                //    fGravity.z != m_tSpriteParticleData.fGravityDiagram.z ||
                //    fGravity.w != m_tSpriteParticleData.fGravityDiagram.w ||
                //    fDelayTime != m_tSpriteParticleData.fDelayTime ||
                //    fSizeDiagrams.size() != m_tSpriteParticleData.fSizeDiagrams.size()) {
                //    Desc.iNumInstance = m_tSpriteParticleData.iNumInstance;
                //    Desc.vCenter = m_tSpriteParticleData.fCenter;
                //    Desc.vPivot = m_tSpriteParticleData.fPivot;
                //    Desc.vRange = m_tSpriteParticleData.fRange;
                //    Desc.vSize = m_tSpriteParticleData.fSize;
                //    Desc.vLifeTime = m_tSpriteParticleData.fLifeTime;
                //    Desc.vSpeed = m_tSpriteParticleData.fSpeed;
                //    Desc.isLoop = m_tSpriteParticleData.bisLoop;
                //    m_pSpriteParticles[m_iSelectSpriteParticle]->Set_Components(m_tSpriteParticleData);
                //}
                //else if (fSizeDiagrams.size() == m_tSpriteParticleData.fSizeDiagrams.size()) {
                //    for (_uint i = 0; i < fSizeDiagrams.size(); ++i) {
                //        if (fSizeDiagrams[i].x != m_tSpriteParticleData.fSizeDiagrams[i].x ||
                //            fSizeDiagrams[i].y != m_tSpriteParticleData.fSizeDiagrams[i].y ||
                //            fSizeDiagrams[i].z != m_tSpriteParticleData.fSizeDiagrams[i].z) {
                //            m_pSpriteParticles[m_iSelectSpriteParticle]->Set_Components(m_tSpriteParticleData);
                //        }
                //    }
                //}
            }
            break;
            case 1:
            {

                if (ImGui::TreeNode("Color")) {
                    ImGui::ColorPicker4("Color", (_float*)&m_tSpriteParticleData.fColor, ImGuiColorEditFlags_PickerHueWheel);
                    ImGui::TreePop();
                }

                if (ImGui::TreeNode("Shader")) {
                    ImGui::TreePop();
                }
                string szRender;
                switch (m_tSpriteParticleData.iSelectRender) {
                case 0:
                    szRender = "NONBLEND";
                    break;
                case 1:
                    szRender = "NONLIGHT";
                    break;
                case 2:
                    szRender = "BLEND";
                    break;
                case 3:
                    szRender = "BLUR";
                    break;
                case 4:
                    szRender = "GLOW";
                    break;
                case 5:
                    szRender = "METABALL";
                    break;
                case 6:
                    szRender = "DISTORTION";
                    break;
                }
                if (ImGui::BeginCombo("RenderType", szRender.c_str()))
                {
                    for (_uint i = 0; i <= 6; ++i) {
                        _bool sel = i == ENUM_CLASS(m_tSpriteParticleData.iSelectRender);
                        switch (i)
                        {
                        case 0:
                            szRender = "NONBLEND";
                            break;
                        case 1:
                            szRender = "NONLIGHT";
                            break;
                        case 2:
                            szRender = "BLEND";
                            break;
                        case 3:
                            szRender = "BLUR";
                            break;
                        case 4:
                            szRender = "GLOW";
                            break;
                        case 5:
                            szRender = "METABALL";
                            break;
                        case 6:
                            szRender = "DISTORTION";
                            break;
                        }
                        if (ImGui::Selectable(szRender.c_str(), sel))
                            m_tSpriteParticleData.iSelectRender = i;
                        if (sel)
                            ImGui::SetItemDefaultFocus();
                    }
                    ImGui::EndCombo();
                }
                char szName[30] = {};
                strncpy_s(szName, sizeof(szName), m_szCS.c_str(), _TRUNCATE);
                if (ImGui::InputText("File", szName, sizeof(szName))) {
                    m_szCS = szName;
                }
                if (ImGui::Button("Refresh Shader", btn)) {
                    m_tSpriteParticleData.szCS = m_szCS;
                    m_pSpriteParticles[m_iSelectSpriteParticle]->Set_Components(m_tSpriteParticleData);
                }
                ImGui::InputInt("Shader Begine", &m_tSpriteParticleData.iBegin);
                if (ImGui::BeginCombo("ImageType", m_iSpriteImageType == 0 ? "Mask" : m_iSpriteImageType == 1 ? "Diffuse" : "Normal"))
                {
                    if (ImGui::Selectable("Mask", 0 == m_iSpriteImageType))
                        m_iSpriteImageType = 0;
                    if (0 == m_iSpriteImageType)
                        ImGui::SetItemDefaultFocus();
                    if (ImGui::Selectable("Diffuse", 1 == m_iSpriteImageType))
                        m_iSpriteImageType = 1;
                    if (1 == m_iSpriteImageType)
                        ImGui::SetItemDefaultFocus();
                    if (ImGui::Selectable("Normal", 3 == m_iSpriteImageType))
                        m_iSpriteImageType = 3;
                    if (3 == m_iSpriteImageType)
                        ImGui::SetItemDefaultFocus();
                    ImGui::EndCombo();
                }
                ImGui::DragFloat2("Sprite Size", reinterpret_cast<_float*>(&m_tSpriteParticleData.fParticleSize), 0.1f, 0.f, 100.f);
                ImGui::Checkbox("Billboard", &m_tSpriteParticleData.bisBillboard);
                ImGui::Checkbox("AngleBillboard", &m_tSpriteParticleData.bisAngleBillboard);
                ImGui::Checkbox("CoreStart", &m_tSpriteParticleData.bisStart);
                ImGui::Checkbox("Spectrum", &m_tSpriteParticleData.bisSpectrum);
                ImGui::Checkbox("Animation", &m_tSpriteParticleData.bisAnimation);
                if (!m_tSpriteParticleData.bisCircle)
                    ImGui::Checkbox("Sphere Pointer", &m_tSpriteParticleData.bisSphere);
                if (!m_tSpriteParticleData.bisSphere)
                    ImGui::Checkbox("Circle Pointer", &m_tSpriteParticleData.bisCircle);
                if (m_tSpriteParticleData.bisSphere) {
                    ImGui::DragFloat("SpereSize", reinterpret_cast<_float*>(&m_tSpriteParticleData.fSphereSize), 0.1f, 0.f, 100.f);
                }
                if (m_tSpriteParticleData.bisCircle) {
                    ImGui::DragFloat("CircleSize", reinterpret_cast<_float*>(&m_tSpriteParticleData.fSphereSize), 0.1f, 0.f, 100.f);
                    ImGui::DragFloat("CircleSpeed", reinterpret_cast<_float*>(&m_tSpriteParticleData.fCircleSpeed), 0.1f, 0.f, 100.f);
                    ImGui::DragFloat2("Circle", reinterpret_cast<_float*>(&m_tSpriteParticleData.fCircle), 0.1f, 0.f, 100.f);
                }
                if (ImGui::TreeNode("TurnPower"))
                {
                    ImGui::DragFloat2("TurnPower", reinterpret_cast<_float*>(&m_tSpriteParticleData.fTurnPower), 0.1f, -100.f, 100.f);
                    ImGui::TreePop();
                }
                if (ImGui::TreeNode("MaskUV"))
                {
                    ImGui::DragFloat2("MaskUV", reinterpret_cast<_float*>(&m_tSpriteParticleData.fMaskUV), 0.1f, -100.f, 100.f);
                    ImGui::DragFloat2("MaskUV Speed", reinterpret_cast<_float*>(&m_tSpriteParticleData.fMaskUVSpeed), 0.1f, -100.f, 100.f);
                    ImGui::DragFloat2("MaskUV Size", reinterpret_cast<_float*>(&m_tSpriteParticleData.fMaskUVSize), 0.1f, -100.f, 100.f);
                    ImGui::TreePop();
                }
                if (ImGui::TreeNode("DiffuseUV"))
                {
                    ImGui::DragFloat2("DiffuseUV", reinterpret_cast<_float*>(&m_tSpriteParticleData.fDiffuseUV), 0.1f, -100.f, 100.f);
                    ImGui::DragFloat2("DiffuseUV Speed", reinterpret_cast<_float*>(&m_tSpriteParticleData.fDiffuseUVSpeed), 0.1f, -100.f, 100.f);
                    ImGui::DragFloat2("DiffuseUV Size", reinterpret_cast<_float*>(&m_tSpriteParticleData.fDiffuseUVSize), 0.1f, -100.f, 100.f);
                    ImGui::TreePop();
                }
                if (ImGui::TreeNode("DissolveUV"))
                {
                    ImGui::DragFloat2("DissolveUV", reinterpret_cast<_float*>(&m_tSpriteParticleData.fDissolveUV), 0.1f, -100.f, 100.f);
                    ImGui::DragFloat2("DissolveUV Speed", reinterpret_cast<_float*>(&m_tSpriteParticleData.fDissolveUVSpeed), 0.1f, -100.f, 100.f);
                    ImGui::DragFloat2("DissolveUV Size", reinterpret_cast<_float*>(&m_tSpriteParticleData.fDissolveUVSize), 0.1f, -100.f, 100.f);
                    ImGui::TreePop();
                }

                ImGui::DragFloat("Sprite Angle", reinterpret_cast<_float*>(&m_tSpriteParticleData.fAngle), 0.1f, 0.f, 360.f);
                ImGui::InputInt2("Sprite UV", reinterpret_cast<_int*>(&m_tSpriteParticleData.iUV));

                ImGui::Separator();

                ImGui::BeginChild("ImageScroll", ImVec2(300, 200), true);
                if (0 < m_SRVs[m_iSpriteImageType].size()) {
                    _uint i = 0;
                    for (auto SRV : m_SRVs[m_iSpriteImageType]) {
                        if (ImGui::ImageButton(m_ImageFiles[m_iSpriteImageType][i].c_str(), (ImTextureRef)SRV, ImVec2(100, 100))) {
                            m_pSpriteParticles[m_iSelectSpriteParticle]->Set_Texture(0 == m_iSpriteImageType ? 0 : 1 == m_iSpriteImageType ? 1 : 2, m_ImageFiles[m_iSpriteImageType][i].c_str());
                            switch (m_iSpriteImageType) {
                            case 0:
                                m_tSpriteParticleData.szMaskTexture = m_ImageFiles[m_iSpriteImageType][i];
                                break;
                            case 1:
                                m_tSpriteParticleData.szDiffuseTexture = m_ImageFiles[m_iSpriteImageType][i];
                                break;
                            case 3:
                                m_tSpriteParticleData.szNormalTexture = m_ImageFiles[m_iSpriteImageType][i];
                                break;
                            }
                        }
                        if (1 == ++i % 2)
                            ImGui::SameLine();
                    }
                }
                ImGui::EndChild();
            }
            break;
            }
            ImGui::EndChild();
        }
        break;
    case 1:
        if (ImGui::Button("Add Mesh Particle", btn)) {
            Add_Particle();
        }
        if (0 < m_pParticles.size())
        {
            ImGui::SameLine();
            if (ImGui::Button("Delete Mesh Particle", btn)) {
                Delete_Particle();
                if (0 >= m_pParticles.size()) {
                    ImGui::End();
                    return;
                }
            }
            if (ImGui::Button("Effect", btn)) {
                m_iLevel = 0;
            }
            ImGui::SameLine();
            if (ImGui::Button("Shader", btn)) {
                m_iLevel = 1;
            }
            char str[3];
            snprintf(str, sizeof(str), "%d", m_iSelectParticle);
            if (ImGui::BeginCombo("Mesh Particles", str))
            {
                for (_uint i = 0; i < m_pParticles.size(); ++i) {
                    _bool sel = i == m_iSelectParticle;
                    snprintf(str, sizeof(str), "%d", i);
                    if (ImGui::Selectable(str, sel)) {
                        m_iSelectParticle = i;
                        m_tParticleData = m_pParticles[m_iSelectParticle]->Get_Data();
                    }
                    if (sel)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }
            if (ImGui::Button("All Replay", btn)) {
                m_fTime = 0.f;
                m_bisReplay = true;
                for (_uint i = 0; i < m_pParticles.size(); ++i) {
                    m_pParticles[i]->Set_Components(m_pParticles[i]->Get_Data());
                }
                for (_uint i = 0; i < m_pSpriteParticles.size(); ++i) {
                    m_pSpriteParticles[i]->Set_Components(m_pSpriteParticles[i]->Get_Data());
                }
                for (_uint i = 0; i < m_pMeshs.size(); ++i) {
                    m_pMeshs[i]->Set_Components(m_pMeshs[i]->Get_Data());
                }
                ImGui::End();
                return;
            }

            ImGui::SameLine();
            if (ImGui::Button("Replay", btn)) {
                m_bisReplay = true;
                m_fTime = 0.f;
                m_pParticles[m_iSelectParticle]->Set_Components(m_pParticles[m_iSelectParticle]->Get_Data());
                ImGui::End();
                return;
            }
            if (ImGui::Button("Stop", btn)) {
                m_pParticles[m_iSelectParticle]->Stop();
            }
            if (ImGui::Button("Play", btn)) {
                m_pParticles[m_iSelectParticle]->Play();
            }

            m_pParticles[m_iSelectParticle]->Update(m_tParticleData);

            if (ImGui::BeginCombo("Models", m_ModelFilePaths[m_iSelectModel].c_str()))
            {
                for (_uint i = 0; i < m_ModelFilePaths.size(); ++i) {
                    _bool sel = i == m_iSelectModel;
                    if (ImGui::Selectable(m_ModelFilePaths[i].c_str(), sel)) {
                        m_iSelectModel = i;
                        _tchar szPath[256] = { 0, };
                        MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, m_ModelFilePaths[m_iSelectModel].c_str(), strlen(m_ModelFilePaths[m_iSelectModel].c_str()), szPath, 256);

                        char szModelPath[MAX_PATH] = {};
                        strncpy_s(szModelPath, sizeof(szModelPath), m_ModelFilePaths[m_iSelectModel].c_str(), _TRUNCATE);
                        m_tParticleData.szModel = szModelPath;
                    }
                    if (sel)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }

            ImGui::BeginChild("Menu", ImVec2(300, 500), true);
            switch (m_iLevel)
            {
            case 0:
            {
                _float4 fGravity = m_tParticleData.fGravityDiagram;
                vector<_float3> fSizeDiagrams = m_tParticleData.fSizeDiagrams;
                _float fDelayTime = m_tParticleData.fDelayTime;
                _float fEndTime = m_tParticleData.fEndTime;

                _float time = 1.f / 100;
                _float fMax = 5;
                float values[100] = {};

                if (ImGui::TreeNode("Particle"))
                {
                    if (ImGui::TreeNode("Particle Transform"))
                    {
                        ImGui::DragFloat3("Particle Position", reinterpret_cast<_float*>(&m_tParticleData.fPosition), 0.1f, -1000.f, 1000.f);
                        ImGui::DragFloat3("Particle Rotation", reinterpret_cast<_float*>(&m_tParticleData.fRotation), 1.f, 0.f, 360.f);
                        ImGui::TreePop();
                    }
                    if (ImGui::TreeNode("Particle Data"))
                    {
                        ImGui::DragInt("NumInstance", &m_tParticleData.iNumInstance, 1, 1, 0);
                        ImGui::DragFloat3("Center", reinterpret_cast<_float*>(&m_tParticleData.fCenter), 0.1f, -100.f, 100.f);
                        ImGui::DragFloat3("Pivot", reinterpret_cast<_float*>(&m_tParticleData.fPivot), 0.1f, -100.f, 100.f);
                        ImGui::DragFloat3("Range", reinterpret_cast<_float*>(&m_tParticleData.fRange), 0.1f, -100.f, 100.f);
                        ImGui::DragFloat2("Size", reinterpret_cast<_float*>(&m_tParticleData.fSize), 0.1f, 0.f, 100.f);
                        ImGui::DragFloat2("LifeTime", reinterpret_cast<_float*>(&m_tParticleData.fLifeTime), 0.1f, 0.f, 100.f);
                        ImGui::DragFloat2("Speed", reinterpret_cast<_float*>(&m_tParticleData.fSpeed), 0.1f, 0.f, 100.f);
                        ImGui::DragFloat("Delay Time", reinterpret_cast<_float*>(&m_tParticleData.fDelayTime), 0.1f, 0.f, 100.f);
                        ImGui::DragFloat("End Time", reinterpret_cast<_float*>(&m_tParticleData.fEndTime), 0.1f, 0.f, 100.f);
                        ImGui::Checkbox("Loop", &m_tParticleData.bisLoop);


                        ImGui::TreePop();
                    }

                    ImGui::TreePop();
                }

                ImGui::SetNextItemWidth(180);

                for (_uint i = 0; i < 100; ++i) {
                    _float t = time * i;
                    if (fabsf(m_tParticleData.fGravityDiagram.y) >= 90.f || fabsf(m_tParticleData.fGravityDiagram.w) >= 90.f) {
                        values[i] = m_tParticleData.fGravityDiagram.x;
                    }
                    else {

                        values[i] = (2 * powf(t, 3) - 3 * powf(t, 2) + 1) * m_tParticleData.fGravityDiagram.x
                            + (powf(t, 3) - 2 * powf(t, 2) + t) * tanf(XMConvertToRadians(m_tParticleData.fGravityDiagram.y)) * 100
                            + (-2 * powf(t, 3) + 3 * powf(t, 2)) * m_tParticleData.fGravityDiagram.z
                            + (powf(t, 3) - powf(t, 2)) * tanf(XMConvertToRadians(m_tParticleData.fGravityDiagram.w)) * 100;
                    }
                    fMax = max(fMax, fabsf(values[i]));
                }


                if (ImGui::TreeNode("Size Diagram")) {
                    m_iSelectSize = min(m_iSelectSize, (_uint)m_tMeshData.fSizeDiagrams.size() - 1);
                    if (m_iSelectSize < m_tParticleData.fSizeDiagrams.size()) {
                        _float time = 1.f / 100;
                        _float fMax = 5;
                        for (_uint i = 0; i < 100; ++i) {
                            _float3* in = {};
                            _float3* out = {};
                            for (_uint j = 0; j < (_uint)m_tParticleData.fSizeDiagrams.size(); ++j) {
                                if (m_tParticleData.fSizeDiagrams[j].x <= time * i) {
                                    in = &m_tParticleData.fSizeDiagrams[j];
                                }
                                if (m_tParticleData.fSizeDiagrams[j].x > time * i) {
                                    out = &m_tParticleData.fSizeDiagrams[j];
                                    break;
                                }
                            }
                            if (nullptr == out)
                                values[i] = in->y;
                            else {
                                _float t = (time * i - in->x) / (out->x - in->x);
                                if (fabsf(in->z) >= 90.f || fabsf(out->z) >= 90.f) {
                                    values[i] = in->y;
                                }
                                else {
                                    values[i] = (2 * powf(t, 3) - 3 * powf(t, 2) + 1) * in->y
                                        + (powf(t, 3) - 2 * powf(t, 2) + t) * (tanf(XMConvertToRadians(in->z)) * (out->x - in->x) * 100)
                                        + (-2 * powf(t, 3) + 3 * powf(t, 2)) * out->y
                                        + (powf(t, 3) - powf(t, 2)) * (tanf(XMConvertToRadians(out->z)) * (out->x - in->x) * 100);
                                }
                            }
                            fMax = max(fMax, fabsf(values[i]));
                        }
                        ImGui::PlotLines("Size Wave", values, IM_ARRAYSIZE(values), 0,
                            "Size data", -fMax, fMax, ImVec2(0, 100));


                        char str[10];
                        snprintf(str, sizeof(str), "%d. %.2f", m_iSelectSize + 1, m_tParticleData.fSizeDiagrams[m_iSelectSize].x);

                        if (ImGui::BeginCombo("TimeValue", str))
                        {
                            for (_uint i = 0; i < (_uint)m_tParticleData.fSizeDiagrams.size(); ++i) {
                                _bool sel = i == m_iSelectSize;
                                snprintf(str, sizeof(str), "%d. %.2f", i + 1, m_tParticleData.fSizeDiagrams[i].x);
                                if (ImGui::Selectable(str, sel))
                                    m_iSelectSize = i;
                                if (sel)
                                    ImGui::SetItemDefaultFocus();
                            }
                            ImGui::EndCombo();
                        }
                    }
                    if (ImGui::Button("AddTime", btn)) {
                        if (0 < m_tParticleData.fSizeDiagrams.size()) {
                            _float3 val = m_tParticleData.fSizeDiagrams.back();
                            val.y = 0;
                            m_tParticleData.fSizeDiagrams.push_back(val);
                        }
                        else {
                            _float3 val = { 0,0,0 };
                            m_tParticleData.fSizeDiagrams.push_back(val);
                        }
                        m_iSelectSize = m_tParticleData.fSizeDiagrams.size() - 1;
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("DeleteTime", btn)) {
                        _uint iCount = 0;
                        for (auto i = m_tParticleData.fSizeDiagrams.begin(); i != m_tParticleData.fSizeDiagrams.end();) {
                            if (iCount == m_iSelectSize) {
                                m_tParticleData.fSizeDiagrams.erase(i);
                                break;
                            }
                            ++i;
                            ++iCount;
                        }
                        m_iSelectSize = max(m_iSelectSize - 1, 0);
                    }

                    if (m_iSelectSize < m_tParticleData.fSizeDiagrams.size()) {
                        if (0 < m_iSelectSize)
                            ImGui::DragFloat("Size Time", &m_tParticleData.fSizeDiagrams[m_iSelectSize].x, 0.01f, m_tParticleData.fSizeDiagrams[m_iSelectSize - 1].x, m_iSelectSize != m_tParticleData.fSizeDiagrams.size() - 1 ? m_tParticleData.fSizeDiagrams[m_iSelectSize + 1].x : 1);
                        ImGui::DragFloat("Size fx", &m_tParticleData.fSizeDiagrams[m_iSelectSize].z, 1.f, -90.f, 90.f);
                        ImGui::DragFloat("Size", &m_tParticleData.fSizeDiagrams[m_iSelectSize].y, 0.01f, -100.f, 100.f);
                    }
                    ImGui::TreePop();
                }

                if (ImGui::TreeNode("Gravity Diagram")) {
                    ImGui::PlotLines("Gravity Wave", values, IM_ARRAYSIZE(values), 0,
                        "Gravity Data", -fMax, fMax, ImVec2(0, 100));

                    ImGui::DragFloat("Start Gravity", &m_tParticleData.fGravityDiagram.x, 0.01f, -100.f, 100.f);
                    ImGui::DragFloat("Start Gravity fx", &m_tParticleData.fGravityDiagram.y, 1.f, -90.f, 90.f);
                    ImGui::DragFloat("End Gravity", &m_tParticleData.fGravityDiagram.z, 0.01f, -100.f, 100.f);
                    ImGui::DragFloat("End Gravity fx", &m_tParticleData.fGravityDiagram.w, 1.f, -90.f, 90.f);
                    ImGui::TreePop();
                }
            }
            break;
            case 1:
            {

                if (ImGui::TreeNode("Color")) {
                    ImGui::ColorPicker4("Color", (_float*)&m_tParticleData.fColor, ImGuiColorEditFlags_PickerHueWheel);
                    ImGui::TreePop();
                }

                if (ImGui::TreeNode("Shader")) {
                    ImGui::TreePop();
                }
                ImGui::DragFloat3("Mesh Rotation", reinterpret_cast<_float*>(&m_tParticleData.fMeshRotation), 0.1f, 0.f, 360.f);
                string szRender;
                switch (m_tParticleData.iSelectRender) {
                case 0:
                    szRender = "NONBLEND";
                    break;
                case 1:
                    szRender = "NONLIGHT";
                    break;
                case 2:
                    szRender = "BLEND";
                    break;
                case 3:
                    szRender = "BLUR";
                    break;
                case 4:
                    szRender = "GLOW";
                    break;
                case 5:
                    szRender = "METABALL";
                    break;
                case 6:
                    szRender = "DISTORTION";
                    break;
                }
                if (ImGui::BeginCombo("RenderType", szRender.c_str()))
                {
                    for (_uint i = 0; i <= 6; ++i) {
                        _bool sel = i == m_tParticleData.iSelectRender;
                        switch (i) {
                        case 0:
                            szRender = "NONBLEND";
                            break;
                        case 1:
                            szRender = "NONLIGHT";
                            break;
                        case 2:
                            szRender = "BLEND";
                            break;
                        case 3:
                            szRender = "BLUR";
                            break;
                        case 4:
                            szRender = "GLOW";
                            break;
                        case 5:
                            szRender = "METABALL";
                            break;
                        case 6:
                            szRender = "DISTORTION";
                            break;
                        }
                        if (ImGui::Selectable(szRender.c_str(), sel))
                            m_tParticleData.iSelectRender = i;
                        if (sel)
                            ImGui::SetItemDefaultFocus();
                    }
                    ImGui::EndCombo();
                }
                char szName[30] = {};
                strncpy_s(szName, sizeof(szName), m_szCS.c_str(), _TRUNCATE);
                if (ImGui::InputText("File", szName, sizeof(szName))) {
                    m_szCS = szName;
                }
                if (ImGui::Button("Refresh Shader", btn)) {
                    m_tParticleData.szCS = m_szCS;
                    m_pParticles[m_iSelectParticle]->Set_Components(m_tParticleData);
                }
                ImGui::InputInt("Shader Begine", &m_tParticleData.iBegin);
                if (ImGui::BeginCombo("ImageType", m_iImageType == 0 ? "Mask" : m_iImageType == 1 ? "Diffuse" : "Dissolve"))
                {
                    for (_uint i = 0; i < 3; ++i) {
                        _bool sel = i == m_iImageType;
                        if (ImGui::Selectable(i == 0 ? "Mask" : i == 1 ? "Diffuse" : "Dissolve", sel))
                            m_iImageType = i;
                        if (sel)
                            ImGui::SetItemDefaultFocus();
                    }
                    ImGui::EndCombo();
                }
                ImGui::Checkbox("Spectrum", &m_tParticleData.bisSpectrum);
                if(!m_tParticleData.bisCircle)
                    ImGui::Checkbox("Sphere Pointer", &m_tParticleData.bisSphere);
                if (!m_tParticleData.bisSphere)
                ImGui::Checkbox("Circle Pointer", &m_tParticleData.bisCircle);
                if (m_tParticleData.bisSphere) {
                    ImGui::DragFloat("SpereSize", reinterpret_cast<_float*>(&m_tParticleData.fSphereSize), 0.1f, 0.f, 100.f);
                }
                if (m_tParticleData.bisCircle) {
                    ImGui::DragFloat("CircleSize", reinterpret_cast<_float*>(&m_tParticleData.fSphereSize), 0.1f, 0.f, 100.f);
                    ImGui::DragFloat("CircleSpeed", reinterpret_cast<_float*>(&m_tParticleData.fCircleSpeed), 0.1f, 0.f, 100.f);
                    ImGui::DragFloat2("Circle", reinterpret_cast<_float*>(&m_tParticleData.fCircle), 0.1f, 0.f, 100.f);
                }
                if (ImGui::TreeNode("TurnPower"))
                {
                    ImGui::DragFloat2("TurnPower", reinterpret_cast<_float*>(&m_tParticleData.fTurnPower), 0.1f, -100.f, 100.f);
                    ImGui::TreePop();
                }
                if (ImGui::TreeNode("MaskUV"))
                {
                    ImGui::DragFloat2("MaskUV", reinterpret_cast<_float*>(&m_tParticleData.fMaskUV), 0.1f, -100.f, 100.f);
                    ImGui::DragFloat2("MaskUV Speed", reinterpret_cast<_float*>(&m_tParticleData.fMaskUVSpeed), 0.1f, -100.f, 100.f);
                    ImGui::DragFloat2("MaskUV Size", reinterpret_cast<_float*>(&m_tParticleData.fMaskUVSize), 0.1f, -100.f, 100.f);
                    ImGui::TreePop();
                }
                if (ImGui::TreeNode("DiffuseUV"))
                {
                    ImGui::DragFloat2("DiffuseUV", reinterpret_cast<_float*>(&m_tParticleData.fDiffuseUV), 0.1f, -100.f, 100.f);
                    ImGui::DragFloat2("DiffuseUV Speed", reinterpret_cast<_float*>(&m_tParticleData.fDiffuseUVSpeed), 0.1f, -100.f, 100.f);
                    ImGui::DragFloat2("DiffuseUV Size", reinterpret_cast<_float*>(&m_tParticleData.fDiffuseUVSize), 0.1f, -100.f, 100.f);
                    ImGui::TreePop();
                }
                if (ImGui::TreeNode("DissolveUV"))
                {
                    ImGui::DragFloat2("DissolveUV", reinterpret_cast<_float*>(&m_tParticleData.fDissolveUV), 0.1f, -100.f, 100.f);
                    ImGui::DragFloat2("DissolveUV Speed", reinterpret_cast<_float*>(&m_tParticleData.fDissolveUVSpeed), 0.1f, -100.f, 100.f);
                    ImGui::DragFloat2("DissolveUV Size", reinterpret_cast<_float*>(&m_tParticleData.fDissolveUVSize), 0.1f, -100.f, 100.f);
                    ImGui::TreePop();
                }

                ImGui::Separator();

                ImGui::BeginChild("ImageScroll", ImVec2(300, 200), true);
                if (0 < m_SRVs[m_iImageType].size()) {
                    _uint i = 0;
                    for (auto SRV : m_SRVs[m_iImageType]) {
                        if (ImGui::ImageButton(m_ImageFiles[m_iImageType][i].c_str(), (ImTextureRef)SRV, ImVec2(100, 100))) {
                            m_pParticles[m_iSelectParticle]->Set_Texture(m_iImageType, m_ImageFiles[m_iImageType][i].c_str());


                            switch (m_iImageType) {
                            case 0:
                                m_tParticleData.szMaskTexture = m_ImageFiles[m_iImageType][i];
                                break;
                            case 1:
                                m_tParticleData.szDiffuseTexture = m_ImageFiles[m_iImageType][i];
                                break;
                            case 2:
                                m_tParticleData.szDissolveTexture = m_ImageFiles[m_iImageType][i];
                                break;
                            }

                        }
                        if (1 == ++i % 2)
                            ImGui::SameLine();
                    }
                }
                ImGui::EndChild();
            }
            break;
            }
            ImGui::EndChild();


        }
        break;
    case 2:

        if (ImGui::Button("Add MeshEffect", btn)) {
            Add_MeshEffect();
        }
        if (0 < m_pMeshs.size())
        {
            ImGui::SameLine();
            if (ImGui::Button("Delete MeshEffect", btn)) {
                Delete_MeshEffect();
                if (0 >= m_pMeshs.size()) {
                    ImGui::End();
                    return;
                }
            }

            vector<_float3> fSizeDiagrams = m_tSpriteParticleData.fSizeDiagrams;
            _float fDelayTime = m_tSpriteParticleData.fDelayTime;
            _float fEndTime = m_tSpriteParticleData.fEndTime;

            _float time = 1.f / 100;
            _float fMax = 5;
            float values[100] = {};
            char str[3];
            if (ImGui::BeginCombo("Models", m_ModelFilePaths[m_iSelectModel].c_str()))
            {
                for (_uint i = 0; i < m_ModelFilePaths.size(); ++i) {
                    _bool sel = i == m_iSelectModel;
                    if (ImGui::Selectable(m_ModelFilePaths[i].c_str(), sel)) {
                        m_iSelectModel = i;
                        _tchar szPath[256] = { 0, };
                        MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, m_ModelFilePaths[m_iSelectModel].c_str(), strlen(m_ModelFilePaths[m_iSelectModel].c_str()), szPath, 256);

                        char szModelPath[MAX_PATH] = {};
                        strncpy_s(szModelPath, sizeof(szModelPath), m_ModelFilePaths[m_iSelectModel].c_str(), _TRUNCATE);
                        m_tMeshData.szModel = szModelPath;
                        m_pMeshs[m_iSelectMesh]->Set_Model(szPath);
                    }
                    if (sel)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }
            snprintf(str, sizeof(str), "%d", m_iSelectMesh);
            if (ImGui::BeginCombo("Meshs", str))
            {
                for (_uint i = 0; i < m_pMeshs.size(); ++i) {
                    _bool sel = i == m_iSelectMesh;
                    snprintf(str, sizeof(str), "%d", i);
                    if (ImGui::Selectable(str, sel)) {
                        m_iSelectMesh = i;
                        m_tMeshData = m_pMeshs[m_iSelectMesh]->Get_Data();
                    }
                    if (sel)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }
            if (ImGui::Button("All Replay", btn)) {
                m_bisReplay = true;
                m_fTime = 0.f;
                for (_uint i = 0; i < m_pParticles.size(); ++i) {
                    m_pParticles[i]->Set_Components(m_pParticles[i]->Get_Data());
                }
                for (_uint i = 0; i < m_pSpriteParticles.size(); ++i) {
                    m_pSpriteParticles[i]->Set_Components(m_pSpriteParticles[i]->Get_Data());
                }
                for (_uint i = 0; i < m_pMeshs.size(); ++i) {
                    m_pMeshs[i]->Set_Components(m_pMeshs[i]->Get_Data());
                }
                ImGui::End();
                return;
            }

            ImGui::SameLine();
            if (ImGui::Button("Replay", btn)) {
                m_bisReplay = true;
                m_pMeshs[m_iSelectMesh]->Set_Components(m_pMeshs[m_iSelectMesh]->Get_Data());
                ImGui::End();
                return;
            }
            m_pMeshs[m_iSelectMesh]->Update(m_tMeshData);


            ImGui::DragFloat("Delay Time", reinterpret_cast<_float*>(&m_tMeshData.fDelayTime), 0.1f, 0.f, 100.f);
            ImGui::DragFloat("End Time", reinterpret_cast<_float*>(&m_tMeshData.fEndTime), 0.1f, 0.f, 100.f);

            ImGui::ColorPicker4("MyColor", (_float*)&m_tMeshData.fColor, ImGuiColorEditFlags_PickerHueWheel);

            ImGui::DragFloat3("Effect Scale", reinterpret_cast<_float*>(&m_tMeshData.fScale), 0.1f, 0.f, 100.f);
            ImGui::DragFloat3("Effect Position", reinterpret_cast<_float*>(&m_tMeshData.fPosition), 0.1f, -1000.f, 1000.f);
            ImGui::DragFloat3("Effect Rotation", reinterpret_cast<_float*>(&m_tMeshData.fRotation), 1.f, 0.f, 360.f);

            string szRender;
            switch (m_tMeshData.iSelectRender)
            {
            case 0:
                szRender = "NONBLEND";
                break;
            case 1:
                szRender = "NONLIGHT";
                break;
            case 2:
                szRender = "BLEND";
                break;
            case 3:
                szRender = "BLUR";
                break;
            case 4:
                szRender = "GLOW";
                break;
            case 5:
                szRender = "METABALL";
                break;
            case 6:
                szRender = "DISTORTION";
                break;
            }
            if (ImGui::BeginCombo("RenderType", szRender.c_str()))
            {
                for (_uint i = 0; i <= 6; ++i) {
                    _bool sel = i == m_tMeshData.iSelectRender;
                    switch (i)
                    {
                    case 0:
                        szRender = "NONBLEND";
                        break;
                    case 1:
                        szRender = "NONLIGHT";
                        break;
                    case 2:
                        szRender = "BLEND";
                        break;
                    case 3:
                        szRender = "BLUR";
                        break;
                    case 4:
                        szRender = "GLOW";
                        break;
                    case 5:
                        szRender = "METABALL";
                        break;
                    case 6:
                        szRender = "DISTORTION";
                        break;
                    }
                        if (ImGui::Selectable(szRender.c_str(), sel))
                            m_tMeshData.iSelectRender = i;
                        if (sel)
                            ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }
            if (ImGui::Button("Refresh Shader", btn)) {
                m_pMeshs[m_iSelectMesh]->Set_Components(m_tMeshData);
            }
            ImGui::InputInt("Shader Begine", &m_tMeshData.iBegin);
            if (ImGui::BeginCombo("ImageType", m_iImageType == 0 ? "Mask" : m_iImageType == 1 ? "Diffuse" : "Dissolve"))
            {
                for (_uint i = 0; i < 3; ++i) {
                    _bool sel = i == m_iImageType;
                    if (ImGui::Selectable(i == 0 ? "Mask" : i == 1 ? "Diffuse" : "Dissolve", sel))
                        m_iImageType = i;
                    if (sel)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }

            if (ImGui::TreeNode("MaskUV"))
            {
                ImGui::DragFloat2("MaskUV", reinterpret_cast<_float*>(&m_tMeshData.fMaskUV), 0.01f, -100.f, 100.f);
                ImGui::DragFloat2("MaskUV Speed", reinterpret_cast<_float*>(&m_tMeshData.fMaskUVSpeed), 0.01f, -100.f, 100.f);
                ImGui::DragFloat2("MaskUV Size", reinterpret_cast<_float*>(&m_tMeshData.fMaskUVSize), 0.01f, -100.f, 100.f);
                ImGui::TreePop();
            }
            if (ImGui::TreeNode("DiffuseUV"))
            {
                ImGui::DragFloat2("DiffuseUV", reinterpret_cast<_float*>(&m_tMeshData.fDiffuseUV), 0.1f, -100.f, 100.f);
                ImGui::DragFloat2("DiffuseUV Speed", reinterpret_cast<_float*>(&m_tMeshData.fDiffuseUVSpeed), 0.1f, -100.f, 100.f);
                ImGui::DragFloat2("DiffuseUV Size", reinterpret_cast<_float*>(&m_tMeshData.fDiffuseUVSize), 0.1f, -100.f, 100.f);
                ImGui::TreePop();
            }
            if (ImGui::TreeNode("DissolveUV"))
            {
                ImGui::DragFloat2("DissolveUV", reinterpret_cast<_float*>(&m_tMeshData.fDissolveUV), 0.1f, -100.f, 100.f);
                ImGui::DragFloat2("DissolveUV Speed", reinterpret_cast<_float*>(&m_tMeshData.fDissolveUVSpeed), 0.1f, -100.f, 100.f);
                ImGui::DragFloat2("DissolveUV Size", reinterpret_cast<_float*>(&m_tMeshData.fDissolveUVSize), 0.1f, -100.f, 100.f);
                ImGui::TreePop();
            }


            if (ImGui::TreeNode("Size Diagram")) {
                m_iSelectSize = min(m_iSelectSize, (_uint)m_tMeshData.fSizeDiagrams.size() - 1);
                if (m_iSelectSize < m_tMeshData.fSizeDiagrams.size()) {
                    _float time = 1.f / 100;
                    _float fMax = 5;
                    for (_uint i = 0; i < 100; ++i) {
                        _float3* in = {};
                        _float3* out = {};
                        for (_uint j = 0; j < (_uint)m_tMeshData.fSizeDiagrams.size(); ++j) {
                            if (m_tMeshData.fSizeDiagrams[j].x <= time * i) {
                                in = &m_tMeshData.fSizeDiagrams[j];
                            }
                            if (m_tMeshData.fSizeDiagrams[j].x > time * i) {
                                out = &m_tMeshData.fSizeDiagrams[j];
                                break;
                            }
                        }
                        if (nullptr == out)
                            values[i] = in->y;
                        else {
                            _float t = (time * i - in->x) / (out->x - in->x);
                            if (fabsf(in->z) >= 90.f || fabsf(out->z) >= 90.f) {
                                values[i] = in->y;
                            }
                            else {
                                values[i] = (2 * powf(t, 3) - 3 * powf(t, 2) + 1) * in->y
                                    + (powf(t, 3) - 2 * powf(t, 2) + t) * (tanf(XMConvertToRadians(in->z)) * (out->x - in->x) * 100)
                                    + (-2 * powf(t, 3) + 3 * powf(t, 2)) * out->y
                                    + (powf(t, 3) - powf(t, 2)) * (tanf(XMConvertToRadians(out->z)) * (out->x - in->x) * 100);
                            }
                        }
                        fMax = max(fMax, fabsf(values[i]));
                    }
                    ImGui::PlotLines("Size Wave", values, IM_ARRAYSIZE(values), 0,
                        "Size data", -fMax, fMax, ImVec2(0, 100));


                    char str[10];
                    snprintf(str, sizeof(str), "%d. %.2f", m_iSelectSize + 1, m_tMeshData.fSizeDiagrams[m_iSelectSize].x);

                    if (ImGui::BeginCombo("TimeValue", str))
                    {
                        for (_uint i = 0; i < (_uint)m_tMeshData.fSizeDiagrams.size(); ++i) {
                            _bool sel = i == m_iSelectSize;
                            snprintf(str, sizeof(str), "%d. %.2f", i + 1, m_tMeshData.fSizeDiagrams[i].x);
                            if (ImGui::Selectable(str, sel))
                                m_iSelectSize = i;
                            if (sel)
                                ImGui::SetItemDefaultFocus();
                        }
                        ImGui::EndCombo();
                    }
                }
                if (ImGui::Button("AddTime", btn)) {
                    if (0 < m_tMeshData.fSizeDiagrams.size()) {
                        _float3 val = m_tMeshData.fSizeDiagrams.back();
                        val.y = 0;
                        m_tMeshData.fSizeDiagrams.push_back(val);
                    }
                    else {
                        _float3 val = { 0,0,0 };
                        m_tMeshData.fSizeDiagrams.push_back(val);
                    }
                    m_iSelectSize = m_tMeshData.fSizeDiagrams.size() - 1;
                }
                ImGui::SameLine();
                if (ImGui::Button("DeleteTime", btn)) {
                    _uint iCount = 0;
                    for (auto i = m_tMeshData.fSizeDiagrams.begin(); i != m_tMeshData.fSizeDiagrams.end();) {
                        if (iCount == m_iSelectSize) {
                            m_tMeshData.fSizeDiagrams.erase(i);
                            break;
                        }
                        ++i;
                        ++iCount;
                    }
                    m_iSelectSize = max(m_iSelectSize - 1, 0);
                }

                if (m_iSelectSize < m_tMeshData.fSizeDiagrams.size()) {
                    if (0 < m_iSelectSize)
                        ImGui::DragFloat("Size Time", &m_tMeshData.fSizeDiagrams[m_iSelectSize].x, 0.01f, m_tMeshData.fSizeDiagrams[m_iSelectSize - 1].x, m_iSelectSize != m_tMeshData.fSizeDiagrams.size() - 1 ? m_tMeshData.fSizeDiagrams[m_iSelectSize + 1].x : 1);
                    ImGui::DragFloat("Size fx", &m_tMeshData.fSizeDiagrams[m_iSelectSize].z, 1.f, -90.f, 90.f);
                    ImGui::DragFloat("Size", &m_tMeshData.fSizeDiagrams[m_iSelectSize].y, 0.01f, -100.f, 100.f);
                }
                ImGui::TreePop();
            }

            ImGui::Separator();

            ImGui::BeginChild("ImageScroll", ImVec2(300, 200), true);
            if (0 < m_SRVs[m_iImageType].size()) {
                _uint i = 0;
                for (auto SRV : m_SRVs[m_iImageType]) {
                    if (ImGui::ImageButton(m_ImageFiles[m_iImageType][i].c_str(), (ImTextureRef)SRV, ImVec2(100, 100))) {
                        m_pMeshs[m_iSelectMesh]->Set_Texture(m_iImageType, m_ImageFiles[m_iImageType][i].c_str());

                        switch (m_iImageType) {
                        case 0:
                            m_tMeshData.szMaskTexture = m_ImageFiles[m_iImageType][i];
                            break;
                        case 1:
                            m_tMeshData.szDiffuseTexture = m_ImageFiles[m_iImageType][i];
                            break;
                        case 2:
                            m_tMeshData.szDissolveTexture = m_ImageFiles[m_iImageType][i];
                            break;
                        }
                    }
                    if (1 == ++i % 2)
                        ImGui::SameLine();
                }
            }
            ImGui::EndChild();

        }
        break;
    case 3: {
        ImGui::SameLine();

        if (ImGui::Button("Add TrailData", btn)) {
            Add_TrailEffectData();
        }
        if (0 < m_pTrailDatas.size())
        {
            ImGui::SameLine();
            if (ImGui::Button("Delete TrailData", btn)) {
                Delete_TrailEffectData();
                if (0 >= m_pTrailDatas.size()) {
                    ImGui::End();
                    return;
                }
            }

            char str[3];
            snprintf(str, sizeof(str), "%d", m_iSelectTrailData);
            if (ImGui::BeginCombo("Particles", str))
            {
                for (_uint i = 0; i < m_pTrailDatas.size(); ++i) {
                    _bool sel = i == m_iSelectTrailData;
                    snprintf(str, sizeof(str), "%d", i);
                    if (ImGui::Selectable(str, sel)) {
                        m_iSelectTrailData = i;
                        m_tTrailData = m_pTrailDatas[m_iSelectTrailData]->Get_Data();
                    }
                    if (sel)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }

            if (ImGui::Button("Replay", btn)) {
                m_fTime = 0.f;
                m_pTrailEffect->Refresh();
            }
            _float speed = m_pTrailEffect->Get_Speed();
            ImGui::DragFloat("Trail Speed", reinterpret_cast<_float*>(&speed), 0.01f, -100.f, 100.f);
            m_pTrailEffect->Set_Speed(speed);
            string szRender;
            switch (m_tTrailData.iSelectRender)
            {
            case 0:
                szRender = "NONBLEND";
                break;
            case 1:
                szRender = "NONLIGHT";
                break;
            case 2:
                szRender = "BLEND";
                break;
            case 3:
                szRender = "BLUR";
                break;
            case 4:
                szRender = "GLOW";
                break;
            case 5:
                szRender = "METABALL";
                break;
            case 6:
                szRender = "DISTORTION";
                break;
            }
            if (ImGui::BeginCombo("RenderType", szRender.c_str()))
            {
                for (_uint i = 0; i <= 6; ++i) {

                    _bool sel = i == ENUM_CLASS(m_tTrailData.iSelectRender);
                    switch (i)
                    {
                    case 0:
                        szRender = "NONBLEND";
                        break;
                    case 1:
                        szRender = "NONLIGHT";
                        break;
                    case 2:
                        szRender = "BLEND";
                        break;
                    case 3:
                        szRender = "BLUR";
                        break;
                    case 4:
                        szRender = "GLOW";
                        break;
                    case 5:
                        szRender = "METABALL";
                        break;
                    case 6:
                        szRender = "DISTORTION";
                        break;
                    }
                    if (ImGui::Selectable(szRender.c_str(), sel))
                        m_tTrailData.iSelectRender = i;
                    if (sel)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }
            if (ImGui::Button("Refresh Shader", btn)) {
                m_pTrailDatas[m_iSelectTrailData]->Set_Components(m_tTrailData);
                m_fTime = 0.f;
                m_pTrailEffect->Refresh();
            }
            ImGui::InputInt("Shader Begine", &m_tTrailData.iBegin);
            if (ImGui::BeginCombo("ImageType", m_iImageType == 0 ? "Mask" : m_iImageType == 1 ? "Diffuse" : "Dissolve"))
            {
                for (_uint i = 0; i < 3; ++i) {
                    _bool sel = i == m_iImageType;
                    if (ImGui::Selectable(i == 0 ? "Mask" : i == 1 ? "Diffuse" : "Dissolve", sel))
                        m_iImageType = i;
                    if (sel)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }

            if (ImGui::TreeNode("Sprite Color"))
            {
                ImGui::ColorPicker4("MyColor", (_float*)&m_tTrailData.fColor, ImGuiColorEditFlags_PickerHueWheel);
                ImGui::TreePop();
            }

            if (ImGui::TreeNode("MaskUV"))
            {
                ImGui::DragFloat2("MaskUV", reinterpret_cast<_float*>(&m_tTrailData.fMaskUV), 0.01f, -100.f, 100.f);
                ImGui::DragFloat2("MaskUV Speed", reinterpret_cast<_float*>(&m_tTrailData.fMaskUVSpeed), 0.01f, -100.f, 100.f);
                ImGui::DragFloat2("MaskUV Size", reinterpret_cast<_float*>(&m_tTrailData.fMaskUVSize), 0.01f, -100.f, 100.f);
                ImGui::TreePop();
            }
            if (ImGui::TreeNode("DiffuseUV"))
            {
                ImGui::DragFloat2("DiffuseUV", reinterpret_cast<_float*>(&m_tTrailData.fDiffuseUV), 0.1f, -100.f, 100.f);
                ImGui::DragFloat2("DiffuseUV Speed", reinterpret_cast<_float*>(&m_tTrailData.fDiffuseUVSpeed), 0.1f, -100.f, 100.f);
                ImGui::DragFloat2("DiffuseUV Size", reinterpret_cast<_float*>(&m_tTrailData.fDiffuseUVSize), 0.1f, -100.f, 100.f);
                ImGui::TreePop();
            }
            if (ImGui::TreeNode("DissolveUV"))
            {
                ImGui::DragFloat2("DissolveUV", reinterpret_cast<_float*>(&m_tTrailData.fDissolveUV), 0.1f, -100.f, 100.f);
                ImGui::DragFloat2("DissolveUV Speed", reinterpret_cast<_float*>(&m_tTrailData.fDissolveUVSpeed), 0.1f, -100.f, 100.f);
                ImGui::DragFloat2("DissolveUV Size", reinterpret_cast<_float*>(&m_tTrailData.fDissolveUVSize), 0.1f, -100.f, 100.f);
                ImGui::TreePop();
            }


            ImGui::Separator();

            ImGui::BeginChild("ImageScroll", ImVec2(300, 200), true);
            if (0 < m_SRVs[m_iImageType].size()) {
                _uint i = 0;
                for (auto SRV : m_SRVs[m_iImageType]) {
                    if (ImGui::ImageButton(m_ImageFiles[m_iImageType][i].c_str(), (ImTextureRef)SRV, ImVec2(100, 100))) {
                        m_pTrailDatas[m_iSelectTrailData]->Set_Texture(m_iImageType, m_ImageFiles[m_iImageType][i].c_str());
                        switch (m_iImageType) {
                        case 0:
                            m_tTrailData.szMaskTexture = m_ImageFiles[m_iImageType][i];
                            break;
                        case 1:
                            m_tTrailData.szDiffuseTexture = m_ImageFiles[m_iImageType][i];
                            break;
                        case 2:
                            m_tTrailData.szDissolveTexture = m_ImageFiles[m_iImageType][i];
                            break;
                        }
                    }
                    if (1 == ++i % 2)
                        ImGui::SameLine();
                }
            }
            m_pTrailDatas[m_iSelectTrailData]->Update(m_tTrailData);
            ImGui::EndChild();
        }
    }
          break;
    }
    ImGui::End();

    if (m_bisReplay)
    {
        m_bisReplay = false;
        return;
    }
    if (m_iSelectMeshParticle == 3) {
        if (m_bisPause) {
            for (_uint i = 0; i < m_pMeshs.size(); ++i) {
                m_pMeshs[i]->Priority_Update(fTimeDelta * m_fSpeed);
                m_pMeshs[i]->Update(fTimeDelta * m_fSpeed);
                m_pMeshs[i]->Late_Update(fTimeDelta * m_fSpeed);
            }
            for (_uint i = 0; i < m_pParticles.size(); ++i) {
                m_pParticles[i]->Priority_Update(fTimeDelta * m_fSpeed);
                m_pParticles[i]->Update(fTimeDelta * m_fSpeed);
                m_pParticles[i]->Late_Update(fTimeDelta * m_fSpeed);
            }
            for (_uint i = 0; i < m_pSpriteParticles.size(); ++i) {
                m_pSpriteParticles[i]->Priority_Update(fTimeDelta * m_fSpeed);
                m_pSpriteParticles[i]->Update(fTimeDelta * m_fSpeed);
                m_pSpriteParticles[i]->Late_Update(fTimeDelta * m_fSpeed);
            }
            m_pTrailEffect->Priority_Update(fTimeDelta * m_fSpeed);
            m_pTrailEffect->Update(fTimeDelta * m_fSpeed);
            m_pTrailEffect->Late_Update(fTimeDelta * m_fSpeed);
            for (_uint i = 0; i < m_pTrailDatas.size(); ++i) {
                m_pTrailDatas[i]->Late_Update(fTimeDelta * m_fSpeed);
            }
        }
        else {
            for (_uint i = 0; i < m_pMeshs.size(); ++i) {
                m_pMeshs[i]->Late_Update(0);
            }
            for (_uint i = 0; i < m_pParticles.size(); ++i) {
                m_pParticles[i]->Late_Update(0);
            }
            for (_uint i = 0; i < m_pSpriteParticles.size(); ++i) {
                m_pSpriteParticles[i]->Late_Update(0);
            }
            for (_uint i = 0; i < m_pTrailDatas.size(); ++i) {
                m_pTrailDatas[i]->Late_Update(0);
            }
        }
    }
    else {
        if (m_bisPause) {
            for (_uint i = 0; i < m_pMeshs.size(); ++i) {
                m_pMeshs[i]->Priority_Update(fTimeDelta * m_fSpeed);
                m_pMeshs[i]->Update(fTimeDelta* m_fSpeed);
                m_pMeshs[i]->Late_Update(fTimeDelta* m_fSpeed);
            }
            for (_uint i = 0; i < m_pParticles.size(); ++i) {
                m_pParticles[i]->Priority_Update(fTimeDelta* m_fSpeed);
                m_pParticles[i]->Update(fTimeDelta* m_fSpeed);
                m_pParticles[i]->Late_Update(fTimeDelta* m_fSpeed);
            }
            for (_uint i = 0; i < m_pSpriteParticles.size(); ++i) {
                m_pSpriteParticles[i]->Priority_Update(fTimeDelta* m_fSpeed);
                m_pSpriteParticles[i]->Update(fTimeDelta* m_fSpeed);
                m_pSpriteParticles[i]->Late_Update(fTimeDelta* m_fSpeed);
            }
        }
        else {
            for (_uint i = 0; i < m_pMeshs.size(); ++i) {
                m_pMeshs[i]->Late_Update(0);
            }
            for (_uint i = 0; i < m_pParticles.size(); ++i) {
                m_pParticles[i]->Late_Update(0);
            }
            for (_uint i = 0; i < m_pSpriteParticles.size(); ++i) {
                m_pSpriteParticles[i]->Late_Update(0);
            }
        }
    }
}

HRESULT CParticle_Setting::Render()
{
    return S_OK;
}

void CParticle_Setting::Free()
{
    __super::Free();
    for (auto pParticle : m_pParticles)
        Safe_Release(pParticle);
    m_pParticles.clear();
    for (auto pSpriteParticle : m_pSpriteParticles)
        Safe_Release(pSpriteParticle);
    m_pSpriteParticles.clear();
    for (auto pMesh : m_pMeshs)
        Safe_Release(pMesh);
    m_pMeshs.clear();
    for (auto pTrailData : m_pTrailDatas)
        Safe_Release(pTrailData);
    m_pTrailDatas.clear();
    
    Safe_Release(m_pTrailEffect);
    
    for (_uint i = 0; i < 4; ++i) {
        for (auto SRV : m_SRVs[i]) {
            Safe_Release(SRV);
        }
        m_SRVs[i].clear();
    }
    Safe_Release(m_pTransform);
}

void CParticle_Setting::WriteString(ofstream& fileBinaryStream, _char* pStr)
{
    _uint iStringLength = strlen(pStr);
    fileBinaryStream.write(reinterpret_cast<const _char*>(&iStringLength), sizeof(iStringLength));

    fileBinaryStream.write(pStr, iStringLength);
}

void CParticle_Setting::WriteInt(ofstream& fileBinaryStream, _int vTmp)
{
    fileBinaryStream.write((_char*)&vTmp, sizeof(_int));
}

void CParticle_Setting::WriteFloat4(ofstream& fileBinaryStream, _float4 vTmp)
{
    fileBinaryStream.write((_char*)&vTmp, sizeof(_float4));
}

void CParticle_Setting::WriteFloat3(ofstream& fileBinaryStream, _float3 vTmp)
{
    fileBinaryStream.write((_char*)&vTmp, sizeof(_float3));
}

void CParticle_Setting::WriteFloat2(ofstream& fileBinaryStream, _float2 vTmp)
{
    fileBinaryStream.write((_char*)&vTmp, sizeof(_float2));
}

void CParticle_Setting::WriteFloat(ofstream& fileBinaryStream, _float vTmp)
{
    fileBinaryStream.write((_char*)&vTmp, sizeof(_float));
}

void CParticle_Setting::WriteInt2(ofstream& fileBinaryStream, _int2 vTmp)
{
    fileBinaryStream.write((_char*)&vTmp, sizeof(_int2));
}

void CParticle_Setting::WriteBool(ofstream& fileBinaryStream, _bool vTmp)
{
    fileBinaryStream.write((_char*)&vTmp, sizeof(_bool));
}

_char* CParticle_Setting::ReadString(ifstream& fileBinaryStream)
{
    _uint iStringLength;
    fileBinaryStream.read((_char*)&iStringLength, sizeof(iStringLength));

    _char* sz = new _char[iStringLength + 1];
    fileBinaryStream.read(&sz[0], iStringLength);

    sz[iStringLength] = '\0';
    return sz;
}

_int CParticle_Setting::ReadInt(ifstream& fileBinaryStream)
{
    _int iValue;
    fileBinaryStream.read((_char*)&iValue, sizeof(_int));
    return iValue;
}

_float4 CParticle_Setting::ReadFloat4(ifstream& fileBinaryStream)
{
    _float4 fValue;
    fileBinaryStream.read((_char*)&fValue, sizeof(_float4));
    return fValue;
}

_float3 CParticle_Setting::ReadFloat3(ifstream& fileBinaryStream)
{
    _float3 fValue;
    fileBinaryStream.read((_char*)&fValue, sizeof(_float3));
    return fValue;
}

_float2 CParticle_Setting::ReadFloat2(ifstream& fileBinaryStream)
{
    _float2 fValue;
    fileBinaryStream.read((_char*)&fValue, sizeof(_float2));
    return fValue;
}

_float CParticle_Setting::ReadFloat(ifstream& fileBinaryStream)
{
    _float fValue;
    fileBinaryStream.read((_char*)&fValue, sizeof(_float));
    return fValue;
}

_int2 CParticle_Setting::ReadInt2(ifstream& fileBinaryStream)
{
    _int2 iValue;
    fileBinaryStream.read((_char*)&iValue, sizeof(_int2));
    return iValue;
}

_bool CParticle_Setting::ReadBool(ifstream& fileBinaryStream)
{
    _bool bisFlag;
    fileBinaryStream.read((_char*)&bisFlag, sizeof(_bool));
    return bisFlag;
}
