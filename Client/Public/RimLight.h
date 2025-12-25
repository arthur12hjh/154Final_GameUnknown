#pragma once
#include "Client_Defines.h"
#include "Component.h"

NS_BEGIN(Engine)
class CShader;
NS_END 

//림라이트 조금이나마 더 편하게 쓰라고 만든 컴포넌트

NS_BEGIN(Client)
class CRimLight final : public CComponent
{
private:
	CRimLight(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CRimLight() = default;

public:
	virtual HRESULT					Initialize_Prototype();
	virtual HRESULT					Initialize(void* pArg);

	void Set_Color(_float4 vColor) { m_tRimLightDesc.vRimLightColor = vColor; }
	void Set_Power(_float fPower) { m_tRimLightDesc.fRimLightPower = fPower; }
	void Set_Intensity(_float fIntensity) { m_tRimLightDesc.fRimLightIntensity = fIntensity; }
	void Set_RimLightDesc(RIMLIGHT_DESC tDesc) { m_tRimLightDesc = tDesc; }

public:
	HRESULT Bind_RimLightShaderResources(CShader* pShader, 
	const _char* pRimColorTag = nullptr, const _char* pRimPowerTag = nullptr,
	const _char* pRimLightIntensity = nullptr, const _char* pCamPositionTag = nullptr);

private:
	//_float4		vRimLightColor = { 1.f, 1.f, 1.f, 1.f };
	//_float		fRimLightPower = 2.f;
	//_float		fRimLightIntensity = 1.f;
	RIMLIGHT_DESC m_tRimLightDesc;

public:
	static		CRimLight*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual		CComponent*			Clone(void* pArg) override;
	virtual		void				Free() override;

};
NS_END