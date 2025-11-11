#pragma once
#include "Client_Defines.h"
#include "UIObject.h"

NS_BEGIN(Engine)
class CVIBuffer;
class CTexture;
class CShader;
NS_END

NS_BEGIN(Client)
class CMousePointer : public CUIObject
{
public :
	enum class MOUSE_SCALE { SCLAE_X06, SCLAE_DEFUALT, SCLAE_X1_3, SCLAE_X2, SCLAE_END };

private:
	CMousePointer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMousePointer(const CMousePointer& Prototype);
	virtual ~CMousePointer() = default;

public:
	virtual HRESULT				Initialize_Prototype() override;
	virtual HRESULT				Initialize(void* pArg) override;

	virtual void				Priority_Update(_float fTimeDelta) override;
	virtual void				Update(_float fTimeDelta) override;
	virtual void				Late_Update(_float fTimeDelta) override;

	virtual HRESULT				Render() override;

private:
	CVIBuffer*					m_pVIBufferCom = { nullptr };
	CTexture*					m_pTextureCom = { nullptr };
	CShader*					m_pShaderCom = { nullptr };

	MOUSE_SCALE					m_eMosueScale = {};

private:
	HRESULT						Ready_Components();
	HRESULT						Bind_ShaderResources();

public:
	static CMousePointer*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg) override;
	virtual void				Free() override;

};
NS_END