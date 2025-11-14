#pragma once
#include "Client_Defines.h"
#include "PartObject.h"

NS_BEGIN(Engine)
class CModel;
class CShader;
class CCollider;
NS_END

NS_BEGIN(Client)
class CGorilla_Body final : public CPartObject
{
private:
	CGorilla_Body(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGorilla_Body(const CGorilla_Body& Prototype);
	virtual ~CGorilla_Body() = default;

public:
	virtual HRESULT					Initialize_Prototype() override;
	virtual HRESULT					Initialize(void* pArg) override;

	virtual void					Priority_Update(_float fTimeDelta) override;
	virtual void					Update(_float fTimeDelta) override;
	virtual void					Late_Update(_float fTimeDelta) override;

	virtual HRESULT					Render() override;
	virtual HRESULT					Render_Shadow() override;

	_bool							isFinish_Att();

private:
	CCollider*						m_pColliderCom = { nullptr };

private:
	const _uint*					m_pParentState = { nullptr };
	_bool							m_isAnimFinish = { false };

private:
	HRESULT							Ready_Components();
	HRESULT							Bind_ShaderResources();

public:
	static CGorilla_Body*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*			Clone(void* pArg) override;
	virtual void					Free() override;
};

NS_END