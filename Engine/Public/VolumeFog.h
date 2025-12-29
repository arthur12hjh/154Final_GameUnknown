#pragma once

#include "Deferred.h"

NS_BEGIN(Engine)

/*
거리기반 안개,
실제 볼륨이 존재하는 3D 텍스쳐 기반 노이즈안개

2개 다 기능으로 제공할 것.
*/

class CVolumeFog final : public CDeferred
{
private:
	CVolumeFog(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CVolumeFog() = default;

public:
	virtual void* Get_Desc() override;
	virtual void Set_Desc(void* pArg) override;

public:
	virtual HRESULT Initialize() override;
	virtual HRESULT Add_RenderObject(class CGameObject* pRenderObject) override { return S_OK; };

	void Update(_float fTimeDelta);
	virtual HRESULT Render(class CVIBuffer_Rect* pVIBuffer) override;
	virtual HRESULT Bind_RenderTarget(class CShader* pShader, const _char* pConstantName) override;
#ifdef _DEBUG
	virtual HRESULT Ready_Debug(_float fX, _float fY, _float fSizeX, _float fSizeY) override;
	virtual HRESULT Render_Debug(class CVIBuffer_Rect* pVIBuffer, class CShader* pShader) override;
#endif

private:
	ID3D11SamplerState* m_pNoiseSampler = { nullptr };
	ID3D11Texture3D* m_pVolumeTexture = { nullptr };
	ID3D11ShaderResourceView* m_pVolumeSRV = { nullptr };
	ID3D11UnorderedAccessView* m_pVolumeUAV = { nullptr };

	_float3     m_vVolumeTextureSize = { 240, 135, 128 };

	class CTexture* m_pNoiseTextureCom = { nullptr };
	class CComputeShader* m_pInScatterShader = { nullptr };
	class CComputeShader* m_pAccumulateScatteringShader = { nullptr };
	
	ID3D11Buffer* m_pInscatterBuffer = { nullptr };
	ID3D11Buffer* m_pFogDescBuffer = { nullptr };

	_float m_fExposure = {};
	_float m_fDepthPackExponent = {};
	_float m_fNearPlaneDist = {};
	_float m_fFarPlaneDist = {};
	_float m_fNoiseTimeAcc = { };
	_bool  m_isActive = { true };

	INSCATTER_DESC m_tInscatterDesc = {};
	VOLUMEFOG_COMMON_DESC m_tVolumeFogCommonDesc = {};
	VOLUMEFOG_DESC m_tVolumeFogDesc = {};

private:
	HRESULT Ready_Texture();
	HRESULT Ready_ConstantBuffer();

public:
	static CVolumeFog* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

NS_END