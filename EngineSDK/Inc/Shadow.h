#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class CShadow final : public CBase
{
private:
	CShadow();
	virtual ~CShadow() = default;

public:
	HRESULT					Ready_Shadow_Light(const SHADOW_LIGHT_DESC& Desc);
	HRESULT					Bind_Shader_Resource(class CShader* pShader, const _char* pConstantName, D3DTS eType);

	const SHADOW_LIGHT_DESC&	GetCameraInfo() { return m_ShadowLight; }
	const _float4x4*			GetShadowMatrix(D3DTS eType);
	const _float4x4*			GetInverseShadowMatrix(D3DTS eType);

private:
	_float4x4				m_TransformationMatrices[ENUM_CLASS(D3DTS::END)] = {};
	_float4x4				m_TransformationMatrices_Inv[ENUM_CLASS(D3DTS::END)] = {};
	
	SHADOW_LIGHT_DESC		m_ShadowLight = {};

public:
	static CShadow* Create();
	virtual void Free() override;
};

NS_END