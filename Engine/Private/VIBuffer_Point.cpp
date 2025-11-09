#include "VIBuffer_Point.h"

CVIBuffer_Point::CVIBuffer_Point(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CVIBuffer{pDevice, pContext}
{
}

CVIBuffer_Point::CVIBuffer_Point(const CVIBuffer_Point& Prototype)
    : CVIBuffer { Prototype }
{
}

HRESULT CVIBuffer_Point::Initialize_Prototype()
{
    m_iNumVertexBuffers = 1;
    m_iNumVertices = 1;
    m_iVertexStride = sizeof(VTXPOS);

    m_iNumIndices = 0;
    m_iIndexStride = 0;

	m_eIndexFormat = DXGI_FORMAT_R8_UINT;
    m_ePrimitive = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;

#pragma region VERTEX_BUFFER
	D3D11_BUFFER_DESC		VBDesc{};
	VBDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	VBDesc.Usage = D3D11_USAGE_DEFAULT;
	VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	VBDesc.StructureByteStride = m_iVertexStride;
	VBDesc.CPUAccessFlags = 0;
	VBDesc.MiscFlags = 0;

	VTXPOSTEX* pVertices = new VTXPOSTEX[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXPOSTEX) * m_iNumVertices);

	m_pVertexPositions = new _float3[m_iNumVertices];
	ZeroMemory(m_pVertexPositions, sizeof(_float3) * m_iNumVertices);

	m_pVertexPositions[0] = pVertices[0].vPosition = _float3(0.f, 0.f, 0.f);

	D3D11_SUBRESOURCE_DATA	InitialVBData{};
	InitialVBData.pSysMem = pVertices;

	if (FAILED(m_pDevice->CreateBuffer(&VBDesc, &InitialVBData, &m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);
#pragma endregion

    return S_OK;
}

HRESULT CVIBuffer_Point::Initialize(void* pArg)
{
    return S_OK;
}

HRESULT CVIBuffer_Point::Bind_Resources()
{
	ID3D11Buffer* VertexBuffers[] = {
	m_pVB,

	};

	_uint		VertexStrides[] = {
		m_iVertexStride,


	};

	_uint		Offsets[] = {
		0,
	};

	m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, VertexBuffers, VertexStrides, Offsets);
	m_pContext->IASetPrimitiveTopology(m_ePrimitive);

	return S_OK;
}

HRESULT CVIBuffer_Point::Render()
{
	m_pContext->Draw(m_iNumVertices, 0);
	return S_OK;
}

CVIBuffer_Point* CVIBuffer_Point::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CVIBuffer_Point* pVIBuffer_Point = new CVIBuffer_Point(pDevice, pContext);
	if (FAILED(pVIBuffer_Point->Initialize_Prototype()))
	{
		Safe_Release(pVIBuffer_Point);
		MSG_BOX("Create Fail : VI Buffer Point");
	}
    return pVIBuffer_Point;
}

CComponent* CVIBuffer_Point::Clone(void* pArg)
{
	CVIBuffer_Point* pVIBuffer_Point = new CVIBuffer_Point(*this);
	if (FAILED(pVIBuffer_Point->Initialize(pArg)))
	{
		Safe_Release(pVIBuffer_Point);
		MSG_BOX("Clone Fail : VI Buffer Point");
	}
	return pVIBuffer_Point;
}

void CVIBuffer_Point::Free()
{
	__super::Free();
}
