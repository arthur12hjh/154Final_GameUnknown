#pragma once 
#include "Client_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CVIBuffer_Instance_Model;
class CShader;
NS_END

NS_BEGIN(Client)
class CInstance_Model : public CGameObject
{
private:
	CInstance_Model(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CInstance_Model(const CInstance_Model& Prototype);
	virtual ~CInstance_Model() = default;

public:
	virtual HRESULT				Initialize_Prototype() override;
	virtual HRESULT				Initialize(void* pArg) override;

	virtual void				Priority_Update(_float fTimeDelta) override;
	virtual void				Update(_float fTimeDelta) override;
	virtual void				Late_Update(_float fTimeDelta) override;

	virtual HRESULT				Render() override;

private:
	CVIBuffer_Instance_Model*	m_pModelCom = { nullptr };
	CShader*					m_pShaderCom = { nullptr };

private:
	HRESULT						Ready_Components();
	HRESULT						Bind_ShaderResources();

public:
	static CInstance_Model*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg) override;
	virtual void				Free() override;
};
NS_END