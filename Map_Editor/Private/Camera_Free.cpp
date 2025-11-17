#include "pch.h"
#include "Camera_Free.h"

#include "GameInstance.h"
#include "CinemaTrack.h"

CCamera_Free::CCamera_Free(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCamera{ pDevice, pContext }
{

}

CCamera_Free::CCamera_Free(const CCamera_Free& Prototype)
	: CCamera{ Prototype }
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

	m_pGameInstance->Add_Camera(TEXT("MainCamera"), this);
	m_pGameInstance->SetMainCamera(TEXT("MainCamera"));
	return S_OK;
}

void CCamera_Free::Priority_Update(_float fTimeDelta)
{
	if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_W))
		m_pTransformCom->Go_Straight(fTimeDelta * 10.f);

	if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_S))
		m_pTransformCom->Go_Backward(fTimeDelta * 5.f);

	if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_A))
		m_pTransformCom->Go_Left(fTimeDelta * 5.f);

	if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_D))
		m_pTransformCom->Go_Right(fTimeDelta * 5.f);

	if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_SPACE))
		m_pTransformCom->Set_State(STATE::POSITION, m_pTransformCom->Get_State(STATE::POSITION) + XMVectorSet(0.f, 15.f * fTimeDelta, 0.f, 0.f));

	if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_LSHIFT))
		m_pTransformCom->Set_State(STATE::POSITION, m_pTransformCom->Get_State(STATE::POSITION) - XMVectorSet(0.f, 15.f * fTimeDelta, 0.f, 0.f));


	_long		MouseMove = {};

	if (m_pGameInstance->KeyPressed(KEY_INPUT::MOUSE, ENUM_CLASS(MOUSEKEYSTATE::RBUTTON)))
	{

		if (MouseMove = m_pGameInstance->GetMouseAxis(ENUM_CLASS(MOUSEMOVESTATE::HORIZONTAL)))
		{
			m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * MouseMove * m_fMouseSensor);
		}

		if (MouseMove = m_pGameInstance->GetMouseAxis(ENUM_CLASS(MOUSEMOVESTATE::VERTICAL)))
		{
			m_pTransformCom->Turn(m_pTransformCom->Get_State(STATE::RIGHT), fTimeDelta * MouseMove * m_fMouseSensor);
		}
	}

	__super::Bind_Matrices();
}

void CCamera_Free::Update(_float fTimeDelta)
{
	if (m_pAnimation)
	{
		if (m_pAnimation->Play_Animation(m_pTransformCom, fTimeDelta))
			m_pAnimation = nullptr;
	}
}

void CCamera_Free::Late_Update(_float fTimeDelta)
{


}

HRESULT CCamera_Free::Render()
{

	return S_OK;
}

void CCamera_Free::CameraAnimtaionTest(CCinemaTrack* pCinemaData, _float fPlayTime)
{
	m_pAnimation = pCinemaData;
	m_pAnimation->Set_Animation(fPlayTime);
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
