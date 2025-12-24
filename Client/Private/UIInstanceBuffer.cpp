#include "pch.h"
#include "UIInstanceBuffer.h"

#include "GameInstance.h"

CUIInstanceBuffer::CUIInstanceBuffer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) :
    CVIBuffer_Instance(pDevice, pContext)
{
}

CUIInstanceBuffer::CUIInstanceBuffer(const CUIInstanceBuffer& Prototype)
    : CVIBuffer_Instance{ Prototype }
    , m_pInstanceVertices{ Prototype.m_pInstanceVertices }
{
}

HRESULT CUIInstanceBuffer::Initialize_Prototype(const UI_INSTANCE_DESC* pInstanceDesc)
{
	m_iNumVertexBuffers = 2;
	m_iNumVertices = 4;
	m_iVertexStride = sizeof(VTXPOSTEX);

	m_iNumIndices = 6;
	m_iIndexStride = 2;

	m_eIndexFormat = DXGI_FORMAT_R16_UINT;
	m_ePrimitive = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

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

	m_pVertexPositions[0] = pVertices[0].vPosition = _float3(-0.5f, 0.5f, 0.f);
	pVertices[0].vTexcoord = _float2(0.0f, 0.0f);

	m_pVertexPositions[1] = pVertices[1].vPosition = _float3(0.5f, 0.5f, 0.f);
	pVertices[1].vTexcoord = _float2(1.0f, 0.0f);

	m_pVertexPositions[2] = pVertices[2].vPosition = _float3(0.5f, -0.5f, 0.f);
	pVertices[2].vTexcoord = _float2(1.0f, 1.0f);

	m_pVertexPositions[3] = pVertices[3].vPosition = _float3(-0.5f, -0.5f, 0.f);
	pVertices[3].vTexcoord = _float2(0.0f, 1.0f);

	D3D11_SUBRESOURCE_DATA	InitialVBData{};
	InitialVBData.pSysMem = pVertices;

	if (FAILED(m_pDevice->CreateBuffer(&VBDesc, &InitialVBData, &m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

#pragma endregion

#pragma region INDEX_BUFFER
	D3D11_BUFFER_DESC		IBDesc{};
	IBDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	IBDesc.Usage = D3D11_USAGE_DEFAULT;
	IBDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	IBDesc.StructureByteStride = m_iIndexStride;
	IBDesc.CPUAccessFlags = 0;
	IBDesc.MiscFlags = 0;

	_ushort* pIndices = new _ushort[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_ushort) * m_iNumIndices);

	pIndices[0] = 0;
	pIndices[1] = 1;
	pIndices[2] = 2;

	pIndices[3] = 0;
	pIndices[4] = 2;
	pIndices[5] = 3;

	D3D11_SUBRESOURCE_DATA	InitialIBData{};
	InitialIBData.pSysMem = pIndices;

	if (FAILED(m_pDevice->CreateBuffer(&IBDesc, &InitialIBData, &m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pIndices);

#pragma endregion

#pragma region INSTANCE_BUFFER
	const UI_INSTANCE_DESC* pDesc = static_cast<const UI_INSTANCE_DESC*>(pInstanceDesc);

	m_iNumInstance = pDesc->iNumInstance;
	m_iInstanceStride = sizeof(VTX_INSTANCE_DESC);
	m_iNumIndexPerInstance = 6;

	m_InstanceBufferDesc.ByteWidth = m_iInstanceStride * m_iNumInstance;
	m_InstanceBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_InstanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_InstanceBufferDesc.StructureByteStride = m_iInstanceStride;
	m_InstanceBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_InstanceBufferDesc.MiscFlags = 0;

	m_pInstanceVertices = new VTX_INSTANCE_DESC[m_iNumInstance];
	ZeroMemory(m_pInstanceVertices, sizeof(VTX_INSTANCE_DESC) * m_iNumInstance);

	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		m_pInstanceVertices[i].vUVAtlasSize = pDesc->vUVAtlasSize;
		m_pInstanceVertices[i].vUVAtlasOffset = _float4(pDesc->vUVAtlasOffset.x, pDesc->vUVAtlasOffset.y, 0, 0);
		m_pInstanceVertices[i].vAtlasIndex = _float4(pDesc->vAtlasIndex.x, pDesc->vAtlasIndex.y, 0, 0);
	}

	m_InstanceInitialDesc.pSysMem = m_pInstanceVertices;

#pragma endregion

	return S_OK;
}

HRESULT CUIInstanceBuffer::Initialize(void* pArg)
{
	if (nullptr != pArg)
	{
		const UI_INSTANCE_DESC* pDesc = static_cast<const UI_INSTANCE_DESC*>(pArg);

		if (pDesc != nullptr)
		{
			Safe_Delete_Array(m_pInstanceVertices);
			m_iNumInstance = pDesc->iNumInstance;
			m_InstanceBufferDesc.ByteWidth = m_iInstanceStride * m_iNumInstance;

			m_pInstanceVertices = new VTX_INSTANCE_DESC[m_iNumInstance];
			ZeroMemory(m_pInstanceVertices, sizeof(VTX_INSTANCE_DESC) * m_iNumInstance);

			for (size_t i = 0; i < m_iNumInstance; i++)
			{
				m_pInstanceVertices[i].vUVAtlasSize = pDesc->vUVAtlasSize;
				m_pInstanceVertices[i].vUVAtlasOffset = _float4(pDesc->vUVAtlasOffset.x, pDesc->vUVAtlasOffset.y, 0, 0);
				m_pInstanceVertices[i].vAtlasIndex = _float4(pDesc->vAtlasIndex.x, pDesc->vAtlasIndex.y, 0, 0);
			}

			m_InstanceInitialDesc.pSysMem = m_pInstanceVertices;
		}
	}
	Safe_Release(m_pVBInstance);

	if (FAILED(m_pDevice->CreateBuffer(&m_InstanceBufferDesc, &m_InstanceInitialDesc, &m_pVBInstance)))
		return E_FAIL;

    return S_OK;
}

HRESULT CUIInstanceBuffer::Bind_Resources()
{
    __super::Bind_Resources();
    return S_OK;
}

void CUIInstanceBuffer::Update_Instance(vector<VTX_INSTANCE_DESC> Instances)
{
	if (Instances.empty())
		return;

	const _uint count = min((_uint)Instances.size(), m_iNumInstance);

	D3D11_MAPPED_SUBRESOURCE mapped{};
	Lock(D3D11_MAP_WRITE_DISCARD, &mapped);

	memcpy(mapped.pData, Instances.data(), sizeof(VTX_INSTANCE_DESC) * count);

	UnLock();
}

CUIInstanceBuffer* CUIInstanceBuffer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const UI_INSTANCE_DESC* pInstanceDesc)
{
    CUIInstanceBuffer* pUIInstanceBuffer = new CUIInstanceBuffer(pDevice, pContext);
    if (FAILED(pUIInstanceBuffer->Initialize_Prototype(pInstanceDesc)))
    {
        Safe_Release(pUIInstanceBuffer);
        MSG_BOX("Create Fail : UIInstanceBuffer");
    }
    return pUIInstanceBuffer;
}

CComponent* CUIInstanceBuffer::Clone(void* pArg)
{
	Safe_Delete_Array(m_pInstanceVertices);

    CUIInstanceBuffer* pUIInstanceBuffer = new CUIInstanceBuffer(*this);
    if (FAILED(pUIInstanceBuffer->Initialize(pArg)))
    {
        Safe_Release(pUIInstanceBuffer);
        MSG_BOX("Clone Fail : UIInstanceBuffer");
    }
    return pUIInstanceBuffer;
}

void CUIInstanceBuffer::Free()
{
    __super::Free();

	Safe_Delete_Array(m_pInstanceVertices);
}
