#pragma once

#include "Base.h"

NS_BEGIN(Engine)
class CCollider;

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
	HRESULT					Initialize(const LIGHT_DESC& LightDesc);	
	HRESULT					Render(class CShader* pShader, class CVIBuffer* pVIBuffer);

#ifdef _DEBUG
	void					Debug_Render();
#endif // DEBUG

private:
#ifdef _DEBUG
	CCollider*				m_pCollier = nullptr;
	_float4x4				m_WorldMat = {};
#endif // _DEBUG

	LIGHT_DESC				m_LightDesc{};

public:
	static CLight*			Create(const LIGHT_DESC& LightDesc);
	virtual void			Free() override;
};

NS_END