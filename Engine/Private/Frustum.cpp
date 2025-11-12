#include "Frustum.h"

#include "GameInstance.h"
#include "Collider.h"
#include "Camera.h"

#ifdef _DEBUG
#include "DebugDraw.h"

CFrustum::CFrustum(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) :
	m_pDevice(pDevice),
	m_pContext(pContext),
	m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pGameInstance);
}
#else
CFrustum::CFrustum()
	: m_pGameInstance { CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}
#endif // _DEBUG

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

void CFrustum::Make_Planes(const _float4* pPoints, _float4* pPlanes)
{
	XMStoreFloat4(&pPlanes[0], XMPlaneFromPoints(XMLoadFloat4(&pPoints[1]), XMLoadFloat4(&pPoints[5]), XMLoadFloat4(&pPoints[6])));
	XMStoreFloat4(&pPlanes[1], XMPlaneFromPoints(XMLoadFloat4(&pPoints[4]), XMLoadFloat4(&pPoints[0]), XMLoadFloat4(&pPoints[3])));
	XMStoreFloat4(&pPlanes[2], XMPlaneFromPoints(XMLoadFloat4(&pPoints[4]), XMLoadFloat4(&pPoints[5]), XMLoadFloat4(&pPoints[1])));
	XMStoreFloat4(&pPlanes[3], XMPlaneFromPoints(XMLoadFloat4(&pPoints[3]), XMLoadFloat4(&pPoints[2]), XMLoadFloat4(&pPoints[6])));
	XMStoreFloat4(&pPlanes[4], XMPlaneFromPoints(XMLoadFloat4(&pPoints[5]), XMLoadFloat4(&pPoints[4]), XMLoadFloat4(&pPoints[7])));
	XMStoreFloat4(&pPlanes[5], XMPlaneFromPoints(XMLoadFloat4(&pPoints[0]), XMLoadFloat4(&pPoints[1]), XMLoadFloat4(&pPoints[2])));
}

#ifdef _DEBUG
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
#else
CFrustum* CFrustum::Create()
{
	CFrustum* pInstance = new CFrustum();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CFrustum");
		Safe_Release(pInstance);
	}

	return pInstance;
}
#endif // _DEBUG

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

	Safe_Release(m_pGameInstance);
	Safe_Delete(m_OrizinBoundingFrustom);
}
