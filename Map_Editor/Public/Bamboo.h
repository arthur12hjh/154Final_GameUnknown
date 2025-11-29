#pragma once 
#include "MapTool_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CVIBuffer_Instance_Model;
class CShader;
NS_END

NS_BEGIN(Tool_Map)
class CBamboo : public CGameObject
{
private:
	CBamboo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBamboo(const CBamboo& Prototype);
	virtual ~CBamboo() = default;

public:
	virtual HRESULT				Initialize_Prototype() override;
	virtual HRESULT				Initialize(void* pArg) override;

	virtual void				Priority_Update(_float fTimeDelta) override;
	virtual void				Update(_float fTimeDelta) override;
	virtual void				Late_Update(_float fTimeDelta) override;

	virtual HRESULT				Render() override;

private:
	CVIBuffer_Instance_Model* m_pModelCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };

private:
	HRESULT						Ready_Components();
	HRESULT						Bind_ShaderResources();

public:
	static CBamboo* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void				Free() override;
};
NS_END