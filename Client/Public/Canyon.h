#pragma once

#include "Client_Defines.h"
#include "ClientStruct.h"

#include "Actor.h"

NS_BEGIN(Engine)
class CModel;
class CCollider;
class CCamera;
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

	virtual HRESULT Render_Shadow() override;

private :
	CModel* m_pModelCom = { nullptr };

	_bool m_bIsOccluder = true;
	_bool m_bRender = false;
private:
	HRESULT Ready_Components(const _tchar* pComponentTag);
	HRESULT Ready_Col(const _tchar* pComponentTag);
	HRESULT Bind_ShaderResources();

	void	SetCullingCollider(_uint iObjectID);

public:
	static CCanyon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END