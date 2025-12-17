#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CVIBuffer_Point_Instance;
class CShader;
class CComputeShader;
class CTransform;
class CTexture;
NS_END

NS_BEGIN(Client)

class CMeshEffect;
class CPointParticle;
class CSpriteParticle;
class CSpriteUVEffect;
class CEffect : public CGameObject
{
public:
    typedef struct tagEffectTransform : public CTransform::TRANSFORM_DESC
    {
        const   _float4x4* pWorldMatrix = { nullptr };
        const   _float4x4* pRootMatrix = { nullptr };
        const   _float3*   pDir = { nullptr };
        _vector vPos;
        _float3 fRot;
        _float fSize;
        _int   iFloor;
    }EFFECT_TRANSFORM_DESC;
    
private:
    CEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CEffect(const CEffect& Prototype);
    virtual ~CEffect() = default;

public:
    virtual HRESULT Initialize_Prototype(const _char* szFile);
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Update(_float fTimeDelta) override;
    virtual void Update(_float fTimeDelta) override;
    virtual void Late_Update(_float fTimeDelta) override;
    virtual HRESULT Render() override;
    void    Stop();
    void    Play(_float fTime = -10);
    void    End();

private:
    HRESULT Load_Binary(const _char* szFile);
    _char* ReadString(ifstream& fileBinaryStream);
    _int    ReadInt(ifstream& fileBinaryStream);
    RENDER    ReadRENDER(ifstream& fileBinaryStream);
    _float4 ReadFloat4(ifstream& fileBinaryStream);
    _float3 ReadFloat3(ifstream& fileBinaryStream);
    _float2 ReadFloat2(ifstream& fileBinaryStream);
    _float  ReadFloat(ifstream& fileBinaryStream);
    _int2   ReadInt2(ifstream& fileBinaryStream);
    _bool   ReadBool(ifstream& fileBinaryStream);

private:
    const   _float4x4* m_pParentWorldMat = { nullptr };
    const   _float4x4* m_pParentMat = { nullptr };
    _float4x4	        m_CombinedWorldMatrix = {};
    _float	        m_fStopTime = {};
    _int           m_iFloor = {};
    vector<CMeshEffect*> m_pMeshEffects;
    vector<CPointParticle*> m_pPointParticles;
    vector<CSpriteParticle*> m_pSpriteParticles;
    vector<CSpriteUVEffect*> m_pSpriteEffects;

public:
    static  CEffect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _char* szFile);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void					Free() override;
};

NS_END
