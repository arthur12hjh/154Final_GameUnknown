#pragma once

#include "Base.h"

/* 뷰, 투영행렬을 저장한다, */
/* 역행렬 매프레임당 한번씩만 구해서 저장한다. */
/* 카메라의 위치도 구해서 저장한다. */

NS_BEGIN(Engine)

class CPipeLine final : public CBase
{
private:
	CPipeLine();
	virtual ~CPipeLine() = default;

public:
	void Set_Transform(D3DTS eState, _fmatrix TransformStateMatrix); 

public:
	const _float4x4*	Get_PreTransform_Float4x4(D3DTS eState);
	_matrix				Get_PreTransform_Matrix(D3DTS eState);
	const _float4x4*	Get_Transform_Float4x4(D3DTS eState);
	_matrix				Get_Transform_Matrix(D3DTS eState);
	const _float4x4*	Get_Transform_Float4x4_Inverse(D3DTS eState);
	_matrix				Get_Transform_Matrix_Inverse(D3DTS eState);
	const _float4*		Get_CamPosition();

	// 항등행렬 꺼내오기
	_matrix				GetIdentityMatrix() { return XMLoadFloat4x4(&m_IdentityMatrix); }

	// 항등행렬 포인터 꺼내오기
	const _float4x4*	GetIdentityMatrixPtr() { return &m_IdentityMatrix; }

public:
	void				Update();

private:
	_float4x4				m_TransformStateMatrices[ENUM_CLASS(D3DTS::END)] = {};
	_float4x4				m_TransformStateMatrixInverse[ENUM_CLASS(D3DTS::END)] = {};

	_float4x4				m_PreTransformStateMatrices[ENUM_CLASS(D3DTS::END)] = {}; 
	_float4x4				m_PreTransformStateMatrixInverse[ENUM_CLASS(D3DTS::END)] = {};

	_float4x4				m_IdentityMatrix = {};

	_float4					m_vCamPosition = {};


public:
	static CPipeLine* Create();
	virtual void Free() override;
	
};

NS_END