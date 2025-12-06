#pragma once
#include "Prob_Interaction.h"

NS_BEGIN(Engine)
class CModel;
class CEventHandle;
NS_END

NS_BEGIN(Client)
class CCanBox final : public CProb_Interaction
{
private:
	CCanBox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCanBox(const CCanBox& Prototype);
	virtual ~CCanBox() = default;

public:
	virtual HRESULT					Initialize_Prototype() override;
	virtual HRESULT					Initialize(void* pArg) override;

	virtual void					Priority_Update(_float fTimeDelta) override;
	virtual void					Update(_float fTimeDelta) override;
	virtual void					Late_Update(_float fTimeDelta) override;

	virtual HRESULT					Render() override;

private:
	CModel*							m_pModelCom = { nullptr };
	CEventHandle*					m_pEventHandle = { nullptr };

private:
	HRESULT							ADD_Components(const ACTOR_DESC& Desc);
	HRESULT							Bind_ShaderResources();

	virtual HRESULT					Begin_OverlapCallBack() override;
	virtual HRESULT					End_OverlapCallBack() override;
	virtual void					Excute_CallBack(CGameObject* pActionObject) override;

public:
	static		CCanBox*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual		CGameObject*		Clone(void* pArg) override;
	virtual		void				Free() override;
};
NS_END
	