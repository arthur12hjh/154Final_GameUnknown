#include "pch.h"
#include "Task_ScarletHit.h"

#include "GameInstance.h"
#include "BossBlackBoard.h"
#include "BehaviorTree.h"
#include "Nayitba.h"

CTask_ScarletHit::CTask_ScarletHit() : CTask()
{
}

HRESULT CTask_ScarletHit::Initialize_Prototype(CBehaviorTree* pOwnerTree)
{
	if (FAILED(__super::Initialize_Prototype(pOwnerTree)))
		return E_FAIL;

	if (nullptr == m_pOwner)
		m_pOwner = static_cast<CNaytiba*>(m_pOwnerTree->GetOwner());

	if (nullptr == m_pBlackBoard)
		m_pBlackBoard = static_cast<CBossBlackBoard*>(m_pOwnerTree->GetBlackBoard());


	return S_OK;
}

CBehaviorNode::NODE_STATE CTask_ScarletHit::Update(_float fTimeDelta)
{
	// 이거 비헤비어 밖에서 데미지 처리가 이루어지는데 
	// 비헤비어 안에서 애니메이션을 재생해야하네?
	// 아니지 이거 BlackBoard 접근해서 처리하자
	auto pTarget = m_pBlackBoard->GetTarget();
	if (nullptr == pTarget)
		return NODE_STATE::FAIL;

	auto pHit_Data = m_pBlackBoard->GetHitData();
	if (pHit_Data)
	{
		m_pHit_Data = pHit_Data;
		Refresh_HitMotion();
	}

	if (false == m_pBlackBoard->bIsExcution())
	{
		if (m_pBlackBoard->IsAttackEnable() || nullptr == m_pHit_Data)
		{
			m_pHit_Data = nullptr;
			m_pSkill_Data = nullptr;

			if (CBossBlackBoard::BOSS_STATE::HIT == m_pBlackBoard->GetCurState())
				m_pBlackBoard->SetCurState(CBossBlackBoard::BOSS_STATE::IDLE);
			return NODE_STATE::FAIL;
		}
	}
	m_fAnimSpeed = 1.f;

	_float fAnimationRatio = m_pOwner->Get_AnimationRatio();
	Hit_Reaction(fTimeDelta);
	
	if (m_pOwner->Play_Animation(fTimeDelta * m_fAnimSpeed))
	{
		m_pHit_Data = nullptr;
		m_pSkill_Data = nullptr;

		if (m_bIsHitRepulse)
		{
			m_bIsHitRepulse = false;
			m_pOwner->Set_Animation("Result_State_Groggy_E", false, 1.f, 0.12f);
			return NODE_STATE::RUNNING;
		}
		else
		{
			if (m_pBlackBoard->bIsExcution())
			{
				m_pOwner->RecoveryPoint(RECOVERY_TYPE::RECOVERY_STEMINA);
				m_pBlackBoard->EnterExcution(NAYITBA_EXECUTION_TYPE::END);
			}
		}

		if (m_pBlackBoard->UnconditionallyAttack())
			m_pBlackBoard->SetAttackDelay(0.f);

		m_pBlackBoard->SetCurState(CBossBlackBoard::BOSS_STATE::IDLE);
		return NODE_STATE::COMPLETE;
	}
	else if (false == m_pBlackBoard->bIsExcution())
	{
		if (0.5f >= fAnimationRatio)
			m_pOwner->GetTransform()->Move_Direction(fTimeDelta, XMLoadFloat3(&m_vImpactDir), m_fImpactForce);

	}
	
	return NODE_STATE::RUNNING;
}

void CTask_ScarletHit::Refresh_HitMotion()
{
	_vector vOwnerPos = m_pOwner->GetTransform()->Get_State(STATE::POSITION);
	_vector vAttackerPos = m_pBlackBoard->GetTarget()->GetTransform()->Get_State(STATE::POSITION);

	vAttackerPos.m128_f32[1] = vOwnerPos.m128_f32[1] = 0.f;
	_vector vDir = XMVector3Normalize(vAttackerPos - vOwnerPos);

	auto pSkill_Data = static_cast<const CHARACTER_SKILL_DESC *>(m_pHit_Data->pSkillData);
	Damaged_Attack(pSkill_Data, vDir);
	
	m_pBlackBoard->SetAttackData(nullptr);
	m_pBlackBoard->SetHitData(nullptr);
}

void CTask_ScarletHit::Play_Sound(SKILL_TYPE eSkillType)
{
	_float fRandomIndex = m_pGameInstance->Random(0.f, 100.f);
	if (SKILL_TYPE::MIMESIS_SKILL >= eSkillType || SKILL_TYPE::SECOND_PHASE_SKILL == eSkillType)
	{
		if (20.f >= fRandomIndex)
			m_pGameInstance->Manager_PlaySound(TEXT("vo_Scarlet_Dmg_S_1_VO.wav"), CHANNELID::EFFECT, 3.f);
		else if (40.f >= fRandomIndex)
			m_pGameInstance->Manager_PlaySound(TEXT("vo_Scarlet_Dmg_S_2_VO.wav"), CHANNELID::EFFECT, 3.f);
		else if (60.f >= fRandomIndex)
			m_pGameInstance->Manager_PlaySound(TEXT("vo_Scarlet_Dmg_S_3_VO.wav"), CHANNELID::EFFECT, 3.f);
		else if (80.f >= fRandomIndex)
			m_pGameInstance->Manager_PlaySound(TEXT("vo_Scarlet_Dmg_S_4_VO.wav"), CHANNELID::EFFECT, 3.f);
		else
			m_pGameInstance->Manager_PlaySound(TEXT("vo_Scarlet_Dmg_S_5_VO.wav"), CHANNELID::EFFECT, 3.f);
	}
	else if (SKILL_TYPE::BLINK_SKILL >= eSkillType)
	{
		if (20.f >= fRandomIndex)
			m_pGameInstance->Manager_PlaySound(TEXT("vo_Scarlet_Dmg_M_1_VO.wav"), CHANNELID::EFFECT, 3.f);
		else if (40.f >= fRandomIndex)
			m_pGameInstance->Manager_PlaySound(TEXT("vo_Scarlet_Dmg_M_2_VO.wav"), CHANNELID::EFFECT, 3.f);
		else if (60.f >= fRandomIndex)
			m_pGameInstance->Manager_PlaySound(TEXT("vo_Scarlet_Dmg_M_3_VO.wav"), CHANNELID::EFFECT, 3.f);
		else if (80.f >= fRandomIndex)
			m_pGameInstance->Manager_PlaySound(TEXT("vo_Scarlet_Dmg_M_4_VO.wav"), CHANNELID::EFFECT, 3.f);
		else
			m_pGameInstance->Manager_PlaySound(TEXT("vo_Scarlet_Dmg_M_5_VO.wav"), CHANNELID::EFFECT, 3.f);
	}
}

void CTask_ScarletHit::Hit_Reaction(_float fTimeDelta)
{
	if (nullptr == m_pSkill_Data)
		return;

	_float fAnimRatio = m_pOwner->Get_AnimationRatio();
	switch (m_pSkill_Data->iSkillID)
	{
	case 1010 :
		if(0.62f >= fAnimRatio)
			m_fAnimSpeed = 3.f;
		
		if (0.62f <= fAnimRatio && fAnimRatio <= 0.82f)
		{
			m_pOwner->GetTransform()->Move_Direction(fTimeDelta,
				m_pOwner->GetTransform()->Get_State(STATE::LOOK) * -1.f, 10.f);

			m_fAnimSpeed = 2.f;
		}
		break;
	}
}

void CTask_ScarletHit::Damaged_Attack(const CHARACTER_SKILL_DESC* pData, _vector vDir)
{
	m_pSkill_Data = pData;
	if (!strcmp(pData->szHitAnimationName, "None"))
	{
		_float fScalar = XMVectorGetX(XMVector3Dot(m_pOwner->GetTransform()->Get_State(STATE::LOOK), vDir));
		if (0 <= fScalar)
		{
			switch (pData->eATK_Direction)
			{
			case ATTACK_DIRECTION::ATK_LEFT:
				m_szAnimationName = "Result_Hit_Stand_Light_Fw_Lw";
				break;
			case ATTACK_DIRECTION::ATK_RIGHT:
				m_szAnimationName = "Result_Hit_Stand_Light_Fw_Rw";
				break;
			default:
				m_szAnimationName = "Result_Hit_Stand_Light_Fw";
				break;
			}
		}
		else
			m_szAnimationName = "Result_Hit_Stand_Light_Bw";

		XMStoreFloat3(&m_vImpactDir, -1.f * vDir);
		m_fImpactForce = 3.f;
	}
	else
	{
		if (SKILL_TYPE::REPULSE_SKILL != pData->eSkillType)
		{
			XMStoreFloat3(&m_vImpactDir, XMVectorZero());
			m_fImpactForce = 0.f;

			if (SKILL_PROPERTY::EXCUTION & pData->eProPerty)
			{
				m_szAnimationName = m_pBlackBoard->GetBossDefaultInfo()->szAnimationName;
				m_szAnimationName += "_";
				m_szAnimationName += pData->szHitAnimationName;
			}
			else
				m_szAnimationName = pData->szHitAnimationName;
		}
		else
		{
			m_bIsHitRepulse = true;

			XMStoreFloat3(&m_vImpactDir, -1.f * vDir);
			m_fImpactForce = 10.f;
			m_szAnimationName = "Result_State_Groggy_S";
		}
	}

	Play_Sound(m_pSkill_Data->eSkillType);
	if (m_pBlackBoard->bIsExcution())
		m_pOwner->Set_Animation(m_szAnimationName.c_str(), false, 1.f, 0.12f);
	else
		m_pOwner->Set_Animation(m_szAnimationName.c_str(), false, 1.f, 0.12f, true);
}

CTask_ScarletHit* CTask_ScarletHit::Create(CBehaviorTree* pOwnerTree)
{
	CTask_ScarletHit* pTask_Hit = new CTask_ScarletHit();
	if (FAILED(pTask_Hit->Initialize_Prototype(pOwnerTree)))
	{
		Safe_Release(pTask_Hit);
		MSG_BOX("Create Fail : Task Scarlet Hit");
	}
	return pTask_Hit;
}

void CTask_ScarletHit::Free()
{
	__super::Free();

	Safe_Release(m_pBlackBoard);
}
