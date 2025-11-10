#pragma once
#include "ImgBase.h"

NS_BEGIN(Engine)
class CVIBuffer_Instance;
NS_END

NS_BEGIN(Tool_Effect)

class CParticle_Setting : public CImgBase
{
public:
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

private:
    _uint   m_iLevel = { 0 };
    CVIBuffer_Instance* m_pVIBufferCom;
private:

public:
    HRESULT Save_Binary(const _tchar* pFilePath);
    HRESULT Load_Binary(const _tchar* pFilePath);
};

NS_END
