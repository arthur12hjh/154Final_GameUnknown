#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class CGameInstance;
class CGameObject;

class ENGINE_DLL CComponent abstract : public CBase
{
protected:
	CComponent(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CComponent(const CComponent& Prototype);
	virtual ~CComponent() = default;

public:
	virtual HRESULT			Initialize_Prototype();
	virtual HRESULT			Initialize(void* pArg);
	virtual HRESULT			Render() { return S_OK; }

	void					SetOwner(CGameObject* pGameObject) {
		m_pOwner = pGameObject; 
	}

	CGameObject*			GetOwner() { return m_pOwner; }

protected:
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };

	CGameInstance*			m_pGameInstance = { nullptr };
	CGameObject*			m_pOwner = nullptr;

	_bool					m_isCloned = { false };

public:
	virtual CComponent*		Clone(void* pArg) = 0;
	virtual void			Free();

};

NS_END