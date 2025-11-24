#pragma once
#include "ImgBase.h"
#include "Particle.h"
#include "MeshEffect.h"
#include "SpriteEffect.h"
#include "TrailEffect.h"
#include "SpriteParticle.h"

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
    void    WriteString(ofstream& fileBinaryStream, _char* pStr);
    void    WriteRENDER(ofstream& fileBinaryStream, RENDER vTmp);
    void    WriteInt(ofstream& fileBinaryStream, _int vTmp);
    void    WriteFloat4(ofstream& fileBinaryStream, _float4 vTmp);
    void    WriteFloat3(ofstream& fileBinaryStream, _float3 vTmp);
    void    WriteFloat2(ofstream& fileBinaryStream, _float2 vTmp);
    void    WriteFloat(ofstream& fileBinaryStream, _float vTmp);
    void    WriteInt2(ofstream& fileBinaryStream, _int2 vTmp);
    void    WriteBool(ofstream& fileBinaryStream, _bool vTmp);
    _char*  ReadString(ifstream& fileBinaryStream);
    RENDER    ReadRENDER(ifstream& fileBinaryStream);
    _int    ReadInt(ifstream& fileBinaryStream);
    _float4 ReadFloat4(ifstream& fileBinaryStream);
    _float3 ReadFloat3(ifstream& fileBinaryStream);
    _float2 ReadFloat2(ifstream& fileBinaryStream);
    _float  ReadFloat(ifstream& fileBinaryStream);
    _int2   ReadInt2(ifstream& fileBinaryStream);
    _bool   ReadBool(ifstream& fileBinaryStream);
private:
    _uint   m_iSelectMeshParticle = { 4 };
    _uint   m_iLevel = { 0 };
    _float4  m_fPosition;
    _float3  m_fScale;
    _float3  m_fRotation;
    _float  m_fTime;
    _float  m_fSpeed = { 1.f };
    CParticle::PARTICLE_DATA m_tParticleData;
    CSpriteParticle::SPRITE_PARTICLE_DATA m_tSpriteParticleData;
    CMeshEffect::MESH_DATA m_tMeshData;
    CSpriteEffect::SPRITE_DATA m_tSpriteData;
    CTrailEffect::TRAIL_DATA    m_tTrailData;
    CTransform* m_pTransform = nullptr;
    vector<string> m_ImageFiles[4];
    vector<string> m_ShaderFiles;
    vector<string> m_ModelFiles;
    vector<string> m_ModelFilePaths;
    vector<string> m_Effects;
    vector<string> m_TrailEffects;
    vector<ID3D11ShaderResourceView*>	m_SRVs[4];
    vector<CParticle*> m_pParticles;
    vector<CSpriteParticle*> m_pSpriteParticles;
    vector<CMeshEffect*> m_pMeshs;
    vector<CSpriteEffect*> m_pSprites;
    CTrailEffect*    m_pTrailEffect = nullptr;
    CTrailEffect* m_pDistortionTrailEffect = nullptr;
    _char m_SaveFile[128] = "SaveEffect";
    _int       m_iSelectParticle;
    _int       m_iSelectSpriteParticle;
    _int       m_iSelectMesh = { 0 };
    _uint       m_iSelectModel = { 0 };
    _uint       m_iImageType = { 0 };
    _uint       m_iSpriteImageType = { 1 };
    _uint       m_iSelectSize = { 0 };
    _uint       m_iSelectEffect = { 0 };
    _uint       m_iSelectTrailEffect = { 0 };
    _int       m_iShaderBegine = { 0 };
    _int       m_iSelectSprite = { 0 };
    _bool       m_bisPause = true;
    _bool       m_bisReplay = false;
    string      m_szCS;
private:
    void    Add_Particle();
    void    Delete_Particle();
    void    Add_SpriteParticle();
    void    Delete_SpriteParticle();
    void    Add_MeshEffect();
    void    Delete_MeshEffect();
    void    Add_SpriteEffect();
    void    Delete_SpriteEffect();
public:
    HRESULT Save_Binary(const _char* szFile);
    HRESULT Load_Binary(const _char* szFile);
    HRESULT Save_TrailBinary(const _char* szFile);
    HRESULT Load_TrailBinary(const _char* szFile);
};

NS_END
