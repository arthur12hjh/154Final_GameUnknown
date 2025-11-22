#include "VIBuffer_Terrain.h"
#include "GameInstance.h"
#include "QuadTree.h"

CVIBuffer_Terrain::CVIBuffer_Terrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer { pDevice, pContext }
{
}

CVIBuffer_Terrain::CVIBuffer_Terrain(const CVIBuffer_Terrain& Prototype)
	: CVIBuffer { Prototype }
	, m_iNumVerticesX { Prototype.m_iNumVerticesX }
	, m_iNumVerticesZ { Prototype.m_iNumVerticesZ }
	, m_pQuadTree { Prototype.m_pQuadTree }
{
	Safe_AddRef(m_pQuadTree);
}

_float* CVIBuffer_Terrain::Get_HeightData()
{
	_float* pHeightData = new _float[m_iNumVertices];
	for (_uint i = 0; i < m_iNumVertices; ++i)
	{
		pHeightData[i] = m_pVertexPositions[i].y;
	}
	return pHeightData;
}

HRESULT CVIBuffer_Terrain::Initialize_Prototype(const _tchar* pHeightMapFilePath)
{
	_ulong			dwByte = {};
	HANDLE			hFile = CreateFile(pHeightMapFilePath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	BITMAPFILEHEADER			fh{};
	BITMAPINFOHEADER			ih{};

	ReadFile(hFile, &fh, sizeof fh, &dwByte, nullptr);
	ReadFile(hFile, &ih, sizeof ih, &dwByte, nullptr);

	m_iNumVerticesX = ih.biWidth;
	m_iNumVerticesZ = ih.biHeight;
	m_iNumVertices = m_iNumVerticesX * m_iNumVerticesZ;

	_uint* pPixels = new _uint[m_iNumVertices];
	ZeroMemory(pPixels, sizeof(_uint) * m_iNumVertices);

	ReadFile(hFile, pPixels, sizeof(_uint) * m_iNumVertices, &dwByte, nullptr);

	m_iNumVertexBuffers = 1;

	m_iVertexStride = sizeof(VTXNORTEX);

	m_iNumIndices = (m_iNumVerticesX - 1) * (m_iNumVerticesZ - 1) * 2 * 3;
	m_iIndexStride = 4;
	
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_ePrimitive = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

#pragma region VERTEX_BUFFER
	D3D11_BUFFER_DESC		VBDesc{};
	VBDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	VBDesc.Usage = D3D11_USAGE_DEFAULT;
	VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	VBDesc.StructureByteStride = m_iVertexStride;
	VBDesc.CPUAccessFlags = 0;
	VBDesc.MiscFlags = 0; 

	VTXNORTEX* pVertices = new VTXNORTEX[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXNORTEX) * m_iNumVertices);

	m_pVertexPositions = new _float3[m_iNumVertices];
	ZeroMemory(m_pVertexPositions, sizeof(_float3) * m_iNumVertices);

	for (size_t i = 0; i < m_iNumVerticesZ; i++)
	{
		for (size_t j = 0; j < m_iNumVerticesX; j++)
		{
			_uint		iIndex = i * m_iNumVerticesX + j;

			m_pVertexPositions[iIndex] = pVertices[iIndex].vPosition = _float3(j, (pPixels[iIndex] & 0x000000ff) / 10.f, i);
			pVertices[iIndex].vNormal = _float3(0.f, 0.f, 0.f);
			pVertices[iIndex].vTexcoord = _float2(j / (m_iNumVerticesX - 1.f), i / (m_iNumVerticesZ - 1.f));
		}
	}

	D3D11_SUBRESOURCE_DATA	InitialVBData{};
	InitialVBData.pSysMem = pVertices;	
#pragma endregion

#pragma region INDEX_BUFFER
	D3D11_BUFFER_DESC		IBDesc{};
	IBDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	IBDesc.Usage = D3D11_USAGE_DYNAMIC;
	IBDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	IBDesc.StructureByteStride = m_iIndexStride;
	IBDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	IBDesc.MiscFlags = 0;


	_uint* pIndices = new _uint[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_uint) * m_iNumIndices);

	_uint		iNumIndices = {};

	for (size_t i = 0; i < m_iNumVerticesZ - 1; i++)
	{
		for (size_t j = 0; j < m_iNumVerticesX - 1; j++)
		{
			_uint		iIndex = i * m_iNumVerticesX + j;

			_uint		iIndices[4] = {
				iIndex + m_iNumVerticesX, 
				iIndex + m_iNumVerticesX + 1,
				iIndex + 1,
				iIndex
			};

			_vector		vSourDir, vDestDir, vNormal;

			pIndices[iNumIndices++] = iIndices[0];
			pIndices[iNumIndices++] = iIndices[1];
			pIndices[iNumIndices++] = iIndices[2];

			vSourDir = XMLoadFloat3(&pVertices[iIndices[1]].vPosition) - XMLoadFloat3(&pVertices[iIndices[0]].vPosition);
			vDestDir = XMLoadFloat3(&pVertices[iIndices[2]].vPosition) - XMLoadFloat3(&pVertices[iIndices[1]].vPosition);
			vNormal = XMVector3Normalize(XMVector3Cross(vSourDir, vDestDir));

			XMStoreFloat3(&pVertices[iIndices[0]].vNormal, XMLoadFloat3(&pVertices[iIndices[0]].vNormal) + vNormal);
			XMStoreFloat3(&pVertices[iIndices[1]].vNormal, XMLoadFloat3(&pVertices[iIndices[1]].vNormal) + vNormal);
			XMStoreFloat3(&pVertices[iIndices[2]].vNormal, XMLoadFloat3(&pVertices[iIndices[2]].vNormal) + vNormal);

			pIndices[iNumIndices++] = iIndices[0];
			pIndices[iNumIndices++] = iIndices[2];
			pIndices[iNumIndices++] = iIndices[3];

			vSourDir = XMLoadFloat3(&pVertices[iIndices[2]].vPosition) - XMLoadFloat3(&pVertices[iIndices[0]].vPosition);
			vDestDir = XMLoadFloat3(&pVertices[iIndices[3]].vPosition) - XMLoadFloat3(&pVertices[iIndices[2]].vPosition);
			vNormal = XMVector3Normalize(XMVector3Cross(vSourDir, vDestDir));

			XMStoreFloat3(&pVertices[iIndices[0]].vNormal, XMLoadFloat3(&pVertices[iIndices[0]].vNormal) + vNormal);
			XMStoreFloat3(&pVertices[iIndices[2]].vNormal, XMLoadFloat3(&pVertices[iIndices[2]].vNormal) + vNormal);
			XMStoreFloat3(&pVertices[iIndices[3]].vNormal, XMLoadFloat3(&pVertices[iIndices[3]].vNormal) + vNormal);
		}
	}

	for (size_t i = 0; i < m_iNumVertices; i++)
	{
		XMStoreFloat3(&pVertices[i].vNormal,
			XMVector3Normalize(XMLoadFloat3(&pVertices[i].vNormal)));
	}

	D3D11_SUBRESOURCE_DATA	InitialIBData{};
	InitialIBData.pSysMem = pIndices;
#pragma endregion

	if (FAILED(m_pDevice->CreateBuffer(&VBDesc, &InitialVBData, &m_pVB)))
		return E_FAIL;

	if (FAILED(m_pDevice->CreateBuffer(&IBDesc, &InitialIBData, &m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);
	Safe_Delete_Array(pIndices);

	Safe_Delete_Array(pPixels);
	CloseHandle(hFile);	

	m_pQuadTree = CQuadTree::Create(m_iNumVerticesX * m_iNumVerticesZ - m_iNumVerticesX, m_iNumVerticesX * m_iNumVerticesZ - 1, m_iNumVerticesX - 1, 0);
	if (nullptr == m_pQuadTree)
		return E_FAIL;

	m_pQuadTree->SetUp_Neighbors();

	return S_OK;
}

_float CVIBuffer_Terrain::Get_Interpolated_Height_Local(_float fLocalX, _float fLocalZ) const
{
	_int iX = (_int)fLocalX;
	_int iZ = (_int)fLocalZ;

	if (iX < 0 || iX >= m_iNumVerticesX - 1 || iZ < 0 || iZ >= m_iNumVerticesZ - 1)
	{
		return 0.0f;
	}

	_uint iIndexLT = iZ * m_iNumVerticesX + iX;           // Left Top
	_uint iIndexRT = iZ * m_iNumVerticesX + (iX + 1);     // Right Top
	_uint iIndexLB = (iZ + 1) * m_iNumVerticesX + iX;     // Left Bottom
	_uint iIndexRB = (iZ + 1) * m_iNumVerticesX + (iX + 1);// Right Bottom

	const _float3& vPosLT = m_pVertexPositions[iIndexLT];
	const _float3& vPosRT = m_pVertexPositions[iIndexRT];
	const _float3& vPosLB = m_pVertexPositions[iIndexLB];
	const _float3& vPosRB = m_pVertexPositions[iIndexRB];

	// 4. 사각형 내 로컬 좌표 (0~1 범위) 계산
	_float fU = fLocalX - iX; // X축 방향 비율 (0~1)
	_float fV = fLocalZ - iZ; // Z축 방향 비율 (0~1)

	// 5. 삼각형 선택 및 보간
	// 사각형을 나누는 두 삼각형 중 어느 쪽에 속하는지 판별
	_float fInterPolatedY = 0.0f;

	if (fU + fV <= 1.0f) // 삼각형 1 (LT - LB - RT)
	{
		// baricentric 좌표 또는 간단한 선형 보간
		// fInterPolatedY = (1 - fU - fV) * vPosLT.y + fU * vPosRT.y + fV * vPosLB.y;

		// 선형 보간 (X, Z 평면에서 평평하다고 가정)
		_float fY_X = vPosLT.y + fU * (vPosRT.y - vPosLT.y); // X축 방향 보간된 높이
		_float fY_Z = vPosLT.y + fV * (vPosLB.y - vPosLT.y); // Z축 방향 보간된 높이

		fInterPolatedY = fY_X + (fV * (vPosLB.y - fY_X));
	}
	else // 삼각형 2 (RB - RT - LB) 또는 (LB - RB - RT)
	{
		_float fU_prime = fU - 1.0f; // (fU - 1)
		_float fV_prime = fV - 1.0f; // (fV - 1)


		_float fY_X = vPosRB.y + fU_prime * (vPosLB.y - vPosRB.y); // X축 방향 보간된 높이
		_float fY_Z = vPosRB.y + fV_prime * (vPosRT.y - vPosRB.y); // Z축 방향 보간된 높이

		fInterPolatedY = fY_X + (fV_prime * (vPosRT.y - fY_X));
	}


	return fInterPolatedY;
}

HRESULT CVIBuffer_Terrain::Initialize(void* pArg)
{
	return S_OK;
}

void CVIBuffer_Terrain::Culling(_fmatrix WorldMatrix)
{
	m_pGameInstance->Transform_Frustum_ToLocalSpace(XMMatrixInverse(nullptr, WorldMatrix));

	D3D11_MAPPED_SUBRESOURCE	SubResource{};

	m_pContext->Map(m_pIB, 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResource);

	_uint* pIndices = static_cast<_uint*>(SubResource.pData);

	_uint		iNumIndices = {};

	m_pQuadTree->Culling(m_pGameInstance, m_pVertexPositions, pIndices, &iNumIndices);

	/* 
	for (size_t i = 0; i < m_iNumVerticesZ - 1; i++)
	{
		for (size_t j = 0; j < m_iNumVerticesX - 1; j++)
		{
			_uint		iIndex = i * m_iNumVerticesX + j;

			_uint		iIndices[4] = {
				iIndex + m_iNumVerticesX,
				iIndex + m_iNumVerticesX + 1,
				iIndex + 1,
				iIndex
			};	

			_bool		isIn[4] = {
				m_pGameInstance->isIn_LocalFrustum(XMLoadFloat3(&m_pVertexPositions[iIndices[0]])),
				m_pGameInstance->isIn_LocalFrustum(XMLoadFloat3(&m_pVertexPositions[iIndices[1]])),
				m_pGameInstance->isIn_LocalFrustum(XMLoadFloat3(&m_pVertexPositions[iIndices[2]])),
				m_pGameInstance->isIn_LocalFrustum(XMLoadFloat3(&m_pVertexPositions[iIndices[3]])),
			};

			if (true == isIn[0] &&
				true == isIn[1] &&
				true == isIn[2])
			{
				pIndices[iNumIndices++] = iIndices[0];
				pIndices[iNumIndices++] = iIndices[1];
				pIndices[iNumIndices++] = iIndices[2];
			}

			if (true == isIn[0] &&
				true == isIn[2] &&
				true == isIn[3])
			{
				pIndices[iNumIndices++] = iIndices[0];
				pIndices[iNumIndices++] = iIndices[2];
				pIndices[iNumIndices++] = iIndices[3];
			}
		}
	}
	*/

	m_pContext->Unmap(m_pIB, 0);

	m_iNumIndices = iNumIndices;
}

void CVIBuffer_Terrain::Change_Height_Rect(_vector vPickingPos, _float fHeight, _float fRadius)
{
	_float fHalfWidth = fRadius; 
	_float fHalfDepth = fRadius; 

	_float fMinX = XMVectorGetX(vPickingPos) - fHalfWidth;
	_float fMaxX = XMVectorGetX(vPickingPos) + fHalfWidth;
	_float fMinZ = XMVectorGetZ(vPickingPos) - fHalfDepth;
	_float fMaxZ = XMVectorGetZ(vPickingPos) + fHalfDepth;

	for (size_t i = 0; i < m_iNumVertices; i++)
	{
		_vector vVertexPos = XMLoadFloat3(&m_pVertexPositions[i]);
		_float fVertexX = XMVectorGetX(vVertexPos);
		_float fVertexZ = XMVectorGetZ(vVertexPos);

		// 완전 평평한 사각형으로 일단 변경
		if (fVertexX >= fMinX && fVertexX <= fMaxX && fVertexZ >= fMinZ && fVertexZ <= fMaxZ)
		{
			m_pVertexPositions[i].y = fHeight;
		}
	}

	// 2. 변경된 m_pVertexPositions를 기반으로 새로운 정점 데이터(VTXNORTEX)를 생성
	VTXNORTEX* pVertices = new VTXNORTEX[m_iNumVertices];
	for (size_t i = 0; i < m_iNumVertices; i++)
	{
		pVertices[i].vPosition = m_pVertexPositions[i];
		// 노멀은 나중에 다시 계산할 것이므로 일단 초기화
		pVertices[i].vNormal = _float3(0.f, 0.f, 0.f);
		pVertices[i].vTexcoord = _float2(m_pVertexPositions[i].x / (m_iNumVerticesX - 1.f), m_pVertexPositions[i].z / (m_iNumVerticesZ - 1.f));
	}

	// 3. 인덱스 배열을 이용해 노멀 재계산
	for (size_t i = 0; i < m_iNumVerticesZ - 1; i++)
	{
		for (size_t j = 0; j < m_iNumVerticesX - 1; j++)
		{
			_uint iIndex = i * m_iNumVerticesX + j;
			_uint iIndices[4] = { iIndex + m_iNumVerticesX, iIndex + m_iNumVerticesX + 1, iIndex + 1, iIndex };

			// 삼각형 1 (iIndices[0], iIndices[1], iIndices[2])
			_vector vSourDir1 = XMLoadFloat3(&pVertices[iIndices[1]].vPosition) - XMLoadFloat3(&pVertices[iIndices[0]].vPosition);
			_vector vDestDir1 = XMLoadFloat3(&pVertices[iIndices[2]].vPosition) - XMLoadFloat3(&pVertices[iIndices[1]].vPosition);
			_vector vNormal1 = XMVector3Normalize(XMVector3Cross(vSourDir1, vDestDir1));

			XMStoreFloat3(&pVertices[iIndices[0]].vNormal, XMLoadFloat3(&pVertices[iIndices[0]].vNormal) + vNormal1);
			XMStoreFloat3(&pVertices[iIndices[1]].vNormal, XMLoadFloat3(&pVertices[iIndices[1]].vNormal) + vNormal1);
			XMStoreFloat3(&pVertices[iIndices[2]].vNormal, XMLoadFloat3(&pVertices[iIndices[2]].vNormal) + vNormal1);

			// 삼각형 2 (iIndices[0], iIndices[2], iIndices[3])
			_vector vSourDir2 = XMLoadFloat3(&pVertices[iIndices[2]].vPosition) - XMLoadFloat3(&pVertices[iIndices[0]].vPosition);
			_vector vDestDir2 = XMLoadFloat3(&pVertices[iIndices[3]].vPosition) - XMLoadFloat3(&pVertices[iIndices[2]].vPosition);
			_vector vNormal2 = XMVector3Normalize(XMVector3Cross(vSourDir2, vDestDir2));

			XMStoreFloat3(&pVertices[iIndices[0]].vNormal, XMLoadFloat3(&pVertices[iIndices[0]].vNormal) + vNormal2);
			XMStoreFloat3(&pVertices[iIndices[2]].vNormal, XMLoadFloat3(&pVertices[iIndices[2]].vNormal) + vNormal2);
			XMStoreFloat3(&pVertices[iIndices[3]].vNormal, XMLoadFloat3(&pVertices[iIndices[3]].vNormal) + vNormal2);
		}
	}

	// 4. 노멀 벡터 정규화
	for (size_t i = 0; i < m_iNumVertices; i++)
	{
		XMStoreFloat3(&pVertices[i].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[i].vNormal)));
	}

	m_pContext->UpdateSubresource(
		m_pVB,                      // 대상 버퍼
		0,                          // 서브 리소스 인덱스
		nullptr,                    // 전체 영역 업데이트
		pVertices,                  // 원본 데이터 (CPU 메모리)
		m_iVertexStride,            // 한 정점의 크기
		0                           // 사용하지 않음
	);

	Safe_Delete_Array(pVertices);
}

void CVIBuffer_Terrain::Change_Height_Flat(_vector vPickingPos, _float fHeight, _float fRadius)
{
	_float fRadiusSq = fRadius * fRadius;
	for (size_t i = 0; i < m_iNumVertices; i++)
	{
		_vector vVertexPos = XMLoadFloat3(&m_pVertexPositions[i]);
		_vector vDist = vVertexPos - vPickingPos;
		_float fDistSq = XMVectorGetX(XMVector3Dot(vDist, vDist));

		if (fDistSq < fRadiusSq)
		{
			m_pVertexPositions[i].y = fHeight;
		}
	}

	VTXNORTEX* pVertices = new VTXNORTEX[m_iNumVertices];
	for (size_t i = 0; i < m_iNumVertices; i++)
	{
		pVertices[i].vPosition = m_pVertexPositions[i];
		pVertices[i].vNormal = _float3(0.f, 0.f, 0.f);
		pVertices[i].vTexcoord = _float2(m_pVertexPositions[i].x / (m_iNumVerticesX - 1.f), m_pVertexPositions[i].z / (m_iNumVerticesZ - 1.f));
	}

	for (size_t i = 0; i < m_iNumVerticesZ - 1; i++)
	{
		for (size_t j = 0; j < m_iNumVerticesX - 1; j++)
		{
			_uint iIndex = i * m_iNumVerticesX + j;
			_uint iIndices[4] = { iIndex + m_iNumVerticesX, iIndex + m_iNumVerticesX + 1, iIndex + 1, iIndex };

			_vector vSourDir1 = XMLoadFloat3(&pVertices[iIndices[1]].vPosition) - XMLoadFloat3(&pVertices[iIndices[0]].vPosition);
			_vector vDestDir1 = XMLoadFloat3(&pVertices[iIndices[2]].vPosition) - XMLoadFloat3(&pVertices[iIndices[1]].vPosition);
			_vector vNormal1 = XMVector3Normalize(XMVector3Cross(vSourDir1, vDestDir1));

			XMStoreFloat3(&pVertices[iIndices[0]].vNormal, XMLoadFloat3(&pVertices[iIndices[0]].vNormal) + vNormal1);
			XMStoreFloat3(&pVertices[iIndices[1]].vNormal, XMLoadFloat3(&pVertices[iIndices[1]].vNormal) + vNormal1);
			XMStoreFloat3(&pVertices[iIndices[2]].vNormal, XMLoadFloat3(&pVertices[iIndices[2]].vNormal) + vNormal1);

			_vector vSourDir2 = XMLoadFloat3(&pVertices[iIndices[2]].vPosition) - XMLoadFloat3(&pVertices[iIndices[0]].vPosition);
			_vector vDestDir2 = XMLoadFloat3(&pVertices[iIndices[3]].vPosition) - XMLoadFloat3(&pVertices[iIndices[2]].vPosition);
			_vector vNormal2 = XMVector3Normalize(XMVector3Cross(vSourDir2, vDestDir2));

			XMStoreFloat3(&pVertices[iIndices[0]].vNormal, XMLoadFloat3(&pVertices[iIndices[0]].vNormal) + vNormal2);
			XMStoreFloat3(&pVertices[iIndices[2]].vNormal, XMLoadFloat3(&pVertices[iIndices[2]].vNormal) + vNormal2);
			XMStoreFloat3(&pVertices[iIndices[3]].vNormal, XMLoadFloat3(&pVertices[iIndices[3]].vNormal) + vNormal2);
		}
	}

	for (size_t i = 0; i < m_iNumVertices; i++)
	{
		XMStoreFloat3(&pVertices[i].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[i].vNormal)));
	}

	m_pContext->UpdateSubresource(
		m_pVB,                      // 대상 버퍼
		0,                          // 서브 리소스 인덱스
		nullptr,                    // 전체 영역 업데이트
		pVertices,                  // 원본 데이터 (CPU 메모리)
		m_iVertexStride,            // 한 정점의 크기
		0                           // 사용하지 않음
	);

	Safe_Delete_Array(pVertices);
}

CVIBuffer_Terrain* CVIBuffer_Terrain::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pHeightMapFilePath)
{
	CVIBuffer_Terrain* pInstance = new CVIBuffer_Terrain(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pHeightMapFilePath)))
	{
		MSG_BOX("Failed to Created : CVIBuffer_Terrain");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CVIBuffer_Terrain::Clone(void* pArg)
{
	CVIBuffer_Terrain* pInstance = new CVIBuffer_Terrain(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CVIBuffer_Terrain");
		Safe_Release(pInstance);
	}

	return pInstance;
}
void CVIBuffer_Terrain::Free()
{
	__super::Free();

	Safe_Release(m_pQuadTree);
}
