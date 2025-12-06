#pragma once 
#include "Client_Defines.h"
#include "ClientStruct.h"

#include "Actor.h"

NS_BEGIN(Engine)
class CVIBuffer_Instance_Model;
class CCollider;
NS_END

NS_BEGIN(Client)
class CInstance_Desert : public CActor
{
private:
	CInstance_Desert(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CInstance_Desert(const CInstance_Desert& Prototype);
	virtual ~CInstance_Desert() = default;

public:
	_uint				Get_NumInstance() const;
	VTX_INSTANCE_MODEL* Get_InstanceVertices() const;

public:
	virtual HRESULT				Initialize_Prototype() override;
	virtual HRESULT				Initialize(void* pArg) override;

	virtual void				Priority_Update(_float fTimeDelta) override;
	virtual void				Update(_float fTimeDelta) override;
	virtual void				Late_Update(_float fTimeDelta) override;

	virtual HRESULT				Render() override;

private:
	CVIBuffer_Instance_Model*	m_pModelCom = { nullptr };
	CVIBuffer_Instance_Model* m_pModelCom = { nullptr };
	CCollider* m_pColliderCom = { nullptr };

private:
	HRESULT						Ready_Components(const _tchar* PrototypeTag);
	HRESULT						Bind_ShaderResources();
	HRESULT						Update_Instancing_Buffer(const MODEL_INSTANCE_LOAD_DESC* pDesc);

public:
	static CInstance_Desert* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void				Free() override;
};
NS_END