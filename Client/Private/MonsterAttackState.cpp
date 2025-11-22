#include "pch.h"
#include "MonsterAttackState.h"

#include "GameInstance.h"
#include "Nayitba.h"

CMonsterAttackState::CMonsterAttackState() :
	CState()
{
}

HRESULT CMonsterAttackState::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CMonsterAttackState::Start(void* pArg)
{
	auto pEntity = static_cast<CNayitba*>(m_pOwner);
	auto SkillList = pEntity->GetMonsterData().iAttackList;

	// 랜덤 기본 로직
	// 이러면 무조건 랜덤으로 돌아감
	_uint iNumSkill = (_uint)SkillList.size();
	_uint iSKillIndex = (_uint)m_pGameInstance->Random(0.f, iNumSkill);

	m_pSkillData = SkillList[iSKillIndex];
	pEntity->Set_Animation(m_pSkillData->szAnimationName, false);
}

void CMonsterAttackState::Update(_float fTimeDelta)
{
	// 여기서 공격 분기
	auto pEntity = static_cast<CNayitba*>(m_pOwner);
	m_bIsFinished = pEntity->Play_Animation(fTimeDelta);
}

void CMonsterAttackState::End()
{
}

CMonsterAttackState* CMonsterAttackState::Create(void* pArg)
{
	CMonsterAttackState* pMonsterAttackState = new CMonsterAttackState();
	if (FAILED(pMonsterAttackState->Initialize(pArg)))
	{
		Safe_Release(pMonsterAttackState);
		MSG_BOX("Create Fail : Monster Attack State");
	}
	return pMonsterAttackState;
}

void CMonsterAttackState::Free()
{
	__super::Free();
}
