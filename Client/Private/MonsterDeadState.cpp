#include "pch.h"
#include "MonsterDeadState.h"

#include "GameInstance.h"
#include "GameStruct.h"
#include "Nayitba.h"

CMonsterDeadState::CMonsterDeadState() :
	CState()
{
}

HRESULT CMonsterDeadState::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CMonsterDeadState::Start(void* pArg, CState* pPreState)
{
	auto pEntity = static_cast<CNayitba*>(m_pOwner);
	auto pDesc = static_cast<DEFAULT_DAMAGE_DESC*>(pArg);
	pEntity->GetStaticMonsterData();
	m_pLastHitSkill = static_cast<CHARACTER_SKILL_DESC*>(pDesc->pSkillData);
	// 이거 공격한 대상이랑 외적으로 하든 내적으로하든 앞뒤 판단해서 
	// 이름 더해주자
	if (nullptr == pDesc->pAttacker)
	{
		m_bIsFinished = true;
		MSG_BOX("Not Bind Attacker");
		return;
	}

	_vector vOwnerPos = m_pOwner->GetTransform()->Get_State(STATE::POSITION);
	_vector vAttackerPos = pDesc->pAttacker->GetTransform()->Get_State(STATE::POSITION);
	if (SKILL_TYPE::BETA_SKILL == m_pLastHitSkill->eSkillType)
	{
		m_szAnimationName = "Result_State_KnockDown_S";
		_vector vDir = XMVector3Normalize(vAttackerPos - vOwnerPos);
		_float fScalar = XMVectorGetX(XMVector3Dot(m_pOwner->GetTransform()->Get_State(STATE::LOOK), vDir));
		if (0 <= fScalar)
			m_szAnimationName += "_Bw";
		else
			m_szAnimationName += "_Fw";
		pEntity->Set_Animation(m_szAnimationName.c_str(), false);
	}
	else
	{
		m_szAnimationName = "Result_State_Groggy_S";
		pEntity->Set_Animation(m_szAnimationName.c_str(), false);
	}

	
	m_fDeadEndTime.y = 3.f;
}

void CMonsterDeadState::Update(_float fTimeDelta)
{
	auto pEntity = static_cast<CNayitba*>(m_pOwner);

	_bool bIsFinished = pEntity->Play_Animation(fTimeDelta);
	if (0 == m_iSectionIndex)
	{
		if (bIsFinished)
		{
			m_szAnimationName = "Result_State_KnockDown_L";
			pEntity->Set_Animation(m_szAnimationName.c_str(), true, 1.f, 0.4f);
			m_iSectionIndex++;
		}
	}
	else
	{
		m_fDeadEndTime.x += fTimeDelta;
		if (m_fDeadEndTime.x >= m_fDeadEndTime.y)
		{
			// 디졸브 대충 시간 기다리기
			m_pOwner->Set_Dead(true);
		}
	}
}

void CMonsterDeadState::End()
{
	m_iSectionIndex = 0;
	m_pNaytibaData = nullptr;
	m_pLastHitSkill = nullptr;
	m_bIsFinished = false;
}

CMonsterDeadState* CMonsterDeadState::Create(void* pArg)
{
	CMonsterDeadState* pMonsterDeadState = new CMonsterDeadState();
	if (FAILED(pMonsterDeadState->Initialize(pArg)))
	{
		Safe_Release(pMonsterDeadState);
		MSG_BOX("Create Fail : Monster Dead State");
	}

	return pMonsterDeadState;
}

void CMonsterDeadState::Free()
{
	__super::Free();
}
