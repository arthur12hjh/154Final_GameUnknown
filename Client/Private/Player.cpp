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
#include "Notify.h"
#include "PlayerCCTHitReporter.h"
#include "PlayerBehaviorCallback.h"

#include "PlayerBattleFSM.h"
#include "PlayerIdleFSM.h"
#include "PlayerLockOnFSM.h"
#include "Player_HitState.h"

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

void CPlayer::Change_PlayerMode(PLAYER_MODE eMode, PLAYER_STATE eState)
{
	m_PlayerDesc.ePlayerMode = eMode;

	// FSM 탐색.
	CPlayerFSM* pNextFSM = m_FSMs.find(eMode)->second;
	 
	if(eState == PLAYER_STATE::STATE_END)
		pNextFSM->Change_FSM(m_pCurrentFSM->Get_CurrentState()->Get_State());
	// Change_PlayerMode로 변경해준다.
	else
		pNextFSM->Change_FSM(eState);
		//m_pCurrentFSM->Clear_FSM();

	m_pCurrentFSM = pNextFSM;
}

HRESULT CPlayer::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPlayer::Initialize(void* pArg)
{
	CGameManager::GetInstance()->Bind_GameCharacter(this);
	if(FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_PartObjects()))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_PlayerDesc()))
		return E_FAIL;

	if (FAILED(Ready_FSM()))
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
	__super::Update(fTimeDelta);

	m_pGameManager->Lockon(fTimeDelta);
	Update_FSM(fTimeDelta);

	m_pColliderCom->UpdateColiision(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));


	//m_fTime += fTimeDelta;
	//if (5 <= m_fTime) {
	//	CEffect::EFFECT_TRANSFORM_DESC desc;
	//	desc.fRotationPerSec = 1.f;
	//	desc.fSpeedPerSec = 1.f;
	//	desc.pRootMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	//	desc.vPos = XMVectorSet(0, 0, 0, 1);
	//	desc.fRot = _float3(0, 0, 0);
	//	desc.fSize = 1.f;
	//	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Effect_Slash"),
	//		ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_Effect"), &desc)))
	//		return;
	//
	//	desc.fRotationPerSec = 1.f;
	//	desc.fSpeedPerSec = 1.f;
	//	desc.pWorldMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	//	desc.pRootMatrix = m_pBody->Get_BoneMatrixPtr("Bip001-R-Hand");
	//	desc.vPos = XMVectorSet(0, 0, 0, 1);
	//	desc.fRot = _float3(0, 0, 0);
	//	desc.fSize = 30.f;
	//	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Effect_SheildBreak_Yellow"),
	//		ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_Effect"), &desc)))
	//		return;
	//
	//	desc.fRotationPerSec = 1.f;
	//	desc.fSpeedPerSec = 1.f;
	//	desc.pWorldMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	//	desc.pRootMatrix = m_pBody->Get_BoneMatrixPtr("Bip001-L-Hand");
	//	desc.vPos = XMVectorSet(0, 0, 0, 1);
	//	desc.fRot = _float3(0, 0, 0);
	//	desc.fSize = 15.f;
	//	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Effect_Spectrum_Test"),
	//		ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_Effect"), &desc)))
	//		return;
	//
	//	desc.fRotationPerSec = 1.f;
	//	desc.fSpeedPerSec = 1.f;
	//	desc.pWorldMatrix = nullptr;
	//	desc.pRootMatrix = nullptr;
	//	desc.vPos = XMVectorSet(5, 3, 0, 1);
	//	desc.fRot = _float3(0, 0, 0);
	//	desc.fSize = 2.f;
	//	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Effect_Hit_Spark"),
	//		ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_Effect"), &desc)))
	//		return;
	//
	//	m_fTime = 0.f;
	//}
	//m_pTrail->Update_Trail(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()), fTimeDelta, true);
}

void CPlayer::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta); 
	
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

HRESULT CPlayer::Damaged(void* pArg)
{
	DEFAULT_DAMAGE_DESC* pDamageDesc = static_cast<DEFAULT_DAMAGE_DESC*>(pArg);
	CHARACTER_SKILL_DESC* pSkillDesc = static_cast<CHARACTER_SKILL_DESC*>(pDamageDesc->pSkillData);

	m_PlayerDesc.iCurrentHealth -= pSkillDesc->iSkillDamage;
	_float3 vHitDir{}, vHitPoint{}, vImpactDir{};
	_float4 vAttackerPos{};
	_float fImpactForce;

	vHitDir = pDamageDesc->vHitDir;
	vHitPoint = pDamageDesc->vHitPoint;
	vImpactDir = pDamageDesc->vImpactDir;
	fImpactForce = pDamageDesc->fImpactForce;

	XMStoreFloat4(&vAttackerPos, pDamageDesc->pAttacker->GetTransform()->Get_State(STATE::POSITION));

	m_PlayerDesc.iCurrentHealth -= pSkillDesc->iSkillDamage;

	m_pCurrentFSM->Change_State(CPlayer_HitState::Create(nullptr));

	if (SKILL_TYPE::INTERACTION_SKILL == pSkillDesc->eSkillType)
	{
		CCharacter* pCharacter = static_cast<CCharacter*>(pDamageDesc->pAttacker);

		// 임시입니다 잡기 테스트용 나중에 넘겨받거나 넘겨줄데이터 생기면 말좀해주세요
		// ㄴ 여기서 아마 상태 추가할거같긴 한데 몬스터 본이랑 몬스터 애니메이션 정보 연동해야 될 듯?
		pCharacter->ActionSuccess(nullptr);
	}

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
	m_pColliderCom->ADD_IgnoreObject(HIT_TYPE::SENCE);
	m_pColliderCom->SetColliderHitType(HIT_TYPE::PLAYER);

	/* Com_CCT */
	CCharacterController::CCT_DESC Desc;
	PxUserData tUserData;
	tUserData.szActorTag = TEXT("Player_CCT");

	Desc.eCharacterControllerType = CCharacterController::CCT_SHAPE::CAPSULE;
	Desc.tUserData = tUserData;
	Desc.vSize = _float3(2.5f, 2.5f, 0.f);
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

	CWeapon::WEAPON_DESC	WeaponDesc{};
	WeaponDesc.pSocketMatrix = m_pBody->Get_BoneMatrixPtr("Weapon");
	WeaponDesc.pParentTransform = m_pTransformCom;
	
	/* Part_Weapon */
	if (FAILED(__super::Add_PartObject(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Weapon"),
		TEXT("Part_Weapon"), &WeaponDesc)))
		return E_FAIL;

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

	Import_ModelPtr();
	m_pNotifyCom->Set_ModelCom(m_pBodyModelCom);

	return S_OK;
}

HRESULT CPlayer::Ready_PlayerDesc()
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

	m_PlayerDesc.eRushState = SKILL_STATE::DEFAULT;
	m_PlayerDesc.fMaxRushCoolTime = 5.f;
	m_PlayerDesc.fCurrentRushCoolTime = 0.f;

	for (int i = 0; i < 4; ++i)
		m_PlayerDesc.eBetaSkillState[i] = SKILL_STATE::DEFAULT;

	m_PlayerDesc.pPlayerTransform = m_pTransformCom;
	m_PlayerDesc.pPlayerController = m_pCCT;
	m_PlayerDesc.ePlayerMode = PLAYER_MODE::IDLE;

	return S_OK;
}

HRESULT CPlayer::Ready_FSM()
{
	m_FSMs.emplace(PLAYER_MODE::BATTLE, CPlayerBattleFSM::Create());
	m_FSMs.emplace(PLAYER_MODE::LOCKON, CPlayerLockonFSM::Create());
	m_FSMs.emplace(PLAYER_MODE::IDLE, CPlayerIdleFSM::Create());

	m_pCurrentFSM = m_FSMs.find(PLAYER_MODE::IDLE)->second;

	return S_OK;
}

void CPlayer::Update_FSM(_float fTimeDelta)
{
	m_pCurrentFSM->Update(fTimeDelta);
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

	for (auto& iter : m_FSMs)
		Safe_Release(iter.second);

	m_FSMs.clear();

	Safe_Release(m_pCurrentFSM);
}


//m_fTime += fTimeDelta;
//if (5 <= m_fTime) {
//	CEffect::EFFECT_TRANSFORM_DESC desc;
//	desc.fRotationPerSec = 1.f;
//	desc.fSpeedPerSec = 1.f;
//	desc.pRootMatrix = m_pTransformCom->Get_WorldMatrixPtr();
//	desc.vPos = XMVectorSet(0, 0, 0, 1);
//	desc.fRot = _float3(0, 0, 0);
//	desc.fSize = 1.f;
//	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Effect_Slash"),
//		ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_Effect"), &desc)))
//		return;
//
//	desc.fRotationPerSec = 1.f;
//	desc.fSpeedPerSec = 1.f;
//	desc.pWorldMatrix = m_pTransformCom->Get_WorldMatrixPtr();
//	desc.pRootMatrix = m_pBody->Get_BoneMatrixPtr("Bip001-R-Hand");
//	desc.vPos = XMVectorSet(0, 0, 0, 1);
//	desc.fRot = _float3(0, 0, 0);
//	desc.fSize = 30.f;
//	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Effect_SheildBreak_Yellow"),
//		ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_Effect"), &desc)))
//		return;
//
//	desc.fRotationPerSec = 1.f;
//	desc.fSpeedPerSec = 1.f;
//	desc.pWorldMatrix = m_pTransformCom->Get_WorldMatrixPtr();
//	desc.pRootMatrix = m_pBody->Get_BoneMatrixPtr("Bip001-L-Hand");
//	desc.vPos = XMVectorSet(0, 0, 0, 1);
//	desc.fRot = _float3(0, 0, 0);
//	desc.fSize = 15.f;
//	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Effect_Spectrum_Test"),
//		ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_Effect"), &desc)))
//		return;
//
//	desc.fRotationPerSec = 1.f;
//	desc.fSpeedPerSec = 1.f;
//	desc.pWorldMatrix = nullptr;
//	desc.pRootMatrix = nullptr;
//	desc.vPos = XMVectorSet(5, 3, 0, 1);
//	desc.fRot = _float3(0, 0, 0);
//	desc.fSize = 2.f;
//	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Effect_Hit_Spark"),
//		ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_Effect"), &desc)))
//		return;
//
//	m_fTime = 0.f;
//}
//m_pTrail->Update_Trail(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()), fTimeDelta, true);