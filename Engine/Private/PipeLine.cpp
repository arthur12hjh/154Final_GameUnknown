#include "PipeLine.h"

#include "GameInstance.h"
#include "Camera.h"

CPipeLine::CPipeLine() :
	m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
	
	XMStoreFloat4x4(&m_IdentityMatrix, XMMatrixIdentity());

	/* 역행렬도 초기화 해놓기.*/
	for (_uint i = 0; i < ENUM_CLASS(D3DTS::END); ++i)
	{
		XMStoreFloat4x4(&m_TransformStateMatrices[i], XMMatrixIdentity());
		XMStoreFloat4x4(&m_TransformStateMatrixInverse[i], XMMatrixIdentity());

		XMStoreFloat4x4(&m_PreTransformStateMatrices[i], XMMatrixIdentity());
		XMStoreFloat4x4(&m_PreTransformStateMatrixInverse[i], XMMatrixIdentity());
	}
}

void CPipeLine::Set_Transform(D3DTS eState, _fmatrix TransformStateMatrix)
{
	//뷰, 투영 행렬이 업데이트 될때 기존 정보를 PreTransformMatrices에 넣어주자.
	XMStoreFloat4x4(&m_PreTransformStateMatrices[ENUM_CLASS(eState)], XMLoadFloat4x4(&m_TransformStateMatrices[ENUM_CLASS(eState)]));
	XMStoreFloat4x4(&m_TransformStateMatrices[ENUM_CLASS(eState)], TransformStateMatrix);

}

const _float4x4* CPipeLine::Get_PreTransform_Float4x4(D3DTS eState)
{
	return &m_PreTransformStateMatrices[ENUM_CLASS(eState)];
}

_matrix CPipeLine::Get_PreTransform_Matrix(D3DTS eState)
{
	return XMLoadFloat4x4(&m_PreTransformStateMatrices[ENUM_CLASS(eState)]);
}

const _float4x4* CPipeLine::Get_Transform_Float4x4(D3DTS eState)
{
	return &m_TransformStateMatrices[ENUM_CLASS(eState)];
}

_matrix CPipeLine::Get_Transform_Matrix(D3DTS eState)
{
	return XMLoadFloat4x4(&m_TransformStateMatrices[ENUM_CLASS(eState)]);
}

const _float4x4* CPipeLine::Get_Transform_Float4x4_Inverse(D3DTS eState)
{
	return &m_TransformStateMatrixInverse[ENUM_CLASS(eState)];
}

_matrix CPipeLine::Get_Transform_Matrix_Inverse(D3DTS eState)
{
	return XMLoadFloat4x4(&m_TransformStateMatrixInverse[ENUM_CLASS(eState)]);
}

const _float4* CPipeLine::Get_CamRight()
{
	return &m_vCamRight;
}

const _float4* CPipeLine::Get_CamUp()
{
	return &m_vCamUp;
}

const _float4* CPipeLine::Get_CamLook()
{
	return &m_vCamLook;
}

const _float4* CPipeLine::Get_CamPosition()
{
	return &m_vCamPosition;
}

const _float4* CPipeLine::Get_PreCamRight()
{
	return &m_vPreCamRight;
}

const _float4* CPipeLine::Get_PreCamUp()
{
	return &m_vPreCamUp;
}

const _float4* CPipeLine::Get_PreCamLook()
{
	return &m_vPreCamLook;
}

const _float4* CPipeLine::Get_PreCamPosition()
{
	return &m_vPreCamPosition;
}

void CPipeLine::Update()
{
	for (_uint i = 0; i < ENUM_CLASS(D3DTS::END); ++i)
	{
		XMStoreFloat4x4(&m_TransformStateMatrixInverse[i], XMMatrixInverse(nullptr, Get_Transform_Matrix(static_cast<D3DTS>(i))));
	}

	memcpy(&m_vPreCamRight, &m_vCamRight, sizeof(_float4));
	memcpy(&m_vPreCamUp, &m_vCamUp, sizeof(_float4));
	memcpy(&m_vPreCamLook, &m_vCamLook, sizeof(_float4));
	memcpy(&m_vPreCamPosition, &m_vCamPosition, sizeof(_float4));

	memcpy(&m_vCamRight, &m_TransformStateMatrixInverse[ENUM_CLASS(D3DTS::VIEW)].m[0], sizeof(_float4));
	memcpy(&m_vCamUp, &m_TransformStateMatrixInverse[ENUM_CLASS(D3DTS::VIEW)].m[1], sizeof(_float4));
	memcpy(&m_vCamLook, &m_TransformStateMatrixInverse[ENUM_CLASS(D3DTS::VIEW)].m[2], sizeof(_float4));
	memcpy(&m_vCamPosition, &m_TransformStateMatrixInverse[ENUM_CLASS(D3DTS::VIEW)].m[3], sizeof(_float4));

	CCamera* pCam = m_pGameInstance->GetMainCamera();
	Safe_Release(pCam);

	if(nullptr != pCam)
		m_tCamDesc = pCam->GetCameraInfo();
}

CPipeLine* CPipeLine::Create()
{
	return new CPipeLine();
}

void CPipeLine::Free()
{
	__super::Free();

	Safe_Release(m_pGameInstance);
}

