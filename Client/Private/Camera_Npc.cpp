#include "pch.h"
#include "Camera_Npc.h"

#include "GameInstance.h"
#include "Interaction_Component.h"

#include "Npc.h"
#include "GameManager.h"
#include "Player.h"

CCamera_Npc::CCamera_Npc(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCamera { pDevice, pContext }
{
}

CCamera_Npc::CCamera_Npc(const CCamera_Npc& Prototype) 
	: CCamera { Prototype }
{
}

HRESULT CCamera_Npc::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCamera_Npc::Initialize(void* pArg)
{	
	Camera_Npc_DESC* pDesc = static_cast<Camera_Npc_DESC*>(pArg);
	m_fMouseSensor = pDesc->fMouseSensor;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CCamera_Npc::Priority_Update(_float fTimeDelta)
{
	if (false == m_pGameInstance->IsMainCamera(this))
		return;

	if (m_pGameInstance->IsMainCamera(this) && m_pTargetNpc)
	{
		if (m_fAccTime <= m_fLerpTime && !m_isLerpEnd)
		{
			m_fAccTime += fTimeDelta;

			_float t = m_fAccTime / m_fLerpTime;
			t = Clamp(t, 0.f, 1.f);
			if (m_isReverse)
				t = 1.f - t;

			_matrix StartMat = XMLoadFloat4x4(&m_fStartLerpMatrix);
			_matrix EndMat = XMLoadFloat4x4(&m_fEndLerpMatrix);

			// Pivot (NPC �� EndPos �߰�)
			_vector vNpcPos = m_pTargetNpc->Get_Position();
			_vector vEndPos = EndMat.r[3];
			_vector vPivot = XMVectorLerp(vNpcPos, vEndPos, 0.5f);

			// Start �Ķ���� ����
			_vector vStartToCam = StartMat.r[3] - vPivot;

			_float startAngle = atan2f(
				XMVectorGetZ(vStartToCam),
				XMVectorGetX(vStartToCam)
			);

			_float startRadius =
				XMVectorGetX(XMVector3Length(vStartToCam));

			_float startHeight =
				XMVectorGetY(vStartToCam);

			// End �Ķ���� ����
			_vector vEndToCam = EndMat.r[3] - vPivot;

			_float endAngle = atan2f(
				XMVectorGetZ(vEndToCam),
				XMVectorGetX(vEndToCam)
			);

			_float endRadius =
				XMVectorGetX(XMVector3Length(vEndToCam));

			_float endHeight =
				XMVectorGetY(vEndToCam);

			// ���� ���� (�ִ� ȸ��)
			_float deltaAngle = endAngle - startAngle;
			if (deltaAngle > XM_PI)  deltaAngle -= XM_2PI;
			if (deltaAngle < -XM_PI) deltaAngle += XM_2PI;

			_float angle = startAngle + deltaAngle * t;
			_float radius = Lerp(startRadius, endRadius, t);
			_float height = Lerp(startHeight, endHeight, t);

			// ���� ��ġ (��ȣ)
			_vector vFinalPos =
				vPivot +
				XMVectorSet(
					cosf(angle) * radius,
					height,
					sinf(angle) * radius,
					0.f
				);

			m_pTransformCom->Set_State(STATE::POSITION, vFinalPos);

			// LookAt (�����: Pivot �߽�)
			m_pTransformCom->LookAt(XMLoadFloat4(&m_fTargetLookPos));

			__super::Bind_Matrices(fTimeDelta);
		}
		else
		{
			m_isLerpEnd = true;
		}
	}

	if (m_isLerpEnd && m_isReverse)
	{
		m_fAccTime = 0.f;
		m_isReverse = false;

		m_pGameInstance->SetMainCamera(TEXT("PlayerCamera"));
		auto pMainCamera = m_pGameInstance->GetMainCamera();
		pMainCamera->GetTransform()->LookAt(XMLoadFloat4(&m_fTargetLookPos));

		m_pTargetNpc = nullptr;

		Safe_Release(pMainCamera);

		XMStoreFloat4x4(&m_fStartLerpMatrix, XMMatrixIdentity());
		XMStoreFloat4x4(&m_fEndLerpMatrix, XMMatrixIdentity());
		m_fTargetRadius = 0.f;
		m_fTargetLookPos = _float4(0.f, 0.f, 0.f, 0.f);
		Safe_Release(pPlayerCam);
	}
}

void CCamera_Npc::Update(_float fTimeDelta)
{
	//_matrix WorldMat = XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr());
	//m_pColliderCom->UpdateColiision(WorldMat);

	//ī�޶� ����ŷ�� ���ؼ� �θ� Update ȣ��. 
	//__super::Update(fTimeDelta);
}

void CCamera_Npc::Late_Update(_float fTimeDelta)
{
	if (false == m_pGameInstance->IsMainCamera(this))
		return;
	
//	m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
//
//#ifdef _DEBUG
//	m_pGameInstance->Add_DebugComponent(m_pColliderCom);
//	m_pGameInstance->ADD_Collider(m_pColliderCom);
//#endif
}

HRESULT CCamera_Npc::Render()
{
	return S_OK;
}

void CCamera_Npc::SetCameraAnimation(const _float4x4* StartLerpMatrix, const _float4x4* EndLerpMatrix, const _float4 fTargetLookPos, const _float fTargetRadius, _bool bIsReturn)
{
	m_isLerpEnd = false;
	m_fTargetRadius = fTargetRadius;
	m_fTargetLookPos = fTargetLookPos;
	memcpy(&m_fStartLerpMatrix, StartLerpMatrix, sizeof(_float4x4));
	memcpy(&m_fEndLerpMatrix, EndLerpMatrix, sizeof(_float4x4));
}

void CCamera_Npc::ReverseCameraAnimation()
{
	m_isReverse = true;
	m_isLerpEnd = false;
	m_fAccTime = 0.f;
}

void CCamera_Npc::CameraLock(_bool bIsKeyBoard, _bool bIsMouse)
{
	m_bIsLock[0] = bIsKeyBoard;
	m_bIsLock[1] = bIsMouse;
}

void CCamera_Npc::GetCameraLock(_bool (&pOut)[2])
{
	pOut[0] = m_bIsLock[0];
	pOut[1] = m_bIsLock[1];
}

void CCamera_Npc::SetCameraSpeed(_float fCameraSpeed)
{
	m_fCameraSpeed = fCameraSpeed;
}

void CCamera_Npc::SetTargetNpc(CNpc* pNpc)
{
	m_pTargetNpc = pNpc;
}

CCamera_Npc* CCamera_Npc::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCamera_Npc* pInstance = new CCamera_Npc(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCamera_Npc::Clone(void* pArg)
{
	CCamera_Npc* pInstance = new CCamera_Npc(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCamera_Npc");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCamera_Npc::Free()
{
	__super::Free();
}
