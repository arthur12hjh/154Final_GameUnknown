#include "Trail.h"

CTrail::CTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{
}

CTrail::CTrail(const CTrail& Prototype)
	: CComponent(Prototype),
	m_pIB(Prototype.m_pIB),
	m_iNumVertices(Prototype.m_iNumVertices),
	m_iNumIndices(Prototype.m_iNumIndices)
{
	Safe_AddRef(m_pIB);
}

HRESULT CTrail::Initialize_Prototype()
{
	m_iNumVertices = 250;
	m_iNumIndices = ((m_iNumVertices / 2) - 1) * 6;

#pragma region IDX_BUFFER
	D3D11_BUFFER_DESC IBDesc{};
	IBDesc.ByteWidth = sizeof(_uint) * m_iNumIndices;
	IBDesc.Usage = D3D11_USAGE_IMMUTABLE;
	IBDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	IBDesc.CPUAccessFlags = 0;
	IBDesc.MiscFlags = 0;
	IBDesc.StructureByteStride = sizeof(_uint);


	_uint* pIndices = new _uint[m_iNumIndices]{};
	_uint iIndex = 0;
	for (_uint iStartIndex = 0; iStartIndex < (m_iNumVertices / 2) - 1; ++iStartIndex)
	{
		_uint i0 = iStartIndex * 2;
		_uint i1 = i0 + 1;
		_uint i2 = i0 + 2;
		_uint i3 = i0 + 3;

		pIndices[iIndex++] = i0;
		pIndices[iIndex++] = i1;
		pIndices[iIndex++] = i2;

		pIndices[iIndex++] = i2;
		pIndices[iIndex++] = i1;
		pIndices[iIndex++] = i3;
	}



	D3D11_SUBRESOURCE_DATA InitialIBData{};
	InitialIBData.pSysMem = pIndices;

	if (FAILED(m_pDevice->CreateBuffer(&IBDesc, &InitialIBData, &m_pIB))) {
		return E_FAIL;
	}
	Safe_Delete_Array(pIndices);
#pragma endregion

	return S_OK;
}
void CTrail::Initialize_Trail()
{
	m_iNumPresent = 0;
	m_iEndIndex = 0;
	memset(m_pVTXPOSTEXs, 0, sizeof(VTXPOSTEX) * m_iNumVertices);
}

void CTrail::Update_Trail(_fmatrix matCurrentWorld, _float fTimeDelta, _bool bMakeTrail)
{
	m_fTime += fTimeDelta;
	if (m_fTime < 0.015f)
		return;
	if (false == bMakeTrail) {
		if (2 < m_iNumPresent) {
			_uint index = m_fTime / 0.015f;
			m_fTime -= index * 0.015f;
			_int i = m_iNumPresent - 2 * index;
			m_iNumPresent = max(i, 0);
			memmove(m_pVTXPOSTEXs, m_pVTXPOSTEXs + 2 * index, sizeof(VTXPOSTEX) * m_iNumPresent);
		}
		else {
			_uint index = m_fTime / 0.015f;
			m_fTime -= index * 0.015f;
			m_iNumPresent = 0;
			memset(m_pVTXPOSTEXs, 0, 0);
		}
		if (0 != m_iNumPresent) {
			D3D11_MAPPED_SUBRESOURCE SubResource{};
			m_pContext->Map(m_pVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResource);
			_uint iNumActivatedPairs = m_iNumPresent >> 1;
			_uint iIndexLow;
			_uint iIndexHigh;
			for (_uint iIndex = 0; iIndex < iNumActivatedPairs; ++iIndex) {
				_float u = (_float)iIndex / (_float)(iNumActivatedPairs - 1);
				iIndexLow = iIndex << 1;
				iIndexHigh = iIndexLow + 1;

				m_pVTXPOSTEXs[iIndexHigh].vTexcoord = { u, 1.f };
				m_pVTXPOSTEXs[iIndexLow].vTexcoord = { u, 0.f };
			}
			VTXPOSTEX* pVertices = static_cast<VTXPOSTEX*>(SubResource.pData);
			for (_uint i = 0; i < m_iNumPresent; ++i) {
				pVertices[i] = m_pVTXPOSTEXs[i];
			}
			m_pContext->Unmap(m_pVB, 0);
		}
		return;
	}
	memmove(m_vPreHighPositions, m_vPreHighPositions + 1, sizeof(_float4) * 2);
	memmove(m_vPreLowPositions, m_vPreLowPositions + 1, sizeof(_float4) * 2);

	XMStoreFloat4(&m_vPreHighPositions[2], XMVector3TransformCoord(XMLoadFloat4(&m_vHigh), matCurrentWorld));
	XMStoreFloat4(&m_vPreLowPositions[2], XMVector3TransformCoord(XMLoadFloat4(&m_vLow), matCurrentWorld));

	_uint iNumActivatedPairs = m_iNumPresent >> 1;
	if (iNumActivatedPairs < 2) {

		if (m_iNumPresent + 2 >= m_iNumVertices) {
			m_iNumPresent -= 2;
			memmove(m_pVTXPOSTEXs, m_pVTXPOSTEXs + 2, sizeof(VTXPOSTEX) * m_iNumPresent);
		}

		XMStoreFloat3(&m_pVTXPOSTEXs[m_iNumPresent + 1].vPosition, XMLoadFloat4(&m_vPreHighPositions[2]));
		XMStoreFloat3(&m_pVTXPOSTEXs[m_iNumPresent].vPosition, XMLoadFloat4(&m_vPreLowPositions[2]));

		m_iNumPresent += 2;

		return;
	}

	_vector vBeforeHigh = XMLoadFloat4(&m_vPreHighPositions[1]);
	_vector vAfterHigh = XMVector3TransformCoord(XMLoadFloat4(&m_vHigh), matCurrentWorld);
	_float fLength = XMVectorGetX(XMVector3Length(vAfterHigh - vBeforeHigh));
	_int iNum = 1;
	if (1 < fLength)
		iNum = fLength;
	_uint index = m_fTime / 0.015f;
	m_fTime -= 0.015f * index;
	_vector vHighPositions[4]{};
	_vector vLowPositions[4]{};
	_float fValue = {};
	for (int i = 0; i < 3; ++i) {
		vHighPositions[i] = XMLoadFloat4(&m_vPreHighPositions[i]);
		vLowPositions[i] = XMLoadFloat4(&m_vPreLowPositions[i]);
	}
	for (int i = 0; i < 4 * iNum * index; ++i) {
		fValue = (_float)i / ((4 * iNum * index) - 1);

		vHighPositions[3] = XMVectorCatmullRom(vHighPositions[0], vHighPositions[1], vHighPositions[2], vHighPositions[2] + (vHighPositions[2] - vHighPositions[1]), fValue);
		vLowPositions[3] = XMVectorCatmullRom(vLowPositions[0], vLowPositions[1], vLowPositions[2], vLowPositions[2] + (vLowPositions[2] - vLowPositions[1]), fValue);

		if (m_iNumPresent + 2 >= m_iNumVertices) {
			m_iNumPresent -= 2;
			memmove(m_pVTXPOSTEXs, m_pVTXPOSTEXs + 2, sizeof(VTXPOSTEX) * m_iNumPresent);
		}
		XMStoreFloat3(&m_pVTXPOSTEXs[m_iNumPresent + 1].vPosition, vHighPositions[3]);
		XMStoreFloat3(&m_pVTXPOSTEXs[m_iNumPresent].vPosition, vLowPositions[3]);

		m_iNumPresent += 2;
	}
	_uint iIndexLow;
	_uint iIndexHigh;
	for (_uint iIndex = 0; iIndex < iNumActivatedPairs; ++iIndex) {
		_float u = (_float)iIndex / (_float)(iNumActivatedPairs - 1);
		iIndexLow = iIndex << 1;
		iIndexHigh = iIndexLow + 1;

		m_pVTXPOSTEXs[iIndexHigh].vTexcoord = { u, 1.f };
		m_pVTXPOSTEXs[iIndexLow].vTexcoord = { u, 0.f };
	}

	D3D11_MAPPED_SUBRESOURCE SubResource{};
	m_pContext->Map(m_pVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResource);

	VTXPOSTEX* pVertices = static_cast<VTXPOSTEX*>(SubResource.pData);
	for (_uint i = 0; i < m_iNumPresent; ++i) {
		pVertices[i] = m_pVTXPOSTEXs[i];
	}
	m_pContext->Unmap(m_pVB, 0);
}

HRESULT CTrail::Render()
{
	if (m_iNumPresent < 4) { return S_OK; }

	ID3D11Buffer* VertexBuffers[] = { m_pVB };
	_uint			VertexStrides[] = { sizeof(VTXPOSTEX) };
	_uint			Offsets[] = { 0 };

	m_pContext->IASetVertexBuffers(0, 1, VertexBuffers, VertexStrides, Offsets);
	m_pContext->IASetIndexBuffer(m_pIB, DXGI_FORMAT_R32_UINT, 0);
	m_pContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	_uint iNumPair = m_iNumPresent >> 1;
	_uint iActiveIndices = (iNumPair - 1) * 6;

	m_pContext->DrawIndexed(iActiveIndices, 0, 0);
	return S_OK;
}

_bool CTrail::IsRenderable()
{
	return (m_iNumPresent < 4);
}


HRESULT CTrail::Initialize(void* pArg)
{
	TRAILHIGHLOW* pDesc = static_cast<TRAILHIGHLOW*>(pArg);
	m_vHigh = pDesc->vHigh;
	m_vLow = pDesc->vLow;

#pragma region VTX_BUFFER
	D3D11_BUFFER_DESC VBDesc{};
	VBDesc.ByteWidth = sizeof(VTXPOSTEX) * m_iNumVertices;
	VBDesc.Usage = D3D11_USAGE_DYNAMIC;
	VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	VBDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	VBDesc.MiscFlags = 0;
	VBDesc.StructureByteStride = sizeof(VTXPOSTEX);

	m_pVTXPOSTEXs = new VTXPOSTEX[m_iNumVertices]{};
	if (FAILED(m_pDevice->CreateBuffer(&VBDesc, nullptr, &m_pVB))) {
		return E_FAIL;
	}
#pragma endregion

	Initialize_Trail();
	return S_OK;
}

CTrail* CTrail::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTrail* pInstance = new CTrail(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CTrail");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CTrail* CTrail::Clone(void* pArg)
{
	CTrail* pInstance = new CTrail(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTrail");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTrail::Free()
{
	__super::Free();

	Safe_Delete_Array(m_pVTXPOSTEXs);
	Safe_Release(m_pVB);
	Safe_Release(m_pIB);
}
