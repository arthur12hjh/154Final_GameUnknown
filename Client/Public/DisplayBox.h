#pragma once

#include "Client_Defines.h"
#include "Prob_Interaction.h"

NS_BEGIN(Engine)
class CModel;
class CShader;
class CCollider;
NS_END

NS_BEGIN(Client)
class CUIActionEvent;

class CDisplayBox final : public CProb_Interaction
{
public:
	enum class BOX_STATE { OPEN, IDLE };

private:
	CDisplayBox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDisplayBox(const CDisplayBox& Prototype);
	virtual ~CDisplayBox() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;


private:
	BOX_STATE					m_eCurState = { BOX_STATE::IDLE };

private:
	CModel*						m_pModelCom = { nullptr };
	_tchar						m_szComponentTag[256] = {};

private:
	HRESULT						Ready_Components(const _tchar* pComponentTag);
	//HRESULT						Ready_Col(const _tchar* pComponentTag);
	HRESULT						Bind_ShaderResources();

	//virtual void				Excute_CallBack(_float fTimeDelta, CGameObject* pActionObject) override;

public:
	static CDisplayBox* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END