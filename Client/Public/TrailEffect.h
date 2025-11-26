#pragma once
#include "Client_Defines.h"
#include "PartObject.h"

NS_BEGIN(Engine)
class CVIBuffer_Rect;
class CTrail;
class CTexture;
NS_END

NS_BEGIN(Client)

class CTrailEffect : public CPartObject
{
public:
    typedef struct TrailEffectData
    {
        string	szMaskTexture = {};
        string	szDiffuseTexture = {};
        string	szDissolveTexture = {};
        _float4 fColor = {};

        _float2	fMaskUV;
        _float2	fMaskUVSpeed;
        _float2	fMaskUVSize;
        _float2	fDiffuseUV;
        _float2	fDiffuseUVSpeed;
        _float2	fDiffuseUVSize;
        _float2	fDissolveUV;
        _float2	fDissolveUVSpeed;
        _float2	fDissolveUVSize;

        _int	iBegin = {};
        RENDER	eSelectRender;
    }TRAIL_DATA;

private:
    CTrailEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CTrailEffect(const CTrailEffect& Prototype);
    virtual ~CTrailEffect() = default;

public:
    virtual HRESULT Initialize_Prototype(const _char* szFile);
    virtual HRESULT Initialize(void* pArg) override;
    void    Update_Trail(_fmatrix matCurrentWorld, _float fTimeDelta, _bool bMakeTrail = false);
    virtual HRESULT Render() override;

private:
    HRESULT Load_Binary(const _char* szFile);
    _char*  ReadString(ifstream& fileBinaryStream);
    _int    ReadInt(ifstream& fileBinaryStream);
    RENDER  ReadRENDER(ifstream& fileBinaryStream);
    _float4 ReadFloat4(ifstream& fileBinaryStream);
    _float2 ReadFloat2(ifstream& fileBinaryStream);
private:
    CShader* m_pShaderCom = { nullptr };
    CTexture* m_pTexture[3] = {};
    CTrail* m_pTrail = { nullptr };
    TRAIL_DATA  m_tData;
    _float    m_fTime;
private:
    HRESULT							Ready_Components();
    HRESULT							Bind_ShaderResources();

public:
    static  CTrailEffect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _char* szFile);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void					Free() override;
};

NS_END
