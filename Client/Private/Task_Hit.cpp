#include "pch.h"
#include "Task_Hit.h"

#include "GameInstance.h"
#include "BossBlackBoard.h"
#include "BehaviorTree.h"
#include "Nayitba.h"

CTask_Hit::CTask_Hit() : CTask()
{
}

HRESULT CTask_Hit::Initialize_Prototype(CBehaviorTree* pOwnerTree)
{
	if (FAILED(__super::Initialize_Prototype(pOwnerTree)))
		return E_FAIL;

	if (nullptr == m_pOwner)
		m_pOwner = static_cast<CNayitba*>(m_pOwnerTree->GetOwner());

	if (nullptr == m_pBlackBoard)
		m_pBlackBoard = static_cast<CBossBlackBoard*>(m_pOwnerTree->GetBlackBoard());


	return S_OK;
}

CBehaviorNode::NODE_STATE CTask_Hit::Update(_float fTimeDelta)
{
	// 이거 비헤비어 밖에서 데미지 처리가 이루어지는데 
	// 비헤비어 안에서 애니메이션을 재생해야하네?
	// 아니지 이거 BlackBoard 접근해서 처리하자
	auto pHit_Data = m_pBlackBoard->GetHitData();
	if (pHit_Data)
	{
		m_pHit_Data = pHit_Data;
		Refresh_HitMotion();
	}

	if (false == m_pBlackBoard->bIsExcution())
	{
		if (nullptr == m_pHit_Data || m_pBlackBoard->IsAttackEnable())
		{
			// 여기서 피격 데이터가 Nullptr 이거나 공격중에 특정 무시속성이 달려있는지
			// 확인하고 EFail;
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
		if (m_pBlackBoard->bIsExcution())
			m_pBlackBoard->EnterExcution(false);

		return NODE_STATE::COMPLETE;
	}
	else if (false == m_pBlackBoard->bIsExcution())
	{
		if (0.5f >= fAnimationRatio)
			m_pOwner->GetTransform()->Move_Direction(fTimeDelta, XMLoadFloat3(&m_vImpactDir), m_fImpactForce);

	}
	
	return NODE_STATE::RUNNING;
}

void CTask_Hit::Refresh_HitMotion()
{
	_vector vOwnerPos = m_pOwner->GetTransform()->Get_State(STATE::POSITION);
	_vector vAttackerPos = m_pBlackBoard->GetTarget()->GetTransform()->Get_State(STATE::POSITION);

	vAttackerPos.m128_f32[1] = vOwnerPos.m128_f32[1] = 0.f;
	_vector vDir = XMVector3Normalize(vAttackerPos - vOwnerPos);

	auto pSkill_Data = static_cast<const CHARACTER_SKILL_DESC *>(m_pHit_Data->pSkillData);
	if (!strcmp(pSkill_Data->szHitAnimationName, "None"))
	{
		_float fScalar = XMVectorGetX(XMVector3Dot(m_pOwner->GetTransform()->Get_State(STATE::LOOK), vDir));
		if (0 <= fScalar)
		{
			switch (pSkill_Data->eATK_Direction)
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
		XMStoreFloat3(&m_vImpactDir, XMVectorZero());
		m_fImpactForce = 0.f;
		m_szAnimationName = pSkill_Data->szHitAnimationName;
	}

	m_pOwner->Set_Animation(m_szAnimationName.c_str(), false, 1.f, 0.12f, true);
	m_pBlackBoard->SetAttackData(nullptr);
	m_pBlackBoard->SetHitData(nullptr);
}

CTask_Hit* CTask_Hit::Create(CBehaviorTree* pOwnerTree)
{
	CTask_Hit* pTask_GorillaHit = new CTask_Hit();
	if (FAILED(pTask_GorillaHit->Initialize_Prototype(pOwnerTree)))
	{
		Safe_Release(pTask_GorillaHit);
		MSG_BOX("Create Fail : Task Gorilla Hit");
	}
	return pTask_GorillaHit;
}

void CTask_Hit::Free()
{
	__super::Free();

	Safe_Release(m_pBlackBoard);
}
