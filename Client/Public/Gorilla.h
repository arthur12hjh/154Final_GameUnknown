#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CModel;
class CShader;
class CCollider;
NS_END

NS_BEGIN(Client)

class CGorilla final : public CGameObject
{
private:
	CGorilla(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGorilla(const CGorilla& Prototype);
	virtual ~CGorilla() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* pArg) override;

	virtual void			Priority_Update(_float fTimeDelta) override;
	virtual void			Update(_float fTimeDelta) override;
	virtual void			Late_Update(_float fTimeDelta) override;

	virtual HRESULT			Render() override;

private:
	CModel*					m_pModelCom = { nullptr };	
	CShader*				m_pShaderCom = { nullptr };
	CCollider*				m_pColliderCom[ENUM_CLASS(COLLIDER::END)] = {nullptr};

	_int					m_iIndex = {};
	_uint					m_iMaxIndex = {};

private:
	HRESULT					Ready_Components();
	HRESULT					Bind_ShaderResources();

public:
	static CGorilla*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

NS_END