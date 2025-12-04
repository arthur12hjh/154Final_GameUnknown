#pragma once

#include "Client_Defines.h"
#include "ClientStruct.h"

#include "Actor.h"

NS_BEGIN(Engine)
class CVIBuffer_Instance_Model;
NS_END

NS_BEGIN(Client)

class CCanyon final : public CActor
{
private:
	CCanyon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCanyon(const CCanyon& Prototype);
	virtual ~CCanyon() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private :
	CVIBuffer_Instance_Model* m_pInstanceModelCom = { nullptr };

private:
	HRESULT Ready_Components(const _tchar* pComponentTag);
	HRESULT Bind_ShaderResources();

public:
	static CCanyon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END