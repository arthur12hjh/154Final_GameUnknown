#pragma once

#include "Component.h"

NS_BEGIN(Engine)

class ENGINE_DLL CVIBuffer abstract : public CComponent
{
protected:
	CVIBuffer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer(const CVIBuffer& Prototype);
	virtual ~CVIBuffer() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

public:
	virtual HRESULT Bind_Resources();
	virtual HRESULT Render();

	// Buffer를 가져오는 행위는 레퍼런스 카운트가 증가합니다.
	ID3D11Buffer*					GetVIBuffer();

	// Buffer를 가져오는 행위는 레퍼런스 카운트가 증가합니다.
	ID3D11Buffer*					GetIBBuffer();

	_uint							GetVertexStride();
	_uint							GetIndices();
	DXGI_FORMAT						GetIndexFormat();

protected:
	ID3D11Buffer*					m_pVB = { nullptr };
	ID3D11Buffer*					m_pIB = { nullptr };	

	_float3*						m_pVertexPositions = {};

	_uint							m_iNumVertices = {};
	_uint							m_iVertexStride = {};

	_uint							m_iNumIndices = {};
	_uint							m_iIndexStride = {};

	_uint							m_iNumVertexBuffers = {};
	DXGI_FORMAT						m_eIndexFormat = {};
	D3D11_PRIMITIVE_TOPOLOGY		m_ePrimitive = {};

public:	
	virtual CComponent* Clone(void* pArg) =0;
	virtual void Free() override;
};

NS_END