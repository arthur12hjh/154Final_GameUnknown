#include "PipeLine.h"

CPipeLine::CPipeLine()
{
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

const _float4* CPipeLine::Get_CamPosition()
{
	return &m_vCamPosition;
}

void CPipeLine::Update()
{
	for (_uint i = 0; i < ENUM_CLASS(D3DTS::END); ++i)
	{
		XMStoreFloat4x4(&m_TransformStateMatrixInverse[i], XMMatrixInverse(nullptr, Get_Transform_Matrix(static_cast<D3DTS>(i))));
	}

	memcpy(&m_vCamPosition, &m_TransformStateMatrixInverse[ENUM_CLASS(D3DTS::VIEW)].m[3], sizeof(_float4));
}

CPipeLine* CPipeLine::Create()
{
	return new CPipeLine();
}

void CPipeLine::Free()
{
	__super::Free();
}

