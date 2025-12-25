#include "Camera.h"
#include "GameInstance.h"

CCamera::CCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject { pDevice, pContext }
{
}

CCamera::CCamera(const CCamera& Prototype)
	: CGameObject{ Prototype }
{
}

void CCamera::Shake(_float fShakeTime, _float fIntensity)
{
	m_IsShake = true;

	m_fIntensity = fIntensity;
	m_fShakeTime = fShakeTime;
	m_fShakeTimeAcc = 0.f;
	XMStoreFloat4(&m_vOriginPos, m_pTransformCom->Get_State(STATE::POSITION));
}

void CCamera::Set_MainCamera(const WCHAR* szCameraTag)
{
	m_pGameInstance->SetMainCamera(szCameraTag, &m_PreLerpMatrix);

	m_fTransitionLerpTime.x = 0.f;
	m_bIsTransition = TRUE;
}

HRESULT CCamera::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCamera::Initialize(void* pArg)
{
	CAMERA_DESC* pDesc = static_cast<CAMERA_DESC*>(pArg);	

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSetW(XMLoadFloat3(&pDesc->vEye), 1.f));
	m_pTransformCom->LookAt(XMVectorSetW(XMLoadFloat3(&pDesc->vAt), 1.f));

	_uint		iNumViewports = { 1 };
	D3D11_VIEWPORT	ViewportDesc{};
	m_pContext->RSGetViewports(&iNumViewports, &ViewportDesc);

	m_pCameraInfo.fFov = pDesc->fFov;
	m_pCameraInfo.fNear = pDesc->fNear;
	m_pCameraInfo.fFar = pDesc->fFar;
	m_pCameraInfo.fAspect = ViewportDesc.Width / ViewportDesc.Height;

	m_fTransitionLerpTime = { 0.f, 1.f };
	m_bIsTransition = FALSE;

	return S_OK;
}

void CCamera::Priority_Update(_float fTimeDelta)
{


}

void CCamera::Update(_float fTimeDelta)
{
}

void CCamera::Late_Update(_float fTimeDelta)
{
}

HRESULT CCamera::Render()
{
	return S_OK;
}


void CCamera::SetCameraInfo(const CAMERA_INFO& CameraInfo, bitset<4> bitFlag)
{
	if (true == bitFlag.test(0))
		m_pCameraInfo.fFov = CameraInfo.fFov;

	if (true == bitFlag.test(1))
		m_pCameraInfo.fNear = CameraInfo.fNear;

	if (true == bitFlag.test(2))
		m_pCameraInfo.fFar = CameraInfo.fFar;

	if (true == bitFlag.test(3))
		m_pCameraInfo.fAspect = CameraInfo.fAspect;
}

HRESULT CCamera::Bind_Matrices(_float fTimeDelta)
{
	if (true == m_IsShake)
	{
		m_fShakeTimeAcc += fTimeDelta;
		_vector vPos = m_pTransformCom->Get_State(STATE::POSITION);
		_vector vRight = m_pTransformCom->Get_State(STATE::RIGHT);
		_vector vUp = m_pTransformCom->Get_State(STATE::UP);

		// 정규화 
		vRight = XMVector3Normalize(vRight);
		vUp = XMVector3Normalize(vUp);

		// 시간이 지날수록 약하게
		_float fAttenuation = 1.f - (m_fShakeTimeAcc / m_fShakeTime);
		fAttenuation = max(0.f, fAttenuation);

		_float fOffsetRight = m_pGameInstance->Random_Normal() * m_fIntensity * fAttenuation;
		_float fOffsetUp = m_pGameInstance->Random_Normal() * m_fIntensity * fAttenuation;

		// 오프셋 벡터 계산 (right,up 방향으로만 흔들림, look 방향으론 안흔들림.)
		_vector vShakeOffset = vRight * fOffsetRight + vUp * fOffsetUp;
		_vector vNewPos = vPos + vShakeOffset;

		m_pTransformCom->Set_State(STATE::POSITION, vNewPos);

		if (m_fShakeTime <= m_fShakeTimeAcc)
		{
			m_IsShake = false;
			m_fShakeTimeAcc = 0.f;
		}
	}

	m_pGameInstance->Set_Transform(D3DTS::VIEW, XMMatrixInverse(nullptr, XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr())));
	m_pGameInstance->Set_Transform(D3DTS::PROJ, XMMatrixPerspectiveFovLH(m_pCameraInfo.fFov, m_pCameraInfo.fAspect, m_pCameraInfo.fNear, m_pCameraInfo.fFar));

	return S_OK;
}

void CCamera::Free()
{
	__super::Free();
}
