#include "pch.h"
#include "Nayitba.h"

#include "GameInstance.h"

#include "StringHelper.h"
#include "NayitbaPartBody.h"

#include "GameManager.h"
#include "BossController.h"
#include "MonsterHitState.h"

#include "AttackHitBox.h"
#include "Bullet.h"

#pragma region PartObject
#include "NaytibaLeftWeaponPart.h"
#include "NaytibaRightWeaponPart.h"
#include "NaytibaFace.h"
#include "NaytibaBeam_Part.h"
#pragma endregion

#pragma region Component
#include "Notify.h"
#include "TargetComponent.h"
#include "DropComponent.h"
#pragma endregion

#include "Player.h"
#include "PlayerCCTHitReporter.h"
#include "PlayerBehaviorCallback.h"
#include "PlayerCCTQueryFilterCallback.h"

#include "UIHUD.h"
#include "UIBase.h"

CNaytiba::CNaytiba(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) :
	CCharacter(pDevice, pContext)
{
}

CNaytiba::CNaytiba(const CNaytiba& Prototype) :
	CCharacter(Prototype)
{
}

HRESULT CNaytiba::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CNaytiba::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	NAYITBA_DESC* pDesc = static_cast<NAYITBA_DESC*>(pArg);
	m_iMonsterID = pDesc->iMonsterID;
	m_bIsSuperMonster = pDesc->bIsSuperMonster;

	m_pBulletList.reserve(30);
	m_SkillCandidates.reserve(30);
	if (FAILED(Ready_CharacterData()))
		return E_FAIL;

	if (FAILED(ADD_PartObjects()))
		return E_FAIL;

	if (FAILED(ADD_Components()))
		return E_FAIL;

	if (NAYTIBA_TYPE::ELITE <= m_pInitMonsterInfo->eNaytiba_Type)
		SetActiveMonster(false);

	// 아래 세개중에서 하나
	// Bip001-Spine
	// Bip001_Spine1
	// Bip001_Spine2

	m_pLockOnMatrix = m_pBodyModelCom->Get_BoneMatrixPtr("Bip001-Spine");
	m_pLinkTargetBoneMatrix = m_pBodyModelCom->Get_BoneMatrixPtr("SC_LinkTarget");

	return S_OK;
}

void CNaytiba::Priority_Update(_float fTimeDelta)
{
	if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_0))
		SetActiveMonster(true);

	if (VISIBILITY::HIDDEN == m_eVisibility)
		return;

	m_pCCT->Update_PrePxPosition(m_pTransformCom);

	m_pAIController->Priority_Update(fTimeDelta);

	__super::Priority_Update(fTimeDelta);
}

void CNaytiba::Update(_float fTimeDelta)
{
	if (VISIBILITY::HIDDEN == m_eVisibility)
		return;

	if (NAYTIBA_STATE::BATTLE == m_MonsterInfo.eNaytibaState)
	{
		if (m_pAISenceCom->IsTagetEmpty())
		{
			BattleEvent(nullptr, NAYTIBA_STATE::DEFAULT);
		}

	/*	if (NAYTIBA_TYPE::ELITE > m_pInitMonsterInfo->eNaytiba_Type)
			VisibleStatusUI(fTimeDelta);*/
	} 

	m_MonsterPreState = m_MonsterInfo.eNaytibaState;
	// 이건 말해봐야할듯 락온이 플레이어 기준으로 반경을 체크하는데
	// 락온보고 일단 고정상수로 두고 하는데 어디서 받아오거나 했으면함
	_matrix WorldMatrix = XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr());
	_matrix SpineMatrix = XMLoadFloat4x4(m_pLockOnMatrix);

	if (m_pGameInstance->isIn_DistanceFrustum(m_pTransformCom->Get_State(STATE::POSITION), 200.f) || m_pTargetCom->GetTarget())
	{
		m_pAIController->Update(fTimeDelta);
		m_bIsPlaySound = true;
	}
	else
		m_bIsPlaySound = false;

	if (NAYTIBA_STATE::DEAD != m_MonsterInfo.eNaytibaState)
	{
		m_pAISenceCom->UpdatSenceComponent(fTimeDelta);
		m_pTargetCom->Target_Search(m_pAISenceCom->GetSearchAllObject());
		m_pColliderCom->UpdateColiision(WorldMatrix);
	}
	
	if (m_pGameInstance->isIn_WorldFrustum(m_pColliderCom))
	{
		if (m_pLockOnMatrix)
			XMStoreFloat3(&m_MonsterInfo.vLockOnPoint, (SpineMatrix * WorldMatrix).r[3]);

		XMStoreFloat3(&m_MonsterInfo.vStatusBarPoint, m_pTransformCom->Get_State(STATE::POSITION));
		m_MonsterInfo.vStatusBarPoint.y += static_cast<COBBCollider *>(m_pColliderCom)->GetBounding().Extents.y * 2.f;
	}

	__super::Update(fTimeDelta);
}

void CNaytiba::Late_Update(_float fTimeDelta)
{
	if (VISIBILITY::HIDDEN == m_eVisibility)
		return;

	//모든 트랜스폼의 이동이 끝난 후 실행되어야 함.
	if (NAYTIBA_STATE::DEAD != m_MonsterInfo.eNaytibaState)
		m_pGameInstance->ADD_Collider(m_pColliderCom);

	if (false == m_isDead)
	{
		m_pCCT->Update_PxPosition(fTimeDelta, m_pTransformCom);
		
	}

	if (m_pGameInstance->isIn_WorldFrustum(m_pColliderCom))
	{
		__super::Late_Update(fTimeDelta);

#ifdef _DEBUG
		m_pAISenceCom->Update_Debuge();
		m_pGameInstance->Add_DebugComponent(m_pColliderCom);
#endif // _DEBUG
	}
}

HRESULT CNaytiba::Render()
{
	//for (auto& pPartObject : m_PartObjects)
	//{
	//	if (TEXT("Part_Beam") == pPartObject.first)
	//		continue;

	//	pPartObject.second->Render();
	//}

	return S_OK;
}

HRESULT CNaytiba::Render_Shadow()
{
	for (auto& pPartObject : m_PartObjects)
		pPartObject.second->Render_Shadow();

	return S_OK;
}

HRESULT CNaytiba::Damaged(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	// 데미지를 입을떄 히트렉에 걸리게 하자
	// 0. 보스는 히트랙 조금 줄여
	// 1. 히트랙은 평타만
	// 2. 잡몹들은 확실하게 히트렉 주자.

	// 1,2타 때 1프레임
	// 3,4타 때 2프레임
	// -> 그냥 줘도 될듯?
	m_pGameInstance->GamePauseDurationTime(1, 0.f, 10000.f);

	DEFAULT_DAMAGE_DESC* pDesc = static_cast<DEFAULT_DAMAGE_DESC*>(pArg);
	if (nullptr == pDesc->pSkillData)
		return E_FAIL;


	const CHARACTER_SKILL_DESC* pSkillDesc = static_cast<const CHARACTER_SKILL_DESC*>(pDesc->pSkillData);
	pDesc->bIsHitMotion = ActionDamageLogic(pDesc);
	if (NAYTIBA_TYPE::ELITE <= m_pInitMonsterInfo->eNaytiba_Type) 
	{
		if (10 >= m_MonsterInfo.iCurrentHealth)
		{
			if (false == (SKILL_PROPERTY::EXCUTION & pSkillDesc->eProPerty))
				m_MonsterInfo.iCurrentHealth = 10.f;
		}
	}
	
	if (0 >= m_MonsterInfo.iCurrentHealth)
	{
		m_eExcution = NAYITBA_EXECUTION_TYPE::END;
		m_MonsterInfo.eNaytibaState = NAYTIBA_STATE::DEAD;
	}

	VisibleStatusUI(0.f);
	m_pAIController->Damage(pArg);

	_uint RimLightIndex = 0;
	if (NAYTIBA_TYPE::ELITE <= m_pInitMonsterInfo->eNaytiba_Type)
	{
		CBossController* pBossController = static_cast<CBossController*>(m_pAIController);
		if (SKILL_PROPERTY::EXCUTION & pSkillDesc->eProPerty)
		{
			RimLightIndex = 1;
		}
	}

	// 몬스터 림라이트 처리
	if (0 == RimLightIndex && NAYITBA_EXECUTION_TYPE::END == m_eExcution)
		m_pPartBody->SetRimLightData(true, 1.f, 0.9f, { 0.8f, 0.8f, 0.8f, 1.f }, 0.4f);
	return S_OK;
}

HRESULT CNaytiba::ActionSuccess(void* pArg)
{
	ResetToBaseState();
	m_pAIController->ActionSuccess(pArg);

	return S_OK;
}

HRESULT CNaytiba::CallNotify(_uint iNotiType, const AnimNotify* pNotify)
{
	CNotify::NOTIFY_TYPE NotiType = CNotify::NOTIFY_TYPE(iNotiType);

	switch (NotiType)
	{
	case CNotify::PLAY_SFX:
		Play_SFXEffect(pNotify);
		break;
	case CNotify::ACTIVE_COLLISION:
		CreateHitBox(pNotify);
		break;
	case CNotify::SPAWN_OBJECT:
		SpawnObject(pNotify);
		break;
	case CNotify::SHOOT_PROJECTILE:
		ShootProjectile(pNotify);
		break;
	case CNotify::ACTIVE_PHYSX_COLLISION:
		EnablePhysxController(pNotify->iNumData01);
		break;
	case CNotify::ATTACK_INTERACTION:
		Attack_Interaction(pNotify);
		break;
	case CNotify::CHANGE_COLOR:
		Change_Color(pNotify);
		break;
	}

	return S_OK;
}

void CNaytiba::Play_Sound(const ANIM_NOTIFY& NotifyReference)
{
	if (false == m_bIsPlaySound)
		return;

	if ("WALK" == NotifyReference.szNotifyArg01)
	{
		// 여기서 Walk Sound
		if (1 == m_iMonsterID)
			Play_GorillaMoveSound(0, NotifyReference.iNumData01);
		else if (3 == m_iMonsterID)
			Play_BanacleMoveSound(0);
		else if (7 == m_iMonsterID)
			Play_AntlionMoveSound(0);
		else if (8 == m_iMonsterID)
			Play_ScarletMoveSound(0);
		else
			Play_MoveSound(0);
	}
	if ("RUN" == NotifyReference.szNotifyArg01)
	{
		// 여기서 Walk Sound
		if (1 == m_iMonsterID)
			Play_GorillaMoveSound(1, NotifyReference.iNumData01);
		else if (3 == m_iMonsterID)
			Play_BanacleMoveSound(1);
		else if (7 == m_iMonsterID)
			Play_AntlionMoveSound(1);
		else if (8 == m_iMonsterID)
			Play_ScarletMoveSound(1);
		else
			Play_MoveSound(1);
	}
	if ("JUMP" == NotifyReference.szNotifyArg01)
	{
		_float fRandomIndex = m_pGameInstance->Random(0.f, 100.f);
		// 여기서 JUMP Sound
		if (8 == m_iMonsterID)
		{
			if (33 >= fRandomIndex)
				m_pGameInstance->Manager_PlaySound(TEXT("M_Scarlet_bodyfall_3.wav"), CHANNELID::EFFECT, 1.f, 1.f);
			else if (66 >= fRandomIndex)
				m_pGameInstance->Manager_PlaySound(TEXT("M_Scarlet_bodyfall_2.wav"), CHANNELID::EFFECT, 1.f, 1.f);
			else
				m_pGameInstance->Manager_PlaySound(TEXT("M_Scarlet_bodyfall_1.wav"), CHANNELID::EFFECT, 1.f, 1.f);
		}
	}
}

void CNaytiba::RecoveryPoint(RECOVERY_TYPE eRecoveryType, long long iCost)
{
	switch (eRecoveryType)
	{
	case RECOVERY_TYPE::RECOVERY_HP:
	{
		if (0 == iCost)
			m_MonsterInfo.iCurrentHealth = m_pInitMonsterInfo->iMaxHealth;
		else
		{
			m_MonsterInfo.iCurrentHealth += iCost;
			m_MonsterInfo.iCurrentHealth = Clamp<long long>((long long)m_MonsterInfo.iCurrentHealth, 0, (long long)m_pInitMonsterInfo->iMaxHealth);
		}
	}
	break;
	case RECOVERY_TYPE::RECOVERY_SHILED:
	{
		if (0 == iCost)
			m_MonsterInfo.iCurrentShield = m_pInitMonsterInfo->iMaxShield;
		else
		{
			m_MonsterInfo.iCurrentShield += iCost;
			m_MonsterInfo.iCurrentShield = Clamp<long long>((long long)m_MonsterInfo.iCurrentShield, 0, (long long)m_pInitMonsterInfo->iMaxShield);
		}
	}
	break;
	case RECOVERY_TYPE::RECOVERY_STEMINA:
	{
		if (0 == iCost)
			m_MonsterInfo.iCurrentStamina = m_pInitMonsterInfo->iMaxStamina;
		else
		{
			m_MonsterInfo.iCurrentStamina += iCost;
			m_MonsterInfo.iCurrentStamina = Clamp<long long>((long long)m_MonsterInfo.iCurrentStamina, 0, (long long)m_pInitMonsterInfo->iMaxStamina);
		}
	}
	break;
	default:
		return;
	}
}

void CNaytiba::PlayDeadEffect()
{
	if(NAYTIBA_TYPE::ELITE > m_pInitMonsterInfo->eNaytiba_Type)
		m_pDropCom->ItemDrop(1);

	m_pCCT->Set_Active(false);
	m_pPartBody->Play_DeadEffect();
}

void CNaytiba::Attack_Interaction(void* pArg)
{
	ATK_INTERACTION_DESC* pATK_Interaction_Desc = static_cast<ATK_INTERACTION_DESC*>(pArg);

	// 이제 플레이어 반응에 맞춰서 나도 반응 할수있다.

}

_uint CNaytiba::GetMonsterID()
{
	return m_iMonsterID;
}

void CNaytiba::Excution()
{
	m_MonsterInfo.iCurrentHealth = 0.f;
	m_pBodyModelCom->Set_Animation("M_Finish_Dead_cine", FALSE, 1.f);
	m_pTransformCom->Set_State(Engine::STATE::POSITION, XMVectorSet(738.66f, 2.022f, 608.569f, 1.f));
	m_pTransformCom->LookAt(XMVectorSet(738.66f, 2.022f, 607.569f, 1.f));

}

CGameObject* CNaytiba::GetTarget()
{
	return m_pTargetCom->GetTarget();
}

void CNaytiba::SetVelocity(_bool bIsFlag, _float fVelocity)
{
	m_pCCT->Set_Gravity(bIsFlag, fVelocity);
}

void CNaytiba::Setting_Data(_float fTimeDelta, const NAYITBA_DESC& Desc)
{
	m_pTransformCom->Set_State(STATE::POSITION, XMLoadFloat3(&Desc.vPosition));
	m_pTransformCom->Set_Rotation(XMLoadFloat4(&Desc.vRotation));
	m_pTransformCom->Set_Scale(XMLoadFloat3(&Desc.vScale));
	m_pCCT->Set_Position(XMVectorSetW(XMLoadFloat3(&Desc.vPosition), 1.f));

	m_iMonsterID = Desc.iMonsterID;
	if (FAILED(Ready_CharacterData()))
		return;

	if (Desc.pTarget)
	{
		m_pAISenceCom->Add_SenceTargetObject(Desc.pTarget);
		m_MonsterInfo.eNaytibaState = NAYTIBA_STATE::BATTLE;

		//if (pDesc->bIsSpanwer)
		//{
		//	// 여기서 스폰 상태로 변경
		//	// 일단 생성 되는지만 확인하고 가자
		//}
	}
}

const CHARACTER_SKILL_DESC* CNaytiba::FindSkillData(_uint iTypeIndex, _uint iSkillIndex)
{
	if (0 > iSkillIndex || (_uint)m_MonsterInfo.iAttackList[iTypeIndex].size() <= iSkillIndex)
		return nullptr;

	return  m_MonsterInfo.iAttackList[iTypeIndex][iSkillIndex];
}

const CHARACTER_SKILL_DESC* CNaytiba::GetSkillData(_bool bIsRandom, _uint iTypeIndex)
{
	const CHARACTER_SKILL_DESC* pSkill = { nullptr };
	if(-1 == iTypeIndex)
	{
		if (0 == m_iNumCandidate)
			return nullptr;

		if (bIsRandom)
		{
			_uint iRandomIndex = (_uint)m_pGameInstance->Random(0.f, (_float)m_iNumCandidate);
			pSkill = m_SkillCandidates[iRandomIndex];
		}
		else
		{
			if (m_iNumCandidate <= m_iSkillIndex)
				m_iSkillIndex = 0;

			pSkill = m_SkillCandidates[m_iSkillIndex];
			m_iSkillIndex++;
		}
	}
	else
	{
		size_t iNumSize = m_MonsterInfo.iAttackList[iTypeIndex].size();
		_uint iRandomIndex = (_uint)m_pGameInstance->Random(0, iNumSize);
		pSkill = m_MonsterInfo.iAttackList[iTypeIndex][iRandomIndex];
	}

	return pSkill;
}

void CNaytiba::SetActive(_bool bIsActive)
{
	m_bIsActive = bIsActive;
	SetActiveMonster(bIsActive);
}

void CNaytiba::SetActive()
{
	m_bIsActive = !m_bIsActive;
	SetActiveMonster(m_bIsActive);
}

void CNaytiba::SetActiveMonster(_bool bIsFlag)
{
	if (bIsFlag)
	{
		m_pCCT->Set_Active(true);
		m_eVisibility = VISIBILITY::VISIBLE;
	}
	else
	{
		m_pCCT->Set_Active(false);
		m_eVisibility = VISIBILITY::HIDDEN;
	}
}

void CNaytiba::SetActivePartObject(_bool bIsActive)
{
	__super::SetActive(bIsActive);
}

void CNaytiba::SetAttackData(const CHARACTER_SKILL_DESC* pATKDesc)
{
	m_pAttack_Data = pATKDesc;
	m_iComboCount = 0;
	m_iRepulseCount = 0;
}

void CNaytiba::SetThesholdAction(NAYITBA_EXECUTION_TYPE eExcution)
{
	m_eExcution = eExcution;

	if (NAYTIBA_TYPE::ELITE <= m_pInitMonsterInfo->eNaytiba_Type)
	{
		if (NAYITBA_EXECUTION_TYPE::LINK_ATTACK == m_eExcution)
			m_pPartBody->SetRimLightData(true, 2.5f, 0.5f, { 0.18f, 0.04f, 0.04f, 1.f }, 5.f);
	}
}

void CNaytiba::EnablePhysxController(_bool bEnable)
{
	m_pCCT->Set_CCTCollision(bEnable);
}

_bool CNaytiba::bIsParryHitReaction()
{
	if (nullptr == m_pAttack_Data || 0 == m_pAttack_Data->iMaxComboCount)
		return false;

	if (0 == m_pAttack_Data->iMaxComboCount - m_iComboCount)
		return true;

	return false;
}

_bool CNaytiba::bIsRepulseHitReaction()
{
	if (nullptr == m_pAttack_Data || 0 == m_pAttack_Data->iMaxRepulseCount)
		return false;
	
	if (0 < m_iRepulseCount)
		return true;

	/*if (0 >= m_pAttack_Data->iMaxRepulseCount - m_iRepulseCount)
	{
		ResetToBaseState();
		return true;
	}*/

	return false;
}

NAYITBA_EXECUTION_TYPE CNaytiba::bIsThesholdAction()
{
	if (NAYTIBA_TYPE::ELITE > m_pInitMonsterInfo->eNaytiba_Type)
		return NAYITBA_EXECUTION_TYPE::END;

	return m_eExcution;
}

CAIController* CNaytiba::GetController()
{
	Safe_AddRef(m_pAIController);
	return m_pAIController;
}

void CNaytiba::Active_SFX(const _wstring& strPartTag, const _wstring& strObjectTag, const ANIM_NOTIFY& NotifyReference)
{
	if (strPartTag.empty())
	{
	}
	else
	{
		Find_PartObject(strPartTag)->Active_SFX(strObjectTag, NotifyReference);
	}
}

void CNaytiba::Activate_PartObject_Collider(const _wstring& strPartTag, const _wstring& strColliderTag, const ANIM_NOTIFY& NotifyRef)
{
	auto pPartObject = Find_PartObject(strPartTag);
	if (nullptr == pPartObject)
		return;

	pPartObject->Activate_PartObject_Collider(strColliderTag, NotifyRef);
}

HRESULT CNaytiba::Ready_CharacterData()
{
	m_pInitMonsterInfo = m_pGameManager->Find_BossData(m_iMonsterID);
	if (nullptr != m_pInitMonsterInfo)
	{
		size_t iNumSkill = m_pInitMonsterInfo->iAttackList.size();

		for (_uint i = 0; i < ENUM_CLASS(SKILL_TYPE::END); ++i)
			m_MonsterInfo.iAttackList[i].reserve(iNumSkill);

		for (size_t j = 0; j < iNumSkill; ++j)
		{
			auto pSkillData = m_pGameManager->Find_SkillData(m_pInitMonsterInfo->iAttackList[j]);
			_uint iIndex = ENUM_CLASS(pSkillData->eSkillType);
			m_MonsterInfo.iAttackList[iIndex].push_back(pSkillData);
		}

		if (AI_TYPE::PASSIVE == m_pInitMonsterInfo->eAI_Type)
			m_MonsterInfo.eNaytibaState = NAYTIBA_STATE::MIMESSIS;
		else
			m_MonsterInfo.eNaytibaState = NAYTIBA_STATE::DEFAULT;

		m_MonsterInfo.iCurrentHealth = m_pInitMonsterInfo->iMaxHealth;
		if (8 != m_pInitMonsterInfo->iMonsetID)
			m_MonsterInfo.iCurrentShield = m_pInitMonsterInfo->iMaxShield;
		else
			m_MonsterInfo.iCurrentShield = 0.f;

		m_MonsterInfo.iCurrentStamina = m_pInitMonsterInfo->iMaxStamina;

		m_MonsterInfo.fAttackCoolTime.y = m_pInitMonsterInfo->fAttackCoolTime;
		m_MonsterInfo.fAttackRange = m_pInitMonsterInfo->fAttackRange;
		m_MonsterInfo.fMoveSpeed = m_pInitMonsterInfo->fMoveSpeed;
		m_MonsterInfo.iCurrentPhase = m_pInitMonsterInfo->iNumPhase;

		m_eTeam = OBJECT_TEAM::ENEMY;

		_uint iEndIndex = ENUM_CLASS(SKILL_TYPE::INTERACTION_SKILL);
		for (_uint i = 0; i <= iEndIndex; ++i)
		{
			for (auto& iter : m_MonsterInfo.iAttackList[i])
				m_SkillCandidates.push_back(iter);
		}
		m_iNumCandidate = m_SkillCandidates.size();
	}
	else
		int a = 10;
	return S_OK;
}

HRESULT CNaytiba::ADD_Components()
{
	/* Com_Collider_AABB */
	COBBCollider::OBB_COLLIDER_DESC		OBBDesc{};
	OBBDesc.vSize = m_pInitMonsterInfo->fColliderExtents;
	OBBDesc.vCenter = { 0.f, m_pInitMonsterInfo->fColliderExtents.y, 0.f };
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider_OBB"), reinterpret_cast<CComponent**>(&m_pColliderCom), &OBBDesc)))
		return E_FAIL;

	m_pColliderCom->SetColliderHitType(HIT_TYPE::MONSTER);

	/* Sence Component */
	CTargetComponent::TARGET_COMPONENT_DESC TargetComDesc = {};
	TargetComDesc.fRadius = m_pInitMonsterInfo->fAttackRange - 3.f;
	TargetComDesc.iNumPoints = 6.f;

	/* Prototype_Component_TargetComponent */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_TargetComponent"),
		TEXT("Com_TargetCom"), reinterpret_cast<CComponent**>(&m_pTargetCom), &TargetComDesc)))
		return E_FAIL;

	/* Drop Component */
	CDropComponent::DROP_COMPONENT_DESC DropComDesc = {};
	DropComDesc.fDropRange = 7.f;

	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_DropComponent"),
		TEXT("Com_DropCom"), reinterpret_cast<CComponent**>(&m_pDropCom), &DropComDesc)))
		return E_FAIL;

#pragma region DropItem Setting
	m_pDropCom->ADD_DropItem(make_pair( 1, 100.f ), 40.f, 60.f);
#pragma endregion

	WCHAR	ControllerProtoType[MAX_PATH] = {};
	CStringHelper::ConvertUTFToWide(m_pInitMonsterInfo->szAIControllerPrototype, ControllerProtoType);

	CBase* pInstnace = nullptr;
	if (NAYTIBA_TYPE::ELITE == m_pInitMonsterInfo->eNaytiba_Type)
	{
		WCHAR	BehaviorTreePrototpye[MAX_PATH] = {};
		CStringHelper::ConvertUTFToWide(m_pInitMonsterInfo->szAIBehaviorPrototype, BehaviorTreePrototpye);

		CBossController::BOSS_CONTROLLER_DESC ControllerDesc = { };
		ControllerDesc.pParent = this;
		ControllerDesc.szBehaviorProtoType = BehaviorTreePrototpye;
		pInstnace = m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::GAMEPLAY), ControllerProtoType, &ControllerDesc);
		if (nullptr == pInstnace)
			return E_FAIL;

		CAISenceComponent::AI_SENCE_COMPONENT_DESC SenceComDesc = {};
		SenceComDesc.fAiSearchRadius = 360.f;
		SenceComDesc.fAiTargetSearchDistance = 50.f;
		SenceComDesc.m_fAiTargetLostTime = 20.f;

		/* Prototype_Component_TargetComponent */
		if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_AISence"),
			TEXT("Com_AI_SenceCom"), reinterpret_cast<CComponent**>(&m_pAISenceCom), &SenceComDesc)))
			return E_FAIL;

		m_pAISenceCom->SetTraceHitType(HIT_TYPE::SENCE);
		m_pAISenceCom->ADD_SenceOnlyTraceObject(HIT_TYPE::PLAYER);
		m_pAISenceCom->Bind_TargetSearch([&](CGameObject* pTarget) { BattleEvent(pTarget, NAYTIBA_STATE::BATTLE); });
	}
	else
	{
		CAIController::GAMEOBJECT_DESC ControllerDesc = { };
		ControllerDesc.pParent = this;

		pInstnace = m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::GAMEPLAY), ControllerProtoType, &ControllerDesc);
		if (nullptr == pInstnace)
			return E_FAIL;

		CAISenceComponent::AI_SENCE_COMPONENT_DESC SenceComDesc = {};
		if (AI_TYPE::PASSIVE == m_pInitMonsterInfo->eAI_Type && 9 != m_pInitMonsterInfo->iMonsetID)
		{
			SenceComDesc.fAiSearchRadius = 60.f;
		}
		else
			SenceComDesc.fAiSearchRadius = 360.f;
		SenceComDesc.fAiTargetSearchDistance = 20.f;
		SenceComDesc.m_fAiTargetLostTime = 20.f;

		/* Prototype_Component_TargetComponent */
		if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_AISence"),
			TEXT("Com_AI_SenceCom"), reinterpret_cast<CComponent**>(&m_pAISenceCom), &SenceComDesc)))
			return E_FAIL;

		m_pAISenceCom->SetTraceHitType(HIT_TYPE::SENCE);
		m_pAISenceCom->ADD_SenceOnlyTraceObject(HIT_TYPE::PLAYER);
		m_pAISenceCom->Bind_TargetSearch([&](CGameObject* pTarget) { BattleEvent(pTarget, NAYTIBA_STATE::BATTLE); });
	}

	/* Com_CCT */
	CCharacterController::CCT_DESC Desc;
	PxUserData tUserData;
	tUserData.szActorTag = TEXT("Player_CCT");

	Desc.eCharacterControllerType = CCharacterController::CCT_SHAPE::CAPSULE;
	Desc.tUserData = tUserData;
	//캡슐 컨트롤러에서 x는 구 성분 y는 기둥 성분
	Desc.vSize = { m_pInitMonsterInfo->fColliderExtents.x, m_pInitMonsterInfo->fColliderExtents.y, 0.f };
	XMStoreFloat4(&Desc.vStartPos, m_pTransformCom->Get_State(STATE::POSITION));
	Desc.vMaterial = _float3(0.5f, 0.5f, 0.f);
	Desc.pHitReporter = CPlayerCCTHitReporter::Create();
	Desc.pBehaviorCallback = CPlayerBehaviorCallback::Create();
	Desc.pQueryFilterCallback = CPlayerCCTQueryFilterCallback::Create();
	Desc.iCollisionGroup = PHYSX_CCT;
	Desc.fStepOffset = 0.05f;

	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_CharacterController"),
		TEXT("Com_CCT"), reinterpret_cast<CComponent**>(&m_pCCT), &Desc)))
		return E_FAIL;

	m_pGameInstance->Add_CCT_ToPhysx(this, m_pCCT);
	m_pAIController = static_cast<CAIController*>(pInstnace);

	m_pNotifyCom->Set_ModelCom(m_pBodyModelCom);

	return S_OK;
}

HRESULT CNaytiba::ADD_PartObjects()
{
	WCHAR	ModelProtoType[MAX_PATH] = {};
	CStringHelper::ConvertUTFToWide(m_pInitMonsterInfo->szModelPrototype, ModelProtoType);

	CNayitbaPartBody::NAYITBA_PART_BODY_DESC BodyDesc = { };
	BodyDesc.pParent = this;
	BodyDesc.pParentTransform = m_pTransformCom;
	BodyDesc.vScale = { 1.f, 1.f, 1.f };
	BodyDesc.szBodyModel = ModelProtoType;
	BodyDesc.fSpeedPerSec = 5.f;
	if(FAILED(__super::Add_PartObject(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Nayitba_Body"), TEXT("Part_Body"), &BodyDesc)))
		return E_FAIL;

	auto pPartBody = Find_PartObject(TEXT("Part_Body"));
	if (nullptr == pPartBody)
		return E_FAIL;

	m_pPartBody = static_cast<CNayitbaPartBody*>(pPartBody);
	Import_ModelPtr();

	 // 무기랑 빔?
	if (strcmp("None", m_pInitMonsterInfo->szFaceName))
	{
		CNaytibaFace::NAYTIBA_FACE_DESC FaceDesc = { };
		FaceDesc.pParent = this;
		FaceDesc.pParentTransform = m_pTransformCom;
		FaceDesc.vScale = { 1.f, 1.f, 1.f };
		FaceDesc.pBodyModelCom = static_cast<CModel*>(m_pPartBody->Find_Component(TEXT("Com_Model")));
		CStringHelper::ConvertUTFToWide(m_pInitMonsterInfo->szFaceName, FaceDesc.szFaceJsonDataName);
		if (FAILED(__super::Add_PartObject(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Nayitba_Face"), TEXT("Part_Face"), &FaceDesc)))
			return E_FAIL;
	}

	if (strcmp("None", m_pInitMonsterInfo->szLeftWeaponPrototypeName))
	{
		CNaytibaLeftWeaponPart::WEAPON_DESC LWeaponDesc = { };
		LWeaponDesc.pParent = this;
		LWeaponDesc.pParentTransform = m_pTransformCom;
		LWeaponDesc.pSocketMatrix = m_pBodyModelCom->Get_BoneMatrixPtr(m_pInitMonsterInfo->szLeftBoneName);
		LWeaponDesc.vScale = { 1.f, 1.f, 1.f };
		//LWeaponDesc.vRotation = {(90.f), (-90.f), (0.f), 1.f};
		//LWeaponDesc.vRotation = {XMConvertToRadians(90.f), XMConvertToRadians(-90.f), XMConvertToRadians(0.f), 1.f};
		CStringHelper::ConvertUTFToWide(m_pInitMonsterInfo->szLeftWeaponPrototypeName, LWeaponDesc.szWeaponModelPrototype);
		LWeaponDesc.fSpeedPerSec = 5.f;
		if (FAILED(__super::Add_PartObject(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Nayitba_Left_Weapon"), TEXT("Part_WeaponL"), &LWeaponDesc)))
			return E_FAIL;

		auto pPartWeapon = Find_PartObject(TEXT("Part_WeaponL"));
		if (nullptr == pPartWeapon)
			return E_FAIL;

		m_pLeftWeapon = static_cast<CNaytibaLeftWeaponPart*>(pPartWeapon);
	}

	if (strcmp("None", m_pInitMonsterInfo->szRightWeaponPrototypeName))
	{
		CNaytibaRightWeaponPart::WEAPON_DESC RWeaponDesc = { };
		RWeaponDesc.pParent = this;
		RWeaponDesc.pParentTransform = m_pTransformCom;
		RWeaponDesc.pSocketMatrix = m_pBodyModelCom->Get_BoneMatrixPtr(m_pInitMonsterInfo->szRightBoneName);
		RWeaponDesc.vScale = { 1.f, 1.f, 1.f };
		//RWeaponDesc.vRotation = { (90.f), (-81.5f), (0.f), 1.f };
		//RWeaponDesc.vRotation = { XMConvertToRadians(90.f), XMConvertToRadians(-81.5f), XMConvertToRadians(0.f), 1.f };
		CStringHelper::ConvertUTFToWide(m_pInitMonsterInfo->szRightWeaponPrototypeName, RWeaponDesc.szWeaponModelPrototype);
		RWeaponDesc.fSpeedPerSec = 5.f;
		if (FAILED(__super::Add_PartObject(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Nayitba_Right_Weapon"), TEXT("Part_WeaponR"), &RWeaponDesc)))
			return E_FAIL;
	}

	/*if (10 == m_pInitMonsterInfo->iMonsetID)
	{
		CNaytibaBeam_Part::BEAM_DESC BeamDesc = { };
		BeamDesc.bIsApplyTransform = true;
		BeamDesc.pParentTransform = m_pTransformCom;
		BeamDesc.pSocketMatrix = m_pBodyModelCom->Get_BoneMatrixPtr("GunBarrel_Back");
		BeamDesc.vScale = { 0.02f, 0.02f, 1.f };
		BeamDesc.vRotation = { XMConvertToRadians(90.f), 0.f, 0.f, 0.f };
		BeamDesc.fSpeedPerSec = 5.f;

		if (FAILED(__super::Add_PartObject(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Nayitba_BeamPart"), TEXT("Part_Beam"), &BeamDesc)))
			return E_FAIL;
	}*/

	return S_OK;
}

void CNaytiba::BattleEvent(CGameObject* pTarget, NAYTIBA_STATE eState)
{
	m_MonsterInfo.eNaytibaState = eState;

	if (NAYTIBA_STATE::BATTLE == eState)
	{
		if (NAYTIBA_TYPE::ELITE <= m_pInitMonsterInfo->eNaytiba_Type)
		{
			m_pAISenceCom->Add_SenceTargetObject(pTarget);

			CUIHUD* pUIHUD = dynamic_cast<CUIHUD*>(m_pGameInstance->GetCurrentLevelHUD());
			if (pUIHUD)
				pUIHUD->Set_Boss_Desc(m_pInitMonsterInfo, &m_MonsterInfo);

			Safe_Release(pUIHUD);
		}
		else
			VisibleStatusUI(0.f);
	}
	else
	{
		VisibleStatusUI(0.f, true);
	}
}

void CNaytiba::ResetToBaseState()
{
	if (false == m_bIsActive)
		m_bIsActive = true;

	if(false == m_pCCT->Get_Active())
		m_pCCT->Set_Active(true);
	
	if (m_pPartBody)
	{
		m_pPartBody->SetPart_BodyColor(false);
		m_pPartBody->SetRimLightData(false, 0.f, 0.f, {}, 0.f);
		m_pPartBody->Stop_All_Effect();
	}

	if(m_pLeftWeapon)
		m_pLeftWeapon->Stop_All_Effect();
}

void CNaytiba::VisibleStatusUI(_float fTimeDelta, _bool bIsForce)
{
	if (0 >= m_MonsterInfo.iCurrentHealth || bIsForce)
	{
		//m_MonsterInfo.eNaytibaState = NAYTIBA_STATE::DEAD;
		auto pCurHUD = dynamic_cast<CUIHUD*>(m_pGameInstance->GetCurrentLevelHUD());

		if(pCurHUD)
			pCurHUD->Return_WorldUI(m_pStatusUI);
		// 몬스터 체력
			
		m_pStatusUI = nullptr;
		Safe_Release(pCurHUD);
	}
	else
	{
		if (m_bIsTimeVisible)
		{
			if (m_vHitVisibleDuration.x <= m_vHitVisibleDuration.y)
			{
				if (VISIBILITY::HIDDEN == m_pStatusUI->GetVisibility())
					m_pStatusUI->SetVisibility(VISIBILITY::VISIBLE);

				m_vHitVisibleDuration.x += fTimeDelta;
			}
			else
				m_pStatusUI->SetVisibility(VISIBILITY::HIDDEN);
		}

		if (nullptr == m_pStatusUI)
		{
			if (NAYTIBA_TYPE::ELITE > m_pInitMonsterInfo->eNaytiba_Type)
			{
				auto pCurHUD = dynamic_cast<CUIHUD*>(m_pGameInstance->GetCurrentLevelHUD());
				if (pCurHUD)
					m_pStatusUI = pCurHUD->Rent_WorldUI(TEXT("Pool_MonsterVital"), this, &m_MonsterInfo.vStatusBarPoint);
				Safe_Release(pCurHUD);
			}
		}
	}
	
}

_bool CNaytiba::ActionDamageLogic(const DEFAULT_DAMAGE_DESC* pDamageDesc)
{
	if (nullptr == pDamageDesc || nullptr == pDamageDesc->pSkillData)
		return false;

	const CHARACTER_SKILL_DESC* pSkillDesc = static_cast<const CHARACTER_SKILL_DESC*>(pDamageDesc->pSkillData);

	m_vHitVisibleDuration.x = 0.f;
	m_pGameManager->Start_Lockon();
	
	if (SKILL_TYPE::BETA_SKILL == pSkillDesc->eSkillType)
	{
		if (!m_pBulletList.empty())
			m_pBulletList.clear();
	}

	if (NAYTIBA_STATE::BATTLE != m_MonsterInfo.eNaytibaState)
	{
		m_pAISenceCom->Add_SenceTargetObject(pDamageDesc->pAttacker);
		m_MonsterInfo.eNaytibaState = NAYTIBA_STATE::BATTLE;
	}

	if (SKILL_PROPERTY::PARRY & pSkillDesc->eProPerty)
	{
		_bool bIsScarletParry = false;
		if (8 == m_pInitMonsterInfo->iMonsetID)
		{
			// 홍련일때는 라스트 기믹에서만 패링했을때 방어력이 까인다
			auto pBossController = static_cast<CBossController*>(m_pAIController);
			if (pBossController->bIsLastAttack() || pBossController->bIsEntranceAttack())
			{
				bIsScarletParry = true;
			}
		}

		if (bIsScarletParry)
		{
			_float fDamage = m_pGameInstance->Random(100.f, 150.f);
			m_MonsterInfo.iCurrentShield -= fDamage;

			if (0 >= m_MonsterInfo.iCurrentShield)
				m_MonsterInfo.iCurrentShield = 0.f;
		}
		else
		{
			if (0 != pSkillDesc->iSkillID)
			{
				if (0 < m_MonsterInfo.iCurrentStamina)
					m_MonsterInfo.iCurrentStamina--;

				if (0 == m_MonsterInfo.iCurrentStamina)
					ResetToBaseState();
			}
		}
	}
	else
	{
		if (8 == m_pInitMonsterInfo->iMonsetID)
		{
			// 홍련일때는 라스트 기믹에서만 패링했을때 방어력이 까인다
			auto pBossController = static_cast<CBossController*>(m_pAIController);
			if (pBossController->bIsLastAttack() || pBossController->bIsEntranceAttack())
				return false;
		}

		switch (m_pInitMonsterInfo->eAI_Type)
		{
		case AI_TYPE::DEFENSIVE: // 방어형
			return DefenseTypeDamage(pDamageDesc);
		case AI_TYPE::AGGRESSIVE:
		case AI_TYPE::PASSIVE:  // 공격형
		{
			Play_HitSound();
			if (m_bIsSuperMonster)
				return m_pGameManager->ComputeDamageLogic(&m_MonsterInfo, 0);
			else
				return m_pGameManager->ComputeDamageLogic(&m_MonsterInfo, pSkillDesc->iSkillDamage);
		}
		}
	}
	
	return true;
}

_bool CNaytiba::DefenseTypeDamage(const DEFAULT_DAMAGE_DESC* pDamageDesc, _float fDamageReductionRate)
{
	const CHARACTER_SKILL_DESC* pSkillDesc = static_cast<const CHARACTER_SKILL_DESC*>(pDamageDesc->pSkillData);

	long long iDamage = pSkillDesc->iSkillDamage;
	if (m_bIsSuperMonster)
		iDamage = 0;

	_bool bIsCheckDefenceLogic = false;
	
	if (m_pAttack_Data)
	{
		if (false == (SKILL_PROPERTY::GUARD & m_pAttack_Data->eProPerty) )
		{
			m_pGameManager->ComputeDamageLogic(&m_MonsterInfo, iDamage);
		}
		else
			bIsCheckDefenceLogic = true;
	}
	else
	{
		if (NAYITBA_EXECUTION_TYPE::END != m_eExcution)
		{
			m_pGameManager->ComputeDamageLogic(&m_MonsterInfo, iDamage);
		}
		bIsCheckDefenceLogic = true;
	}

	if (bIsCheckDefenceLogic)
	{
		_float fRandomIndex = m_pGameInstance->Random(0.f, 100.f);
		m_pGameInstance->Manager_PlaySound(TEXT("EVE_PC_Normal_Parry_Sword_02.wav"), CHANNELID::EFFECT, 10.f, 1.f);

		_vector vOwnerPos{}, vTempOwnerPos{}, vTargetPos{}, vDir{};
		vOwnerPos = vTempOwnerPos = m_pTransformCom->Get_State(STATE::POSITION);
		vTargetPos = pDamageDesc->pAttacker->GetTransform()->Get_State(STATE::POSITION);

		vTempOwnerPos.m128_f32[1] = vTargetPos.m128_f32[1] = 0.f;
		vDir = XMVector3Normalize(vTargetPos - vTempOwnerPos);

		_vector vOwnerLook = m_pTransformCom->Get_State(STATE::LOOK);

		_float fScalar = XMVectorGetX(XMVector3Dot(vOwnerLook, vDir));
		fScalar = Clamp<_float>(fScalar, -1.0f, 1.0f);   // NaN 방지
		_float fRadian = acosf(fScalar);

		// 앞
		if (0 < fScalar)
		{
			if (fRadian < m_pAISenceCom->GetSenceRadiusRadian())
			{
				long long iFrontDamage = iDamage * fDamageReductionRate;
				m_pGameManager->ComputeDamageLogic(&m_MonsterInfo, iFrontDamage);
				return false;
			}
		}
		else
			m_pGameManager->ComputeDamageLogic(&m_MonsterInfo, iDamage);
	}
	else
		Play_HitSound();
	
	return true;
}

void CNaytiba::CreateHitBox(const AnimNotify* pNotify)
{
	CAttackHitBox::HIT_BOX_DESC HitBoxDesc = {};
	HitBoxDesc.pAttacker = this;

	_uint iGameLevel = ENUM_CLASS(LEVEL::GAMEPLAY);
	_wstring szProtoType(pNotify->szNotifyArg01.begin(), pNotify->szNotifyArg01.end());
	_wstring szLayerName(pNotify->szNotifyArg02.begin(), pNotify->szNotifyArg02.end());

	CAttackHitBox::HIT_BOX_DESC pHitBoxDesc = {};
	auto pSkillData = m_pGameManager->Find_SkillData(pNotify->iNumData01);
	pHitBoxDesc.pData = pSkillData;

	pHitBoxDesc.eColType = COLLIDER(pNotify->iNumData02);
	pHitBoxDesc.eHitBoxType = HIT_TYPE(pNotify->iNumData03);
	pHitBoxDesc.bIsApplyTransform = true;
	pHitBoxDesc.pAttacker = this;

	_float fRange = {};
	if (XMVector3Equal(XMLoadFloat3(&pNotify->vNotifyScale), XMVectorZero()))
	{
		pHitBoxDesc.vScale = pSkillData->vHitBoxExtents;
		fRange = m_pInitMonsterInfo->fAttackRange;
	}
	else
	{
		pHitBoxDesc.vScale = pNotify->vNotifyScale;
		fRange = pNotify->fNumData01;
	}

	pHitBoxDesc.fImpactForce = m_fImpactForce;
	_vector vCharacterPos = GetTransform()->Get_State(STATE::POSITION);
	_vector vCharacterLook = GetTransform()->Get_State(STATE::LOOK);
	if (1 == pNotify->iNumData04)
		vCharacterLook * -1.f;

	vCharacterPos += vCharacterLook * fRange;
	XMStoreFloat3(&pHitBoxDesc.vPosition, vCharacterPos);

	auto pHitBox = m_pGameManager->SetActivePoolObject(ENUM_CLASS(LEVEL::STATIC), ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("GamePlay_Layer_HitBox"), TEXT("Hit_Box"));
	if(pHitBox)
		static_cast<CAttackHitBox*>(pHitBox)->Initialize(pHitBoxDesc);
	m_iComboCount++;
}

void CNaytiba::SpawnObject(const AnimNotify* pNotify)
{ 
	// 여기서 파트오브젝트로 만들고 파트오브젝트업데이트에서 소켓에 붙여서
	// 랜더링하다가 특정 Shoot 함수가 들어오면 발사하자

	//	"iNumData01" : 스킬 번호
	//	"iNumData02" : 충돌체 번호,
	//	"iNumData03" : 어떤 타입이랑 충돌할지
	 
	//	프로토 타입 데이터 만들거
	//	"szNotifyArg01" : "Prototype_GameObject_RockBullet"
	//	"szNotifyArg02" : "RockBullet_Layer",
	//	"szNotifyArg03" : "Bip001-R-Hand",
	if (0 == pNotify->iNumData01)
		return;

	_wstring	szPrototypeName(pNotify->szNotifyArg01.begin(),  pNotify->szNotifyArg01.end());
	_wstring	szLayerName(pNotify->szNotifyArg02.begin(), pNotify->szNotifyArg02.end());
	
	// 돌 오브젝트 만들어서
	// 행렬 받고 붙여놨다가 특정 이벤트때 처리한다.
	CBullet::BULLET_DESC pBulletDesc = {};
	pBulletDesc.pParent = this;
	pBulletDesc.fRotationPerSec = XMConvertToRadians(90.f);
	pBulletDesc.vScale = pNotify->vNotifyScale;
	if (XMVector3Equal(XMLoadFloat3(&pNotify->vNotifyScale), XMVectorZero()))
		pBulletDesc.vScale = {1.f, 1.f, 1.f};
	else
		pBulletDesc.vScale = pNotify->vNotifyScale;

	if ("" != pNotify->szNotifyArg03)
		pBulletDesc.pSocketMatrix = m_pBodyModelCom->Get_BoneMatrixPtr(pNotify->szNotifyArg03.c_str());
	else
		pBulletDesc.pSocketMatrix = m_pGameInstance->GetIdentityMatrixPtr();

	 
	pBulletDesc.iSkillID = pNotify->iNumData01;
	pBulletDesc.iHitType = pNotify->iNumData03;
	pBulletDesc.iBulletType = pNotify->iNumData04;

	_bool bTraceBullet = true;
	_vector vOwnerPos = m_pTransformCom->Get_State(STATE::POSITION);
	_vector vTargetPos = m_pTargetCom->GetTarget()->GetTransform()->Get_State(STATE::POSITION);
	_vector vLook = m_pTransformCom->Get_State(STATE::LOOK);

	 _float fRadian = acosf(XMVectorGetX(XMVector3Dot(vLook, XMVector3Normalize(vTargetPos - vOwnerPos))));
	 _float fLength = XMVectorGetX(XMVector3Length(vTargetPos - vOwnerPos));

	 pBulletDesc.bIsApplyTransform = true;
	 _matrix WeaponMatrix = XMLoadFloat4x4(pBulletDesc.pSocketMatrix) * XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr());
	 XMStoreFloat3(&pBulletDesc.vPosition, WeaponMatrix.r[3]);
	 if (0 < XMVectorGetX(XMVector3Dot(vOwnerPos, vTargetPos)))
	 {
		 if (fRadian <= XMConvertToRadians(pNotify->fNumData01))
		 {
			 _vector vBulletTargetPos = m_pTargetCom->GetTarget()->GetTransform()->Get_State(STATE::POSITION);
			 vBulletTargetPos.m128_f32[1] += 2.f;

			XMStoreFloat3(&pBulletDesc.vTargetPoint, vBulletTargetPos);
			bTraceBullet = false;
		 }
	 }

	 if (bTraceBullet)
	 {
		 _vector vBulletTargetPos = vOwnerPos + vLook * 50.f;
		 vBulletTargetPos.m128_f32[1] += 2.f;
		 XMStoreFloat3(&pBulletDesc.vTargetPoint, vBulletTargetPos);
	 }
		 

	m_pBulletList.clear();
	_uint iLevel = ENUM_CLASS(LEVEL::GAMEPLAY);
	auto pBase = m_pGameInstance->Add_Get_GameObject(iLevel, szPrototypeName.c_str(), iLevel, szLayerName.c_str(), &pBulletDesc);
	
	if (pBase)
	{
		auto pBullet = dynamic_cast<CBullet*>(pBase);

		if(pBullet)
			m_pBulletList.push_back(pBullet);
		else
			pBullet->Set_Dead(true);
	}
		
}

void CNaytiba::ShootProjectile(const AnimNotify* pNotify)
{
	// 여기서 소유하고 있는 Projectile을 모두 발사한다.
	// pNotify->iNumData01; <- true : 활성화
	// false : 비활성화
	_vector vTargetPos = m_pTargetCom->GetTarget()->GetTransform()->Get_State(STATE::POSITION);
	for (auto& iter : m_pBulletList)
	{
		if (iter->bIsHit())
			iter->Set_Dead(true);
		else
			iter->Shoot_Projectile(vTargetPos, 40.f);
	}

	m_pBulletList.clear();
}

void CNaytiba::Attack_Interaction(const AnimNotify* pNotify)
{
	auto pTarget = m_pTargetCom->GetTarget();
	if (nullptr == pTarget || nullptr == pNotify)
		return;

	// 여기서 타겟에게 Attack Interaction을 넘겨준다.
	ATK_INTERACTION_DESC  ATK_InteractionDesc = {};
	ATK_InteractionDesc.eInteraction_Type = ATK_INTERACTION_TYPE(pNotify->iNumData01);
	ATK_InteractionDesc.iFrameCnt = pNotify->iNumData02;
	ATK_InteractionDesc.pArg = (void *)pNotify;
	
	if (ATK_INTERACTION_TYPE::REPULSE == ATK_InteractionDesc.eInteraction_Type)
		m_iRepulseCount ++;

	// 필요하면 작업하면 됩니다.
	ATK_InteractionDesc.pArg = nullptr;
	static_cast<CCharacter*>(pTarget)->Attack_Interaction(&ATK_InteractionDesc);
}

void CNaytiba::Change_Color(const AnimNotify* pNotify)
{
	// pNotify->iNumData01 : 컬러를 활성화할지 끌지 
	// pNotify->iNumData02 : 패턴 색상팔레트 인덱스
	// pNotify->iNumData03 : 사라질지 말지
	if (false == pNotify->iNumData01)
	{
		if (false == m_bIsActive)
			m_bIsActive = true;
	}

	m_pPartBody->SetPart_BodyColor(pNotify->iNumData01, pNotify->iNumData03 , CLINET_COLOR_PATTERN[pNotify->iNumData02]);
}

void CNaytiba::Draw_AttackLine(const AnimNotify* pNotify)
{
	auto pBoneMatrix = m_pBodyModelCom->Get_BoneMatrixPtr(pNotify->szNotifyArg01.c_str());
	

	_vector vStartPos = XMLoadFloat4x4(pBoneMatrix).r[3];
	_vector vEndPos = vStartPos + m_pTransformCom->Get_State(STATE::LOOK) * pNotify->fNumData01;

	
}

void CNaytiba::Play_SFXEffect(const AnimNotify* pNotify)
{
	if (Compare_SFX_Name(pNotify->szNotifyArg01))
	{
		m_pPartBody->SetRimLightData(true, 4.f, 3.f, { 0.8f ,0.8f, 0.8f, 1.f }, 0.4f);
	}
}

void CNaytiba::Play_MoveSound(_uint iType)
{
	// Type Number : 0 => WALK 사운드
	// Type Number : 1 => RUN  사운드
	_float fRandomIndex = m_pGameInstance->Random(0.f, 100.f);
	switch (m_eGroundSoundType)
	{
	case GROUND_SOUND_TYPE::IRON:
		if (0 == iType)
		{
			if (25 >= fRandomIndex)
				m_pGameInstance->Manager_PlaySound(TEXT("PC_foot_metal_thin_Walk_1.wav"), CHANNELID::EFFECT, 0.1f, 1.f);
			else if (50 >= fRandomIndex)
				m_pGameInstance->Manager_PlaySound(TEXT("PC_foot_metal_thin_Walk_2.wav"), CHANNELID::EFFECT, 0.1f, 1.f);
			else if (75 >= fRandomIndex)
				m_pGameInstance->Manager_PlaySound(TEXT("PC_foot_metal_thin_Walk_3.wav"), CHANNELID::EFFECT, 0.1f, 1.f);
			else
				m_pGameInstance->Manager_PlaySound(TEXT("PC_foot_metal_thin_Walk_4.wav"), CHANNELID::EFFECT, 0.1f, 1.f);
		}
		else
		{
			if (25 >= fRandomIndex)
				m_pGameInstance->Manager_PlaySound(TEXT("PC_foot_metal_thin_run_1.wav"), CHANNELID::EFFECT, 0.1f, 1.f);
			else if (50 >= fRandomIndex)
				m_pGameInstance->Manager_PlaySound(TEXT("PC_foot_metal_thin_run_2.wav"), CHANNELID::EFFECT, 0.1f, 1.f);
			else if (75 >= fRandomIndex)
				m_pGameInstance->Manager_PlaySound(TEXT("PC_foot_metal_thin_run_3.wav"), CHANNELID::EFFECT, 0.1f, 1.f);
			else
				m_pGameInstance->Manager_PlaySound(TEXT("PC_foot_metal_thin_run_4.wav"), CHANNELID::EFFECT, 0.1f, 1.f);
		}
		break;
	default :
		if (0 == iType)
		{
			if (4 == m_iMonsterID || 5 == m_iMonsterID)
				Play_StatueMoveSound(0, fRandomIndex);
			else if (8 == m_iMonsterID)
				Play_ScarletMoveSound(0);
			else
			{
				if (20 >= fRandomIndex)
					m_pGameInstance->Manager_PlaySound(TEXT("Mon_foot_M_Default_1.wav"), CHANNELID::EFFECT, 0.1f, 1.f);
				else if (40 >= fRandomIndex)
					m_pGameInstance->Manager_PlaySound(TEXT("Mon_foot_M_Default_2.wav"), CHANNELID::EFFECT, 0.1f, 1.f);
				else if (60 >= fRandomIndex)
					m_pGameInstance->Manager_PlaySound(TEXT("Mon_foot_M_Default_3.wav"), CHANNELID::EFFECT, 0.1f, 1.f);
				else if (80 >= fRandomIndex)
					m_pGameInstance->Manager_PlaySound(TEXT("Mon_foot_M_Default_4.wav"), CHANNELID::EFFECT, 0.1f, 1.f);
				else
					m_pGameInstance->Manager_PlaySound(TEXT("Mon_foot_M_Default_5.wav"), CHANNELID::EFFECT, 0.1f, 1.f);
			}
		}
		else
		{
			if (8 == m_iMonsterID)
				Play_ScarletMoveSound(1);
			else
			{
				if (25 >= fRandomIndex)
					m_pGameInstance->Manager_PlaySound(TEXT("Pc_Footstep_Sand_Run_6.wav"), CHANNELID::EFFECT, 0.1f, 1.f);
				else if (50 >= fRandomIndex)
					m_pGameInstance->Manager_PlaySound(TEXT("Pc_Footstep_Sand_Run_5.wav"), CHANNELID::EFFECT, 0.1f, 1.f);
				else if (75 >= fRandomIndex)
					m_pGameInstance->Manager_PlaySound(TEXT("Pc_Footstep_Sand_Run_3.wav"), CHANNELID::EFFECT, 0.1f, 1.f);
				else
					m_pGameInstance->Manager_PlaySound(TEXT("Pc_Footstep_Sand_Run_2.wav"), CHANNELID::EFFECT, 0.1f, 1.f);
			}
		}
		break;
	}
}

void CNaytiba::Play_StatueMoveSound(_uint iType, _float fRandomIndex)
{
	if (25 >= fRandomIndex)
		m_pGameInstance->Manager_PlaySound(TEXT("Mon_Statue_Foot_1.wav"), CHANNELID::EFFECT, 0.1f, 1.f);
	else if (50 >= fRandomIndex)
		m_pGameInstance->Manager_PlaySound(TEXT("Mon_Statue_Foot_2.wav"), CHANNELID::EFFECT, 0.1f, 1.f);
	else if (75 >= fRandomIndex)
		m_pGameInstance->Manager_PlaySound(TEXT("Mon_Statue_Foot_3.wav"), CHANNELID::EFFECT, 0.1f, 1.f);
	else
		m_pGameInstance->Manager_PlaySound(TEXT("Mon_Statue_Foot_4.wav"), CHANNELID::EFFECT, 0.1f, 1.f);
}

void CNaytiba::Play_BanacleMoveSound(_uint iType)
{
	_float fRandomIndex = m_pGameInstance->Random(0.f, 100.f);
	switch (m_eGroundSoundType)
	{
	case GROUND_SOUND_TYPE::IRON:
		if (25 >= fRandomIndex)
			m_pGameInstance->Manager_PlaySound(TEXT("M_Barnacle_footstep_metal_2.wav"), CHANNELID::EFFECT, 0.1f, 1.f);
		else if (50 >= fRandomIndex)
			m_pGameInstance->Manager_PlaySound(TEXT("M_Barnacle_footstep_metal_3.wav"), CHANNELID::EFFECT, 0.1f, 1.f);
		else if (75 >= fRandomIndex)
			m_pGameInstance->Manager_PlaySound(TEXT("M_Barnacle_footstep_metal_4.wav"), CHANNELID::EFFECT, 0.1f, 1.f);
		else
			m_pGameInstance->Manager_PlaySound(TEXT("M_Barnacle_footstep_metal_5.wav"), CHANNELID::EFFECT, 0.1f, 1.f);
		break;
	case GROUND_SOUND_TYPE::SAND:
		if (25 >= fRandomIndex)
			m_pGameInstance->Manager_PlaySound(TEXT("M_Barnacle_footstep_sand_1.wav"), CHANNELID::EFFECT, 0.1f, 1.f);
		else if (50 >= fRandomIndex)
			m_pGameInstance->Manager_PlaySound(TEXT("M_Barnacle_footstep_sand_2.wav"), CHANNELID::EFFECT, 0.1f, 1.f);
		else if (75 >= fRandomIndex)
			m_pGameInstance->Manager_PlaySound(TEXT("M_Barnacle_footstep_sand_3.wav"), CHANNELID::EFFECT, 0.1f, 1.f);
		else
			m_pGameInstance->Manager_PlaySound(TEXT("M_Barnacle_footstep_sand_4.wav"), CHANNELID::EFFECT, 0.1f, 1.f);
		break;
	}
}

void CNaytiba::Play_AntlionMoveSound(_uint iType)
{
	_float fRandomIndex = m_pGameInstance->Random(0.f, 100.f);
	switch (m_eGroundSoundType)
	{
	case GROUND_SOUND_TYPE::IRON:
		if (25 >= fRandomIndex)
			m_pGameInstance->Manager_PlaySound(TEXT("M_Antlion_Footstep_metal_1.wav"), CHANNELID::EFFECT, 0.1f, 1.f);
		else if (50 >= fRandomIndex)
			m_pGameInstance->Manager_PlaySound(TEXT("M_Antlion_Footstep_metal_2.wav"), CHANNELID::EFFECT, 0.1f, 1.f);
		else if (75 >= fRandomIndex)
			m_pGameInstance->Manager_PlaySound(TEXT("M_Antlion_Footstep_metal_3.wav"), CHANNELID::EFFECT, 0.1f, 1.f);
		else
			m_pGameInstance->Manager_PlaySound(TEXT("M_Antlion_Footstep_metal_4.wav"), CHANNELID::EFFECT, 0.1f, 1.f);
		break;
	case GROUND_SOUND_TYPE::SAND:
		if (25 >= fRandomIndex)
			m_pGameInstance->Manager_PlaySound(TEXT("M_Antlion_Footstep_sand_1.wav"), CHANNELID::EFFECT, 0.1f, 1.f);
		else if (50 >= fRandomIndex)
			m_pGameInstance->Manager_PlaySound(TEXT("M_Antlion_Footstep_sand_2.wav"), CHANNELID::EFFECT, 0.1f, 1.f);
		else if (75 >= fRandomIndex)
			m_pGameInstance->Manager_PlaySound(TEXT("M_Antlion_Footstep_sand_3.wav"), CHANNELID::EFFECT, 0.1f, 1.f);
		else
			m_pGameInstance->Manager_PlaySound(TEXT("M_Antlion_Footstep_sand_4.wav"), CHANNELID::EFFECT, 0.1f, 1.f);
		break;
	}
}

void CNaytiba::Play_GorillaMoveSound(_uint iType, _uint SoundType)
{
	_float fRandomIndex = m_pGameInstance->Random(0.f, 100.f);
	if (1 == SoundType)
	{
		if (25 >= fRandomIndex)
			m_pGameInstance->Manager_PlaySound(TEXT("Mon_GorillaB_Hand_1.wav"), CHANNELID::EFFECT, 0.5f, 1.f);
		else if (50 >= fRandomIndex)
			m_pGameInstance->Manager_PlaySound(TEXT("Mon_GorillaB_Hand_2.wav"), CHANNELID::EFFECT, 0.5f, 1.f);
		else if (75 >= fRandomIndex)
			m_pGameInstance->Manager_PlaySound(TEXT("Mon_GorillaB_Hand_3.wav"), CHANNELID::EFFECT, 0.5f, 1.f);
		else
			m_pGameInstance->Manager_PlaySound(TEXT("Mon_GorillaB_Hand_4.wav"), CHANNELID::EFFECT, 0.5f, 1.f);
	}
	else
	{
		if (25 >= fRandomIndex)
			m_pGameInstance->Manager_PlaySound(TEXT("EVE_M_Gorilla_Attack_footstep_2.wav"), CHANNELID::EFFECT, 1.f, 1.f);
		else if (50 >= fRandomIndex)
			m_pGameInstance->Manager_PlaySound(TEXT("EVE_M_Gorilla_SmashChain_footstep_4.wav"), CHANNELID::EFFECT, 1.f, 1.f);
		else if (75 >= fRandomIndex)
			m_pGameInstance->Manager_PlaySound(TEXT("EVE_M_Gorilla_SmashChain_footstep_3.wav"), CHANNELID::EFFECT, 1.f, 1.f);
		else
			m_pGameInstance->Manager_PlaySound(TEXT("EVE_M_Gorilla_SmashChain_footstep_2.wav"), CHANNELID::EFFECT, 1.f, 1.f);
	}
}

void CNaytiba::Play_ScarletMoveSound(_uint iType)
{
	_float fRandomIndex = m_pGameInstance->Random(0.f, 100.f);
	//Walk
	if (0 == iType)
	{
		if (25 >= fRandomIndex)
			m_pGameInstance->Manager_PlaySound(TEXT("M_Scarlet_Foot_Walk_1.wav"), CHANNELID::EFFECT, 2.f, 1.f);
		else if (50 >= fRandomIndex)
			m_pGameInstance->Manager_PlaySound(TEXT("M_Scarlet_Foot_Walk_2.wav"), CHANNELID::EFFECT, 2.f, 1.f);
		else if (75 >= fRandomIndex)
			m_pGameInstance->Manager_PlaySound(TEXT("M_Scarlet_Foot_Walk_3.wav"), CHANNELID::EFFECT, 2.f, 1.f);
		else
			m_pGameInstance->Manager_PlaySound(TEXT("M_Scarlet_Foot_Walk_4.wav"), CHANNELID::EFFECT, 2.f, 1.f);
	}
	else if (1 == iType)
	{
		if (25 >= fRandomIndex)
			m_pGameInstance->Manager_PlaySound(TEXT("M_Scarlet_Foot_Run_1.wav"), CHANNELID::EFFECT, 2.f, 1.f);
		else if (50 >= fRandomIndex)
			m_pGameInstance->Manager_PlaySound(TEXT("M_Scarlet_Foot_Run_2.wav"), CHANNELID::EFFECT, 2.f, 1.f);
		else if (75 >= fRandomIndex)
			m_pGameInstance->Manager_PlaySound(TEXT("M_Scarlet_Foot_Run_3.wav"), CHANNELID::EFFECT, 2.f, 1.f);
		else
			m_pGameInstance->Manager_PlaySound(TEXT("M_Scarlet_Foot_Run_4.wav"), CHANNELID::EFFECT, 2.f, 1.f);
	}
}

void CNaytiba::Play_HitSound()
{
	_float fRandomIndex = m_pGameInstance->Random(0.f, 100.f);
	if (4 == m_iMonsterID || 5 == m_iMonsterID)
		Play_HitStatueSound(fRandomIndex);
	else
	{
		if(50 > fRandomIndex)
			m_pGameInstance->Manager_PlaySound(TEXT("hit_common_blood_00.wav"), CHANNELID::EFFECT, 1.f, 1.f);
		else
			m_pGameInstance->Manager_PlaySound(TEXT("hit_common_blood_01.wav"), CHANNELID::EFFECT, 1.f, 1.f);
	}
}

void CNaytiba::Play_HitStatueSound(_float fRandomIndex)
{
	if (25 >= fRandomIndex)
		m_pGameInstance->Manager_PlaySound(TEXT("CPS_Monster_Statue_Impact01.wav"), CHANNELID::EFFECT, 1.f, 1.f);
	else if (50 >= fRandomIndex)
		m_pGameInstance->Manager_PlaySound(TEXT("CPS_Monster_Statue_Impact02.wav"), CHANNELID::EFFECT, 1.f, 1.f);
	else if (75 >= fRandomIndex)
		m_pGameInstance->Manager_PlaySound(TEXT("CPS_Monster_Statue_Impact03.wav"), CHANNELID::EFFECT, 1.f, 1.f);
	else
		m_pGameInstance->Manager_PlaySound(TEXT("CPS_Monster_Statue_Impact04.wav"), CHANNELID::EFFECT, 1.f, 1.f);
		

}

_bool CNaytiba::Compare_SFX_Name(const string& szSFXName)
{
	if ("Prototype_Component_Effect_Power_Yellow" == szSFXName ||
		"Prototype_Component_Effect_SheildBreak_Yellow" == szSFXName ||
		"Prototype_Component_Effect_Scarlet_Yellow" == szSFXName)
		return true;

	return false;
}

CNaytiba* CNaytiba::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CNaytiba* pNayitba = new CNaytiba(pDevice, pContext);
	if (FAILED(pNayitba->Initialize_Prototype()))
	{
		Safe_Release(pNayitba);
		MSG_BOX("Create Fail : NayitBa");
	}
	return pNayitba;
}

CGameObject* CNaytiba::Clone(void* pArg)
{
	CNaytiba* pNayitba = new CNaytiba(*this);
	if (FAILED(pNayitba->Initialize(pArg)))
	{
		Safe_Release(pNayitba);
		MSG_BOX("Clone Fail : NayitBa");
	}
	return pNayitba;
}

void CNaytiba::Free()
{
	__super::Free();

	Safe_Release(m_pDropCom);
	Safe_Release(m_pAISenceCom);
	Safe_Release(m_pAIController);
	Safe_Release(m_pTargetCom);
}