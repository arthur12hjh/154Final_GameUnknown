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

HRESULT CCamera::Bind_Matrices()
{
	m_pGameInstance->Set_Transform(D3DTS::VIEW, XMMatrixInverse(nullptr, XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr())));
	m_pGameInstance->Set_Transform(D3DTS::PROJ, XMMatrixPerspectiveFovLH(m_pCameraInfo.fFov, m_pCameraInfo.fAspect, m_pCameraInfo.fNear, m_pCameraInfo.fFar));

	return S_OK;
}

void CCamera::Free()
{
	__super::Free();
}
