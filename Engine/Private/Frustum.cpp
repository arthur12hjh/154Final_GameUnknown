#include "Frustum.h"

#include "GameInstance.h"
#include "Collider.h"
#include "Camera.h"

#ifdef _DEBUG
#include "DebugDraw.h"
#endif

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

	// 3으로 일단 지정해서 3등분해보자.
	m_iNumCascadeCount = 3;

	// 케스케이드 숫자 + 1은 케스케이드에서 상이 맺어지는 부분의 평면임
	m_CascadeFar.resize(m_iNumCascadeCount + 1);
	if (FAILED(Ready_CasCadeTexture()))
		return E_FAIL;


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
	_matrix ShadowViewMatrix = XMLoadFloat4x4(m_pGameInstance->GetShadowMatrix(D3DTS::VIEW));
	_matrix WorldMatrix = m_pGameInstance->GetMainCameraWorldMatrix();

	for(_uint i = 0; i < 3; ++i)
		m_CaseCadeMatrix.ViewMatrix[i] = *m_pGameInstance->GetShadowMatrix(D3DTS::VIEW);

	auto pCamera = m_pGameInstance->GetMainCamera();
	if (nullptr == pCamera)
		return;

	auto& CameraDesc = pCamera->GetCameraInfo();

	// 수직 시야각을 이용하여 수평시야각을 구함
	_float fHalfHFov = (CameraDesc.fFov / 2.f) * CameraDesc.fAspect;

	// 그림자 품질은 로그 + 선형 분할
	// 선형 분할
	// 케스케이드 개수에 따라서 로그 + 선형 분할로 나눈다.
	// 이러면 좀더 품질좋은 그림자가 나온다고한다.
	m_CascadeFar[0] = CameraDesc.fNear;
	m_CaseCadeDefferdDesc.CasCadeDist[m_iNumCascadeCount - 1] = m_CascadeFar[m_iNumCascadeCount] = CameraDesc.fFar;
	
	for (_uint i = 1; i < m_iNumCascadeCount; ++i)
		m_CaseCadeDefferdDesc.CasCadeDist[i-1] = m_CascadeFar[i] = Mix<_float>(CameraDesc.fNear, CameraDesc.fFar, _float(i) / m_iNumCascadeCount);

	for (_uint i = 0; i < m_iNumCascadeCount; ++i)
	{
		// 우선 평면의 4개의 AABB 박스의 점을 구한다.
		_float NearX = m_CascadeFar[i] * fHalfHFov;
		_float NearY = m_CascadeFar[i] * fHalfHFov;

		_float FarX = m_CascadeFar[i + 1] * fHalfHFov;
		_float FarY = m_CascadeFar[i + 1] * fHalfHFov;

		m_vCascadeFrustumConers[0] = { NearX, NearY, m_CascadeFar[i], 1.f };
		m_vCascadeFrustumConers[1] = { -NearX, NearY, m_CascadeFar[i], 1.f };
		m_vCascadeFrustumConers[2] = { NearX, -NearY, m_CascadeFar[i], 1.f };
		m_vCascadeFrustumConers[3] = { -NearX, -NearY, m_CascadeFar[i], 1.f };

		m_vCascadeFrustumConers[4] = { FarX, FarY, m_CascadeFar[i + 1], 1.f };
		m_vCascadeFrustumConers[5] = { -FarX, FarY, m_CascadeFar[i + 1], 1.f };
		m_vCascadeFrustumConers[6] = { FarX, -FarY, m_CascadeFar[i + 1], 1.f };
		m_vCascadeFrustumConers[7] = { -FarX, -FarY, m_CascadeFar[i + 1], 1.f };

		// AABB 박스의 중점을 구하는 수식
		_vector vCenterPos = {};
		for (_uint j = 0; j < 8; ++j)
		{
			_vector ConerWorld =  XMVector3TransformNormal(XMLoadFloat4(&m_vCascadeFrustumConers[j]), WorldMatrix);
			XMStoreFloat4(&m_vCascadeFrustumConers[j], ConerWorld);
			vCenterPos += ConerWorld;
		}
		vCenterPos /= 8.0f;

		// 가장 멀리 떨어진 길이를 찾는다.
		_float	fRadius = {};
		for (_uint j = 0; j < 8; ++j)
		{
			_float Distance = XMVectorGetX(XMVector3Length(XMLoadFloat4(&m_vCascadeFrustumConers[j]) - vCenterPos));
			fRadius = max(fRadius, Distance);
		}

		// 카메라가 보는 시점이 변경될때 떨리는걸 방지하기 위한 공식이라고함
		fRadius = ceil(fRadius * 16.0f) / 16.0f;
		_vector MaxExtents = { fRadius, fRadius, fRadius };
  		_vector MinExtents = MaxExtents * -1.f;
		
		auto& ShadowLightDesc = m_pGameInstance->GetShadowCameraInfo();
		_vector vShadowDir = XMVector3Normalize(XMLoadFloat4(&ShadowLightDesc.vAt) - XMLoadFloat4(&ShadowLightDesc.vEye) );
		_vector vShadowCameraPos = vCenterPos + vShadowDir * MaxExtents.m128_f32[2];

		_vector CasCadeExtents = MaxExtents - MinExtents;

		if (0 < m_CascadeFar[i])
		{
			_matrix OrthMatrix = XMMatrixOrthographicLH(MaxExtents.m128_f32[0] - MinExtents.m128_f32[0],
				MaxExtents.m128_f32[1] - MinExtents.m128_f32[1],
				0.f, CasCadeExtents.m128_f32[2]);

			XMStoreFloat4x4(&m_CaseCadeMatrix.ProjMatrix[i], OrthMatrix);
			XMStoreFloat4x4(&m_CaseCadeDefferdDesc.VPMatrix[i], ShadowViewMatrix * OrthMatrix);
		}
	}

	Safe_Release(pCamera);
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
	Make_Planes(m_vWorldPoints, m_vWorldPlanes);
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
		if (fRange < XMVectorGetX(XMPlaneDotCoord(XMLoadFloat4(&m_vWorldPlanes[i]), vWorldPos)))
			return false;	
	}

	return true;
}

_bool CFrustum::isIn_WorldFrustum(CCollider* pCollider)
{
	if (nullptr == pCollider)
		return false;

	switch (pCollider->GetCollisionType())
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

void CFrustum::Bind_CasCadeSRV()
{
	m_pContext->PSSetShaderResources(5, 1, &m_pCasCadeSRV);
}

void CFrustum::Bind_ShadowMatrix()
{
	// 상수 버퍼를 통해서 넘길거임
	// 여기서 구한 View, Proj 전부 넘겨줄거임
	m_pContext->UpdateSubresource(m_pCaseCadeCB[0], 0, nullptr, &m_CaseCadeMatrix, 0, 0);
	m_pContext->GSSetConstantBuffers(0, 1, &m_pCaseCadeCB[0]);
}

void CFrustum::Bind_ShadowDefferd()
{
	m_pContext->UpdateSubresource(m_pCaseCadeCB[1], 0, nullptr, &m_CaseCadeDefferdDesc, 0, 0);
	m_pContext->PSSetConstantBuffers(0, 1, &m_pCaseCadeCB[1]);
}

ID3D11DepthStencilView* CFrustum::GetShadowDSV()
{
	ID3D11ShaderResourceView* nullSRV[8] = {};
	m_pContext->PSSetShaderResources(0, 8, nullSRV);
	return m_pCasecasdeDSV;
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

HRESULT CFrustum::Ready_CasCadeTexture()
{
#pragma region CB
	D3D11_BUFFER_DESC BufferDesc = {};
	BufferDesc.ByteWidth = sizeof(CASCADE_DESC);
	BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	BufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	D3D11_SUBRESOURCE_DATA ConstBufferSubResource = {};
	ConstBufferSubResource.pSysMem = &m_CaseCadeMatrix;

	if (FAILED(m_pDevice->CreateBuffer(&BufferDesc, &ConstBufferSubResource, &m_pCaseCadeCB[0])))
		return E_FAIL;

	BufferDesc.ByteWidth = sizeof(CASCADE_DEFFERD_DESC);
	BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	BufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	ConstBufferSubResource = {};
	ConstBufferSubResource.pSysMem = &m_CaseCadeDefferdDesc;

	if (FAILED(m_pDevice->CreateBuffer(&BufferDesc, &ConstBufferSubResource, &m_pCaseCadeCB[1])))
		return E_FAIL;
#pragma endregion

	D3D11_TEXTURE2D_DESC texDesc = {};
	texDesc.Width = 2048;
	texDesc.Height = 2048;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = m_iNumCascadeCount;                // <--- cascade 개수
	texDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	texDesc.SampleDesc.Count = 1;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;

	ID3D11Texture2D* pTex = { nullptr };
	m_pDevice->CreateTexture2D(&texDesc, nullptr, &pTex);
	if (nullptr == pTex)
		return E_FAIL;

	D3D11_DEPTH_STENCIL_VIEW_DESC DSVDesc = {};
	DSVDesc.Format = DXGI_FORMAT_D32_FLOAT;
	DSVDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
	DSVDesc.Texture2DArray.ArraySize = m_iNumCascadeCount;
	DSVDesc.Texture2DArray.FirstArraySlice = 0;
	DSVDesc.Texture2DArray.MipSlice = 0;

	m_pDevice->CreateDepthStencilView(pTex, &DSVDesc, &m_pCasecasdeDSV);
	if (nullptr == m_pCasecasdeDSV)
		return E_FAIL;

	m_pContext->ClearDepthStencilView(m_pCasecasdeDSV, D3D11_CLEAR_DEPTH, 1.f, 0);

	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
	SRVDesc.Format = DXGI_FORMAT_R32_FLOAT;
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	SRVDesc.Texture2DArray.MipLevels = 1;
	SRVDesc.Texture2DArray.MostDetailedMip = 0;
	SRVDesc.Texture2DArray.FirstArraySlice = 0;
	SRVDesc.Texture2DArray.ArraySize = m_iNumCascadeCount;
	m_pDevice->CreateShaderResourceView(pTex, &SRVDesc, &m_pCasCadeSRV);
	if (nullptr == m_pCasCadeSRV)
		return E_FAIL;

	D3D11_TEXTURE2D_DESC stagingDesc = texDesc;
	stagingDesc.Usage = D3D11_USAGE_STAGING;
	stagingDesc.BindFlags = 0;
	stagingDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;


	m_pDevice->CreateTexture2D(&stagingDesc, nullptr, &pStagingTex);
	m_pContext->CopyResource(pStagingTex, pTex);


	Safe_Release(pTex);
	return S_OK;
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

#ifdef _DEBUG
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	Safe_Delete(m_pBatch);
	Safe_Delete(m_pEffect);
	Safe_Release(m_pInputLayout);
#endif // _DEBUG

	Safe_Release(m_pCaseCadeCB[0]);
	Safe_Release(m_pCaseCadeCB[1]);
	Safe_Release(m_pCasecasdeDSV);
	Safe_Release(m_pCasCadeSRV);

	Safe_Release(m_pGameInstance);
	Safe_Delete(m_OrizinBoundingFrustom);
}
