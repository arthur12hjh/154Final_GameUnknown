#include "VIBuffer_Instance.h"

CVIBuffer_Instance::CVIBuffer_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer { pDevice, pContext }
{
}

CVIBuffer_Instance::CVIBuffer_Instance(const CVIBuffer_Instance& Prototype)
	: CVIBuffer { Prototype}
	, m_pVBInstance { Prototype.m_pVBInstance }
	, m_InstanceBufferDesc{ Prototype.m_InstanceBufferDesc }
	, m_InstanceInitialDesc{ Prototype.m_InstanceInitialDesc }
	, m_iInstanceStride{ Prototype.m_iInstanceStride }
	, m_iNumInstance{ Prototype.m_iNumInstance }
	, m_iNumIndexPerInstance{ Prototype.m_iNumIndexPerInstance }
	, m_bIsLoop{ Prototype.m_bIsLoop }
{
	Safe_AddRef(m_pVBInstance);
}

HRESULT CVIBuffer_Instance::Initialize_Prototype(const INSTANCE_DESC* pDesc)
{
	return S_OK;
}

HRESULT CVIBuffer_Instance::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CVIBuffer_Instance::Bind_Resources()
{
	ID3D11Buffer* VertexBuffers[] = {
		   m_pVB,
		   m_pVBInstance, 

	};

	_uint		VertexStrides[] = {
		m_iVertexStride,
		m_iInstanceStride, 
	};

	_uint		Offsets[] = {
		0,
		0
	};

	m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, VertexBuffers, VertexStrides, Offsets);
	m_pContext->IASetIndexBuffer(m_pIB, m_eIndexFormat, 0);
	m_pContext->IASetPrimitiveTopology(m_ePrimitive);

	return S_OK;
}

HRESULT CVIBuffer_Instance::Render()
{
	m_pContext->DrawIndexedInstanced(m_iNumIndexPerInstance, m_iNumInstance, 0, 0, 0);

	return S_OK;
}

void CVIBuffer_Instance::Lock(D3D11_MAP eLockType, D3D11_MAPPED_SUBRESOURCE* pOutData)
{
	m_pContext->Map(m_pVBInstance, 0, eLockType, 0, pOutData);
}

void CVIBuffer_Instance::UnLock()
{
	m_pContext->Unmap(m_pVBInstance, 0);
}

void CVIBuffer_Instance::CopyResource(ID3D11Buffer* pResource)
{
	m_pContext->CopyResource(pResource, m_pVBInstance);
}

void CVIBuffer_Instance::PasteResource(ID3D11Buffer* pResource)
{
	m_pContext->CopyResource(m_pVBInstance, pResource);
}

void CVIBuffer_Instance::StopEffect()
{
	m_bIsLoop = false;
}


void CVIBuffer_Instance::Free()
{
	__super::Free();

	Safe_Release(m_pVBInstance);
}
