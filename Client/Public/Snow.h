#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CVIBuffer_Rect_Instance;
class CComputeShader;

class CTexture;
class CShader;
NS_END

NS_BEGIN(Client)

class CSnow final : public CGameObject
{
public :
	struct RectConstBufferData
	{
		_float4			vGravity;
		_float2			fTimeDelta;
		_int2			iLoopAndCount;
	};

private:
	CSnow(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSnow(const CSnow& Prototype);
	virtual ~CSnow() = default;

public:
	virtual HRESULT					Initialize_Prototype() override;
	virtual HRESULT					Initialize(void* pArg) override;
	virtual void					Priority_Update(_float fTimeDelta) override;
	virtual void					Update(_float fTimeDelta) override;
	virtual void					Late_Update(_float fTimeDelta) override;
	virtual HRESULT					Render() override;

private:
	CVIBuffer_Rect_Instance*		m_pVIBufferCom = { nullptr };
	CComputeShader*					m_pComputeShader = { nullptr };

	CTexture*						m_pTextureCom = { nullptr };
	CShader*						m_pShaderCom = { nullptr };
	
	RectConstBufferData				m_CBData = {};
	ID3D11Buffer*					m_pReadSource = { nullptr };

private:
	HRESULT							Ready_Components();
	HRESULT							Ready_ComputeShader();
	HRESULT							Bind_ShaderResources();

	void							Drop(_float	fTimeDelta);

public:
	static CSnow*					Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*			Clone(void* pArg) override;
	virtual void					Free() override;
};

NS_END