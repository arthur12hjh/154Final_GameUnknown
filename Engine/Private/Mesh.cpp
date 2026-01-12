#include "Mesh.h"

#include "Bone.h"
#include "Model.h"
#include "ShapeKey.h"
#include "Shader.h"

CMesh::CMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer { pDevice, pContext }
{
}

CMesh::CMesh(const CMesh& Prototype)
	: CVIBuffer{ Prototype }
	, m_iNumShapeKeys{ Prototype.m_iNumShapeKeys }
	, m_pCombinedPositionsSRV{Prototype.m_pCombinedPositionsSRV }
	, m_pCombinedNormalsSRV{Prototype.m_pCombinedNormalsSRV }
{
	Safe_AddRef(m_pCombinedPositionsSRV);
	Safe_AddRef(m_pCombinedNormalsSRV);

	m_ShapeKeys.reserve(Prototype.m_ShapeKeys.size());
	for (auto& pShapeKey : Prototype.m_ShapeKeys)
	{
		m_ShapeKeys.push_back(pShapeKey);
		Safe_AddRef(pShapeKey);
		m_ShapeKeyWeights.push_back(0.f);
	}
}

void CMesh::Reset_ShapeWeight()
{
	fill(m_ShapeKeyWeights.begin(), m_ShapeKeyWeights.end(), 0.f);
}

void CMesh::Set_ShapeWeight(_uint iShapeKeyIndex, _float fWeight)
{
	if (iShapeKeyIndex >= m_ShapeKeyWeights.size())
		return;

	m_ShapeKeyWeights[iShapeKeyIndex] = fWeight;
}


HRESULT CMesh::Bind_ShapeKeys(CShader* pShader, const _char* pConstantName)
{
	/*if (m_ShapeKeyWeights.empty())
	return S_OK;

	pShader->Bind_RawValue(pConstantName, m_ShapeKeyWeights.data(), sizeof(float) * (_uint)m_ShapeKeyWeights.size());*/

	//pShader->Bind_SRV("g_ShapeKeyDeltaPositionBuffer", m_pCombinedShapeKeySRV);

	if (m_pCombinedPositionsSRV == nullptr)
		return E_FAIL;

	if (FAILED(pShader->Bind_RawValue("g_iNumVertices",
		&m_iNumVertices, sizeof(_uint))))
		return E_FAIL;

	if (FAILED(pShader->Bind_SRV("g_ShapeKeyDeltaPositionsBuffer", m_pCombinedPositionsSRV)))
		return E_FAIL;
	
	if (FAILED(pShader->Bind_SRV("g_ShapeKeyDeltaNormalsBuffer", m_pCombinedNormalsSRV)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMesh::Initialize_Prototype(MODEL_TYPE eType, const class CModel* pModel, const binMesh* pBinMesh, _fmatrix PreTransformMatrix)
{
	strcpy_s(m_szName, pBinMesh->szName);

	m_iMaterialIndex = pBinMesh->iMaterialIndex;
	m_iNumVertexBuffers = 1;
	m_iNumVertices = pBinMesh->iNumVertices;
	
	m_iNumIndices = pBinMesh->iNumFaces * 3;
	m_iIndexStride = 4;
	
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_ePrimitive = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

#pragma region VERTEX_BUFFER

	HRESULT hr = MODEL_TYPE::NONANIM == eType ?
		Ready_VertexBuffer_For_NonAnim(pBinMesh, PreTransformMatrix) :
		Ready_VertexBuffer_For_Anim(pModel, pBinMesh);

	if (FAILED(hr))
		return E_FAIL;

#pragma endregion

#pragma region INDEX_BUFFER
	D3D11_BUFFER_DESC		IBDesc{};
	IBDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	IBDesc.Usage = D3D11_USAGE_DEFAULT;
	IBDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	IBDesc.StructureByteStride = m_iIndexStride;
	IBDesc.CPUAccessFlags = 0;
	IBDesc.MiscFlags = 0;


	m_pIndices = new _uint[m_iNumIndices];
	ZeroMemory(m_pIndices, sizeof(_uint) * m_iNumIndices);

	_uint	iNumIndices = {};

	for (size_t i = 0; i < pBinMesh->iNumFaces; i++)
	{		
		m_pIndices[iNumIndices++] = pBinMesh->vFaces[i].vIndices[0];
		m_pIndices[iNumIndices++] = pBinMesh->vFaces[i].vIndices[1];
		m_pIndices[iNumIndices++] = pBinMesh->vFaces[i].vIndices[2];
	}


	D3D11_SUBRESOURCE_DATA	InitialIBData{};
	InitialIBData.pSysMem = m_pIndices;

	if (FAILED(m_pDevice->CreateBuffer(&IBDesc, &InitialIBData, &m_pIB)))
		return E_FAIL;

#pragma endregion
	
	if (eType == MODEL_TYPE::FACIAL)
	{
		if (FAILED(Ready_ShapeKeys(pModel, pBinMesh)))
			return E_FAIL;

		if (FAILED(Ready_CombinedShapeKeySRVs()))
			return E_FAIL;

		//Set_ShapeWeight(0, 0.f);
	}
	

	return S_OK;
}

HRESULT CMesh::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CMesh::Bind_BoneMatrices(const vector<CBone*>& Bones, CShader* pShader, const _char* pConstantName)
{
	if (m_iNumBones >= 512)
		return E_FAIL;


	for (size_t i = 0; i < m_iNumBones; i++)
	{
		/* 최종적으로 렌더링하기위한 뼈의 행렬(CombinedTransformationMatrix). */
		XMStoreFloat4x4(&m_pBoneMatrices[i], XMLoadFloat4x4(&m_OffsetMatrices[i]) * Bones[m_BoneIndices[i]]->Get_CombinedTransformationMatrix());
	}

	if (0 == m_iNumBones)
		return S_OK;

	if (FAILED(pShader->Bind_Matrices(pConstantName, m_pBoneMatrices, m_iNumBones)))
	{
		MSG_BOX("Bind OffsetMatrices Failed");
		return E_FAIL;
	}
	else
		return S_OK;
	
	
}

HRESULT CMesh::Bind_OffsetMatrices(const vector<class CBone*>& Bones, CShader* pShader, const _char* pConstantName)
{
	if (m_iNumBones >= 512)
		return E_FAIL;

	for (size_t i = 0; i < m_iNumBones; i++)
	{
		/* 최종적으로 렌더링하기위한 뼈의 행렬(CombinedTransformationMatrix). */
		XMStoreFloat4x4(&m_pBoneMatrices[i], XMLoadFloat4x4(&m_OffsetMatrices[i]));
	}

	if (m_iNumBones > 0)
		pShader->Bind_Matrices(pConstantName, m_pBoneMatrices, m_iNumBones);

	_uint BoneGlobalIndex[512] = {};
	for (size_t i = 0; i < m_iNumBones; i++)
		BoneGlobalIndex[i] = m_BoneIndices[i];   // 메시 로컬 i → 전역 본 인덱스

	// 상수 배열로 바인딩 (shader variable: uint g_BoneGlobalIndex[512];)
	pShader->Bind_RawValue("g_BoneGlobalIndex", BoneGlobalIndex, sizeof(_uint) * m_iNumBones);

	return S_OK;
}

HRESULT CMesh::Ready_VertexBuffer_For_NonAnim(const binMesh* pBinMesh, _fmatrix PreTransformMatrix)
{
	m_iVertexStride = sizeof(VTXMESH);
	D3D11_BUFFER_DESC		VBDesc{};
	VBDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	VBDesc.Usage = D3D11_USAGE_DEFAULT;
	VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	VBDesc.StructureByteStride = m_iVertexStride;
	VBDesc.CPUAccessFlags = 0;
	VBDesc.MiscFlags = 0;

	VTXMESH* pVertices = new VTXMESH[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXMESH) * m_iNumVertices);

	m_pVertexPositions = new _float3[m_iNumVertices];
	ZeroMemory(m_pVertexPositions, sizeof(_float3) * m_iNumVertices);

	for (_uint i = 0; i < m_iNumVertices; ++i)
	{
		memcpy(&pVertices[i].vPosition, &pBinMesh->vPositions[i], sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vPosition, XMVector3TransformCoord(XMLoadFloat3(&pVertices[i].vPosition), PreTransformMatrix));
		memcpy(&m_pVertexPositions[i], &pVertices[i].vPosition, sizeof(_float3));

		memcpy(&pVertices[i].vNormal, &pBinMesh->vNormals[i], sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vNormal, XMVector3Normalize(XMVector3TransformNormal(XMLoadFloat3(&pVertices[i].vNormal), PreTransformMatrix)));

		memcpy(&pVertices[i].vTangent, &pBinMesh->vTangents[i], sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vTangent, XMVector3Normalize(XMVector3TransformNormal(XMLoadFloat3(&pVertices[i].vTangent), PreTransformMatrix)));

		memcpy(&pVertices[i].vBinormal, &pBinMesh->vBinormals[i], sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vBinormal, XMVector3Normalize(XMVector3TransformNormal(XMLoadFloat3(&pVertices[i].vBinormal), PreTransformMatrix)));


		

		memcpy(&pVertices[i].vTexcoord, &pBinMesh->vTextureCoords[0][i], sizeof(_float2));
	}

	D3D11_SUBRESOURCE_DATA	InitialVBData{};
	InitialVBData.pSysMem = pVertices;

	if (FAILED(m_pDevice->CreateBuffer(&VBDesc, &InitialVBData, &m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

	return S_OK;
}

HRESULT CMesh::Ready_VertexBuffer_For_Anim(const CModel* pModel, const binMesh* pBinMesh)
{
	m_iVertexStride = sizeof(VTXANIMMESH);
	D3D11_BUFFER_DESC		VBDesc{};
	VBDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	VBDesc.Usage = D3D11_USAGE_DEFAULT;
	VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	VBDesc.StructureByteStride = m_iVertexStride;
	VBDesc.CPUAccessFlags = 0;
	VBDesc.MiscFlags = 0;

	VTXANIMMESH* pVertices = new VTXANIMMESH[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXANIMMESH) * m_iNumVertices);

	m_pVertexPositions = new _float3[m_iNumVertices];
	ZeroMemory(m_pVertexPositions, sizeof(_float3) * m_iNumVertices);

	for (_uint i = 0; i < m_iNumVertices; ++i)
	{
		memcpy(&pVertices[i].vPosition, &pBinMesh->vPositions[i], sizeof(_float3));
		memcpy(&pVertices[i].vNormal, &pBinMesh->vNormals[i], sizeof(_float3));
		memcpy(&pVertices[i].vTangent, &pBinMesh->vTangents[i], sizeof(_float3));
		memcpy(&pVertices[i].vBinormal, &pBinMesh->vBinormals[i], sizeof(_float3));
		memcpy(&pVertices[i].vTexcoord, &pBinMesh->vTextureCoords[0][i], sizeof(_float2));
	}

	m_iNumBones = pBinMesh->iNumBones;

	m_OffsetMatrices.reserve(m_iNumBones);

	m_pBoneMatrices = new _float4x4[0 == m_iNumBones ? 1 : m_iNumBones];
	ZeroMemory(m_pBoneMatrices, sizeof(_float4x4) * m_iNumBones);

	_float4x4		OffsetMatrix;
	XMStoreFloat4x4(&OffsetMatrix, XMMatrixIdentity());

	for (size_t i = 0; i < m_iNumBones; i++)
	{
		binBone		binBoneTmp = pBinMesh->vBones[i];

		memcpy(&OffsetMatrix, &binBoneTmp.OffsetMatrix, sizeof(_float4x4));
		XMStoreFloat4x4(&OffsetMatrix, XMMatrixTranspose(XMLoadFloat4x4(&OffsetMatrix)));		

		_int		iBoneIndex = pModel->Get_BoneIndex(binBoneTmp.szName);

		// 본 매트릭스 부착을 위해서라면 여기를 바꿔줘야함.
		if (-1 == iBoneIndex)
		{
			// 
			iBoneIndex = 0;
		}

		m_OffsetMatrices.push_back(OffsetMatrix);
		m_BoneIndices.push_back(iBoneIndex);

		/* 이 뼈는 몇개의 정점에게 영향을 주는가? */
		for (size_t j = 0; j < binBoneTmp.iNumWeights; j++)
		{
			binVertexWeight	binWeight = binBoneTmp.vWeights[j];

			if(0.f == pVertices[binWeight.iVertexId].vBlendWeight.x)
			{
				pVertices[binWeight.iVertexId].vBlendIndex.x = iBoneIndex;
				pVertices[binWeight.iVertexId].vBlendWeight.x = binWeight.fWeight;
			}

			else if (0.f == pVertices[binWeight.iVertexId].vBlendWeight.y)
			{
				pVertices[binWeight.iVertexId].vBlendIndex.y = iBoneIndex;
				pVertices[binWeight.iVertexId].vBlendWeight.y = binWeight.fWeight;
			}

			else if (0.f == pVertices[binWeight.iVertexId].vBlendWeight.z)
			{
				pVertices[binWeight.iVertexId].vBlendIndex.z = iBoneIndex;
				pVertices[binWeight.iVertexId].vBlendWeight.z = binWeight.fWeight;
			}

			else if(0.f == pVertices[binWeight.iVertexId].vBlendWeight.w)
			{
				pVertices[binWeight.iVertexId].vBlendIndex.w = iBoneIndex;
				pVertices[binWeight.iVertexId].vBlendWeight.w = binWeight.fWeight;
			}
			else if (0.f != pVertices[binWeight.iVertexId].vBlendWeight.w)
			{
				pVertices[binWeight.iVertexId].vBlendIndex.w = iBoneIndex;
				pVertices[binWeight.iVertexId].vBlendWeight.w = binWeight.fWeight;
			}
		}
	}

	if (0 == m_iNumBones)
	{
		m_iNumBones = 1;		

		m_BoneIndices.push_back(pModel->Get_BoneIndex(m_szName));

		m_OffsetMatrices.push_back(OffsetMatrix);
	}

	D3D11_SUBRESOURCE_DATA	InitialVBData{};
	InitialVBData.pSysMem = pVertices;

	if (FAILED(m_pDevice->CreateBuffer(&VBDesc, &InitialVBData, &m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

	return S_OK;
}

HRESULT CMesh::Ready_ShapeKeys(const class CModel* pModel, const binMesh* pBinMesh)
{
	m_iNumShapeKeys = pBinMesh->iNumAnimMeshes;

	for (size_t i = 0; i < m_iNumShapeKeys; i++)
	{
		CShapeKey* pShapeKey = CShapeKey::Create(pModel, &pBinMesh->vAnimMesh[i]);
		if (nullptr == pShapeKey)
			return E_FAIL;

		m_ShapeKeys.push_back(pShapeKey);
	}

	m_ShapeKeyWeights.resize(m_ShapeKeys.size(), 0.f);

	return S_OK;
}

HRESULT CMesh::Ready_ShapeKeyBuffers()
{
	D3D11_BUFFER_DESC ConstantBufferDesc = {};
	ConstantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	ConstantBufferDesc.ByteWidth = sizeof(CB_MORPH);
	ConstantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	ConstantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	return m_pDevice->CreateBuffer(&ConstantBufferDesc, nullptr, &m_pShapeKeyWeightsBuffer);
}

HRESULT CMesh::Ready_CombinedShapeKeySRVs()
{
	// 1. ShapeKey 수와 Vertex 수를 받아와 그 크기만큼의 vector<_float3>를 만든다. : ShapeKey[164] * vDeltaPosition[9124] * _float3
	// 2. ShapeKey를 모두 순회하며 vector<_float3>의 vDeltaIndex + 9124 * ShapeKeyIndex 위치에 집어넣는다.
	// 3. ID3D11Buffer* 로 만든다.
	// 4. ID3D11ShaderResourceView* 로 만든다.
	
	vector<_float3> vCombinedDeltaPositions(m_iNumShapeKeys * m_iNumVertices);
	vector<_float3> vCombinedDeltaNormals(m_iNumShapeKeys * m_iNumVertices);

	for (_uint i = 0; i < m_iNumShapeKeys; ++i)
	{
		// CShapeKey* const& pShapeKey
		const auto& pShapeKey = m_ShapeKeys[i];
		const auto& vDeltaPositions = *pShapeKey->Get_DeltaPosition();
		const auto& vDeltaNormals = *pShapeKey->Get_DeltaNormals();

		if ((_uint)vDeltaPositions.size() != m_iNumVertices)
			return E_FAIL;

		for (_uint j = 0; j < vDeltaPositions.size(); ++j)
		{
			_uint iCombinedIndex = j + vDeltaPositions.size() * i;
			vCombinedDeltaPositions[iCombinedIndex] = vDeltaPositions[j];
			vCombinedDeltaNormals[iCombinedIndex] = vDeltaNormals[j];
		}

	}

	ID3D11Buffer* pCombinedPositionsBuffer = nullptr;
	ID3D11Buffer* pCombinedNormalsBuffer = nullptr;

	D3D11_BUFFER_DESC CombinedPositionsBufferDesc = {};
	CombinedPositionsBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	CombinedPositionsBufferDesc.ByteWidth = sizeof(_float3) * vCombinedDeltaPositions.size();
	CombinedPositionsBufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	CombinedPositionsBufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	CombinedPositionsBufferDesc.StructureByteStride = sizeof(_float3);

	D3D11_SUBRESOURCE_DATA CombinedPositionsSubResource{};
	CombinedPositionsSubResource.pSysMem = vCombinedDeltaPositions.data();


	if (FAILED(m_pDevice->CreateBuffer(&CombinedPositionsBufferDesc, &CombinedPositionsSubResource, &pCombinedPositionsBuffer)))
		return E_FAIL;

	D3D11_SHADER_RESOURCE_VIEW_DESC CombinedPositionsSRVDesc{};
	CombinedPositionsSRVDesc.Format = DXGI_FORMAT_UNKNOWN;
	CombinedPositionsSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	CombinedPositionsSRVDesc.Buffer.FirstElement = 0;
	CombinedPositionsSRVDesc.Buffer.NumElements = (_uint)vCombinedDeltaPositions.size();

	if (FAILED(m_pDevice->CreateShaderResourceView(pCombinedPositionsBuffer, &CombinedPositionsSRVDesc, &m_pCombinedPositionsSRV)))
		return E_FAIL;

	Safe_Release(pCombinedPositionsBuffer);

	D3D11_BUFFER_DESC CombinedRotationsBufferDesc = {};
	CombinedRotationsBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	CombinedRotationsBufferDesc.ByteWidth = sizeof(_float3) * vCombinedDeltaNormals.size();
	CombinedRotationsBufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	CombinedRotationsBufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	CombinedRotationsBufferDesc.StructureByteStride = sizeof(_float3);

	D3D11_SUBRESOURCE_DATA CombinedRotationsSubResource{};
	CombinedRotationsSubResource.pSysMem = vCombinedDeltaNormals.data();


	if (FAILED(m_pDevice->CreateBuffer(&CombinedRotationsBufferDesc, &CombinedRotationsSubResource, &pCombinedNormalsBuffer)))
		return E_FAIL;

	D3D11_SHADER_RESOURCE_VIEW_DESC CombinedNormalsSRVDesc{};
	CombinedNormalsSRVDesc.Format = DXGI_FORMAT_UNKNOWN;
	CombinedNormalsSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	CombinedNormalsSRVDesc.Buffer.FirstElement = 0;
	CombinedNormalsSRVDesc.Buffer.NumElements = (_uint)vCombinedDeltaNormals.size();

	if (FAILED(m_pDevice->CreateShaderResourceView(pCombinedNormalsBuffer, &CombinedNormalsSRVDesc, &m_pCombinedNormalsSRV)))
		return E_FAIL;

	Safe_Release(pCombinedNormalsBuffer);


	return S_OK;
}

CMesh* CMesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODEL_TYPE eType, const CModel* pModel, const binMesh* pBinMesh, _fmatrix PreTransformMatrix)
{
	CMesh* pInstance = new CMesh(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eType, pModel, pBinMesh, PreTransformMatrix)))
	{ 
		MSG_BOX("Failed to Created : CMesh");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CMesh::Clone(void* pArg)
{
	CMesh* pInstance = new CMesh(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMesh");
		Safe_Release(pInstance);
	}

	return pInstance;
}
void CMesh::Free()
{
	__super::Free();


	for (auto& pShapeKey : m_ShapeKeys)
		Safe_Release(pShapeKey);
	m_ShapeKeys.clear();


	Safe_Delete_Array(m_pBoneMatrices);

	if(false == m_isCloned)
		Safe_Delete_Array(m_pIndices);

	Safe_Release(m_pShapeKeyWeightsBuffer);
	Safe_Release(m_pCombinedPositionsSRV);
	Safe_Release(m_pCombinedNormalsSRV);

}
