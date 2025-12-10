#pragma once

#include "MapTool_Defines.h"
#include "DesertObject.h"

NS_BEGIN(Engine)
class CTexture;
class CShader;
class CModel;
NS_END

NS_BEGIN(Tool_Map)

class CSky_Desert final : public CDesertObject
{
private:
	CSky_Desert(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSky_Desert(const CSky_Desert& Prototype);
	virtual ~CSky_Desert() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CTexture* m_pTextureCom = { nullptr };

private:
	HRESULT Ready_Components(const _tchar* pComponentTag);
	HRESULT Bind_ShaderResources();

public:
	static CSky_Desert* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END