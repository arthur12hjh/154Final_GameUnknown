#pragma once
#include "ImgBase.h"
#include "Particle.h"
#include "MeshEffect.h"

NS_BEGIN(Engine)
class CVIBuffer_Point_Instance;
class CShader;
class CComputeShader;
class CTransform;
class CTexture;
NS_END

NS_BEGIN(Tool_Effect)

class CParticle_Setting : public CImgBase
{
public:
private:
    CParticle_Setting(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual ~CParticle_Setting() = default;

public:
    static  CParticle_Setting* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual HRESULT Initialize() override;
    virtual void    Update(_float fDeltaTime) override;
    virtual HRESULT    Render() override;
    virtual void    Free() override;

private:
    _uint   m_iSelectMeshParticle = { 0 };
    _uint   m_iLevel = { 0 };
    CParticle::PARTICLE_DATA m_tParticleData;
    CMeshEffect::MESH_DATA m_tMeshData;

    vector<string> m_ImageFiles;
    vector<string> m_ShaderFiles;
    vector<string> m_ModelFiles;
    vector<string> m_ModelFilePaths;
    vector<CTexture*> m_pTextures;
    vector<ID3D11ShaderResourceView*>	m_SRVs;
    vector<CParticle*> m_pParticles;
    vector<CMeshEffect*> m_pMeshs;
    _uint       m_iSelectParticle;
    _uint       m_iSelectMesh = { 0 };
    _uint       m_iSelectModel = { 0 };
    _uint       m_iImageType = { 0 };
    _uint       m_iSelectSize = { 0 };
    _int       m_iShaderBegine = { 0 };
    string      m_szCS;
private:

public:
    HRESULT Save_Binary(const _tchar* pFilePath);
    HRESULT Load_Binary(const _tchar* pFilePath);
};

NS_END
