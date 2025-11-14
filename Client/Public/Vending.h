#pragma once
#include "Actor.h"

NS_BEGIN(Engine)
class CModel;
NS_END

NS_BEGIN(Client)
class CInteraction_Component;

class CVending : public CActor
{
protected:
	CVending(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVending(const CVending& Prototype);
	virtual ~CVending() = default;

public:
	virtual HRESULT					Initialize_Prototype() override;
	virtual HRESULT					Initialize(void* pArg) override;

	virtual void					Priority_Update(_float fTimeDelta) override;
	virtual void					Update(_float fTimeDelta) override;
	virtual void					Late_Update(_float fTimeDelta) override;

	virtual HRESULT					Render() override;

private:
	CModel*							m_pModelCom = { nullptr };
	CInteraction_Component*			m_pInteractionCom = { nullptr };

private:
	HRESULT							ADD_Components(const ACTOR_DESC& Desc);
	HRESULT							Bind_ShaderResources();

public:
	static		CVending*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual		CGameObject*		Clone(void* pArg) override;
	virtual		void				Free() override;
};
NS_END