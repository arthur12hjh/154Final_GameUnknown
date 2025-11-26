#include "pch.h"
#include "Player.h"

#include "Nayitba.h"

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
#include "PlayerFSM.h"
#include "PlayerCCTHitReporter.h"
#include "PlayerBehaviorCallback.h"

CPlayer::CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCharacter {pDevice, pContext}
{
}

CPlayer::CPlayer(const CPlayer& Prototype)
	: CCharacter{ Prototype }
{
}

_float CPlayer::Get_AnimationRatio()
{
	return m_pBodyModelCom->Get_AnimationRatio();
}

HRESULT CPlayer::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPlayer::Initialize(void* pArg)
{
	CGameManager::GetInstance()->Bind_GameCharacter(this);

	CGameObject::GAMEOBJECT_DESC	Desc{};
	Desc.fRotationPerSec = XMConvertToRadians(180.0f);
	Desc.fSpeedPerSec = 10.f;

	if(FAILED(__super::Initialize(&Desc)))
		return E_FAIL;

	if (FAILED(Ready_PartObjects()))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_PlayerDesc()))
		return E_FAIL;

	m_pColliderCom->SetOwner(this);

	return S_OK;
}

void CPlayer::Priority_Update(_float fTimeDelta)
{
	m_pTransformCom->Update_PreWorldMatrix();
	m_pCCT->Update_PrePxPosition(m_pTransformCom);

	__super::Priority_Update(fTimeDelta);
}

void CPlayer::Update(_float fTimeDelta)
{
	m_pPlayerFSM->Update(fTimeDelta);

	m_pColliderCom->UpdateColiision(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));

	__super::Update(fTimeDelta);
}

void CPlayer::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta); 
	
	//모든 트랜스폼의 이동이 끝난 후 실행되어야 함.
	m_pCCT->Update_PxPosition(fTimeDelta, m_pTransformCom);

	m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);

#ifdef _DEBUG
	m_pGameInstance->Add_DebugComponent(m_pColliderCom);
	m_pGameInstance->ADD_Collider(m_pColliderCom);
	m_pGameInstance->Add_PhysxGeometry(m_pCCT->Get_PxActor(), m_pCCT->Get_PxShape());
#endif
}

HRESULT CPlayer::Render()
{
	return S_OK;
}

HRESULT CPlayer::Ready_Components()
{
	/* Com_Collider_AABB */
	CBoxCollider::BOX_COLLIDER_DESC		AABBDesc{};
	AABBDesc.vSize = _float3(1.5f, 2.f, 1.5f);
	AABBDesc.vCenter = _float3(0.f, AABBDesc.vSize.y * 0.5f, 0.f);

	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_Collider_AABB"), reinterpret_cast<CComponent**>(&m_pColliderCom), &AABBDesc)))
		return E_FAIL;
	m_pColliderCom->SetColliderHitType(HIT_TYPE::PLAYER);

	m_pColliderCom->BindBeginOverlapEvent([](_float3 vHitPoint, _float3 vHitDir, CGameObject* pActor) {
		int a = 10;
		});

	/* Player FSM */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_PlayerFSM"),
		TEXT("Com_PlayerFSM"), reinterpret_cast<CComponent**>(&m_pPlayerFSM))))
		return E_FAIL;

	/* Com_CCT */
	CCharacterController::CCT_DESC Desc;
	PxUserData tUserData;
	tUserData.szActorTag = TEXT("Player_CCT");

	Desc.eCharacterControllerType = CCharacterController::CCT_SHAPE::CAPSULE;
	Desc.tUserData = tUserData;
	//캡슐 컨트롤러에서 x는 구 성분 y는 기둥 성분
	Desc.vSize = _float3(1.f, 1.f, 0.f);
	XMStoreFloat4(&Desc.vStartPos, m_pTransformCom->Get_State(STATE::POSITION));
	Desc.vMaterial = _float3(0.5f, 0.5f, 0.f);
	Desc.pHitReporter = CPlayerCCTHitReporter::Create();
	Desc.pBehaviorCallback = CPlayerBehaviorCallback::Create();

	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_CharacterController"),
		TEXT("Com_CCT"), reinterpret_cast<CComponent**>(&m_pCCT), &Desc)))
		return E_FAIL;

	m_pGameInstance->Add_CCT_ToPhysx(this, m_pCCT);

	return S_OK;
}

HRESULT CPlayer::Ready_PartObjects()
{
	CBody_Player::BODY_PLAYER_DESC BodyDesc{};
	BodyDesc.pParentTransform = m_pTransformCom;

	/* Part_Body */
	if (FAILED(__super::Add_PartObject(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Body_Player"),
		TEXT("Part_Body"), &BodyDesc)))
		return E_FAIL;

	m_pBody = dynamic_cast<CBody_Player*>(Find_PartObject(TEXT("Part_Body")));

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
	FaceDesc.pParentTransform = m_pTransformCom;
	FaceDesc.pBodyPtr = m_pBody;

	/* Part_Face */
	if (FAILED(__super::Add_PartObject(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Face_Player"),
		TEXT("Part_Face"), &FaceDesc)))
		return E_FAIL;

	CHair_Player::HAIR_PLAYER_DESC HairDesc{};
	HairDesc.pParentTransform = m_pTransformCom;
	HairDesc.pBodyPtr = m_pBody;
	
	/* Part_Hair */
	if (FAILED(__super::Add_PartObject(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Hair_Player"),
		TEXT("Part_Hair"), &HairDesc)))
		return E_FAIL;
	
	CPonyTail_Player::PONYTAIL_PLAYER_DESC PonyTailDesc{};
	PonyTailDesc.pParentTransform = m_pTransformCom;
	PonyTailDesc.pBodyPtr = m_pBody;
	
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

HRESULT CPlayer::Ready_PlayerDesc()
{
	m_PlayerDesc.iCurrentHealth = 100;
	m_PlayerDesc.iCurrentShield = 100;
	m_PlayerDesc.iCurrentShieldATK = 100;

	m_PlayerDesc.fCurrentCTDamage = 100.f;
	m_PlayerDesc.fCurrentCTPercent = 100.f;
	
	m_PlayerDesc.fCurrentLinkApplyDamage = 100.f;
	m_PlayerDesc.iCurrentAttackPoint = 100;
	m_PlayerDesc.iCurrentBetaEnergy = 100;
	m_PlayerDesc.pPlayerTransform = m_pTransformCom;
	m_PlayerDesc.pPlayerController = m_pCCT;

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

	Safe_Release(m_pCCT);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pPlayerFSM);
}
