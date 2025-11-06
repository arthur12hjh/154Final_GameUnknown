#pragma once

#include "VIBuffer_Instance.h"

NS_BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Rect_Instance final : public CVIBuffer_Instance
{
public:
	typedef struct tagRectInstanceDesc final : public CVIBuffer_Instance::INSTANCE_DESC
	{		
		_float2			vLifeTime;
		_float2			vSpeed;
		_bool			isLoop;
	}RECT_INSTANCE_DESC;
private:
	CVIBuffer_Rect_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Rect_Instance(const CVIBuffer_Rect_Instance& Prototype);
	virtual ~CVIBuffer_Rect_Instance() = default;

public:
	virtual HRESULT Initialize_Prototype(const INSTANCE_DESC* pInstanceDesc) override;
	virtual HRESULT Initialize(void* pArg) override;
public:
	virtual void Drop(_float fTimeDelta) override;


private:
	VTX_INSTANCE_PARTICLE* m_pInstanceVertices = { nullptr };

	_float* m_pSpeeds = { nullptr };
	_bool	m_isLoop = { false };


public:
	static CVIBuffer_Rect_Instance* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const INSTANCE_DESC* pInstanceDesc);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END