#include "pch.h"
#include "UI_Camera.h"

#include "GameInstance.h"
//#include "Interaction_Component.h"

CUI_Camera::CUI_Camera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCamera { pDevice, pContext }
{

}

CUI_Camera::CUI_Camera(const CUI_Camera& Prototype) 
	: CCamera { Prototype }
{

}

HRESULT CUI_Camera::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_Camera::Initialize(void* pArg)
{	
	UI_CAMERA_DESC* pDesc = static_cast<UI_CAMERA_DESC*>(pArg);
	m_fMouseSensor = pDesc->fMouseSensor;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if(FAILED(Ready_PlayerDesc()))
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

void CUI_Camera::Priority_Update(_float fTimeDelta)
{
	m_fMouseSensor = 0.1f;
	if(m_pGameInstance->IsMainCamera(this))
	{
		if (false == m_bIsCameraAnimation)
		{
			if (false == m_bIsLock[0])
			{
				if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_W))
					m_pTransformCom->Go_Straight(fTimeDelta);

				if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_S))
					m_pTransformCom->Go_Backward(fTimeDelta);

				if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_A))
					m_pTransformCom->Go_Left(fTimeDelta);

				if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_D))
					m_pTransformCom->Go_Right(fTimeDelta);

				//if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_F))
				//{
				//	auto pInterraction = m_pGameInstance->GetNearInteraction();
				//	if (pInterraction)
				//		pInterraction->Action_InteractionEvent(this);
				//}
			}

			/*if (false == m_bIsLock[1])
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
			}*/
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
		__super::Bind_Matrices();
	}
}

void CUI_Camera::Update(_float fTimeDelta)
{
	if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_U))
	{
		if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_9) && m_PlayerDesc.iCurrentHealth > 0)
			m_PlayerDesc.iCurrentHealth -= 10;
		if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_0) && m_PlayerDesc.iCurrentHealth < m_PlayerDesc.iMaxHealth)
			m_PlayerDesc.iCurrentHealth += 10;
	}

	if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_O))
	{
		if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_9) && m_PlayerDesc.iCurrentBetaEnergy > 0.f)
			m_PlayerDesc.iCurrentBetaEnergy -= 1;
		if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_0) && m_PlayerDesc.iCurrentBetaEnergy < m_PlayerDesc.iMaxBetaEnergy)
			m_PlayerDesc.iCurrentBetaEnergy += 1;
	}

	if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_P))
	{
		if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_9) && m_PlayerDesc.iCurrentPotions > 0)
			m_PlayerDesc.iCurrentPotions -= 1;
		if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_0) && m_PlayerDesc.iCurrentPotions < m_PlayerDesc.iMaxPotions)
			m_PlayerDesc.iCurrentPotions += 1;
	}

	if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_I))
	{
		if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_9) && m_PlayerDesc.iCurrentShield > 0)
			m_PlayerDesc.iCurrentShield -= 10;
		if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_0) && m_PlayerDesc.iCurrentShield < m_PlayerDesc.iMaxShield)
			m_PlayerDesc.iCurrentShield += 10;
	}

	if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_H))
	{
		if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_7))
			m_PlayerDesc.eBetaSkillState[0] = Client::SKILL_STATE::DEFAULT;
		if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_8))
			m_PlayerDesc.eBetaSkillState[1] = Client::SKILL_STATE::DEFAULT;
		if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_9))
			m_PlayerDesc.eBetaSkillState[2] = Client::SKILL_STATE::DEFAULT;
		if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_0))
			m_PlayerDesc.eBetaSkillState[3] = Client::SKILL_STATE::DEFAULT;
		if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_MINUS))
		{
			for (size_t i = 0; i < 4; ++i)
				m_PlayerDesc.eBetaSkillState[i] = Client::SKILL_STATE::DEFAULT;
		}
	}

	if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_J))
	{
		if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_7))
			m_PlayerDesc.eBetaSkillState[0] = Client::SKILL_STATE::ACTIVE_ON;
		if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_8))
			m_PlayerDesc.eBetaSkillState[1] = Client::SKILL_STATE::ACTIVE_ON;
		if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_9))
			m_PlayerDesc.eBetaSkillState[2] = Client::SKILL_STATE::ACTIVE_ON;
		if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_0))
			m_PlayerDesc.eBetaSkillState[3] = Client::SKILL_STATE::ACTIVE_ON;
		if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_MINUS))
		{
			for (size_t i = 0; i < 4; ++i)
				m_PlayerDesc.eBetaSkillState[i] = Client::SKILL_STATE::ACTIVE_ON;
		}
	}

	if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_K))
	{
		if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_7))
			m_PlayerDesc.eBetaSkillState[0] = Client::SKILL_STATE::ACTIVE;
		if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_8))
			m_PlayerDesc.eBetaSkillState[1] = Client::SKILL_STATE::ACTIVE;
		if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_9))
			m_PlayerDesc.eBetaSkillState[2] = Client::SKILL_STATE::ACTIVE;
		if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_0))
			m_PlayerDesc.eBetaSkillState[3] = Client::SKILL_STATE::ACTIVE;
		if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_MINUS))
		{
			for (size_t i = 0; i < 4; ++i)
				m_PlayerDesc.eBetaSkillState[i] = Client::SKILL_STATE::ACTIVE;
		}
	}

	if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_L))
	{
		if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_7))
			m_PlayerDesc.eBetaSkillState[0] = Client::SKILL_STATE::USE;
		if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_8))
			m_PlayerDesc.eBetaSkillState[1] = Client::SKILL_STATE::USE;
		if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_9))
			m_PlayerDesc.eBetaSkillState[2] = Client::SKILL_STATE::USE;
		if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_0))
			m_PlayerDesc.eBetaSkillState[3] = Client::SKILL_STATE::USE;
		if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_MINUS))
		{
			for (size_t i = 0; i < 4; ++i)
				m_PlayerDesc.eBetaSkillState[i] = Client::SKILL_STATE::USE;
		}
	}

	if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_R))
	{
		if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_7))
		{
			m_PlayerDesc.eRushState = Client::SKILL_STATE::DEFAULT;
			m_PlayerDesc.fCurrentRushCoolTime = 0.f;
		}
		if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_8))
			m_PlayerDesc.eRushState = Client::SKILL_STATE::ACTIVE_ON;
		if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_9))
			m_PlayerDesc.eRushState = Client::SKILL_STATE::ACTIVE;
		if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_0))
		{
			m_PlayerDesc.eRushState = Client::SKILL_STATE::USE;
			m_PlayerDesc.fCurrentRushCoolTime = 0.f;
		}
	}

	/*if (m_PlayerDesc.fCurrentRushCoolTime < m_PlayerDesc.fMaxRushCoolTime)
	{
		m_PlayerDesc.eRushState = Client::SKILL_STATE::DEFAULT;
		m_PlayerDesc.fCurrentRushCoolTime += fTimeDelta;
	}
	else if((m_PlayerDesc.fCurrentRushCoolTime >= m_PlayerDesc.fMaxRushCoolTime) && 
		m_PlayerDesc.eRushState == Client::SKILL_STATE::DEFAULT)
	{
		m_PlayerDesc.eRushState = Client::SKILL_STATE::ACTIVE_ON;
		m_PlayerDesc.fCurrentRushCoolTime = m_PlayerDesc.fMaxRushCoolTime;
	}*/

	//_matrix WorldMat = XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr());
	//m_pColliderCom->UpdateColiision(WorldMat);
}

void CUI_Camera::Late_Update(_float fTimeDelta)
{
//	m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
//
//#ifdef _DEBUG
//	m_pGameInstance->Add_DebugComponent(m_pColliderCom);
//	m_pGameInstance->ADD_Collider(m_pColliderCom);
//#endif
}

HRESULT CUI_Camera::Render()
{
	
	return S_OK;
}

void CUI_Camera::SetCameraAnimation(const _float4x4* StartLerpMatrix, const _float4x4* EndLerpMatrix, _bool bIsLerp)
{
	m_bIsCameraAnimation = bIsLerp;
	if (m_bIsCameraAnimation)
	{
		memcpy(&m_fStartLerpMatrix, StartLerpMatrix, sizeof(_float4x4));
		memcpy(&m_fEndLerpMatrix, EndLerpMatrix, sizeof(_float4x4));
	}
}

void CUI_Camera::CameraLock(_bool bIsKeyBoard, _bool bIsMouse)
{
	m_bIsLock[0] = bIsKeyBoard;
	m_bIsLock[1] = bIsMouse;
}

void CUI_Camera::GetCameraLock(_bool (&pOut)[2])
{
	pOut[0] = m_bIsLock[0];
	pOut[1] = m_bIsLock[1];
}

HRESULT CUI_Camera::Ready_PlayerDesc()
{
	m_PlayerDesc.iMaxHealth = 100;
	m_PlayerDesc.iMaxShield = 100;
	m_PlayerDesc.iMaxBetaEnergy = 20;

	m_PlayerDesc.iCurrentHealth = 100;
	m_PlayerDesc.iCurrentShield = 100;
	m_PlayerDesc.iCurrentShieldATK = 100;

	m_PlayerDesc.fCurrentCTDamage = 100.f;
	m_PlayerDesc.fCurrentCTPercent = 100.f;

	m_PlayerDesc.fCurrentLinkApplyDamage = 100.f;
	m_PlayerDesc.iCurrentAttackPoint = 100;
	m_PlayerDesc.iCurrentBetaEnergy = 20;

	m_PlayerDesc.iCurrentPotions = 3;
	m_PlayerDesc.iMaxPotions = 3;

	m_PlayerDesc.eRushState = Client::SKILL_STATE::DEFAULT;
	m_PlayerDesc.fMaxRushCoolTime = 5.f;
	m_PlayerDesc.fCurrentRushCoolTime = 0.f;

	for (int i = 0; i < 4; ++i)
		m_PlayerDesc.eBetaSkillState[i] = Client::SKILL_STATE::DEFAULT;

	return S_OK;
}

CUI_Camera* CUI_Camera::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Camera* pInstance = new CUI_Camera(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_Camera::Clone(void* pArg)
{
	CUI_Camera* pInstance = new CUI_Camera(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUI_Camera");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Camera::Free()
{
	__super::Free();

	//Safe_Release(m_pColliderCom);
}
