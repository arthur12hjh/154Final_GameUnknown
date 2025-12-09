#include "pch.h"

#include "GameInstance.h"
#include "GameManager.h"

#include "Player.h"
#include "CameraBone_Player.h"
#include "Nayitba.h"

#include "Trail.h"
#include "TrailEffect.h"
#include "Effect.h"

CCameraBone_Player::CCameraBone_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPlayer_Parts{ pDevice, pContext }
{
}

CCameraBone_Player::CCameraBone_Player(const CCameraBone_Player& Prototype)
	: CPlayer_Parts{ Prototype }
{
}

HRESULT CCameraBone_Player::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCameraBone_Player::Initialize(void* pArg)
{
	CAMERABONE_DESC* pDesc = static_cast<CAMERABONE_DESC*>(pArg);

	m_pParentTransformCom = pDesc->pParentTransform;
	m_pSocketMatrix = pDesc->pSocketMatrix;
	m_pCharacter = pDesc->pCharacter;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pModelCom->Set_Animation("CameraBone_Idle", TRUE, 1.f, 1.f);

	return S_OK;
}

void CCameraBone_Player::Priority_Update(_float fTimeDelta)
{
	m_isAnimationChanged += fTimeDelta;

	if(m_isAnimationChanged > 0.f)
		m_pModelCom->Set_Animation("CameraBone_Idle", TRUE, 1.f, 1.f);
	else
	{
		if(m_iAnimationIndex%5 == 0)
			m_pModelCom->Set_Animation("P_Eve_Sword_Normal_MoveBackAttack_A", TRUE, 1.f, 0.f);
		else if(m_iAnimationIndex % 5 == 1)
			m_pModelCom->Set_Animation("P_Eve_Sword_Normal_MoveBackAttackRangeReady1", TRUE, 2.f, 0.f);
		else if(m_iAnimationIndex % 5 == 2)
			m_pModelCom->Set_Animation("P_Eve_Sword_Noraml_BackStab1", TRUE, 1.f, 0.f);
		else if(m_iAnimationIndex % 5 == 3)
			m_pModelCom->Set_Animation("M_GorillaB_S24_PhaseChange2", TRUE, 1.f, 0.f);
		else if(m_iAnimationIndex % 5 == 4)
			m_pModelCom->Set_Animation("CameraBone_FlashBehindAttack_E", TRUE, 1.f, 0.f);
	}

	if(m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_N))
		m_isAnimationChanged = -1.5f;

	if(m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_M))
		m_iAnimationIndex++;

}

void CCameraBone_Player::Update(_float fTimeDelta)
{
 	m_pModelCom->Play_Animation(fTimeDelta, m_pTransformCom, 0.f);
}

void CCameraBone_Player::Late_Update(_float fTimeDelta)
{
	//_matrix		SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);
	//
	//for (size_t i = 0; i < 3; i++)
	//	SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);
	//
	//XMStoreFloat4x4(&m_CombinedWorldMatrix,
	//	XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()) * SocketMatrix * XMLoadFloat4x4(m_pParentTransformCom->Get_WorldMatrixPtr()));
	
	XMStoreFloat4x4(&m_CombinedWorldMatrix,
		XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()) * XMLoadFloat4x4(m_pParentTransformCom->Get_WorldMatrixPtr()));

	m_pModelCom->AddCount_PartialBone("Camera_Bone");

	auto* pBone = m_pModelCom->Get_BoneMatrixPtr("Camera_Bone");


	if (pBone == nullptr)
	{
		OutputDebugStringW(L"[Error] Camera_Bone not found.\n");
	}
	else
	{
		WCHAR szBuffer[256] = {};

		swprintf(
			szBuffer, 256,
			L"CamBone pos = %.3f, %.3f, %.3f\n",
			pBone->_41, pBone->_42, pBone->_43
		);

		OutputDebugStringW(szBuffer);
	}

}

void CCameraBone_Player::Set_Animation(const _char* szAnimationTag, _bool isLoop, _float fAnimationPlayRate, _float fLerpDuration, _bool bIsRestart, _float fEndTrackPosition, _float fStartTrackPosition, _bool isResetTrackPosition)
{
	m_pModelCom->Set_Animation(szAnimationTag, isLoop, fAnimationPlayRate, fLerpDuration, bIsRestart, fEndTrackPosition, fStartTrackPosition, isResetTrackPosition);
}

HRESULT CCameraBone_Player::Ready_Components()
{
	/* Com_Model */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_CameraBone"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	return S_OK;
}

CCameraBone_Player* CCameraBone_Player::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCameraBone_Player* pInstance = new CCameraBone_Player(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCameraBone_Player");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCameraBone_Player::Clone(void* pArg)
{
	CCameraBone_Player* pInstance = new CCameraBone_Player(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCameraBone_Player");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCameraBone_Player::Free()
{

	__super::Free();

	Safe_Release(m_pColliderCom);
}