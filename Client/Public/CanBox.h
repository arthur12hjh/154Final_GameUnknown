#pragma once
#include "Prob_Interaction.h"

NS_BEGIN(Engine)
class CModel;
NS_END

NS_BEGIN(Client)
class CCanBox final : public CProb_Interaction
{
public:
	enum class BOX_STATE { LOCK, UNLCOK, OPEN, END };

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
	BOX_STATE						m_eState = { BOX_STATE::LOCK };

private:
	HRESULT							ADD_Components(const ACTOR_DESC& Desc);
	HRESULT							Bind_ShaderResources();

	HRESULT							Begin_OverlapCallBack();
	void							Excute_CallBack();
	HRESULT							End_OverlapCallBack();

public:
	static		CCanBox*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual		CGameObject*		Clone(void* pArg) override;
	virtual		void				Free() override;
};
NS_END
	