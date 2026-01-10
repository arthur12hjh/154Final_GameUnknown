#include "WaveTrail.h"

CWaveTrail::CWaveTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{
}

CWaveTrail::CWaveTrail(const CWaveTrail& Prototype)
	: CComponent(Prototype),
	m_pIB(Prototype.m_pIB),
	m_iNumVertices(Prototype.m_iNumVertices),
	m_iNumIndices(Prototype.m_iNumIndices),
	m_iNumPositions(Prototype.m_iNumPositions)
{
	Safe_AddRef(m_pIB);
}

HRESULT CWaveTrail::Initialize_Prototype(_int iNum)
{
	m_iNumPositions = iNum;
	m_iNumVertices = iNum * 5;
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
void CWaveTrail::Initialize_WaveTrail()
{
	m_iNumPresent = 0;
	m_iNumPositionPresent = 0;
	m_iEndIndex = 0;
	memset(m_pVTXTrails, 0, sizeof(VTXTRAIL) * m_iNumVertices);
	memset(m_pPostions, 0, sizeof(_vector) * m_iNumPositions);
}

void CWaveTrail::Update_WaveTrail(_fmatrix matCurrentWorld, _float fTimeDelta, _bool bMakeWaveTrail)
{
	m_fTime += fTimeDelta;
	m_fCumulativeTime -= fTimeDelta * 5;
	if (m_fTime < 0.015f)
		return;
	if (false == bMakeWaveTrail) {
		_int index = m_fTime / 0.015f;
		m_iNumRemove += index;
		m_fTime -= index * 0.015f;
		for (_int i = 0; i < index; ++i) {
			if (2 < m_iNumPositionPresent) {
				m_iNumPositionPresent -= 2;
				memmove(m_pPostions, m_pPostions + 2, sizeof(_vector) * m_iNumPositionPresent);
			}
			else {
				m_iNumPositionPresent = 0;
				memset(m_pPostions, 0, 0);
			}
		}

		m_iNumPresent = 0;
		memset(m_pVTXTrails, 0, sizeof(VTXTRAIL) * m_iNumVertices);
		_vector vHighPositions[4]{};
		_vector vLowPositions[4]{};
		_float fValue = {};
		for (int i = m_iNumPositionPresent; i > 2; i -= 2) {
			if (i == 4) {
				for (int j = 0; j < 3; ++j) {
					vHighPositions[j] = m_pPostions[i - j * 2 + 1];
					vLowPositions[j] = m_pPostions[i - j * 2];
				}
				vHighPositions[3] = vHighPositions[2];
				vLowPositions[3] = vLowPositions[2];
			}
			else if (i == m_iNumPositionPresent) {
				for (int j = 1; j < 4; ++j) {
					vHighPositions[j] = m_pPostions[i - j * 2 + 1];
					vLowPositions[j] = m_pPostions[i - j * 2];
				}

				vHighPositions[0] = vHighPositions[1];
				vLowPositions[0] = vLowPositions[1];
			}
			else {
				for (int j = 0; j < 4; ++j) {
					vHighPositions[j] = m_pPostions[i - j * 2 + 1];
					vLowPositions[j] = m_pPostions[i - j * 2];
				}
			}

			_float fLength = XMVectorGetX(XMVector3Length(vHighPositions[2] - vHighPositions[1]));
			_int iNum = 1;
			if (1 < fLength)
				iNum = fLength;
			for (int i = 0; i < 4 * iNum; ++i) {
				fValue = (_float)i / ((4 * iNum));
				XMStoreFloat3(&m_pVTXTrails[m_iNumPresent + 1].vPosition, XMVectorCatmullRom(vHighPositions[0], vHighPositions[1], vHighPositions[2], vHighPositions[3], fValue));
				XMStoreFloat3(&m_pVTXTrails[m_iNumPresent].vPosition, XMVectorCatmullRom(vLowPositions[0], vLowPositions[1], vLowPositions[2], vLowPositions[3], fValue));
				m_iNumPresent += 2;
				if (m_iNumPresent + 2 >= m_iNumVertices) {
					break;
				}
			}
			if (m_iNumPresent + 2 >= m_iNumVertices) {
				break;
			}
		}
		_uint iNumActivatedPairs = m_iNumPositionPresent >> 1;
		if (iNumActivatedPairs >= 2) {
			_uint iIndexLow;
			_uint iIndexHigh;
			_float fPosLengh = 0.f;
			for (_uint iIndex = 0; iIndex + 1 < m_iNumPresent; iIndex += 2) {
				_float u = 1 - min(((iIndex + m_iNumRemove) * 0.5f) / (m_iNumPresent * 0.5f), 1.f);
				iIndexLow = iIndex;
				iIndexHigh = iIndex + 1;

				if (iIndexLow + 2 < m_iNumPresent) {
					fPosLengh += XMVectorGetX(XMVector3Length(XMLoadFloat3(&m_pVTXTrails[iIndexLow].vPosition) - XMLoadFloat3(&m_pVTXTrails[iIndexLow + 2].vPosition))) * 0.025f;
					_vector		vLook = XMVector3Normalize(XMLoadFloat3(&m_pVTXTrails[iIndexLow].vPosition) - XMLoadFloat3(&m_pVTXTrails[iIndexLow + 2].vPosition));
					_vector		vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);
					_vector		vUp = XMVector3Cross(vLook, vRight);
					XMStoreFloat3(&m_pVTXTrails[iIndexHigh].vPosition, XMLoadFloat3(&m_pVTXTrails[iIndexHigh].vPosition) + vUp * sinf(m_fCumulativeTime + fPosLengh  * m_fSpeed) * m_fPow * ((iIndex + m_iNumRemove) / ((m_iNumPresent + m_iNumRemove) * 0.3f)) + vRight * cosf(m_fCumulativeTime + fPosLengh * m_fSpeed) * m_fPow * ((iIndex + m_iNumRemove) / ((m_iNumPresent + m_iNumRemove) * 0.3f)));
					XMStoreFloat3(&m_pVTXTrails[iIndexLow].vPosition, XMLoadFloat3(&m_pVTXTrails[iIndexLow].vPosition) + vUp * sinf(m_fCumulativeTime + fPosLengh * m_fSpeed) * m_fPow * ((iIndex + m_iNumRemove) / ((m_iNumPresent + m_iNumRemove) * 0.3f)) + vRight * cosf(m_fCumulativeTime + fPosLengh * m_fSpeed) * m_fPow * ((iIndex + m_iNumRemove) / ((m_iNumPresent + m_iNumRemove) * 0.3f)));
				}
				if (iIndexLow + 2 < m_iNumPresent) {
					m_pVTXTrails[iIndexHigh].vDirection = m_pVTXTrails[iIndexHigh + 2].vPosition;
					m_pVTXTrails[iIndexLow].vDirection = m_pVTXTrails[iIndexLow + 2].vPosition;
				}
				else {
					XMStoreFloat3(&m_pVTXTrails[iIndexHigh].vDirection, XMLoadFloat3(&m_pVTXTrails[iIndexHigh].vPosition) + (XMLoadFloat3(&m_pVTXTrails[iIndexHigh].vPosition) - XMLoadFloat3(&m_pVTXTrails[iIndexHigh - 2].vPosition)));
					XMStoreFloat3(&m_pVTXTrails[iIndexLow].vDirection, XMLoadFloat3(&m_pVTXTrails[iIndexLow].vPosition) + (XMLoadFloat3(&m_pVTXTrails[iIndexLow].vPosition) - XMLoadFloat3(&m_pVTXTrails[iIndexLow - 2].vPosition)));
				}
				m_pVTXTrails[iIndexHigh].vTexcoord = { u, 1.f };
				m_pVTXTrails[iIndexLow].vTexcoord = { u, 0.f };
			}

			D3D11_MAPPED_SUBRESOURCE SubResource{};
			m_pContext->Map(m_pVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResource);

			VTXTRAIL* pVertices = static_cast<VTXTRAIL*>(SubResource.pData);
			for (_uint i = 0; i < m_iNumPresent; ++i) {
				pVertices[i] = m_pVTXTrails[i];
			}
			m_pContext->Unmap(m_pVB, 0);
		}
		return;
	}
	m_iNumRemove = 0;
	memmove(m_vPreHighPositions, m_vPreHighPositions + 1, sizeof(_float4) * 2);
	memmove(m_vPreLowPositions, m_vPreLowPositions + 1, sizeof(_float4) * 2);

	XMStoreFloat4(&m_vPreHighPositions[2], XMVector3TransformCoord(XMLoadFloat4(&m_vHigh), matCurrentWorld));
	XMStoreFloat4(&m_vPreLowPositions[2], XMVector3TransformCoord(XMLoadFloat4(&m_vLow), matCurrentWorld));

	_uint iNumActivatedPairs = m_iNumPositionPresent >> 1;
	if (iNumActivatedPairs < 2) {

		if (m_iNumPositionPresent + 2 >= m_iNumPositions) {
			m_iNumPositionPresent -= 2;
			memmove(m_pPostions, m_pPostions + 2, sizeof(_vector) * m_iNumPositionPresent);
		}

		m_pPostions[m_iNumPositionPresent + 1] = XMLoadFloat4(&m_vPreHighPositions[2]);
		m_pPostions[m_iNumPositionPresent] = XMLoadFloat4(&m_vPreLowPositions[2]);

		m_iNumPositionPresent += 2;

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
	if (m_iNumPositionPresent + 2 >= m_iNumPositions) {
		m_iNumPositionPresent -= 2;
		memmove(m_pPostions, m_pPostions + 2, sizeof(_vector) * m_iNumPositionPresent);
	}

	m_pPostions[m_iNumPositionPresent + 1] = XMLoadFloat4(&m_vPreHighPositions[2]);
	m_pPostions[m_iNumPositionPresent] = XMLoadFloat4(&m_vPreLowPositions[2]);
	m_iNumPositionPresent += 2;

	m_iNumPresent = 0;
	memset(m_pVTXTrails, 0, sizeof(VTXTRAIL) * m_iNumVertices);
	for (int i = m_iNumPositionPresent; i > 2; i -= 2) {
		if (i == 4) {
			for (int j = 0; j < 3; ++j) {
				vHighPositions[j] = m_pPostions[i - j * 2 + 1];
				vLowPositions[j] = m_pPostions[i - j * 2];
			}
			vHighPositions[3] = vHighPositions[2];
			vLowPositions[3] = vLowPositions[2];
		}
		else if (i == m_iNumPositionPresent) {
			for (int j = 1; j < 4; ++j) {
				vHighPositions[j] = m_pPostions[i - j * 2 + 1];
				vLowPositions[j] = m_pPostions[i - j * 2];
			}
			vHighPositions[0] = vHighPositions[1];
			vLowPositions[0] = vLowPositions[1];
			m_fCumulativeTime -= XMVectorGetX(XMVector3Length(vHighPositions[2] - vHighPositions[1])) * 0.025f * m_fSpeed;
		}
		else {
			for (int j = 0; j < 4; ++j) {
				vHighPositions[j] = m_pPostions[i - j * 2 + 1];
				vLowPositions[j] = m_pPostions[i - j * 2];
			}
		}

		_float fLength = XMVectorGetX(XMVector3Length(vHighPositions[2] - vHighPositions[1]));
		_int iNum = 1;
		if (1 < fLength)
			iNum = fLength;
		for (int i = 0; i < 4 * iNum; ++i) {
			fValue = (_float)i / ((4 * iNum));
			XMStoreFloat3(&m_pVTXTrails[m_iNumPresent + 1].vPosition, XMVectorCatmullRom(vHighPositions[0], vHighPositions[1], vHighPositions[2], vHighPositions[3], fValue));
			XMStoreFloat3(&m_pVTXTrails[m_iNumPresent].vPosition, XMVectorCatmullRom(vLowPositions[0], vLowPositions[1], vLowPositions[2], vLowPositions[3], fValue));
			m_iNumPresent += 2;
			if (m_iNumPresent + 2 >= m_iNumVertices) {
				break;
			}
		}
		if (m_iNumPresent + 2 >= m_iNumVertices) {
			break;
		}
	}
	_uint iIndexLow;
	_uint iIndexHigh;
	_float fPow = min(m_iNumPresent * 0.002f, 1.f);
	_float fPosLengh = 0.f;
	for (_uint iIndex = 0; iIndex + 1 < m_iNumPresent; iIndex += 2) {
		_float u = 1 - (iIndex * 0.5f) / (m_iNumPresent * 0.5f);
		iIndexLow = iIndex;
		iIndexHigh = iIndex + 1;
		if (iIndexLow + 2 < m_iNumPresent) {
			fPosLengh += XMVectorGetX(XMVector3Length(XMLoadFloat3(&m_pVTXTrails[iIndexLow].vPosition) - XMLoadFloat3(&m_pVTXTrails[iIndexLow + 2].vPosition))) * 0.025f;
			_vector		vLook = XMVector3Normalize(XMLoadFloat3(&m_pVTXTrails[iIndexLow].vPosition) - XMLoadFloat3(&m_pVTXTrails[iIndexLow + 2].vPosition));
			_vector		vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);
			_vector		vUp = XMVector3Cross(vLook, vRight);
			XMStoreFloat3(&m_pVTXTrails[iIndexHigh].vPosition, XMLoadFloat3(&m_pVTXTrails[iIndexHigh].vPosition) + vUp * sinf(m_fCumulativeTime + fPosLengh * m_fSpeed) * m_fPow * (_float(iIndex) / (m_iNumPresent * 0.3f)) + vRight * cosf(m_fCumulativeTime + fPosLengh * m_fSpeed) * m_fPow * (_float(iIndex) / (m_iNumPresent * 0.3f)));
			XMStoreFloat3(&m_pVTXTrails[iIndexLow].vPosition, XMLoadFloat3(&m_pVTXTrails[iIndexLow].vPosition) + vUp * sinf(m_fCumulativeTime + fPosLengh * m_fSpeed) * m_fPow * (_float(iIndex) / (m_iNumPresent * 0.3f)) + vRight * cosf(m_fCumulativeTime + fPosLengh * m_fSpeed) * m_fPow * (_float(iIndex) / (m_iNumPresent * 0.3f)));
		}

		if (iIndexLow + 2 < m_iNumPresent) {
			m_pVTXTrails[iIndexHigh].vDirection = m_pVTXTrails[iIndexHigh + 2].vPosition;
			m_pVTXTrails[iIndexLow].vDirection = m_pVTXTrails[iIndexLow + 2].vPosition;
		}
		else {
			XMStoreFloat3(&m_pVTXTrails[iIndexHigh].vDirection, XMLoadFloat3(&m_pVTXTrails[iIndexHigh].vPosition) + (XMLoadFloat3(&m_pVTXTrails[iIndexHigh].vPosition) - XMLoadFloat3(&m_pVTXTrails[iIndexHigh - 2].vPosition)));
			XMStoreFloat3(&m_pVTXTrails[iIndexLow].vDirection, XMLoadFloat3(&m_pVTXTrails[iIndexLow].vPosition) + (XMLoadFloat3(&m_pVTXTrails[iIndexLow].vPosition) - XMLoadFloat3(&m_pVTXTrails[iIndexLow - 2].vPosition)));
		}
		m_pVTXTrails[iIndexHigh].vTexcoord = { u, 1.f };
		m_pVTXTrails[iIndexLow].vTexcoord = { u, 0.f };
	}

	D3D11_MAPPED_SUBRESOURCE SubResource{};
	m_pContext->Map(m_pVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResource);

	VTXTRAIL* pVertices = static_cast<VTXTRAIL*>(SubResource.pData);
	for (_uint i = 0; i < m_iNumPresent; ++i) {
		pVertices[i] = m_pVTXTrails[i];
	}
	m_pContext->Unmap(m_pVB, 0);
}

HRESULT CWaveTrail::Render()
{
	if (m_iNumPresent < 4) { return S_OK; }

	ID3D11Buffer* VertexBuffers[] = { m_pVB };
	_uint			VertexStrides[] = { sizeof(VTXTRAIL) };
	_uint			Offsets[] = { 0 };

	m_pContext->IASetVertexBuffers(0, 1, VertexBuffers, VertexStrides, Offsets);
	m_pContext->IASetIndexBuffer(m_pIB, DXGI_FORMAT_R32_UINT, 0);
	m_pContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	_uint iNumPair = m_iNumPresent >> 1;
	_uint iActiveIndices = (iNumPair - 1) * 6;

	m_pContext->DrawIndexed(iActiveIndices, 0, 0);
	return S_OK;
}

_bool CWaveTrail::IsRenderable()
{
	return (m_iNumPresent < 4);
}


HRESULT CWaveTrail::Initialize(void* pArg)
{
	WAVETRAILHIGHLOW* pDesc = static_cast<WAVETRAILHIGHLOW*>(pArg);
	m_vHigh = pDesc->vHigh;
	m_vLow = pDesc->vLow;
	m_fPow = pDesc->fPow;
	m_fSpeed = pDesc->fSpeed;

#pragma region VTX_BUFFER
	D3D11_BUFFER_DESC VBDesc{};
	VBDesc.ByteWidth = sizeof(VTXTRAIL) * m_iNumVertices;
	VBDesc.Usage = D3D11_USAGE_DYNAMIC;
	VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	VBDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	VBDesc.MiscFlags = 0;
	VBDesc.StructureByteStride = sizeof(VTXTRAIL);

	m_pVTXTrails = new VTXTRAIL[m_iNumVertices]{};
	m_pPostions = new _vector[m_iNumPositions]{};
	if (FAILED(m_pDevice->CreateBuffer(&VBDesc, nullptr, &m_pVB))) {
		return E_FAIL;
	}
#pragma endregion

	Initialize_WaveTrail();
	return S_OK;
}

CWaveTrail* CWaveTrail::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _int iNum)
{
	CWaveTrail* pInstance = new CWaveTrail(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(iNum)))
	{
		MSG_BOX("Failed to Created : CWaveTrail");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CWaveTrail* CWaveTrail::Clone(void* pArg)
{
	CWaveTrail* pInstance = new CWaveTrail(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CWaveTrail");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWaveTrail::Free()
{
	__super::Free();

	Safe_Delete_Array(m_pVTXTrails);
	Safe_Delete_Array(m_pPostions);
	Safe_Release(m_pVB);
	Safe_Release(m_pIB);
}
