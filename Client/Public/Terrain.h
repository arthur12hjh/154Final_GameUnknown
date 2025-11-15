#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)		
class CVIBuffer_Terrain;
class CNavigation;
class CTexture;
class CShader;
NS_END

NS_BEGIN(Client)

class CTerrain final : public CGameObject
{
private:
	CTerrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTerrain(const CTerrain& Prototype);
	virtual ~CTerrain() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CVIBuffer_Terrain*	m_pVIBufferCom = { nullptr };
	CNavigation*		m_pNavigationCom = { nullptr };
	CTexture*			m_pTextureCom = { nullptr };
	CTexture*			m_pMaskCom = { nullptr };
	CShader*			m_pShaderCom = { nullptr };
	
private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CTerrain* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END