#pragma once
#include "Maptool_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CModel;
class CShader;
class CVIBuffer_Cube;
class CCollider;
NS_END

NS_BEGIN(Tool_Map)
class CPad : public CGameObject
{
private:
	CPad(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPad(const CPad& Prototype);
	virtual ~CPad() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* pArg) override;

	virtual void			Priority_Update(_float fTimeDelta) override;
	virtual void			Update(_float fTimeDelta) override;
	virtual void			Late_Update(_float fTimeDelta) override;

	virtual HRESULT			Render() override;

private:
	class CCollider*		m_pCollider = { nullptr };
	class CShader*			m_pShaderCom = { nullptr };
	class CVIBuffer_Cube*	m_pVIBufferCom = { nullptr };

private:
	_float					m_fColorWeight = { 0.f };
	_float					m_fColorTime = { 0.5f };
	_float					m_fTimeAcc = { 0.f };
	_bool					m_bIsColorChange = { false };

private:
	HRESULT					Ready_Components();
	HRESULT					Bind_ShaderResources();

public:
	static CPad* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void			Free() override;
};

NS_END