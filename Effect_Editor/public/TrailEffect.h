#pragma once

#include "Tool_Effect_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CVIBuffer_Rect;
class CTexture;
class CShader;
NS_END

NS_BEGIN(Tool_Effect)
//texture2D g_MaskTexture, g_DiffuseTexture, g_DissolveTexture;
class CTrailEffect final : public CGameObject
{
public:
	typedef struct TrailEffectData
	{
		string	szMaskTexture = {};
		string	szDiffuseTexture = {};
		string	szDissolveTexture = {};
		_float4 fColor = {};

		_float2				fMaskUV;
		_float2				fMaskUVSpeed;
		_float2				fMaskUVSize;
		_float2				fDiffuseUV;
		_float2				fDiffuseUVSpeed;
		_float2				fDiffuseUVSize;
		_float2				fDissolveUV;
		_float2				fDissolveUVSpeed;
		_float2				fDissolveUVSize;

		_int	iBegin = {};
		_int	iSelectRender = {};
	}TRAIL_DATA;

private:
	CTrailEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTrailEffect(const CTrailEffect& Prototype);
	virtual ~CTrailEffect() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	void	Set_Components(TRAIL_DATA tData);
	void	Update(TRAIL_DATA tData);
	TRAIL_DATA	Get_Data() { return m_tData; }
	string	Get_TextureName(_int iIndex) { return m_szFile[iIndex]; }
	HRESULT	Set_Texture(_int iIndex, const char* szPrototype);
	void	Pause() { m_bisPause = !m_bisPause; }
private:
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTexture[3] = {};
	string    m_szFile[3];
	_float		m_fTime = {};
	_bool		m_bisPause = {};
	TRAIL_DATA	m_tData;
	_uint	m_iCount = {};

private:
	HRESULT							Ready_Components();
	HRESULT							Bind_ShaderResources();

public:
	static CTrailEffect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void					Free() override;
};

NS_END