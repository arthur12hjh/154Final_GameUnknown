#include "pch.h"
#include "Player.h"

#include "Body_Player.h"
#include "Face_Player.h"
#include "Hair_Player.h"
#include "PonyTail_Player.h"
#include "Weapon.h"

#include "GameInstance.h"
#include "GameManager.h"
#include "Interaction_Component.h"
#include "Effect.h"
#include "Trail.h"
#include "TrailEffect.h"

CPlayer::CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCharacter {pDevice, pContext}
{
}

CPlayer::CPlayer(const CPlayer& Prototype)
	: CCharacter{ Prototype }
{
}

HRESULT CPlayer::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPlayer::Initialize(void* pArg)
{
	CGameObject::GAMEOBJECT_DESC	Desc{};
	Desc.fRotationPerSec = XMConvertToRadians(180.0f);
	Desc.fSpeedPerSec = 10.f;

	if(FAILED(__super::Initialize(&Desc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_PartObjects()))
		return E_FAIL;

	m_pColliderCom->SetOwner(this);

	CGameManager::GetInstance()->Bind_GameCharacter(this);

	m_bIsEvade = FALSE;
	m_bIsAttacking = FALSE;
	m_fEvadeTime = 0.f;
	m_fAttackTime = 0.f;
	m_iAttackComboIndex = 0;

	return S_OK;
}

void CPlayer::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);

}

void CPlayer::Update(_float fTimeDelta)
{

	if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_X))
	{
		if (FALSE == m_bIsEvade)
		{
			m_pBodyModelCom->Set_Animation("P_Eve_Sword_Evade_Evade1_T", false);
			m_bIsEvade = TRUE;
			m_fEvadeTime = 0.f;
		}
	}

	if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_Z))
	{
		if (!m_bIsEvade && !m_bIsAttacking)
		{
			m_bIsAttacking = true;
			m_iAttackComboIndex = 0;
			m_fAttackTime = 0.f;

			m_pBodyModelCom->Set_Animation(ComboAnims[m_iAttackComboIndex], false);
		}
	}

	if (m_bIsAttacking)
	{
		m_fAttackTime += fTimeDelta;

		if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_Z))
		{
			if (m_fAttackTime > 0.5f && m_fAttackTime < 1.f)
			{
				m_iAttackComboIndex++;
				m_fAttackTime = 0.f;

				if (m_iAttackComboIndex < 5)
					m_pBodyModelCom->Set_Animation(ComboAnims[m_iAttackComboIndex], false);
			}
		}
		else
		{
			if (m_fAttackTime >= ComboDelayTime[m_iAttackComboIndex])
			{
				m_bIsAttacking = FALSE;
				m_pBodyModelCom->Set_Animation("Proto_Battle_Idle");
			}

		}
	}
	else if (TRUE == m_bIsEvade)
	{
		m_fEvadeTime += fTimeDelta;
		if (m_fEvadeTime > 0.9f)
		{
			m_bIsEvade = FALSE;
			m_fEvadeTime = 0.f;
			m_pBodyModelCom->Set_Animation("Proto_Battle_Idle");

		}

		//_float fSpeed = pow(2 - (fEvadeTime - 0.35f), 2);
		_float fSpeed = 2 - (m_fEvadeTime - 0.45f);

		m_pTransformCom->Go_Straight(fTimeDelta * fSpeed);
	}
	else if (GetKeyState(VK_DOWN) & 0x8000)
	{
		m_pTransformCom->Go_Backward(fTimeDelta);
		if (nullptr != m_pBodyModelCom)
			m_pBodyModelCom->Set_Animation("Proto_Battle_Walk");
	}
	else if (GetKeyState(VK_LEFT) & 0x8000)
	{
		m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * -1.f);
	}
	else if (GetKeyState(VK_RIGHT) & 0x8000)
	{
		m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta);
	}
	else if (GetKeyState(VK_UP) & 0x8000)
	{
		m_pTransformCom->Go_Straight(fTimeDelta);

		m_pBodyModelCom->Set_Animation("Proto_Battle_Walk");
	}
	else
	{
		m_pBodyModelCom->Set_Animation("Proto_Battle_Idle");
	}
	

	if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_9))
	{
		CGameManager::GetInstance()->Accept_Quest(1);
	}

	if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_0))
	{
		CGameManager::GetInstance()->CompletedQuest(1);
	}

	if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_F))
	{
		auto pInterraction = m_pGameInstance->GetNearInteraction();
		if(pInterraction)
			pInterraction->Action_InteractionEvent(this);
	}

	m_pNavigationCom->Compute_Height(m_pTransformCom);
	m_fTime += fTimeDelta;
	if (1 < m_fTime) {
		CEffect::EFFECT_TRANSFORM_DESC desc;
		desc.fRotationPerSec = 1.f;
		desc.fSpeedPerSec = 1.f;
		desc.pRootMatrix = m_pTransformCom->Get_WorldMatrixPtr();
		desc.vPos = XMVectorSet(0, 0, 0, 1);
		desc.fRot = _float3(0, 0, 0);
		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Effect_Test"),
			ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Effect"), &desc)))
			return;
		m_fTime = 0;
	}
	m_pTrail->Update_Trail(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()), fTimeDelta, true);

Progress:
	m_pColliderCom->UpdateColiision(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));
	__super::Update(fTimeDelta);
}

void CPlayer::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta); 
	
	m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);

#ifdef _DEBUG
	m_pGameInstance->Add_DebugComponent(m_pNavigationCom);
	m_pGameInstance->Add_DebugComponent(m_pColliderCom);
	m_pGameInstance->ADD_Collider(m_pColliderCom);
#endif
}

HRESULT CPlayer::Render()
{



	return S_OK;
}

HRESULT CPlayer::Ready_Components()
{

	/* Com_Navigation */
	CNavigation::NAVIGATION_DESC		NavigationDesc{};
	NavigationDesc.iCurrentCellIndex = 0;
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Navigation"),
		TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom), &NavigationDesc)))
		return E_FAIL;

	/* Com_Collider_AABB */
	CBoxCollider::BOX_COLLIDER_DESC		AABBDesc{};

	AABBDesc.vSize = _float3(0.8f, 1.3f, 0.8f);
	AABBDesc.vCenter = _float3(0.f, AABBDesc.vSize.y * 0.5f, 0.f);

	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_Collider_AABB"), reinterpret_cast<CComponent**>(&m_pColliderCom), &AABBDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer::Ready_PartObjects()
{
	CBody_Player::BODY_PLAYER_DESC BodyDesc{};
	BodyDesc.pParentState = &m_iState;
	BodyDesc.pParentTransform = m_pTransformCom;

	/* Part_Body */
	if (FAILED(__super::Add_PartObject(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Body_Player"),
		TEXT("Part_Body"), &BodyDesc)))
		return E_FAIL;

	m_pPart_Body = dynamic_cast<CBody_Player*>(Find_PartObject(TEXT("Part_Body")));
	m_pBodyModelCom = dynamic_cast<CModel*>(m_pPart_Body->Find_Component(TEXT("Com_Model")));
	//CWeapon::WEAPON_DESC	WeaponDesc{};
	//WeaponDesc.pParentState = &m_iState;
	//WeaponDesc.pSocketMatrix = pPart_Body->Get_BoneMatrixPtr("SWORD");
	//WeaponDesc.pParentTransform = m_pTransformCom;
	//
	///* Part_Weapon */
	//if (FAILED(__super::Add_PartObject(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Weapon"),
	//	TEXT("Part_Weapon"), &WeaponDesc)))
	//	return E_FAIL;

	CFace_Player::FACE_PLAYER_DESC FaceDesc{};
	FaceDesc.pParentState = &m_iState;
	FaceDesc.pParentTransform = m_pTransformCom;
	FaceDesc.pBodyPtr = m_pPart_Body;

	/* Part_Face */
	if (FAILED(__super::Add_PartObject(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Face_Player"),
		TEXT("Part_Face"), &FaceDesc)))
		return E_FAIL;

	CHair_Player::HAIR_PLAYER_DESC HairDesc{};
	HairDesc.pParentState = &m_iState;
	HairDesc.pParentTransform = m_pTransformCom;
	HairDesc.pBodyPtr = m_pPart_Body;
	
	/* Part_Hair */
	if (FAILED(__super::Add_PartObject(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Hair_Player"),
		TEXT("Part_Hair"), &HairDesc)))
		return E_FAIL;
	
	CPonyTail_Player::PONYTAIL_PLAYER_DESC PonyTailDesc{};
	PonyTailDesc.pParentState = &m_iState;
	PonyTailDesc.pParentTransform = m_pTransformCom;
	PonyTailDesc.pBodyPtr = m_pPart_Body;
	
	/* Part_PonyTail */
	if (FAILED(__super::Add_PartObject(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_PonyTail_Player"),
		TEXT("Part_PonyTail"), &PonyTailDesc)))
		return E_FAIL;

	CTrail::TRAILHIGHLOW Traildesc;
	Traildesc.vHigh = _float4(1, 0, 0, 0);
	Traildesc.vLow = _float4(-1, 0, 0, 0);
	if (FAILED(__super::Add_PartObject(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_TrailEffect_Test"),
		TEXT("Part_Trail"), &Traildesc)))
		return E_FAIL;
	m_pTrail = dynamic_cast<CTrailEffect*>(Find_PartObject(TEXT("Part_Trail")));

	return S_OK;
}

CPlayer* CPlayer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPlayer* pInstance = new CPlayer(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CPlayer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPlayer::Clone(void* pArg)
{
	CPlayer* pInstance = new CPlayer(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CPlayer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pNavigationCom);
}
