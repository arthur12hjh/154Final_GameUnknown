#include "pch.h"
#include "Task_GorillaHit.h"

#include "GameInstance.h"
#include "BossBlackBoard.h"
#include "BehaviorTree.h"
#include "Nayitba.h"

CTask_GorillaHit::CTask_GorillaHit() : CTask()
{
}

HRESULT CTask_GorillaHit::Initialize_Prototype(CBehaviorTree* pOwnerTree)
{
	if (FAILED(__super::Initialize_Prototype(pOwnerTree)))
		return E_FAIL;

	if (nullptr == m_pOwner)
		m_pOwner = static_cast<CNaytiba*>(m_pOwnerTree->GetOwner());

	if (nullptr == m_pBlackBoard)
		m_pBlackBoard = static_cast<CBossBlackBoard*>(m_pOwnerTree->GetBlackBoard());


	return S_OK;
}

CBehaviorNode::NODE_STATE CTask_GorillaHit::Update(_float fTimeDelta)
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
		if (m_pBlackBoard->IsPhaseLastAttack() || m_pBlackBoard->IsAttackEnable() || nullptr == m_pHit_Data)
		{
			m_pHit_Data = nullptr;
			if (CBossBlackBoard::BOSS_STATE::HIT == m_pBlackBoard->GetCurState())
				m_pBlackBoard->SetCurState(CBossBlackBoard::BOSS_STATE::IDLE);
			return NODE_STATE::FAIL;
		}
	}

	_float fAnimationRatio = m_pOwner->Get_AnimationRatio();

	if (m_pOwner->Play_Animation(fTimeDelta))
	{
		m_pHit_Data = nullptr;
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

void CTask_GorillaHit::Refresh_HitMotion()
{
	_vector vOwnerPos = m_pOwner->GetTransform()->Get_State(STATE::POSITION);
	_vector vAttackerPos = m_pBlackBoard->GetTarget()->GetTransform()->Get_State(STATE::POSITION);

	vAttackerPos.m128_f32[1] = vOwnerPos.m128_f32[1] = 0.f;
	_vector vDir = XMVector3Normalize(vAttackerPos - vOwnerPos);

	auto pSkill_Data = static_cast<const CHARACTER_SKILL_DESC*>(m_pHit_Data->pSkillData);
	Damaged_Attack(pSkill_Data, vDir);

	m_pBlackBoard->SetAttackData(nullptr);
	m_pBlackBoard->SetHitData(nullptr);
}

void CTask_GorillaHit::Damaged_Attack(const Character_Skill_Desc* pData, _vector vDir)
{
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

	if (m_pBlackBoard->bIsExcution())
		m_pOwner->Set_Animation(m_szAnimationName.c_str(), false, 1.f, 0.12f);
	else
		m_pOwner->Set_Animation(m_szAnimationName.c_str(), false, 1.f, 0.12f, true);
}

CTask_GorillaHit* CTask_GorillaHit::Create(CBehaviorTree* pOwnerTree)
{
	CTask_GorillaHit* pTask_Hit = new CTask_GorillaHit();
	if (FAILED(pTask_Hit->Initialize_Prototype(pOwnerTree)))
	{
		Safe_Release(pTask_Hit);
		MSG_BOX("Create Fail : Task Gorilla Hit");
	}
	return pTask_Hit;
}

void CTask_GorillaHit::Free()
{
	__super::Free();

	Safe_Release(m_pBlackBoard);
}
