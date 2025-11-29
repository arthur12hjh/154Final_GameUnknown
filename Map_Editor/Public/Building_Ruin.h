#pragma once

#include "Maptool_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CModel;
class CShader;
NS_END

NS_BEGIN(Tool_Map)

class CBuilding_Ruin final : public CGameObject
{
public:
	const _tchar* Get_ComponentTag() const { return m_ComponentTag; }

private:
	CBuilding_Ruin(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBuilding_Ruin(const CBuilding_Ruin& Prototype);
	virtual ~CBuilding_Ruin() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CModel* m_pModelCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };

	const _tchar* m_ComponentTag = {};

private:
	HRESULT Ready_Components(const _tchar* pComponentTag);
	HRESULT Bind_ShaderResources();

public:
	static CBuilding_Ruin* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END