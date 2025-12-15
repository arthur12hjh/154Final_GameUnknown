#include "Shadow.h"
#include "Shader.h"

#include "GameInstance.h"
#include "Camera.h"

CShadow::CShadow()
	: m_pGameInstance { CGameInstance::GetInstance() }
{

}

HRESULT CShadow::Ready_Shadow_Light(const SHADOW_LIGHT_DESC& Desc)
{
	XMStoreFloat4x4(&m_TransformationMatrices[ENUM_CLASS(D3DTS::VIEW)],
		XMMatrixLookAtLH(XMLoadFloat4(&Desc.vEye), XMLoadFloat4(&Desc.vAt), XMVectorSet(0.f, 1.f, 0.f, 0.f)));
	// 툴로 실시간 조절이 가능할 것 같진 않으니까 ㅇㅇ

	XMStoreFloat4x4(&m_TransformationMatrices[ENUM_CLASS(D3DTS::PROJ)],
		XMMatrixPerspectiveFovLH(Desc.fFovy, Desc.fAspect, Desc.fNear, Desc.fFar));

	m_fCascadeEnds[0] = 0.1f;
	m_fCascadeEnds[1] = 10.0f,
	m_fCascadeEnds[2] = 20.0f,
	m_fCascadeEnds[3] = 40.f;
	m_fCascadeEnds[4] = 100.f;
	m_fCascadeEnds[5] = 500.f;

	m_tShadowLightDesc = Desc;

	return S_OK;
}

HRESULT CShadow::Bind_Shader_Resource(CShader* pShader, const _char* pConstantName, D3DTS eType)
{
	return pShader->Bind_Matrix(pConstantName, &m_TransformationMatrices[ENUM_CLASS(eType)]);
}

HRESULT CShadow::Bind_Shader_Resource_Cascade(CShader* pShader, const _char* pConstantName, D3DTS eType)
{
	if(D3DTS::VIEW == eType)
		return pShader->Bind_Matrices(pConstantName, m_CasCadeTransformationViewMatrices, CASCADE_LEVEL);
	if (D3DTS::PROJ == eType)
		return pShader->Bind_Matrices(pConstantName, m_CasCadeTransformationProjMatrices, CASCADE_LEVEL);

	return E_FAIL;
}

HRESULT CShadow::Bind_Cascade_Ends(CShader* pShader, const _char* pConstantName, const _char* pConstantName2)
{
	_float4 vPacked[2] =
	{
		{ m_fCascadeEnds[0], m_fCascadeEnds[1], m_fCascadeEnds[2], m_fCascadeEnds[3] },
		{ m_fCascadeEnds[4], m_fCascadeEnds[5], 0.f, 0.f }
	};

	pShader->Bind_RawValue("g_CascadeEnds0", &vPacked[0], sizeof(_float4));
	pShader->Bind_RawValue("g_CascadeEnds1", &vPacked[1], sizeof(_float4));

	return S_OK;
}

HRESULT CShadow::Initialize(void* pArg)
{
	return S_OK;
}

void CShadow::Update(_float fTimeDelta)
{
	Seperate_CascadeFrustum();
	Calc_CascadeMatrices();
}

void CShadow::Seperate_CascadeFrustum()
{
	CCamera* pCam = m_pGameInstance->GetMainCamera();
	if (nullptr == pCam)
		return;

	CAMERA_INFO CamDesc = {};
	CamDesc = pCam->GetCameraInfo();

	//월드 점 8개랑 모서리 벡터 4개 구하기 
	const _float4* pFrustumPoints = m_pGameInstance->Get_FrustumWorldPoints();
	// 얘가 들고 있는건 월드 상의 벡터고, Near에서 Far로 향하니까.. 
	// 진짜 비율 구해서 구하기만 해주면 된다.
	const _float4* pFrustumRays = m_pGameInstance->Get_FrustumWorldRays();

	for (_uint i = 0; i < CASCADE_LEVEL; ++i)
	{
		for (_uint j = 0; j < 4; ++j)
		{
			_vector vNearPlanePoints = XMLoadFloat4(&pFrustumPoints[j]);
			//near
			XMStoreFloat4(&m_vCascadeNearCorner[i][j], vNearPlanePoints + XMLoadFloat4(&pFrustumRays[j]) * (m_fCascadeEnds[i] - CamDesc.fNear) / (CamDesc.fFar - CamDesc.fNear));
			//far
			XMStoreFloat4(&m_vCascadeFarCorner[i][j], vNearPlanePoints + XMLoadFloat4(&pFrustumRays[j]) * (m_fCascadeEnds[i + 1] - CamDesc.fNear) / (CamDesc.fFar - CamDesc.fNear));
		}
	}

	Safe_Release(pCam);
}

void CShadow::Calc_CascadeMatrices()
{
	for (_uint i = 0; i < CASCADE_LEVEL; ++i)
	{
		_vector vCenter = {};

		//Center 구하기
		for (_uint j = 0; j < 4; ++j)
		{
			_float4 vNear = m_vCascadeNearCorner[i][j];
			_float4 vFar = m_vCascadeFarCorner[i][j];

			vCenter += XMLoadFloat4(&vNear) + XMLoadFloat4(&vFar);
		}
		vCenter /= 8.f;

		//거리가 음수일린 없으니까 ㅇㅇ
		_float fRadius = { -1.f };
		_float4 vFarthestPoint = {};

		//Radius 구하기 
		//near에 대해서 연산
		for (_uint j = 0; j < 4; ++j)
		{
			_float4 vPoint = m_vCascadeNearCorner[i][j];
			_float fDist = XMVectorGetX(XMVector3Length(XMLoadFloat4(&vPoint) - vCenter));

			if (fDist > fRadius)
			{
				fRadius = fDist;
				vFarthestPoint = m_vCascadeNearCorner[i][j];
			}
		}

		//far에 대해서 연산
		for (_uint j = 0; j < 4; ++j)
		{
			_float4 vPoint = m_vCascadeFarCorner[i][j];
			_float fDist = XMVectorGetX(XMVector3Length(XMLoadFloat4(&vPoint) - vCenter));

			if (fDist > fRadius)
			{
				fRadius = fDist;
				vFarthestPoint = m_vCascadeFarCorner[i][j];
			}
		}

		if (fRadius <= 0.f)
			return;

		//월드 상의 Cascade Position 저장.
		XMStoreFloat4(&m_vCascadePositions[i], XMVectorSetW(vCenter + 
			(fRadius + 30.f) * XMVector3Normalize(XMLoadFloat4(&m_tShadowLightDesc.vDir)) * -1.f, 1.f));

		// 뷰 행렬 생성 (Position도 따로 연산해서)
		XMStoreFloat4x4(&m_CasCadeTransformationViewMatrices[i], 
			XMMatrixLookAtLH(XMLoadFloat4(&m_vCascadePositions[i]), vCenter, XMVectorSet(0.f, 1.f, 0.f, 0.f)));

		_float fMinZ = +FLT_MAX;
		_float fMaxZ = -FLT_MAX;

		for (_uint j = 0; j < 4; ++j)
		{
			// near 4개
			_vector vNearWorldSpace = XMLoadFloat4(&m_vCascadeNearCorner[i][j]);
			_vector vNearLightSpace = XMVector3TransformCoord(vNearWorldSpace, XMLoadFloat4x4(&m_CasCadeTransformationViewMatrices[i]));
			_float  fZ0 = XMVectorGetZ(vNearLightSpace);

			fMinZ = min(fMinZ, fZ0);
			fMaxZ = max(fMaxZ, fZ0);

			// far 4개
			_vector vFarWS = XMLoadFloat4(&m_vCascadeFarCorner[i][j]);
			_vector vFarLS = XMVector3TransformCoord(vFarWS, XMLoadFloat4x4(&m_CasCadeTransformationViewMatrices[i]));
			_float  fZ1 = XMVectorGetZ(vFarLS);

			fMinZ = min(fMinZ, fZ1);
			fMaxZ = max(fMaxZ, fZ1);
		}

		_float fZPad = 30.f;
		fMinZ -= fZPad;
		fMaxZ += fZPad;

		// 예외처리 
		if (fMinZ < 0.f) fMinZ = 0.f;
		if (fMaxZ <= fMinZ) fMaxZ = fMinZ + 1.f;

		// Proj 행렬 생성
		XMStoreFloat4x4(&m_CasCadeTransformationProjMatrices[i],
			XMMatrixOrthographicLH(fRadius * 2.0f, fRadius * 2.0f, fMinZ, fMaxZ));
	}
}

CShadow* CShadow::Create()
{
	CShadow* pInstance = new CShadow();

	if (FAILED(pInstance->Initialize(nullptr)))
	{
		MSG_BOX("Create Failed : CShadow");
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CShadow::Free()
{
	__super::Free();

	Safe_Release(m_pGameInstance);
}
