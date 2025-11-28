#pragma once

#include "Maptool_Defines.h"
#include "StaticMap.h"

NS_BEGIN(Engine)
class CModel;
class CShader;
NS_END

NS_BEGIN(Tool_Map)

class CRock final : public CStaticMap
{
private:
	CRock(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRock(const CRock& Prototype);
	virtual ~CRock() = default;

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

private:
	HRESULT Ready_Components(const _tchar* pModelComponentTag);
	HRESULT Bind_ShaderResources();

public:
	static CRock* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CStaticMap* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END