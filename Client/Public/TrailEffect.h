#pragma once
#include "Client_Defines.h"
#include "PartObject.h"
#include "TrailData.h"

NS_BEGIN(Engine)
class CVIBuffer_Rect;
class CTrail;
class CTexture;
NS_END

NS_BEGIN(Client)

class CTrailEffect : public CGameObject
{
private:
    CTrailEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CTrailEffect(const CTrailEffect& Prototype);
    virtual ~CTrailEffect() = default;

public:
    virtual HRESULT Initialize_Prototype(const _char* szFile);
    virtual HRESULT Initialize(void* pArg) override;
    void    Update_Trail(_fmatrix matCurrentWorld, _float fTimeDelta, _bool bMakeTrail = false);
    HRESULT Render(class CTrailData* pTrailData);

private:
    HRESULT Load_Binary(const _char* szFile);
    _char*  ReadString(ifstream& fileBinaryStream);
    _int    ReadInt(ifstream& fileBinaryStream);
    _float4 ReadFloat4(ifstream& fileBinaryStream);
    _float2 ReadFloat2(ifstream& fileBinaryStream);
    void    Set_Color(_float4 fColor);
private:
    vector<CTrailData*> m_pTrailDatas = {};
    vector< CTrailData::TRAIL_DATA> m_tDatas = {};
    CShader* m_pShaderCom = { nullptr };
    CTrail* m_pTrail = { nullptr };
    _float    m_fTime;
private:
    HRESULT							Ready_Components(void* pArg);
    HRESULT							Bind_ShaderResources(CTrailData::TRAIL_DATA tData);

public:
    static  CTrailEffect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _char* szFile);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void					Free() override;
};

NS_END