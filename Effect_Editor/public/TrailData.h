#pragma once

#include "Tool_Effect_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CVIBuffer_Rect;
class CTexture;
class CShader;
class CTrail;
NS_END

NS_BEGIN(Tool_Effect)

class CTrailData final : public CGameObject
{
public:
	typedef struct TrailData
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
		_int	iSelectRender;
	}TRAIL_DATA;

private:
	CTrailData(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTrailData(const CTrailData& Prototype);
	virtual ~CTrailData() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_MotionBlur() override;
	void	Set_Components(TRAIL_DATA tData);
	void	Update(TRAIL_DATA tData);
	TRAIL_DATA	Get_Data() { return m_tData; }
	HRESULT	Set_Texture(_int iIndex, const char* szPrototype);
	HRESULT	Bind_Texture(CShader* pShader);
private:
	CTexture*			m_pTexture[3] = {};
	TRAIL_DATA			m_tData;
	_uint				m_iCount = {};
	RENDER				m_eRender = {};
	class CTrailEffect* m_pTrail;
	_uint				m_iRenderCount = {};

	ID3D11DepthStencilView* m_pOriginalDSV = { nullptr };
	ID3D11Resource* m_resourse = { nullptr };
	ID3D11ShaderResourceView* m_pRSV = { nullptr };

private:
	HRESULT							Ready_Components();
	HRESULT							Bind_ShaderResources();

public:
	static CTrailData* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void					Free() override;
};

NS_END