#pragma once
#include "ImgBase.h"

NS_BEGIN(Engine)
class CVIBuffer_Point_Instance;
class CShader;
class CTransform;
class CTexture;
NS_END

NS_BEGIN(Tool_Effect)

class CParticle_Setting : public CImgBase
{
public:
    struct DiagramData
    {
        _float fTime;
        _float fX;
        _float fValue;
    };
    struct ShaderData {
        wstring     szShaderTag;
        vector<wstring> TextureTags;
    };
    struct ParticleData {
        _float      fDuration;
        _bool       bisLoop;
        _float2     fStartDelay;
        _float2     fStartLifeTime;
        _float2     fStartSize;
        vector<DiagramData> SizeDiagrams;
        _float      fGravity;
        _uint       iMaxParticles;
        _uint       iRateOverTime;
        _uint       iShape;
        _vector     vPosition;
        _vector     vRotation;
        _vector     vScale;


        _uint       iParticleType;

        ShaderData  shaderData;
        vector<ParticleData> ParticleDatas;
    };
private:
    CParticle_Setting(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual ~CParticle_Setting() = default;

public:
    static  CParticle_Setting* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual HRESULT Initialize() override;
    virtual void    Update(_float fDeltaTime) override;
    virtual HRESULT    Render() override;
    virtual void    Free() override;
    CShader* m_pShaderCom = { nullptr };
    CVIBuffer_Point_Instance* m_pVIBufferCom;
    class CTransform* m_pTransformCom = { nullptr };

private:
    _uint   m_iLevel = { 0 };
    _int   m_iNumInstance;
    _float3 m_fCenter;
    _float3 m_fRange;
    _float2 m_fSize;
    _float2 m_fLifeTime;
    _float2 m_fSpeed;
    _bool m_bisLoop;

    _float4 m_fColor = {};

    vector<string> m_ImageFiles;
    vector<string> m_ShaderFiles;
    vector<CTexture*> m_pTextures;
    vector<ID3D11ShaderResourceView*>	m_SRVs;
    CTexture* m_pTexture[3] = {};
    string      szFile[3];
    _uint       m_iImageType = { 0 };
    _int       m_iShaderBegine = { 0 };
private:

public:
    HRESULT Save_Binary(const _tchar* pFilePath);
    HRESULT Load_Binary(const _tchar* pFilePath);
};

NS_END
