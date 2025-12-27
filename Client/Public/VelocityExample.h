#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CGameInstance;
class CShader;
class CVIBuffer_Rect;
class CTexture;
NS_END

NS_BEGIN(Client)

class CVelocityExample final : public CGameObject
{
private:
	CVelocityExample(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVelocityExample(const CVelocityExample& Prototype);
	virtual ~CVelocityExample() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_MotionBlur() override;
private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

private:
	class CShader* m_pShaderCom = { nullptr };
	class CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	class CTexture* m_pTextureCom = { nullptr };
public:
	static CVelocityExample* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END
