#include "VIBuffer_Instance_Model.h"

#include "GameInstance.h"

CVIBuffer_Instance_Model::CVIBuffer_Instance_Model(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) :
	CVIBuffer_Instance(pDevice, pContext)
{
}

CVIBuffer_Instance_Model::CVIBuffer_Instance_Model(const CVIBuffer_Instance_Model& Prototype) :
	CVIBuffer_Instance(Prototype),
	m_pModel(Prototype.m_pModel),
	m_pInstanceVertices(Prototype.m_pInstanceVertices)
{
	Safe_AddRef(m_pModel);
}

HRESULT CVIBuffer_Instance_Model::Initialize_Prototype(const INSTANCE_DESC* pInstanceDesc)
{
	const MODEL_INSTANCE_DESC* pDesc = static_cast<const MODEL_INSTANCE_DESC*>(pInstanceDesc);
	m_pModel = CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM,
		pDesc->pModelFilePath,
		pDesc->PreModelMatrix);

	if (nullptr == m_pModel)
		return E_FAIL;

	m_iNumVertexBuffers = 2;
	m_ePrimitive = D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	m_iNumInstance = pDesc->iNumInstance;
	m_iInstanceStride = sizeof(VTX_INSTANCE_MODEL);
	m_iNumIndexPerInstance = 6;

	m_InstanceBufferDesc.ByteWidth = m_iInstanceStride * m_iNumInstance;
	m_InstanceBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_InstanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_InstanceBufferDesc.StructureByteStride = m_iInstanceStride;
	m_InstanceBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_InstanceBufferDesc.MiscFlags = 0;

	m_pInstanceVertices = new VTX_INSTANCE_MODEL[m_iNumInstance];
	ZeroMemory(m_pInstanceVertices, sizeof(VTX_INSTANCE_MODEL) * m_iNumInstance);

	_matrix			ScaleMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		_matrix			RotationY = XMMatrixRotationY(XMConvertToRadians(m_pGameInstance->Random(0.f, 360.f)));
		_matrix			TransformMatrix = XMMatrixTranslation(m_pGameInstance->Random(pDesc->vCenter.x - pDesc->vRange.x * 0.5f, pDesc->vCenter.x + pDesc->vRange.x * 0.5f),
			m_pGameInstance->Random(pDesc->vCenter.y - pDesc->vRange.y * 0.5f, pDesc->vCenter.y + pDesc->vRange.y * 0.5f),
			m_pGameInstance->Random(pDesc->vCenter.z - pDesc->vRange.z * 0.5f, pDesc->vCenter.z + pDesc->vRange.z * 0.5f));

		_matrix			vWorldMat = ScaleMatrix * RotationY * TransformMatrix;
		XMStoreFloat4(&m_pInstanceVertices[i].vRight, vWorldMat.r[0]);
		XMStoreFloat4(&m_pInstanceVertices[i].vUp, vWorldMat.r[1]);
		XMStoreFloat4(&m_pInstanceVertices[i].vLook, vWorldMat.r[2]);
		XMStoreFloat4(&m_pInstanceVertices[i].vTranslation, vWorldMat.r[3]);
	}

	m_InstanceInitialDesc.pSysMem = m_pInstanceVertices;
	return S_OK;
}

HRESULT CVIBuffer_Instance_Model::Initialize(void* pArg)
{
	/*if (FAILED(m_pDevice->CreateBuffer(&m_InstanceBufferDesc, &m_InstanceInitialDesc, &m_pVBInstance)))
		return E_FAIL;*/

	if (nullptr != pArg)
	{
		const Engine::MODEL_INSTANCE_LOAD_DESC* pLoadDesc = static_cast<const Engine::MODEL_INSTANCE_LOAD_DESC*>(pArg);

		if (pLoadDesc->pInstancingData != nullptr)
		{

			m_pInstanceVertices = nullptr;

			// ?ロ???? ?????? ?ュ?? ?????? ????
			m_iNumInstance = pLoadDesc->iNumInstance;

			// ?ロ???? ???? ??????? ???????
			m_InstanceBufferDesc.ByteWidth = m_iInstanceStride * m_iNumInstance;

			// ???? ???? ?? ????? ????? ??? ??????? ?ュ?? ???????? ???? ???? ????
			m_InstanceInitialDesc.pSysMem = pLoadDesc->pInstancingData->data();
		}
	}
	Safe_Release(m_pVBInstance);
	// 2. ?ロ???? ???? ????
	if (FAILED(m_pDevice->CreateBuffer(&m_InstanceBufferDesc, &m_InstanceInitialDesc, &m_pVBInstance)))
		return E_FAIL;

	return S_OK;
}

HRESULT CVIBuffer_Instance_Model::Render(_uint iIndex)
{
	Bind_Resource(iIndex);

	m_iNumIndices = m_pModel->Get_MeshIndices(iIndex);
	m_pContext->DrawIndexedInstanced(m_iNumIndices, m_iNumInstance, 0, 0, 0);

	return S_OK;
}

HRESULT CVIBuffer_Instance_Model::Bind_MatrialTexture(CShader* pShader, _uint iMeshIndex, const _char* pConstantName, aiTextureType eTextureType, _uint TextureIndex)
{
	if (nullptr == m_pModel)
		return E_FAIL;

	return m_pModel->Bind_Material(iMeshIndex, pShader, pConstantName, eTextureType, TextureIndex);
}

_uint CVIBuffer_Instance_Model::GetModelNumMeshes()
{
	return m_pModel->Get_NumMeshes();
}

HRESULT CVIBuffer_Instance_Model::Bind_Resource(_uint iMeshIndex)
{
	if (nullptr == m_pModel)
		return E_FAIL;

	ID3D11Buffer* pModelVertexBuffer = nullptr;
	ID3D11Buffer* pMoelIndexBuffer = nullptr;

	m_pModel->Copy_MeshBuffer(iMeshIndex, &pModelVertexBuffer, &pMoelIndexBuffer);
	ID3D11Buffer* VertexBuffers[] = {
			pModelVertexBuffer,
			m_pVBInstance,
	};

	_uint		VertexStrides[] = {
	m_pModel->Get_MeshVertexStride(iMeshIndex),
	m_iInstanceStride,
	};

	_uint		Offsets[] = {
		0,
		0
	};

	m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, VertexBuffers, VertexStrides, Offsets);
	m_pContext->IASetIndexBuffer(pMoelIndexBuffer, m_pModel->Get_MeshIndexFormat(iMeshIndex), 0);
	m_pContext->IASetPrimitiveTopology(m_ePrimitive);

	Safe_Release(pModelVertexBuffer);
	Safe_Release(pMoelIndexBuffer);

	return S_OK;
}

CVIBuffer_Instance_Model* CVIBuffer_Instance_Model::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const INSTANCE_DESC* pInstanceDesc)
{
	CVIBuffer_Instance_Model* pVIBufferInstanceModel = new CVIBuffer_Instance_Model(pDevice, pContext);
	if (FAILED(pVIBufferInstanceModel->Initialize_Prototype(pInstanceDesc)))
	{
		Safe_Release(pVIBufferInstanceModel);
		MSG_BOX("Create Fail : Model Isntance Buffer");
	}
	return pVIBufferInstanceModel;
}

CComponent* CVIBuffer_Instance_Model::Clone(void* pArg)
{
	CVIBuffer_Instance_Model* pVIBufferInstanceModel = new CVIBuffer_Instance_Model(*this);
	if (FAILED(pVIBufferInstanceModel->Initialize(pArg)))
	{
		Safe_Release(pVIBufferInstanceModel);
		MSG_BOX("Clone Fail : Model Isntance Buffer");
	}
	return pVIBufferInstanceModel;
}

void CVIBuffer_Instance_Model::Free()
{
	__super::Free();

	Safe_Release(m_pModel);
	if (false == m_isCloned)
	{
		Safe_Delete_Array(m_pInstanceVertices);
	}
}