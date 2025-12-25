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

class CSciFi_Door final : public CProb_Interaction
{
public:
	enum class SCIFI_DOOR_STATE { OPEN, CLOSE, END };

private:
	CSciFi_Door(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSciFi_Door(const CSciFi_Door& Prototype);
	virtual ~CSciFi_Door() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	SCIFI_DOOR_STATE			Get_DoorState() const { return m_eCurState; }

private:
	SCIFI_DOOR_STATE			m_eCurState = { SCIFI_DOOR_STATE::END };

private:
	CModel*						m_pModelCom = { nullptr };
	CUIActionEvent*				m_pDoorEvent = { nullptr };
	CUIActionEvent*				m_pUnlockEvent = { nullptr };
	_bool						m_bUnlocked = false;
	_bool						m_bCanlock = false;

	_tchar						m_szComponentTag[256] = {};

private:
	HRESULT						Ready_Components(const _tchar* pComponentTag);
	HRESULT						Ready_Col(const _tchar* pComponentTag);
	HRESULT						Bind_ShaderResources();

	virtual void				Excute_CallBack(_float fTimeDelta, CGameObject* pActionObject) override;

public:
	static CSciFi_Door* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END