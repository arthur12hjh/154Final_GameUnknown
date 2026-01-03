#pragma once

#include "Client_Defines.h"
#include "StaticMap.h"

NS_BEGIN(Engine)
class CRigidBody;
class CModel;
NS_END

NS_BEGIN(Client)

class CStair final : public CStaticMap
{
private:
	CStair(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CStair(const CStair& Prototype);
	virtual ~CStair() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

	CRigidBody* m_pRigidBody = { nullptr };
	CModel* m_pColModelCom = { nullptr };
	_bool	m_bIsReady = { false };

public:
	static CStair* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CStaticMap* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END