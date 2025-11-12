#include "pch.h"
#include "Camera_AnimationEditor.h"

#include "GameInstance.h"
#include "Tool_Manager.h"

CCamera_AnimationEditor::CCamera_AnimationEditor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCamera{ pDevice, pContext }
{
}

CCamera_AnimationEditor::CCamera_AnimationEditor(const CCamera_AnimationEditor& Prototype)
	: CCamera{ Prototype }
{
}

HRESULT CCamera_AnimationEditor::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCamera_AnimationEditor::Initialize(void* pArg)
{
	m_pTool_Manager = CTool_Manager::GetInstance();
	// 늘 하던 초기화처럼 DESC 넣어주고 super::Initialize ㄱㄱ
	CAMERA_DESC* pDesc = static_cast<CAMERA_DESC*>(pArg);
	m_fMouseSensor = pDesc->fMouseSensor;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	Initialize_Position();

	return S_OK;
}

void CCamera_AnimationEditor::Priority_Update(_float fTimeDelta)
{

	if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_W))
		m_pTransformCom->Go_Straight(fTimeDelta);
	if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_S))
		m_pTransformCom->Go_Backward(fTimeDelta);
	if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_A))
		m_pTransformCom->Go_Left(fTimeDelta);
	if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_D))
		m_pTransformCom->Go_Right(fTimeDelta);
	if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_SPACE))
	{
		m_pTransformCom->Set_State(STATE::POSITION, m_pTransformCom->Get_State(STATE::POSITION) + XMVectorSet(0.f, fTimeDelta * 5.f, 0.f, 0.f));
	}
	if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_LSHIFT))
	{
		m_pTransformCom->Set_State(STATE::POSITION, m_pTransformCom->Get_State(STATE::POSITION) + XMVectorSet(0.f, fTimeDelta * -5.f, 0.f, 0.f));
	}
	//
	//_long MouseMove = {};
	//if (MouseMove = m_pGameInstance->GetMouseAxis(ENUM_CLASS(MOUSEMOVESTATE::HORIZONTAL)))
	//{
	//	m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * MouseMove * m_fMouseSensor);
	//}
	//
	//if (MouseMove = m_pGameInstance->GetMouseAxis(ENUM_CLASS(MOUSEMOVESTATE::VERTICAL)))
	//{
	//	m_pTransformCom->Turn(m_pTransformCom->Get_State(STATE::RIGHT), fTimeDelta * MouseMove * m_fMouseSensor);
	//}
	


	m_pTransformCom->LookAt(XMVectorSet(0.f, -1.f, 0.f, 1.f));



	__super::Bind_Matrices();
}

void CCamera_AnimationEditor::Update(_float fTimeDelta)
{

}

void CCamera_AnimationEditor::Late_Update(_float fTimeDelta)
{
}

HRESULT CCamera_AnimationEditor::Render()
{
	return S_OK;
}

void CCamera_AnimationEditor::Initialize_Position()
{
	m_pTransformCom->Set_State(STATE::RIGHT, XMVectorSet(1.f, 0.f, 0.f, 0.f));
	m_pTransformCom->Set_State(STATE::UP, XMVectorSet(0.f, 1.f, 0.f, 0.f));
	m_pTransformCom->Set_State(STATE::LOOK, XMVectorSet(0.f, 0.f, 1.f, 0.f));
	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(-4.f, 4.f, 0.f, 1.f));
}


CCamera_AnimationEditor* CCamera_AnimationEditor::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCamera_AnimationEditor* pInstance = new CCamera_AnimationEditor(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCamera_AnimationEditor::Clone(void* pArg)
{
	CCamera_AnimationEditor* pInstance = new CCamera_AnimationEditor(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCamera_AnimationEditor");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCamera_AnimationEditor::Free()
{
	__super::Free();
}
