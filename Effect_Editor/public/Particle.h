#pragma once

#include "Tool_Effect_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CVIBuffer_Point_Instance;
class CComputeShader;
class CTexture;
class CShader;
NS_END

NS_BEGIN(Tool_Effect)

class CParticle final : public CGameObject
{
public:
	struct PointConstBufferData
	{
		_float4			vPivot;
		_float4			vGravity;
		_float2			fTimeDelta;
		_int2			iLoopAndCount;
	};

private:
	CParticle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CParticle(const CParticle& Prototype);
	virtual ~CParticle() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	void	Set_Components(CVIBuffer_Point_Instance* pViBufferCom, CComputeShader* pComputeShader, CShader* pShaderCom, _float4 fGravity, _float3 fPivot);
	void	Set_Begin(_int iBegin) { m_iBegin = iBegin; }
private:
	CVIBuffer_Point_Instance* m_pVIBufferCom = { nullptr };
	CComputeShader* m_pComputeShader = { nullptr };
	CShader* m_pShaderCom = { nullptr };

	PointConstBufferData			m_CBData = {};
	ID3D11Buffer* m_pReadSource = { nullptr };
	_float4			m_fGravity = {};
	_float3			m_fPivot = {};
	_uint			m_iBegin = {};

private:
	HRESULT							Ready_Components();
	HRESULT							Bind_ShaderResources();

	HRESULT							Ready_ComputeShader();
	void							Spread(_float fTimeDelta);

public:
	static CParticle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void					Free() override;
};

NS_END