#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CVIBuffer_Point_Instance;
class CComputeShader;
class CTexture;
class CShader;
NS_END

NS_BEGIN(Client)

class CExplosion final : public CGameObject
{
public :
	struct PointConstBufferData
	{
		_float4			vPivot;
		_float2			fTimeDelta;
		_int2			iLoopAndCount;
	};

private:
	CExplosion(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CExplosion(const CExplosion& Prototype);
	virtual ~CExplosion() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CVIBuffer_Point_Instance*		m_pVIBufferCom = { nullptr };
	CComputeShader*					m_pComputeShader = { nullptr };
	CTexture*						m_pTextureCom = { nullptr };
	CShader*						m_pShaderCom = { nullptr };
	
	PointConstBufferData			m_CBData = {};
	ID3D11Buffer*					m_pReadSource = { nullptr };

private:
	HRESULT							Ready_Components();
	HRESULT							Bind_ShaderResources();

	HRESULT							Ready_ComputeShader();
	void							Spread(_float fTimeDelta);

public:
	static CExplosion*				Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*			Clone(void* pArg) override;
	virtual void					Free() override;
};

NS_END