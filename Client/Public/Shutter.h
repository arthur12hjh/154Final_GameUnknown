#pragma once

#include "Client_Defines.h"
#include "ClientStruct.h"
#include "Actor.h"

NS_BEGIN(Engine)
class CModel;
NS_END

NS_BEGIN(Client)

class CShutter final : public CActor
{
private:
	CShutter(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CShutter(const CShutter& Prototype);
	virtual ~CShutter() = default;

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
	//HRESULT Ready_Col(const _tchar* pComponentTag);
	HRESULT Bind_ShaderResources();
	//void	SetCullingCollider(_uint iObjectID);

public:
	static CShutter* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END