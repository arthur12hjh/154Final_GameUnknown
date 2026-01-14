#include "MotionTrailComponent.h"

#include "GameInstance.h"
#include "GameObject.h"
#include "Camera.h"

CMotionTrailComponent::CMotionTrailComponent(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) :
	CVIBuffer_Instance(pDevice, pContext)
{
}

CMotionTrailComponent::CMotionTrailComponent(const CMotionTrailComponent& rhs) :
	CVIBuffer_Instance(rhs),
	m_pInstanceVertices(rhs.m_pInstanceVertices)
{
}

HRESULT CMotionTrailComponent::Initialize_Prototype(_int iNum)
{
	m_iNumVertexBuffers = 2;
	m_ePrimitive = D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	m_iNumInstance = iNum;
	m_iInstanceStride = sizeof(VTX_MOTION_TRAIL_INSTANCE_MODEL);
	m_iNumIndexPerInstance = 6;

	m_InstanceBufferDesc.ByteWidth = m_iInstanceStride * m_iNumInstance;
	m_InstanceBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_InstanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_InstanceBufferDesc.StructureByteStride = m_iInstanceStride;
	m_InstanceBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_InstanceBufferDesc.MiscFlags = 0;

	m_pInstanceVertices = new VTX_MOTION_TRAIL_INSTANCE_MODEL[m_iNumInstance];
	ZeroMemory(m_pInstanceVertices, sizeof(VTX_MOTION_TRAIL_INSTANCE_MODEL) * m_iNumInstance);

	m_InstanceInitialDesc.pSysMem = m_pInstanceVertices;
	return S_OK;
}

HRESULT CMotionTrailComponent::Initialize(void* pArg)
{
	MOTION_TRAIL_COMPONENT_DESC* pDesc = static_cast<MOTION_TRAIL_COMPONENT_DESC*>(pArg);
	m_pModel = pDesc->pModel;
	m_pPreBoneModel = pDesc->pPreBoneModel;

	m_vUpdateTime = { 0.f, pDesc->fUpdateTime };
	m_pTransformMatrix = pDesc->pTransform;
	Safe_AddRef(m_pModel);
	Safe_AddRef(m_pPreBoneModel);

	if (FAILED(Ready_TarilBuffer()))
		return E_FAIL;

	if (FAILED(Ready_Shader()))
		return E_FAIL;

	for (_uint i = 0; i < m_iNumInstance; ++i)
	{
		m_pInstanceVertices[i].iNumber = { 0, i };
		m_pInstanceVertices[i].vLifeTime = { pDesc->fLifeTime + 1.f, pDesc->fLifeTime };
	}

	if (FAILED(m_pDevice->CreateBuffer(&m_InstanceBufferDesc, &m_InstanceInitialDesc, &m_pVBInstance)))
		return E_FAIL;

	return S_OK;
}

void CMotionTrailComponent::EnableMotionTrail(_bool bIsEnable)
{
	m_bEnableMotionTrail = bIsEnable;
}

_bool CMotionTrailComponent::IsEnableMotionTrail()
{
	return m_bEnableMotionTrail;
}

void CMotionTrailComponent::Update_Trail(_float fTimeDelta)
{
	D3D11_MAPPED_SUBRESOURCE pSubResource = {};
	m_iActiveTrailCount = 0;

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &pSubResource);
	VTX_MOTION_TRAIL_INSTANCE_MODEL* pDatas = static_cast<VTX_MOTION_TRAIL_INSTANCE_MODEL*>(pSubResource.pData);
	_bool bIsUpdateAge = false;
	if (m_bEnableMotionTrail && m_pPreBoneModel)
	{
		m_vUpdateTime.x += fTimeDelta;
		if (m_vUpdateTime.x >= m_vUpdateTime.y)
		{
			_matrix worldMat = XMLoadFloat4x4(m_pTransformMatrix);
			m_TrailCount++;
			if (m_iNumInstance <= m_TrailCount)
				m_TrailCount = 0;

			XMStoreFloat4(&pDatas[m_TrailCount].vRight, worldMat.r[0]);
			XMStoreFloat4(&pDatas[m_TrailCount].vUp, worldMat.r[1]);
			XMStoreFloat4(&pDatas[m_TrailCount].vLook, worldMat.r[2]);
			XMStoreFloat4(&pDatas[m_TrailCount].vTranslation, worldMat.r[3]);
			pDatas[m_TrailCount].vLifeTime.x = 0;
			auto pPreBoneBuffer = m_pPreBoneModel->Get_PreBoneMatrix();

			m_pContext->CopySubresourceRegion(
				m_pMotionTrailBuffers,			// 복사 받을 대상 버퍼
				0,								// 서브 리소스
				m_TrailCount * m_iNumBones * sizeof(COMPUTE_BONEMATRIX_OUT),		// 복사 받을 버퍼의 사이즈 X
				0, 0,							// Y, Z 는 ID3DBuffer가 선형이라 0이라고함
				pPreBoneBuffer,					// 복사할 데이터가 있는 버퍼
				0,								// 복사할 데이터의 서브 리소스
				&m_CopyBoxSize);				// 복사할 데이터의 크기

			bIsUpdateAge = true;
			m_vUpdateTime.x = 0.f;
			Safe_Release(pPreBoneBuffer);
		}
	}

	for (_uint i = 0; i < m_iNumInstance; ++i)
	{
		pDatas[i].vLifeTime.x += fTimeDelta;
		if (pDatas[i].vLifeTime.x < pDatas[i].vLifeTime.y)
			m_iActiveTrailCount++;

		if (bIsUpdateAge)
		{
			_int iAge = m_TrailCount - i;
			if (iAge < 0)
				pDatas[i].iNumber.x = m_iNumInstance + iAge;
			else
				pDatas[i].iNumber.x = iAge;
		}
	}

	if (bIsUpdateAge)
		bIsUpdateAge = false;
	
	m_pContext->Unmap(m_pVBInstance, 0);
}

HRESULT CMotionTrailComponent::Render()
{
	if (FAILED(Bind_ShaderResource()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModel->Get_NumMeshes();
	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModel->Bind_GlobalOffsetMatrices(m_pShader)))
			return E_FAIL;

		if (FAILED(m_pShader->Begin(0)))
			return E_FAIL;

		m_pContext->VSSetShaderResources(18, 1, &m_pMotionTrailSRV);
		Bind_Resource(i);
		m_iNumIndices = m_pModel->Get_MeshIndices(i);
		m_pContext->DrawIndexedInstanced(m_iNumIndices, m_iNumInstance, 0, 0, 0);
	}

	return S_OK;
}

void CMotionTrailComponent::SetMotionTrailColor(_float4 vColor)
{
	m_vColor[0] = vColor;
	m_vColor[1] = vColor;
}

void CMotionTrailComponent::SetRimLight(_float fRimLightPower, _float fRimLightIntensity)
{
	m_fRimLightPower = fRimLightPower;
	m_fRimLightIntensity = fRimLightIntensity;
}

HRESULT CMotionTrailComponent::Bind_ShaderResource()
{
	auto pMainCam = m_pGameInstance->GetMainCamera();
	if (nullptr == pMainCam)
		return;

	if (FAILED(m_pShader->Bind_RawValue("g_iNumBone", &m_iNumBones, sizeof(_int))))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_RawValue("g_fFar", &pMainCam->GetCameraInfo().fFar, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_RawValue("g_iNumTrail", &m_iActiveTrailCount, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_RawValue("g_fRimLightPower", &m_fRimLightPower, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_RawValue("g_fRimLightStrength", &m_fRimLightIntensity, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_RawValue("g_vStartColor", &m_vColor[0], sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_RawValue("g_vEndColor", &m_vColor[1], sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
		return E_FAIL;

	Safe_Release(pMainCam);
	return S_OK;
}

HRESULT CMotionTrailComponent::Bind_Resource(_uint iMeshIndex)
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

HRESULT CMotionTrailComponent::Ready_TarilBuffer()
{
	auto pBones = m_pPreBoneModel->Get_Bones();
	if (nullptr == pBones)
		return E_FAIL;

	m_iNumBones = (_uint)pBones->size();
	D3D11_BUFFER_DESC	 PreBoneBufferDesc = {};
	PreBoneBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	PreBoneBufferDesc.ByteWidth = sizeof(COMPUTE_BONEMATRIX_OUT) * m_iNumBones * m_iNumInstance;
	PreBoneBufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	PreBoneBufferDesc.CPUAccessFlags = 0;
	PreBoneBufferDesc.StructureByteStride = sizeof(COMPUTE_BONEMATRIX_OUT);
	PreBoneBufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	if (FAILED(m_pDevice->CreateBuffer(&PreBoneBufferDesc, nullptr, &m_pMotionTrailBuffers)))
		return E_FAIL;

	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	SRVDesc.Format = DXGI_FORMAT_UNKNOWN;
	SRVDesc.Buffer.NumElements = m_iNumBones * m_iNumInstance;
	m_pDevice->CreateShaderResourceView(m_pMotionTrailBuffers, &SRVDesc, &m_pMotionTrailSRV);

	m_CopyBoxSize.left = 0;
	m_CopyBoxSize.right = sizeof(COMPUTE_BONEMATRIX_OUT) * m_iNumBones;
	m_CopyBoxSize.top = 0;
	m_CopyBoxSize.bottom = 1;
	m_CopyBoxSize.front = 0;
	m_CopyBoxSize.back = 1;

	return S_OK;
}

HRESULT CMotionTrailComponent::Ready_Shader()
{
	m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_MotionTrail.hlsl"),
		VTX_MOTION_TRAIL_INSTANCE_MODEL_DESC::Elements, VTX_MOTION_TRAIL_INSTANCE_MODEL_DESC::iNumElements);

	if (nullptr == m_pShader)
		return E_FAIL;

	return S_OK;
}

CMotionTrailComponent* CMotionTrailComponent::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _int iNum)
{
	CMotionTrailComponent* pMotionTrailComponent = new CMotionTrailComponent(pDevice, pContext);
	if (FAILED(pMotionTrailComponent->Initialize_Prototype(iNum)))
	{
		Safe_Release(pMotionTrailComponent);
		MSG_BOX("Create Fail : Motion Trail Component");
	}

	return pMotionTrailComponent;
}

CComponent* CMotionTrailComponent::Clone(void* pArg)
{
	CMotionTrailComponent* pMotionTrailComponent = new CMotionTrailComponent(*this);
	if (FAILED(pMotionTrailComponent->Initialize(pArg)))
	{
		Safe_Release(pMotionTrailComponent);
		MSG_BOX("Clone Fail : Motion Trail Component");
	}

	return pMotionTrailComponent;
}

void CMotionTrailComponent::Free()
{
	__super::Free();

	Safe_Release(m_pModel);
	Safe_Release(m_pPreBoneModel);

	Safe_Release(m_pShader);
	Safe_Release(m_pMotionTrailBuffers);
	Safe_Release(m_pMotionTrailSRV);

	if(false == m_isCloned)
		Safe_Delete_Array(m_pInstanceVertices);
}
