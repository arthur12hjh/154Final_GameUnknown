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
class CEffect : public CGameObject
{
public:
    typedef struct tagEffectTransform : public CTransform::TRANSFORM_DESC
    {
        _vector vPos;
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

private:
    HRESULT Load_Binary(const _char* szFile);
    _char* ReadString(ifstream& fileBinaryStream);
    _int    ReadInt(ifstream& fileBinaryStream);
    _float4 ReadFloat4(ifstream& fileBinaryStream);
    _float3 ReadFloat3(ifstream& fileBinaryStream);
    _float2 ReadFloat2(ifstream& fileBinaryStream);
    _bool   ReadBool(ifstream& fileBinaryStream);
private:
    vector<CMeshEffect*> m_pMeshEffects;
    vector<CPointParticle*> m_pPointParticles;
public:
    static  CEffect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _char* szFile);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void					Free() override;
};

NS_END
