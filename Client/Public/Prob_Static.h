#pragma once
#include "Actor.h"

NS_BEGIN(Engine)
class CModel;
NS_END

NS_BEGIN(Client)
class CProb_Static final : public CActor
{
private:
	CProb_Static(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CProb_Static(const CProb_Static& Prototype);
	virtual ~CProb_Static() = default;

public:
	virtual HRESULT					Initialize_Prototype() override;
	virtual HRESULT					Initialize(void* pArg) override;

	virtual void					Priority_Update(_float fTimeDelta) override;
	virtual void					Update(_float fTimeDelta) override;
	virtual void					Late_Update(_float fTimeDelta) override;

	virtual HRESULT					Render() override;

private:
	CModel*							m_pModelCom = { nullptr };

private:
	HRESULT							ADD_Components(const ACTOR_DESC& Desc);
	HRESULT							Bind_ShaderResources();

public:
	static		CProb_Static*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual		CGameObject*		Clone(void* pArg) override;
	virtual		void				Free() override;

};
NS_END