#pragma once
#include "Prob_Interaction.h"

NS_BEGIN(Engine)
class CModel;
NS_END

NS_BEGIN(Client)
class CStaticInteraction final : public CProb_Interaction
{
private:
	CStaticInteraction(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CStaticInteraction(const CStaticInteraction& Prototype);
	virtual ~CStaticInteraction() = default;

public:
	virtual HRESULT						Initialize_Prototype() override;
	virtual HRESULT						Initialize(void* pArg) override;

	virtual void						Priority_Update(_float fTimeDelta) override;
	virtual void						Update(_float fTimeDelta) override;
	virtual void						Late_Update(_float fTimeDelta) override;

	virtual HRESULT						Render() override;

private:
	CModel*								m_pModelCom = { nullptr };

private:
	HRESULT								ADD_Components(const PROB_INTERACTION_DESC& Desc);
	HRESULT								Ready_COL(const PROB_INTERACTION_DESC& Desc);
	HRESULT								Bind_ShaderResources();

	virtual HRESULT						Begin_OverlapCallBack() override;
	virtual HRESULT						End_OverlapCallBack() override;
	virtual void						Excute_CallBack(_float fTimeDelta, CGameObject* pActionObject) override;

public:
	static		CStaticInteraction*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual		CGameObject*			Clone(void* pArg) override;
	virtual		void					Free() override;
};
NS_END