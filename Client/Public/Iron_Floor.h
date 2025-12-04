#pragma once

#include "Client_Defines.h"
#include "ClientStruct.h"
#include "Actor.h"

NS_BEGIN(Engine)
class CModel;
NS_END

NS_BEGIN(Client)

class CIron_Floor final : public CActor
{
private:
	CIron_Floor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CIron_Floor(const CIron_Floor& Prototype);
	virtual ~CIron_Floor() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private :
	CModel* m_pModelCom = { nullptr };

private:
	HRESULT Ready_Components(const _tchar* pComponentTag);
	HRESULT Bind_ShaderResources();

public:
	static CIron_Floor* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END