#pragma once

#include "Client_Defines.h"
#include "BlendObject.h"

NS_BEGIN(Engine)
class CVIBuffer_Rect;
class CTexture;
class CShader;
NS_END

NS_BEGIN(Client)

class CSpriteUVEffect final : public CBlendObject
{
public:
	typedef struct SpriteUVEffectData
	{
		string				szMaskTexture;
		string				szDiffuseTexture;
		string				szNormalTexture;
		_float4				fPosition;
		_float4				fColor;

		_float2				fSize;
		_int2				iUV;
		_float				fFPS;

		_int				iBegin;
		_int				iSelectRender;
		_bool				bisLoop;
	}SPRITE_DATA;
private:
	CSpriteUVEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSpriteUVEffect(const CSpriteUVEffect& Prototype);
	virtual ~CSpriteUVEffect() = default;

public:
	virtual HRESULT Initialize_Prototype(const SPRITE_DATA* pSpriteData);
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	void	Set_ParentMat(const _float4x4* pParentMat) { m_pParentMat = pParentMat; }

private:
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	CTexture* m_pTexture[3] = {};
	CShader* m_pShaderCom = { nullptr };
	const _float4x4* m_pParentMat = { nullptr };
	_float4x4		m_CombinedWorldMatrix = {};

private:
	_float				m_fTime = {  };
	SPRITE_DATA			m_tData;
private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CSpriteUVEffect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const SPRITE_DATA* pSpriteData);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END