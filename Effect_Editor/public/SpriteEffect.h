#pragma once

#include "Tool_Effect_Defines.h"
#include "BlendObject.h"

NS_BEGIN(Engine)
class CVIBuffer_Rect;
class CTexture;
class CShader;
NS_END

NS_BEGIN(Tool_Effect)

class CSpriteEffect final : public CBlendObject
{
public:
	typedef struct SpriteEffectData
	{
		string				szMaskTexture;
		string				szDiffuseTexture;
		string				szNormalTexture;
		_float4				fPosition;
		_float4				fColor;

		_float2				fSize;
		_int2				iUV;
		_float				fFPS;
		_float				fAngle;
		_float				fDelayTime;

		_int				iBegin;
		RENDER				eSelectRender;
		_bool				bisLoop;
	}SPRITE_DATA;
private:
	CSpriteEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSpriteEffect(const CSpriteEffect& Prototype);
	virtual ~CSpriteEffect() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	SPRITE_DATA	Get_Data() { return m_tData; }
	void	Update(SPRITE_DATA tData);
	void	Set_Components(SPRITE_DATA tData);
	HRESULT	Set_Texture(_int iIndex, const char* szPrototype);
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
	_uint			m_iCount = {};
private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CSpriteEffect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END