#pragma once

#include "Base.h"

NS_BEGIN(Engine)
class CCollider;
class CGameInstance;

class ENGINE_DLL CLight final : public CBase
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

	void					SetDead(_bool bIsDead);
	_bool					IsDead() { return m_bIsDead; }

	HRESULT					Render(class CShader* pShader, class CVIBuffer* pVIBuffer);

	void					SetVisibility(VISIBILITY eVisibility);
	VISIBILITY				GetVisbility() { return m_eVisible; }

#ifdef _DEBUG
	void					Debug_Render();
	const _float4x4*		GetWorldMatrix() { return &m_WorldMat; }
#endif // DEBUG

private:
#ifdef _DEBUG
	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pContext = nullptr;
	CGameInstance*			m_pGameInstance = nullptr;

	CCollider*				m_pCollider = nullptr;
	_float4x4				m_WorldMat = {};
#endif // _DEBUG

	VISIBILITY				m_eVisible = {};
	LIGHT_DESC				m_LightDesc{};
	_bool					m_bIsDead = false;

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