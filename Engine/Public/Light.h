#pragma once

#include "Base.h"

NS_BEGIN(Engine)
class CCollider;

class CLight final : public CBase
{
private:
#ifdef _DEBUG
	CLight(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
#elif
	CLight();
#endif // _DEBUG
	
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
	const _float4x4*		GetWorldMatrix() { return &m_WorldMat; }
#endif // DEBUG

private:
#ifdef _DEBUG
	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pContext = nullptr;

	CCollider*				m_pCollider = nullptr;
	_float4x4				m_WorldMat = {};
#endif // _DEBUG

	LIGHT_DESC				m_LightDesc{};

private :
#ifdef _DEBUG
	HRESULT					CreateDebugCollider();
#endif // _DEBUG

public:
#ifdef _DEBUG
	static CLight* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const LIGHT_DESC& LightDesc);
#elif
	static CLight* Create(const LIGHT_DESC& LightDesc);
#endif // _DEBUG


	virtual void			Free() override;
};

NS_END