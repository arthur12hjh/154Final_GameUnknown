#include "pch.h"

#include "CinematicModel_Eve.h"
#include "Body_Player.h"
#include "Face_Player.h"
#include "Hair_Player.h"
#include "PonyTail_Player.h"
#include "Weapon.h"

#include "GameInstance.h"
#include "GameManager.h"

#include "Effect.h"
#include "Notify.h"

#include "Player.h"
#include "PlayerCCTHitReporter.h"
#include "PlayerBehaviorCallback.h"
#include "PlayerCCTQueryFilterCallback.h"


CCinematicModel_Eve::CCinematicModel_Eve(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCinematicObject{ pDevice, pContext }
{
}

CCinematicModel_Eve::CCinematicModel_Eve(const CCinematicModel_Eve& Prototype)
	: CCinematicObject{ Prototype }
{
}

void CCinematicModel_Eve::Active_SFX(const _wstring& strPartTag, const _wstring& strObjectTag, const ANIM_NOTIFY& NotifyReference)
{
	// 어떤 방식으로 짜야하지?
	// 1) strPartTag가 empty일 경우, CinematicModel_Eve에서 탐색
	//    strPartTag가 있을 경우, strPartTag로 모델을 탐색
	// 2) strObjectTag라는 매핑된 값을 문자열 탐색해서 찾고, Play() 함수를 실행함
	// 3) 그 모델들은 시간이 지난 후 알아서 Stop()

	if (strPartTag.empty())
	{

	}
	else
	{
		Find_PartObject(strPartTag)->Active_SFX(strObjectTag, NotifyReference);
	}

}

void CCinematicModel_Eve::Activate_PartObject_Collider(const _wstring& strPartTag, const _wstring& strColliderTag, const ANIM_NOTIFY& NotifyRef)
{
	auto pPartObject = Find_PartObject(strPartTag);
	if (nullptr == pPartObject)
		return;

	pPartObject->Activate_PartObject_Collider(strColliderTag, NotifyRef);
}

HRESULT CCinematicModel_Eve::CallNotify(_uint iNotiType, const AnimNotify* pNotify)
{
	CNotify::NOTIFY_TYPE NotiType = CNotify::NOTIFY_TYPE(iNotiType);

	return S_OK;
}

HRESULT CCinematicModel_Eve::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCinematicModel_Eve::Initialize(void* pArg)
{
	CCinematicObject::CINEMATICOBJECT_DESC* pDesc = (CCinematicObject::CINEMATICOBJECT_DESC*)pArg;

	m_szObjectTag = pDesc->szObjectTag;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_PlayerDesc()))
		return E_FAIL;

	if (FAILED(Ready_PartObjects()))
		return E_FAIL;

	//m_pColliderCom->SetOwner(this);
	m_vRotationQuaternion = _float3(741.8724f, 3.0678f, 596.6966f);

	return S_OK;
}

void CCinematicModel_Eve::Priority_Update(_float fTimeDelta)
{
	if (m_bIsActive == FALSE)
		return;

	m_pTransformCom->Update_PreWorldMatrix();

	__super::Priority_Update(fTimeDelta);
}

void CCinematicModel_Eve::Update(_float fTimeDelta)
{
	/*if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_P))
		m_vRotationQuaternion.x += fTimeDelta;
	if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_O))
		m_vRotationQuaternion.x -= fTimeDelta;

	if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_L))
		m_vRotationQuaternion.y += fTimeDelta;
	if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_K))
		m_vRotationQuaternion.y -= fTimeDelta;

	if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_M))
		m_vRotationQuaternion.z += fTimeDelta;
	if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_N))
		m_vRotationQuaternion.z -= fTimeDelta;

	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSetW(XMLoadFloat3(&m_vRotationQuaternion), 1.f));*/

	if (m_bIsActive == FALSE)
		return;

	__super::Update(fTimeDelta);

	switch (m_iCinematicCode)
	{
	case 0: // 고릴라 만남 시네마틱
		Play_Cinematic_GorillaMeet(fTimeDelta);
		break;
	case 1: // 고릴라 처형 시네마틱
		Play_Cinematic_GorillaFinish(fTimeDelta);
		break;
	}

	//m_pColliderCom->UpdateColiision(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));
}

void CCinematicModel_Eve::Late_Update(_float fTimeDelta)
{
	if (m_bIsActive == FALSE)
		return;

	m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
	m_pGameInstance->Add_RenderGroup(RENDER::SHADOW, this);
	//m_pGameInstance->ADD_Collider(m_pColliderCom);

#ifdef _DEBUG
	//m_pGameInstance->Add_DebugComponent(m_pColliderCom);
#endif
	__super::Late_Update(fTimeDelta);
}

HRESULT CCinematicModel_Eve::Render()
{
	for (auto& pPartObject : m_PartObjects)
		pPartObject.second->Render();

	return S_OK;
}

HRESULT CCinematicModel_Eve::Render_Shadow()
{
	for (auto& pPartObject : m_PartObjects)
		pPartObject.second->Render_Shadow();

	return S_OK;
}

HRESULT CCinematicModel_Eve::ActiveCinematicObject(const CINEMATIC_NODE_DESC& CinematicNodeDesc)
{
	//m_bIsActive = TRUE;

	return S_OK;
}

HRESULT CCinematicModel_Eve::PlayCinematicObject(const CINEMATIC_NODE_DESC& CinematicNodeDesc)
{
	m_bIsActive = TRUE;

	if (m_iCinematicCode != CinematicNodeDesc.iActiveIndex)
		m_iCinematicCode = CinematicNodeDesc.iActiveIndex;

	switch (m_iCinematicCode)
	{
		case 0: // 고릴라 만남 시네마틱
			Initialize_Cinematic_GorillaMeet();
			break;
		case 1: // 고릴라 처형 시네마틱
			Initialize_Cinematic_GorillaFinish();
			break;
	}

	return S_OK;
}

HRESULT CCinematicModel_Eve::Ready_Components()
{
	///* Com_Collider_AABB */
	//CBoxCollider::BOX_COLLIDER_DESC		AABBDesc{};
	//AABBDesc.vSize = _float3(1.5f, 2.f, 1.5f);
	//AABBDesc.vCenter = _float3(0.f, AABBDesc.vSize.y * 0.5f, 0.f);
	//
	//if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Collider_AABB"),
	//	TEXT("Com_Collider_AABB"), reinterpret_cast<CComponent**>(&m_pColliderCom), &AABBDesc)))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CCinematicModel_Eve::Ready_PlayerDesc()
{
	m_PlayerDesc.iMaxHealth = 100;
	m_PlayerDesc.iMaxShield = 100;

	//베타 에너지
	m_PlayerDesc.iMaxBetaEnergy = 20;
	m_PlayerDesc.iCurrentBetaEnergy = 0;

	m_PlayerDesc.iCurrentHealth = 100;
	m_PlayerDesc.iCurrentShield = 100;
	m_PlayerDesc.iCurrentShieldATK = 100;

	m_PlayerDesc.fCurrentCTDamage = 100.f;
	m_PlayerDesc.fCurrentCTPercent = 100.f;

	m_PlayerDesc.fCurrentLinkApplyDamage = 100.f;
	m_PlayerDesc.iCurrentAttackPoint = 100;

	m_PlayerDesc.iCurrentPotions = 5;
	m_PlayerDesc.iMaxPotions = 5;

	m_PlayerDesc.eRushState = SKILL_STATE::DEFAULT;
	m_PlayerDesc.fMaxRushCoolTime = 5.f;
	m_PlayerDesc.fCurrentRushCoolTime = 0.f;

	//Default가 비활성화 상태
	memset(m_PlayerDesc.iBetaSkillId, 0, sizeof(_uint) * 4);

	m_PlayerDesc.eBetaSkillState[0] = SKILL_STATE::DEFAULT;
	m_PlayerDesc.eBetaSkillState[1] = SKILL_STATE::DEFAULT;
	m_PlayerDesc.eBetaSkillState[2] = SKILL_STATE::DEFAULT;
	m_PlayerDesc.eBetaSkillState[3] = SKILL_STATE::DEFAULT;

	m_PlayerDesc.pPlayerController = nullptr;
	m_PlayerDesc.pPlayerTransform = m_pTransformCom;
	m_PlayerDesc.ePlayerMode = PLAYER_MODE::IDLE;

	m_PlayerDesc.iOwnGold = 0;

	return S_OK;
}

HRESULT CCinematicModel_Eve::Ready_PartObjects()
{
	CBody_Player::BODY_PLAYER_DESC BodyDesc{};
	BodyDesc.pParentTransform = m_pTransformCom;

	/* Part_Body */
	if (FAILED(__super::Add_PartObject(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Body_Player"),
		TEXT("Part_Body"), &BodyDesc)))
		return E_FAIL;
	// 바디가 생성 되자마자 세팅.
	// 이래야 다른 파트오브젝트에서 바디를 참조 가능하지 ㅇㅇ
	Import_ModelPtr();
	m_pNotifyCom->Set_ModelCom(m_pBodyModelCom);

	CBody_Player* pBody = dynamic_cast<CBody_Player*>(Find_PartObject(TEXT("Part_Body")));

	CWeapon::WEAPON_DESC	WeaponDesc{};
	WeaponDesc.pParent = this;
	WeaponDesc.pSocketMatrix = pBody->Get_BoneMatrixPtr("Weapon");
	WeaponDesc.pParentTransform = m_pTransformCom;

	/* Part_Weapon */
	if (FAILED(__super::Add_PartObject(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Weapon"),
		TEXT("Part_Weapon"), &WeaponDesc)))
		return E_FAIL;

	m_pWeapon = static_cast<CWeapon*>(Find_PartObject(TEXT("Part_Weapon")));
	m_pWeapon->Set_PlayerDesc(&m_PlayerDesc);

	CFace_Player::FACE_PLAYER_DESC FaceDesc{};
	FaceDesc.pParentTransform = m_pTransformCom;
	FaceDesc.pBodyPtr = pBody;

	/* Part_Face */
	if (FAILED(__super::Add_PartObject(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Face_Player"),
		TEXT("Part_Face"), &FaceDesc)))
		return E_FAIL;

	CHair_Player::HAIR_PLAYER_DESC HairDesc{};
	HairDesc.pParentTransform = m_pTransformCom;
	HairDesc.pBodyPtr = pBody;

	/* Part_Hair */
	if (FAILED(__super::Add_PartObject(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Hair_Player"),
		TEXT("Part_Hair"), &HairDesc)))
		return E_FAIL;

	CPonyTail_Player::PONYTAIL_PLAYER_DESC PonyTailDesc{};
	PonyTailDesc.pParentTransform = m_pTransformCom;
	PonyTailDesc.pBodyPtr = pBody;

	/* Part_PonyTail */
	if (FAILED(__super::Add_PartObject(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_PonyTail_Player"),
		TEXT("Part_PonyTail"), &PonyTailDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CCinematicModel_Eve::Initialize_Cinematic_GorillaMeet()
{
	m_bIsActive = TRUE;
	m_iAnimationSequence = 0;
	m_fMoveTime = 0.f;
	m_PlayerDesc.isWeaponVisible = FALSE;
	m_pBodyModelCom->Set_Animation("MV_Quest_Sub_033_Gorilla_EVE_01", FALSE, 1.f);
	m_pTransformCom->Set_State(Engine::STATE::POSITION, XMVectorSet(738.66f, 2.022f, 608.569f, 1.f));
	m_pTransformCom->LookAt(XMVectorSet(760.197f, 2.022f, 700.319f, 1.f));

	return S_OK;
}

HRESULT CCinematicModel_Eve::Initialize_Cinematic_GorillaFinish()
{
	m_bIsActive = TRUE;
	m_iAnimationSequence = 0;
	m_fMoveTime = 0.f;
	m_PlayerDesc.isWeaponVisible = TRUE;
	m_pBodyModelCom->Set_Animation("Eve_Gorilla_Finish01_v05_w01_export_fixB", FALSE, 1.5f);
	m_pTransformCom->Set_State(Engine::STATE::POSITION, XMVectorSet(742.f, 2.8678f, 597.f, 1.f));
	m_pTransformCom->LookAt(XMVectorSet(742.f, 2.8678f, 608.6966f, 1.f));

	return S_OK;
}

HRESULT CCinematicModel_Eve::Play_Cinematic_GorillaMeet(_float fTimeDelta)
{
	m_fMoveTime += fTimeDelta;
	_bool isFinished = Play_Animation(fTimeDelta, m_pTransformCom, 1.f);

	if (isFinished)
	{
		m_fMoveTime = 0.f;
		m_bIsActive = FALSE;
		m_iCinematicCode = -1;
	}

	return S_OK;
}

HRESULT CCinematicModel_Eve::Play_Cinematic_GorillaFinish(_float fTimeDelta)
{
	//static _bool isSkipped = FALSE;
	_bool isFinished = Play_Animation(fTimeDelta, m_pTransformCom, 1.f);
	

	_uint iCurrentKeyFrameIndex = m_pBodyModelCom->Get_AnimationKeyFrameIndex();

	if (iCurrentKeyFrameIndex >= 600
		&& iCurrentKeyFrameIndex <= 720)
	{
		m_fMoveTime += fTimeDelta;

		_float fRatio = Clamp(m_fMoveTime / 2.f, 0.f, 1.f);
		_vector vPosition = XMVectorLerp(XMVectorSet(742.f, 2.8678f, 597.f, 1.f), XMVectorSet(767.79528f, 2.8678f, 552.14572f, 1.f), fRatio);

		m_pTransformCom->Set_State(STATE::POSITION, vPosition);
	}
	else if (iCurrentKeyFrameIndex > 720)
		m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(767.79528f, 2.8678f, 552.14572f, 1.f));

	if (isFinished)
	{
		m_fMoveTime = 0.f;
		//isSkipped = FALSE;
		m_bIsActive = FALSE;
		m_iCinematicCode = -1;
	}

	return S_OK;
}

CCinematicModel_Eve* CCinematicModel_Eve::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCinematicModel_Eve* pInstance = new CCinematicModel_Eve(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCinematicModel_Eve");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCinematicModel_Eve::Clone(void* pArg)
{
	CCinematicModel_Eve* pInstance = new CCinematicModel_Eve(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCinematicModel_Eve");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCinematicModel_Eve::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
}
