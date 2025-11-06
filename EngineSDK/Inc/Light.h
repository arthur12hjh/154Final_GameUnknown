#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class CLight final : public CBase
{
private:
	CLight();
	virtual ~CLight() = default;

public:
	void			  SetLightInfo(const LIGHT_DESC& Desc) { m_LightDesc = Desc; }
	const LIGHT_DESC* Get_LightDesc() const {
		return &m_LightDesc;
	}

public:
	HRESULT Initialize(const LIGHT_DESC& LightDesc);	
	HRESULT Render(class CShader* pShader, class CVIBuffer* pVIBuffer);

private:
	LIGHT_DESC			m_LightDesc{};

public:
	static CLight* Create(const LIGHT_DESC& LightDesc);
	virtual void Free() override;
};

NS_END