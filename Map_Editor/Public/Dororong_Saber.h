#pragma once
#include "Maptool_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CModel;
class CShader;
class CVIBuffer_Cube;
class CCollider;
class CTexture;
NS_END

NS_BEGIN(Tool_Map)
class CDororong_Saber : public CGameObject
{
protected:
	CDororong_Saber(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDororong_Saber(const CDororong_Saber& Prototype);
	virtual ~CDororong_Saber() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* pArg) override;

	virtual void			Priority_Update(_float fTimeDelta) override;
	virtual void			Update(_float fTimeDelta) override;
	virtual void			Late_Update(_float fTimeDelta) override;

	virtual HRESULT			Render() override;

protected:
	class CCollider*		m_pCollider = { nullptr };
	class CShader*			m_pShaderCom = { nullptr };
	class CVIBuffer_Cube*	m_pVIBufferCom = { nullptr };
	class CTexture*			m_pTextureCom = { nullptr };

protected:
	_float					m_fColorWeight = { 0.f };
	_float					m_fTimeAcc = { 0.f };
	_bool					m_bIsColorChange = { false };

public:
	static CDororong_Saber* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void			Free() override;
};

NS_END