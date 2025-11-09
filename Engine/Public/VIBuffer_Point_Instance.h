#pragma once

#include "VIBuffer_Instance.h"

NS_BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Point_Instance final : public CVIBuffer_Instance
{
public:
	typedef struct tagPointInstanceDesc final : public CVIBuffer_Instance::INSTANCE_DESC
	{
		_float3			vPivot;
		_float2			vLifeTime;
		_float2			vSpeed;
	}POINT_INSTANCE_DESC;

	typedef struct tagVertexPointInstance_Particle final : public VTX_INSTANCE_PARTICLE
	{
		_float2 vSpeeds;
	}VTX_INSTANCE_POINT_PARTICLE;

	struct PointConstBufferData
	{
		_float4 fPivot;
		_float2 fTimeDelta;
		XMINT2	iLoopAndCount;
	};
private:
	CVIBuffer_Point_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Point_Instance(const CVIBuffer_Point_Instance& Prototype);
	virtual ~CVIBuffer_Point_Instance() = default;

public:
	virtual HRESULT Initialize_Prototype(const INSTANCE_DESC* pInstanceDesc) override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Bind_Resources() override;
	virtual HRESULT Render() override;
public:
	virtual void Drop(_float fTimeDelta) override;
	virtual void Spread(_float fTimeDelta);


private:
	VTX_INSTANCE_POINT_PARTICLE* m_pInstanceVertices = { nullptr };
	ID3D11ComputeShader* m_pDropShaderCom = nullptr;
	ID3D11ComputeShader* m_pSpreadShaderCom = nullptr;
	PointConstBufferData	m_CBData = {};



	ID3D11Buffer* m_pResourceBuffer[2] = { nullptr, nullptr };
	ID3D11ShaderResourceView* m_pPointInputSRV[2] = { nullptr, nullptr };
	ID3D11UnorderedAccessView* m_pPointOutUAV[2] = { nullptr, nullptr };
	ID3D11Buffer* m_pConstPointBuffer = nullptr;
	_float3 m_vPivot = {};
	_bool	m_bFlag = { true };


public:
	static CVIBuffer_Point_Instance* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const INSTANCE_DESC* pInstanceDesc);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END