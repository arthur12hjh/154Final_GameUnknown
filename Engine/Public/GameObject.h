#pragma once

#include "Transform.h"

NS_BEGIN(Engine)
class CTransform;

class ENGINE_DLL CGameObject abstract : public CBase
{
public:
	typedef struct tagGameObjectDesc : public CTransform::TRANSFORM_DESC
	{

	}GAMEOBJECT_DESC;

protected:
	CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject(const CGameObject& Prototype);
	virtual ~CGameObject() = default;

public:
	virtual HRESULT		Initialize_Prototype();
	virtual HRESULT		Initialize(void* pArg);

	virtual void		Priority_Update(_float fTimeDelta);
	virtual void		Update(_float fTimeDelta);
	virtual void		Late_Update(_float fTimeDelta);

	virtual HRESULT		Render();
	virtual HRESULT		Render_Shadow() { return S_OK; }

public:
	_bool isDead() const {
		return m_isDead;
	}

	class CComponent*			Find_Component(const _wstring& strComponentTag);

	void						SetVisibility(VISIBILITY eVisiblility) {
		m_eVisibility = eVisiblility;
	}

	const VISIBILITY&			GetVisibility() { return m_eVisibility;	}
	const	CTransform*			GetTransform() { return m_pTransformCom; }


protected:
	int							m_iObjectID;
	ID3D11Device*				m_pDevice = { nullptr };
	ID3D11DeviceContext*		m_pContext = { nullptr };
	class CGameInstance*		m_pGameInstance = { nullptr };
	VISIBILITY					m_eVisibility = { VISIBILITY::VISIBLE };

	CTransform*					m_pTransformCom = { nullptr };
	_bool						m_isDead = { false };

	map<const _wstring, class CComponent*>		m_Components;

protected:
	HRESULT Add_Component(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, 
		const _wstring& strComponentTag, CComponent** ppOut, void* pArg = nullptr);

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

NS_END