#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CShader;
class CVIBuffer;
class CRigidBody;
NS_END

NS_BEGIN(Client)
class CActor abstract : public CGameObject
{
public :
	typedef struct ActorDesc : public GAMEOBJECT_DESC
	{
		const WCHAR* szVIBuffer_PrototypeName;
	}ACTOR_DESC;

protected :
	CActor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CActor(const CActor& Prototype);
	virtual ~CActor() = default;

public:
	virtual HRESULT				Initialize_Prototype() override;
	virtual HRESULT				Initialize(void* pArg) override;

	virtual void				Priority_Update(_float fTimeDelta) override;
	virtual void				Update(_float fTimeDelta) override;
	virtual void				Late_Update(_float fTimeDelta) override;

	virtual HRESULT				Render() override;

	const _tchar*				Get_ComponentTag() const { return m_ComponentTag; }


protected :
	CShader*					m_pShaderCom = { nullptr };
	CRigidBody*					m_pRigidBody = { nullptr };
	_tchar						m_ComponentTag[256] = {};

public:
	virtual CGameObject*		Clone(void* pArg) override;
	virtual void				Free() override;

};
NS_END