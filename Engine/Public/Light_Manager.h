#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class CLight;

class CLight_Manager final : public CBase
{
private:
	CLight_Manager();
	virtual ~CLight_Manager() = default;

public:
	const LIGHT_DESC*		Get_LightDesc(_uint iIndex) const;

public:
	HRESULT					Add_Light(const LIGHT_DESC& LightDesc);
	void					Remove_Light(CLight* pLight);
	HRESULT					Render_Lights(class CShader* pShader, class CVIBuffer* pVIBuffer);

private:
	list<CLight*>				m_Lights;

public:
	static CLight_Manager*			Create();
	virtual void					Free() override;
};

NS_END