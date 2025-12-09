#pragma once

#include "Client_Defines.h"
#include "Prob_Interaction.h"

NS_BEGIN(Engine)
class CModel;
class CShader;
NS_END

NS_BEGIN(Client)

class CVendingMachine final : public CProb_Interaction
{
private:
	CVendingMachine(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVendingMachine(const CVendingMachine& Prototype);
	virtual ~CVendingMachine() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CModel* m_pModelCom = { nullptr };

private:
	HRESULT Ready_Components(const _tchar* pComponentTag);
	HRESULT Bind_ShaderResources();
	void	SetCullingCollider(_uint iObjectID);
	_uint	Object_Number(const _tchar* pComponentTag);

	virtual HRESULT					    Begin_OverlapCallBack() override;
	virtual HRESULT					    End_OverlapCallBack() override;
	virtual void					    Excute_CallBack(CGameObject* pActionObject) override;

public:
	static CVendingMachine* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END