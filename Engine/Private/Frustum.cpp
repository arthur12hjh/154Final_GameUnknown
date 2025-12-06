#include "Frustum.h"

#include "GameInstance.h"
#include "Collider.h"
#include "Camera.h"

#ifdef _DEBUG
#include "DebugDraw.h"
#endif // _DEBUG

CFrustum::CFrustum(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) :
	m_pDevice(pDevice),
	m_pContext(pContext),
	m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pGameInstance);
}

HRESULT CFrustum::Initialize()
{
	/* 투영공간상의 점 여덟개의 정보를 채운다. */
	m_vOriginalPoints[0] = _float4(-1.f, 1.f, 0.f, 1.f);
	m_vOriginalPoints[1] = _float4(1.f, 1.f, 0.f, 1.f);
	m_vOriginalPoints[2] = _float4(1.f, -1.f, 0.f, 1.f);
	m_vOriginalPoints[3] = _float4(-1.f, -1.f, 0.f, 1.f);

	m_vOriginalPoints[4] = _float4(-1.f, 1.f, 1.f, 1.f);
	m_vOriginalPoints[5] = _float4(1.f, 1.f, 1.f, 1.f);
	m_vOriginalPoints[6] = _float4(1.f, -1.f, 1.f, 1.f);
	m_vOriginalPoints[7] = _float4(-1.f, -1.f, 1.f, 1.f);
	m_OrizinBoundingFrustom = new BoundingFrustum();

	//if (FAILED(Ready_ComputeShader()))
	//	return E_FAIL;

#ifdef _DEBUG
	m_pBatch = new PrimitiveBatch<VertexPositionColor>(m_pContext);
	m_pEffect = new BasicEffect(m_pDevice);
	m_pEffect->SetVertexColorEnabled(true);

	const void* pShaderByteCode = { nullptr };
	size_t		iShaderByteCodeLength = {};

	m_pEffect->GetVertexShaderBytecode(&pShaderByteCode, &iShaderByteCodeLength);

	if (m_pDevice->CreateInputLayout(VertexPositionColor::InputElements, VertexPositionColor::InputElementCount,
		pShaderByteCode, iShaderByteCodeLength, &m_pInputLayout))
		return E_FAIL;
#endif

	return S_OK;
}

void CFrustum::Update()
{
	_matrix		ProjMatrixInverse = m_pGameInstance->Get_Transform_Matrix_Inverse(D3DTS::PROJ);
	_matrix		ViewMatrixInverse = m_pGameInstance->Get_Transform_Matrix_Inverse(D3DTS::VIEW);
	_matrix		matPV = ProjMatrixInverse * ViewMatrixInverse;
	/* 투영 스페이스 (3차원공간) 에서 월드로 내려줌. 절두체의 형태로 바뀐다.*/
	for (size_t i = 0; i < 8; i++)
	{
		XMStoreFloat4(&m_vWorldPoints[i], 
			XMVector3TransformCoord(XMLoadFloat4(&m_vOriginalPoints[i]), matPV));
	}
	auto pMainCamera = m_pGameInstance->GetMainCamera();
	if (pMainCamera)
	{
		auto CameraInfo = pMainCamera->GetCameraInfo();

		m_OrizinBoundingFrustom->Near = CameraInfo.fNear;
		m_OrizinBoundingFrustom->Far = CameraInfo.fFar;

		m_OrizinBoundingFrustom->Transform(m_BoundingFrustom, m_pGameInstance->GetMainCameraWorldMatrix());
	}

	Make_Planes(m_vWorldPoints);
	Safe_Release(pMainCamera);
}

#ifdef _DEBUG
void CFrustum::FrustomRender()
{
	auto pAllCamera = m_pGameInstance->GetAllCamera();
	auto pMainCamera = m_pGameInstance->GetMainCamera();
	for (auto& pCamera : *pAllCamera)
	{
		if (pMainCamera == pCamera.second)
			continue;

		auto CameraInfo = pCamera.second->GetCameraInfo();
		BoundingFrustum tempFrustum{}, tempFrustum2;
		tempFrustum.Near = CameraInfo.fNear;
		tempFrustum.Far = CameraInfo.fFar;
		tempFrustum.Transform(tempFrustum2, XMLoadFloat4x4(pCamera.second->GetTransform()->Get_WorldMatrixPtr()));

		m_pEffect->SetWorld(XMMatrixIdentity());
		m_pEffect->SetView(m_pGameInstance->Get_Transform_Matrix(D3DTS::VIEW));
		m_pEffect->SetProjection(m_pGameInstance->Get_Transform_Matrix(D3DTS::PROJ));

		m_pContext->IASetInputLayout(m_pInputLayout);
		m_pEffect->Apply(m_pContext);

		m_pBatch->Begin();
		DX::Draw(m_pBatch, tempFrustum2, XMVectorSet(1.f, 1.f, 0.f, 1.f));
		m_pBatch->End();
	}
	Safe_Release(pMainCamera);
}	
#endif // _DEBUG

void CFrustum::Transform_Frustum_ToLocalSpace(_fmatrix WorldMatrixInverse)
{
	_float4		vLocalPoints[8] = {};
	for (size_t i = 0; i < 8; i++)
	{
		XMStoreFloat4(&vLocalPoints[i],
			XMVector3TransformCoord(XMLoadFloat4(&m_vWorldPoints[i]), WorldMatrixInverse));		
	}

	Make_Planes(vLocalPoints, m_vLocalPlanes);
}


_bool CFrustum::isIn_WorldFrustum(_fvector vWorldPos, _float fRange)
{
	for (size_t i = 0; i < 6; i++)
	{
		if (fRange < XMVectorGetX(XMPlaneDotCoord(XMLoadFloat4(&m_FrustomConstantDesc.vFrustomPlane[i]), vWorldPos)))
			return false;	
	}

	return true;
}

_bool CFrustum::isIn_WorldFrustum(CCollider* pCollider)
{
	if (nullptr == pCollider)
		return false;

	switch (pCollider->GetCollierType())
	{
	case COLLIDER::AABB :
		return static_cast<CBoxCollider*>(pCollider)->FrustomIntersect(m_BoundingFrustom);
	case COLLIDER::SPHERE:
		return static_cast<CSphereCollider*>(pCollider)->FrustomIntersect(m_BoundingFrustom);
	case COLLIDER::OBB:
		return static_cast<COBBCollider*>(pCollider)->FrustomIntersect(m_BoundingFrustom);
	}
	return  true;
}

_bool CFrustum::isIn_LocalFrustum(_fvector vLocalPos, _float fRange)
{
	for (size_t i = 0; i < 6; i++)
	{
		if (fRange < XMVectorGetX(XMPlaneDotCoord(XMLoadFloat4(&m_vLocalPlanes[i]), vLocalPos)))
			return false;
	}

	return true;
}

void CFrustum::isIn_WorldFrustum(ID3D11Buffer* pInstanceBuffer, ID3D11Buffer* pOut, _uint iNumInstance, _float fDistance, _uint* iNumCullCount)
{
	_uint iIndex = {};
	auto pCamera = m_pGameInstance->GetMainCamera();
	m_FrustomConstantDesc.iNumInstance = iNumInstance;
	m_FrustomConstantDesc.fDistance = fDistance;
	XMStoreFloat3(&m_FrustomConstantDesc.vCamPos, pCamera->GetTransform()->Get_State(STATE::POSITION));
	m_pComputeShader->Update_BufferResource(CComputeShader::BUFFER_TYPE::CONSTATNT, 0, &m_FrustomConstantDesc);
	m_pComputeShader->Update_BufferResource(CComputeShader::BUFFER_TYPE::INPUT, 0, pInstanceBuffer);
	m_pComputeShader->Update_BufferResource(CComputeShader::BUFFER_TYPE::OUTPUT, 1, &iIndex);

	m_pComputeShader->Bind_ConstBuffer(0, 0);
	m_pComputeShader->Bind_InputBuffer(0, 0);

	_uint iOutGroup[2] = {0, 1};
	m_pComputeShader->Bind_OutputBuffer(2, iOutGroup);

	m_pComputeShader->Update_Shader({ 1024, 1, 1 });
	m_pComputeShader->GetBufferResource(CComputeShader::BUFFER_TYPE::OUTPUT, 0, pOut);
	m_pComputeShader->GetBufferResource(CComputeShader::BUFFER_TYPE::OUTPUT, 1, m_pCountBuffer);
 
}

HRESULT CFrustum::Ready_ComputeShader()
{
	// 컴퓨트 셰이더 만들자
	m_pComputeShader = CComputeShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Instance_Cuilling.hlsl"), "CS_Main", m_iNumData);
	if (nullptr == m_pComputeShader)
		return E_FAIL;

	// Input버퍼는 받아올거니까 받아오고
	// 상수 버퍼랑 Out 버퍼 정의해서 뽑아내주면 될거같다.

	ID3D11Buffer* pBuffer = nullptr;
#pragma region ConstantBuffer
	D3D11_BUFFER_DESC BufferDesc = {};
	BufferDesc.ByteWidth = sizeof(CONSTANT_BUFFER_FRUSTOM);
	BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	BufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	D3D11_SUBRESOURCE_DATA ConstBufferSubResource = {};
	ConstBufferSubResource.pSysMem = &m_FrustomConstantDesc;

	if (FAILED(m_pDevice->CreateBuffer(&BufferDesc, &ConstBufferSubResource, &pBuffer)))
		return E_FAIL;
	m_pComputeShader->ADD_Buffer(CComputeShader::BUFFER_TYPE::CONSTATNT, pBuffer);
#pragma endregion

#pragma region Input & Output Base Buffer
	D3D11_BUFFER_DESC InitBufferDesc = {};
	InitBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	InitBufferDesc.ByteWidth = sizeof(VTX_INSTANCE_MODEL) * m_iNumData;
	InitBufferDesc.StructureByteStride = sizeof(VTX_INSTANCE_MODEL);
	InitBufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	InitBufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

#pragma region SRV Buffer
	if (FAILED(m_pDevice->CreateBuffer(&InitBufferDesc, nullptr, &pBuffer)))
		return E_FAIL;

	m_pComputeShader->ADD_Buffer(CComputeShader::BUFFER_TYPE::INPUT, pBuffer);
#pragma endregion

#pragma region UAV Buffer
	if (FAILED(m_pDevice->CreateBuffer(&InitBufferDesc, nullptr, &pBuffer)))
		return E_FAIL;

	m_pComputeShader->ADD_AppendOutBuffer(pBuffer);
#pragma endregion

#pragma region Count Buffer
	InitBufferDesc.ByteWidth = sizeof(_uint);
	InitBufferDesc.StructureByteStride = sizeof(_uint);
	if (FAILED(m_pDevice->CreateBuffer(&InitBufferDesc, nullptr, &pBuffer)))
		return E_FAIL;

	m_pComputeShader->ADD_Buffer(CComputeShader::BUFFER_TYPE::OUTPUT, pBuffer, 1);

	InitBufferDesc = {};
	InitBufferDesc.Usage = D3D11_USAGE_STAGING;
	InitBufferDesc.ByteWidth = sizeof(_uint);
	InitBufferDesc.StructureByteStride = sizeof(_uint);
	InitBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

	if (FAILED(m_pDevice->CreateBuffer(&InitBufferDesc, nullptr, &m_pCountBuffer)))
		return E_FAIL;
#pragma endregion

	return S_OK;
}

void CFrustum::Make_Planes(const _float4* pPoints)
{
	XMStoreFloat4(&m_FrustomConstantDesc.vFrustomPlane[0], XMPlaneFromPoints(XMLoadFloat4(&pPoints[1]), XMLoadFloat4(&pPoints[5]), XMLoadFloat4(&pPoints[6])));
	XMStoreFloat4(&m_FrustomConstantDesc.vFrustomPlane[1], XMPlaneFromPoints(XMLoadFloat4(&pPoints[4]), XMLoadFloat4(&pPoints[0]), XMLoadFloat4(&pPoints[3])));
	XMStoreFloat4(&m_FrustomConstantDesc.vFrustomPlane[2], XMPlaneFromPoints(XMLoadFloat4(&pPoints[4]), XMLoadFloat4(&pPoints[5]), XMLoadFloat4(&pPoints[1])));
	XMStoreFloat4(&m_FrustomConstantDesc.vFrustomPlane[3], XMPlaneFromPoints(XMLoadFloat4(&pPoints[3]), XMLoadFloat4(&pPoints[2]), XMLoadFloat4(&pPoints[6])));
	XMStoreFloat4(&m_FrustomConstantDesc.vFrustomPlane[4], XMPlaneFromPoints(XMLoadFloat4(&pPoints[5]), XMLoadFloat4(&pPoints[4]), XMLoadFloat4(&pPoints[7])));
	XMStoreFloat4(&m_FrustomConstantDesc.vFrustomPlane[5], XMPlaneFromPoints(XMLoadFloat4(&pPoints[0]), XMLoadFloat4(&pPoints[1]), XMLoadFloat4(&pPoints[2])));
}

void CFrustum::Make_Planes(const _float4* pPoints, _float4* pPlanes)
{
	XMStoreFloat4(&pPlanes[0], XMPlaneFromPoints(XMLoadFloat4(&pPoints[1]), XMLoadFloat4(&pPoints[5]), XMLoadFloat4(&pPoints[6])));
	XMStoreFloat4(&pPlanes[1], XMPlaneFromPoints(XMLoadFloat4(&pPoints[4]), XMLoadFloat4(&pPoints[0]), XMLoadFloat4(&pPoints[3])));
	XMStoreFloat4(&pPlanes[2], XMPlaneFromPoints(XMLoadFloat4(&pPoints[4]), XMLoadFloat4(&pPoints[5]), XMLoadFloat4(&pPoints[1])));
	XMStoreFloat4(&pPlanes[3], XMPlaneFromPoints(XMLoadFloat4(&pPoints[3]), XMLoadFloat4(&pPoints[2]), XMLoadFloat4(&pPoints[6])));
	XMStoreFloat4(&pPlanes[4], XMPlaneFromPoints(XMLoadFloat4(&pPoints[5]), XMLoadFloat4(&pPoints[4]), XMLoadFloat4(&pPoints[7])));
	XMStoreFloat4(&pPlanes[5], XMPlaneFromPoints(XMLoadFloat4(&pPoints[0]), XMLoadFloat4(&pPoints[1]), XMLoadFloat4(&pPoints[2])));
}

CFrustum* CFrustum::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CFrustum* pInstance = new CFrustum(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CFrustum");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFrustum::Free()
{
	__super::Free();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	Safe_Release(m_pCountBuffer);
	Safe_Release(m_pComputeShader);

#ifdef _DEBUG
	Safe_Delete(m_pBatch);
	Safe_Delete(m_pEffect);
	Safe_Release(m_pInputLayout);
#endif // _DEBUG

	Safe_Release(m_pGameInstance);
	Safe_Delete(m_OrizinBoundingFrustom);
}
