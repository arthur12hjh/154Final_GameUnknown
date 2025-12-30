#include "pch.h"
#include "MonsterSuccessActionState.h"

#include "GameInstance.h"
#include "MonsterAttackState.h"
#include "Nayitba.h"

CMonsterSuccessActionState::CMonsterSuccessActionState() :
	CState()
{
	m_iStateID = 1;
}

HRESULT CMonsterSuccessActionState::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CMonsterSuccessActionState::Start(void* pArg, CState* pPreState)
{
	auto pEntity = static_cast<CNaytiba*>(m_pOwner);
	CMonsterAttackState* pAttackState = dynamic_cast<CMonsterAttackState*>(pPreState);
	if (pAttackState)
	{
		m_pSkillData = pAttackState->GetSkillData();
		m_szAnimationName = m_pSkillData->szAnimationName;
		m_szAnimationName += "Success";
	}

	pEntity->Set_Animation(m_szAnimationName.c_str(), false, 1.3f);
	m_bIsEnableChange = false;
}

void CMonsterSuccessActionState::Update(_float fTimeDelta)
{
	// 여기서 공격 분기
	auto pEntity = static_cast<CNaytiba*>(m_pOwner);

	m_bIsFinished = m_bIsFinished = pEntity->Play_Animation(fTimeDelta);

	if (m_bIsFinished)
	{
		m_bIsEnableChange = true;
		m_bIsFinished = true;
	}
}

void CMonsterSuccessActionState::End()
{

}

CMonsterSuccessActionState* CMonsterSuccessActionState::Create(void* pArg)
{
	CMonsterSuccessActionState* pSuccessActionState = new CMonsterSuccessActionState();
	if (FAILED(pSuccessActionState->Initialize(pArg)))
	{
		Safe_Release(pSuccessActionState);
		MSG_BOX("Create Fail : Success Action State");
	}
	return pSuccessActionState;
}

void CMonsterSuccessActionState::Free()
{
	__super::Free();
}
