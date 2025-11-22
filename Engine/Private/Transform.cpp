#include "Transform.h"
#include "Shader.h"

#include "Navigation.h"

CTransform::CTransform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent { pDevice, pContext }
{

}

CTransform::CTransform(const CTransform& Prototype)
	: CComponent { Prototype }
	, m_WorldMatrix { Prototype.m_WorldMatrix }
{

}

_float3 CTransform::Get_Scale() const
{	
	return _float3(
		XMVectorGetX(XMVector3Length(Get_State(STATE::RIGHT))),
		XMVectorGetX(XMVector3Length(Get_State(STATE::UP))),
		XMVectorGetX(XMVector3Length(Get_State(STATE::LOOK)))
	);
}

void CTransform::Set_State(STATE eState, _vector vState)
{
	switch (eState)
	{
	case Engine::STATE::RIGHT:
	case Engine::STATE::UP:
	case Engine::STATE::LOOK:
		XMStoreFloat4(reinterpret_cast<_float4*>(&m_WorldMatrix.m[ENUM_CLASS(eState)]), vState);
		break;
	case Engine::STATE::POSITION:
		vState.m128_f32[3] = 1.f;
		XMStoreFloat4(reinterpret_cast<_float4*>(&m_WorldMatrix.m[ENUM_CLASS(eState)]), vState);
		break;
	}
}

void CTransform::Set_Scale(_vector vScale)
{
	Set_State(STATE::RIGHT, XMVector3Normalize(Get_State(STATE::RIGHT)) * vScale.m128_f32[0]);
	Set_State(STATE::UP, XMVector3Normalize(Get_State(STATE::UP)) * vScale.m128_f32[1]);
	Set_State(STATE::LOOK, XMVector3Normalize(Get_State(STATE::LOOK)) * vScale.m128_f32[2]);
}

void CTransform::Set_Rotation(_vector vRotation)
{
	Rotation(vRotation.m128_f32[0], vRotation.m128_f32[1], vRotation.m128_f32[2]);
}

void CTransform::Set_Scale(_float fX, _float fY, _float fZ)
{
	Set_State(STATE::RIGHT, XMVector3Normalize(Get_State(STATE::RIGHT)) * fX);
	Set_State(STATE::UP, XMVector3Normalize(Get_State(STATE::UP)) * fY);
	Set_State(STATE::LOOK, XMVector3Normalize(Get_State(STATE::LOOK)) * fZ);
}

HRESULT CTransform::Initialize_Prototype()
{
	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());

	return S_OK;
}

HRESULT CTransform::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return S_OK;

	TRANSFORM_DESC* pDesc = static_cast<TRANSFORM_DESC*>(pArg);

	m_fSpeedPerSec = pDesc->fSpeedPerSec;
	m_fRotationPerSec = pDesc->fRotationPerSec;



	return S_OK;
}

HRESULT CTransform::Bind_ShaderResource(CShader* pShader, const _char* pConstantName)
{
	return pShader->Bind_Matrix(pConstantName, &m_WorldMatrix);
	
}

// 결과벡터의 주소 D3DXVec3Normalize(&결과, &누구를);
// 길이1짜리벡터 XMVector3Normalize(누구를);

void CTransform::Go_Straight(_float fTimeDelta, class CNavigation* pNavigation)
{
	_vector		vPosition = Get_State(STATE::POSITION);
	_vector		vLook = Get_State(STATE::LOOK);	

	vPosition += XMVector3Normalize(vLook) * m_fSpeedPerSec * fTimeDelta;

	if(nullptr == pNavigation || 
		true == pNavigation->isMove(vPosition))
		Set_State(STATE::POSITION, vPosition);
}

void CTransform::Go_Backward(_float fTimeDelta)
{
	_vector		vPosition = Get_State(STATE::POSITION);
	_vector		vLook = Get_State(STATE::LOOK);

	vPosition -= XMVector3Normalize(vLook) * m_fSpeedPerSec * fTimeDelta;

	Set_State(STATE::POSITION, vPosition);
}

void CTransform::Go_Left(_float fTimeDelta)
{
	_vector		vPosition = Get_State(STATE::POSITION);
	_vector		vRight = Get_State(STATE::RIGHT);

	vPosition -= XMVector3Normalize(vRight) * m_fSpeedPerSec * fTimeDelta;

	Set_State(STATE::POSITION, vPosition);
}

void CTransform::Go_Right(_float fTimeDelta)
{
	_vector		vPosition = Get_State(STATE::POSITION);
	_vector		vRight = Get_State(STATE::RIGHT);

	vPosition += XMVector3Normalize(vRight) * m_fSpeedPerSec * fTimeDelta;

	Set_State(STATE::POSITION, vPosition);
}

void CTransform::Move_Direction(_float fTimeDelta, _vector vDir, _float fSpeed)
{
	_vector		vPosition = Get_State(STATE::POSITION);
	vPosition += (vDir * fSpeed * fTimeDelta);

	Set_State(STATE::POSITION, vPosition);
}

void CTransform::Turn(_fvector vAxis, _float fTimeDelta)
{
	_vector		vRight = Get_State(STATE::RIGHT);
	_vector		vUp = Get_State(STATE::UP);
	_vector		vLook = Get_State(STATE::LOOK);	

	_matrix		RotationMatrix = XMMatrixRotationAxis(vAxis, m_fRotationPerSec * fTimeDelta);

	vRight = XMVector3TransformNormal(vRight, RotationMatrix);
	vUp = XMVector3TransformNormal(vUp, RotationMatrix);
	vLook = XMVector3TransformNormal(vLook, RotationMatrix);

	Set_State(STATE::RIGHT, vRight);
	Set_State(STATE::UP, vUp);
	Set_State(STATE::LOOK, vLook);
}

void CTransform::Rotation(_fvector vAxis, _float fRadian)
{
	_float3		vScale = Get_Scale();

	_vector		vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f) * vScale.x;
	_vector		vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f) * vScale.y;
	_vector		vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f) * vScale.z;

	_matrix		RotationMatrix = XMMatrixRotationAxis(vAxis, fRadian);

	vRight = XMVector3TransformNormal(vRight, RotationMatrix);
	vUp = XMVector3TransformNormal(vUp, RotationMatrix);
	vLook = XMVector3TransformNormal(vLook, RotationMatrix);

	Set_State(STATE::RIGHT, vRight);
	Set_State(STATE::UP, vUp);
	Set_State(STATE::LOOK, vLook);
}

void CTransform::Rotation(_float fRadianX, _float fRadianY, _float fRadianZ)
{
	_float3		vScale = Get_Scale();

	_vector		vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f) * vScale.x;
	_vector		vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f) * vScale.y;
	_vector		vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f) * vScale.z;

	/* 짐벌락 : 복합적인 회전을 적용하다보면 특정 축 회전이 이전 축 회전을 상쇄시켜버리는 경우가 발생하여 */
	/* 특정 축에 대한 회전이 무효화되는 상황이 발생한다. */
	/*_matrix		RotationMatrix = XMMatrixRotationX(fRadianX) * XMMatrixRotationY(fRadianY) * XMMatrixRotationZ(fRadianZ);*/

	/* 복소수가 회전을 의미한다. (a + bi)*/
	/* w + xi + yj + zk */
	_vector		vQuternion = XMQuaternionRotationRollPitchYaw(fRadianX, fRadianY, fRadianZ);
	_matrix		RotationMatrix = XMMatrixRotationQuaternion(vQuternion);

	vRight = XMVector3TransformNormal(vRight, RotationMatrix);
	vUp = XMVector3TransformNormal(vUp, RotationMatrix);
	vLook = XMVector3TransformNormal(vLook, RotationMatrix);

	Set_State(STATE::RIGHT, vRight);
	Set_State(STATE::UP, vUp);
	Set_State(STATE::LOOK, vLook);
}

void CTransform::Rotation(_float fQuatX, _float fQuatY, _float fQuatZ, _float fQuatW)
{
	_float3		vScale = Get_Scale();

	_vector		vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f) * vScale.x;
	_vector		vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f) * vScale.y;
	_vector		vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f) * vScale.z;

	_matrix		RotationMatrix = XMMatrixRotationQuaternion(XMVectorSet(fQuatX, fQuatY, fQuatZ, fQuatW));
	vRight = XMVector3TransformNormal(vRight, RotationMatrix);
	vUp = XMVector3TransformNormal(vUp, RotationMatrix);
	vLook = XMVector3TransformNormal(vLook, RotationMatrix);

	Set_State(STATE::RIGHT, vRight);
	Set_State(STATE::UP, vUp);
	Set_State(STATE::LOOK, vLook);
}

void CTransform::LookAt(_fvector vAt)
{
	_float3		vScale = Get_Scale();

	_vector		vLook = vAt - Get_State(STATE::POSITION);
	_vector		vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);
	_vector		vUp = XMVector3Cross(vLook, vRight);

	Set_State(STATE::RIGHT, XMVector3Normalize(vRight) * vScale.x);
	Set_State(STATE::UP, XMVector3Normalize(vUp) * vScale.y);
	Set_State(STATE::LOOK, XMVector3Normalize(vLook) * vScale.z);
}



CTransform* CTransform::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTransform* pInstance = new CTransform(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CTransform");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CTransform::Clone(void* pArg)
{
	CTransform* pInstance = new CTransform(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTransform");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTransform::Free()
{
	__super::Free();

}
