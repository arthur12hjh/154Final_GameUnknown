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
	m_pTransformCom->Rotation(0, 0, 0);
	_float fLength = XMVectorGetX(XMVector3Length(XMVectorSubtract(XMVectorSet(0, 0, 0, 1), m_pTransformCom->Get_State(STATE::POSITION))));
	_vector dir = m_pTransformCom->Get_State(STATE::LOOK);
	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(0, 0, 0, 1) - dir * fLength);
	return S_OK;
}

void CCamera_Free::Priority_Update(_float fTimeDelta)
{
	_long		MouseMove = {};
	if (m_pGameInstance->KeyPressed(KEY_INPUT::MOUSE, ENUM_CLASS(MOUSEKEYSTATE::RBUTTON))) {
		if (m_bIsCursor) {
			ShowCursor(m_bIsCursor = false);
		}
		SetCursorPos(823, 463);
		if (MouseMove = m_pGameInstance->GetMouseAxis(ENUM_CLASS(MOUSEMOVESTATE::W))) {
			m_pTransformCom->Go_Straight(fTimeDelta * MouseMove * 0.01);
		}
		if (MouseMove = m_pGameInstance->GetMouseAxis(ENUM_CLASS(MOUSEMOVESTATE::HORIZONTAL)))
		{
			m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * MouseMove * m_fMouseSensor);
		}

		if (MouseMove = m_pGameInstance->GetMouseAxis(ENUM_CLASS(MOUSEMOVESTATE::VERTICAL)))
		{
			m_pTransformCom->Turn(m_pTransformCom->Get_State(STATE::RIGHT), fTimeDelta * MouseMove * m_fMouseSensor);
		}
		_float fLength = XMVectorGetX(XMVector3Length(XMVectorSubtract(XMVectorSet(0,0,0,1), m_pTransformCom->Get_State(STATE::POSITION))));
		_vector dir = m_pTransformCom->Get_State(STATE::LOOK);
		m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(0, 0, 0, 1) - dir * fLength);
	}
	else if(!m_bIsCursor){
		ShowCursor(m_bIsCursor = true);
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
