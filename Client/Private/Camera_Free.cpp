#include "pch.h"
#include "Camera_Free.h"

#include "GameInstance.h"
#include "Interaction_Component.h"

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

	/* Com_Collider_AABB */
	//CBoxCollider::BOX_COLLIDER_DESC		AABBDesc{};
	//
	//AABBDesc.vSize = _float3(0.5f, 0.5f, 0.5f);
	//AABBDesc.vCenter = _float3(0.f, AABBDesc.vSize.y * 0.5f, 0.f);
	//
	//if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Collider_AABB"),
	//	TEXT("Com_Collider_AABB"), reinterpret_cast<CComponent**>(&m_pColliderCom), &AABBDesc)))
	//	return E_FAIL;

	return S_OK;
}

void CCamera_Free::Priority_Update(_float fTimeDelta)
{
	fTimeDelta = 0.016;

	if (false == m_pGameInstance->IsMainCamera(this))
		return;

	if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_ESCAPE))
	{
		m_bIsLock[1] = !m_bIsLock[1];
	}

	m_fMouseSensor = 0.1f;
	if(m_pGameInstance->IsMainCamera(this))
	{
		if (false == m_bIsCameraAnimation)
		{
			if (false == m_bIsLock[0])
			{
				if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_UPARROW))
					m_pTransformCom->Go_Straight(fTimeDelta * m_fCameraSpeed);

				if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_DOWNARROW))
					m_pTransformCom->Go_Backward(fTimeDelta * m_fCameraSpeed);

				if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_LEFTARROW))
					m_pTransformCom->Go_Left(fTimeDelta * m_fCameraSpeed);

				if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_RIGHTARROW))
					m_pTransformCom->Go_Right(fTimeDelta * m_fCameraSpeed);

				if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_COMMA))
					m_pTransformCom->Go_Up(fTimeDelta * m_fCameraSpeed);

				if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_PERIOD))
					m_pTransformCom->Go_Down(fTimeDelta * m_fCameraSpeed);


				if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_F))
				{
					auto pInterraction = m_pGameInstance->GetNearInteraction();
					if (pInterraction)
						pInterraction->Action_InteractionEvent(fTimeDelta, this);
				}

			}

			if (false == m_bIsLock[1])
			{
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
				// Somooth Lerp
				_matrix StartMat = XMLoadFloat4x4(&m_fStartLerpMatrix);
				_matrix EndMat = XMLoadFloat4x4(&m_fEndLerpMatrix);

				_float fRatio = m_fAccTime / m_fLerpTime;
				_vector vPosition = XMVectorLerp(StartMat.r[3], EndMat.r[3], fRatio);
				_vector vLookPos = vPosition + XMVectorLerp(StartMat.r[2], EndMat.r[2], fRatio);

				m_pTransformCom->Set_State(STATE::POSITION, vPosition);
				m_pTransformCom->LookAt(vLookPos);
			}
		}
		__super::Bind_Matrices(fTimeDelta);
	}
}

void CCamera_Free::Update(_float fTimeDelta)
{
	//_matrix WorldMat = XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr());
	//m_pColliderCom->UpdateColiision(WorldMat);

	//카메라 쉐이킹을 위해서 부모 Update 호출. 
	//__super::Update(fTimeDelta);
}

void CCamera_Free::Late_Update(_float fTimeDelta)
{
//	m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
//
//#ifdef _DEBUG
//	m_pGameInstance->Add_DebugComponent(m_pColliderCom);
//	m_pGameInstance->ADD_Collider(m_pColliderCom);
//#endif
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

void CCamera_Free::CameraLock(_bool bIsKeyBoard, _bool bIsMouse)
{
	m_bIsLock[0] = bIsKeyBoard;
	m_bIsLock[1] = bIsMouse;
}

void CCamera_Free::GetCameraLock(_bool (&pOut)[2])
{
	pOut[0] = m_bIsLock[0];
	pOut[1] = m_bIsLock[1];
}

void CCamera_Free::SetCameraSpeed(_float fCameraSpeed)
{
	m_fCameraSpeed = fCameraSpeed;
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

	//Safe_Release(m_pColliderCom);
}
