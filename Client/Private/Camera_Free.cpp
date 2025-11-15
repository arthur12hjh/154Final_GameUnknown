#include "pch.h"
#include "Camera_Free.h"

#include "GameInstance.h"

CCamera_Free::CCamera_Free(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCamera { pDevice, pContext }
{

}

CCamera_Free::CCamera_Free(const CCamera_Free& Prototype) 
	: CCamera { Prototype }
{

}

HRESULT CCamera_Free::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCamera_Free::Initialize(void* pArg)
{	
	CAMERA_FREE_DESC* pDesc = static_cast<CAMERA_FREE_DESC*>(pArg);
	m_fMouseSensor = pDesc->fMouseSensor;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CCamera_Free::Priority_Update(_float fTimeDelta)
{
	m_fMouseSensor = 0.1f;
	if (false == m_bIsCameraAnimation)
	{
		if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_W))
			m_pTransformCom->Go_Straight(fTimeDelta);

		if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_S))
			m_pTransformCom->Go_Backward(fTimeDelta);

		if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_A))
			m_pTransformCom->Go_Left(fTimeDelta);

		if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_D))
			m_pTransformCom->Go_Right(fTimeDelta);

		_long		MouseMove = {};
		if (MouseMove = m_pGameInstance->GetMouseAxis(ENUM_CLASS(MOUSEMOVESTATE::HORIZONTAL)))
		{
			m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * MouseMove * m_fMouseSensor);
		}

		if (MouseMove = m_pGameInstance->GetMouseAxis(ENUM_CLASS(MOUSEMOVESTATE::VERTICAL)))
		{
			m_pTransformCom->Turn(m_pTransformCom->Get_State(STATE::RIGHT), fTimeDelta * MouseMove * m_fMouseSensor);
		}
	}
	else
	{
		m_fAccTime += fTimeDelta;
		if (m_fAccTime >= m_fLerpTime)
		{
			m_fAccTime = 0.f;
			m_bIsCameraAnimation = false;
		}
		else
		{
			// ���⼭ lerp ����
			_matrix StartMat = XMLoadFloat4x4(&m_fStartLerpMatrix);
			_matrix EndMat = XMLoadFloat4x4(&m_fEndLerpMatrix);

			_float fRatio = m_fAccTime / m_fLerpTime;
			_vector vPosition = XMVectorLerp(StartMat.r[3], EndMat.r[3], fRatio);
			_vector vLookPos = vPosition + XMVectorLerp(StartMat.r[2], EndMat.r[2], fRatio);

			m_pTransformCom->Set_State(STATE::POSITION, vPosition);
			m_pTransformCom->LookAt(vLookPos);
		}
	}

	__super::Bind_Matrices();
}

void CCamera_Free::Update(_float fTimeDelta)
{
	
}

void CCamera_Free::Late_Update(_float fTimeDelta)
{

	
}

HRESULT CCamera_Free::Render()
{
	
	return S_OK;
}

void CCamera_Free::SetCameraAnimation(const _float4x4* StartLerpMatrix, const _float4x4* EndLerpMatrix, _bool bIsLerp)
{
	m_bIsCameraAnimation = bIsLerp;
	if (m_bIsCameraAnimation)
	{
		memcpy(&m_fStartLerpMatrix, StartLerpMatrix, sizeof(_float4x4));
		memcpy(&m_fEndLerpMatrix, EndLerpMatrix, sizeof(_float4x4));
	}
}

CCamera_Free* CCamera_Free::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCamera_Free* pInstance = new CCamera_Free(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCamera_Free::Clone(void* pArg)
{
	CCamera_Free* pInstance = new CCamera_Free(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCamera_Free");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCamera_Free::Free()
{
	__super::Free();


}
