#include "VIBuffer_Cube.h"

CVIBuffer_Cube::CVIBuffer_Cube(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer { pDevice, pContext }
{
}

CVIBuffer_Cube::CVIBuffer_Cube(const CVIBuffer_Cube& Prototype)
	: CVIBuffer { Prototype }
{

}

HRESULT CVIBuffer_Cube::Initialize_Prototype()
{
	m_iNumVertexBuffers = 1;
	m_iNumVertices = 24;
	m_iVertexStride = sizeof(VTXCUBE);

	m_iNumIndices = 36;
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

	VTXCUBE* pVertices = new VTXCUBE[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXCUBE) * m_iNumVertices);

	m_pVertexPositions = new _float3[m_iNumVertices];
	ZeroMemory(m_pVertexPositions, sizeof(_float3) * m_iNumVertices);

#pragma region Front
	m_pVertexPositions[0] = pVertices[0].vPosition = _float3(-0.5f, 0.5f, -0.5f);
	pVertices[0].vTexcoord = m_pVertexPositions[0];

	m_pVertexPositions[1] = pVertices[1].vPosition = _float3(0.5f, 0.5f, -0.5f);
	pVertices[1].vTexcoord = m_pVertexPositions[1];

	m_pVertexPositions[2] = pVertices[2].vPosition = _float3(0.5f, -0.5f, -0.5f);
	pVertices[2].vTexcoord = m_pVertexPositions[2];

	m_pVertexPositions[3] = pVertices[3].vPosition = _float3(-0.5f, -0.5f, -0.5f);
	pVertices[3].vTexcoord = m_pVertexPositions[3];

	pVertices[0].vNormal = pVertices[1].vNormal = pVertices[2].vNormal = pVertices[3].vNormal = { 0.0f, 0.0f, -1.0f };
#pragma endregion

#pragma region Back
	m_pVertexPositions[4] = pVertices[4].vPosition = _float3(-0.5f, 0.5f, 0.5f);
	pVertices[4].vTexcoord = m_pVertexPositions[4];

	m_pVertexPositions[5] = pVertices[5].vPosition = _float3(0.5f, 0.5f, 0.5f);
	pVertices[5].vTexcoord = m_pVertexPositions[5];

	m_pVertexPositions[6] = pVertices[6].vPosition = _float3(0.5f, -0.5f, 0.5f);
	pVertices[6].vTexcoord = m_pVertexPositions[6];

	m_pVertexPositions[7] = pVertices[7].vPosition = _float3(-0.5f, -0.5f, 0.5f);
	pVertices[7].vTexcoord = m_pVertexPositions[7];

	pVertices[4].vNormal = pVertices[5].vNormal = pVertices[6].vNormal = pVertices[7].vNormal = { 0.0f, 0.0f, 1.0f };
#pragma endregion

#pragma region UP
	m_pVertexPositions[8] = pVertices[8].vPosition = _float3(-0.5f, 0.5f, -0.5f);
	pVertices[8].vTexcoord = m_pVertexPositions[8];

	m_pVertexPositions[9] = pVertices[9].vPosition = _float3(-0.5f, 0.5f, 0.5f);
	pVertices[9].vTexcoord = m_pVertexPositions[9];

	m_pVertexPositions[10] = pVertices[10].vPosition = _float3(0.5f, 0.5f, 0.5f);
	pVertices[10].vTexcoord = m_pVertexPositions[10];

	m_pVertexPositions[11] = pVertices[11].vPosition = _float3(0.5f, 0.5f, -0.5f);
	pVertices[11].vTexcoord = m_pVertexPositions[11];

	pVertices[8].vNormal = pVertices[9].vNormal = pVertices[10].vNormal = pVertices[11].vNormal = { 0.0f, 1.0f, 0.0f };
#pragma endregion

#pragma region Down
	m_pVertexPositions[12] = pVertices[12].vPosition = _float3(-0.5f, -0.5f, -0.5f);
	pVertices[12].vTexcoord = m_pVertexPositions[12];

	m_pVertexPositions[13] = pVertices[13].vPosition = _float3(-0.5f, -0.5f, 0.5f);
	pVertices[13].vTexcoord = m_pVertexPositions[13];

	m_pVertexPositions[14] = pVertices[14].vPosition = _float3(0.5f, -0.5f, 0.5f);
	pVertices[14].vTexcoord = m_pVertexPositions[14];

	m_pVertexPositions[15] = pVertices[15].vPosition = _float3(0.5f, -0.5f, -0.5f);
	pVertices[15].vTexcoord = m_pVertexPositions[15];

	pVertices[12].vNormal = pVertices[13].vNormal = pVertices[14].vNormal = pVertices[15].vNormal = { 0.0f, -1.0f, 0.0f };
#pragma endregion
	
#pragma region Left
	m_pVertexPositions[16] = pVertices[16].vPosition = _float3(-0.5f, 0.5f, 0.5f);
	pVertices[16].vTexcoord = m_pVertexPositions[16];

	m_pVertexPositions[17] = pVertices[17].vPosition = _float3(-0.5f, 0.5f, -0.5f);
	pVertices[17].vTexcoord = m_pVertexPositions[17];

	m_pVertexPositions[18] = pVertices[18].vPosition = _float3(-0.5f, -0.5f, -0.5f);
	pVertices[18].vTexcoord = m_pVertexPositions[18];

	m_pVertexPositions[19] = pVertices[19].vPosition = _float3(-0.5f, -0.5f, 0.5f);
	pVertices[19].vTexcoord = m_pVertexPositions[19];

	pVertices[16].vNormal = pVertices[17].vNormal = pVertices[18].vNormal = pVertices[19].vNormal = { -1.0f, 0.0f, 0.0f };
#pragma endregion

#pragma region Right
	m_pVertexPositions[20] = pVertices[20].vPosition = _float3(0.5f, 0.5f,- 0.5f);
	pVertices[20].vTexcoord = m_pVertexPositions[20];

	m_pVertexPositions[21] = pVertices[21].vPosition = _float3(0.5f, 0.5f, 0.5f);
	pVertices[21].vTexcoord = m_pVertexPositions[21];

	m_pVertexPositions[22] = pVertices[22].vPosition = _float3(0.5f, -0.5f, 0.5f);
	pVertices[22].vTexcoord = m_pVertexPositions[22];

	m_pVertexPositions[23] = pVertices[23].vPosition = _float3(0.5f, -0.5f, -0.5f);
	pVertices[23].vTexcoord = m_pVertexPositions[23];

	pVertices[20].vNormal = pVertices[21].vNormal = pVertices[22].vNormal = pVertices[23].vNormal = { 1.0f, 0.0f, 0.0f };
#pragma endregion

	D3D11_SUBRESOURCE_DATA	InitialVBData{};
	InitialVBData.pSysMem = pVertices;	

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

	pIndices[0] = 0; pIndices[1] = 1; pIndices[2] = 2;
	pIndices[3] = 0; pIndices[4] = 2; pIndices[5] = 3;

	// µÞ¸é
	pIndices[6] = 4; pIndices[7] = 5;  pIndices[8] = 6;
	pIndices[9] = 4; pIndices[10] = 6; pIndices[11] = 7;

	// À­¸é
	pIndices[12] = 8; pIndices[13] = 9;  pIndices[14] = 10;
	pIndices[15] = 8; pIndices[16] = 10; pIndices[17] = 11;

	// ¾Æ·§¸é
	pIndices[18] = 12; pIndices[19] = 13; pIndices[20] = 14;
	pIndices[21] = 12; pIndices[22] = 14; pIndices[23] = 15;

	// ¿ÞÂÊ¸é
	pIndices[24] = 16; pIndices[25] = 17; pIndices[26] = 18;
	pIndices[27] = 16; pIndices[28] = 18; pIndices[29] = 19;

	// ¿À¸¥ÂÊ¸é
	pIndices[30] = 20; pIndices[31] = 21; pIndices[32] = 22;
	pIndices[33] = 20; pIndices[34] = 22; pIndices[35] = 23;

	D3D11_SUBRESOURCE_DATA	InitialIBData{};
	InitialIBData.pSysMem = pIndices;

	if (FAILED(m_pDevice->CreateBuffer(&VBDesc, &InitialVBData, &m_pVB)))
		return E_FAIL;

	if (FAILED(m_pDevice->CreateBuffer(&IBDesc, &InitialIBData, &m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);
	Safe_Delete_Array(pIndices);

#pragma endregion
	
	

	return S_OK;
}

HRESULT CVIBuffer_Cube::Initialize(void* pArg)
{
	return S_OK;
}

CVIBuffer_Cube* CVIBuffer_Cube::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CVIBuffer_Cube* pInstance = new CVIBuffer_Cube(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CVIBuffer_Cube");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CVIBuffer_Cube::Clone(void* pArg)
{
	CVIBuffer_Cube* pInstance = new CVIBuffer_Cube(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CVIBuffer_Cube");
		Safe_Release(pInstance);
	}

	return pInstance;
}
void CVIBuffer_Cube::Free()
{
	__super::Free();


}
