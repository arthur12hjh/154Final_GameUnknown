#include "pch.h"
#include "Task_ScarletAttack.h"

#include "GameManager.h"
#include "GameInstance.h"
#include "BossBlackBoard.h"
#include "GorillaBehaviorTree.h"
#include "Nayitba.h"

CTask_ScarletAttack::CTask_ScarletAttack() : CTask()
{
}

HRESULT CTask_ScarletAttack::Initialize_Prototype(CBehaviorTree* pOwnerTree)
{
	if (FAILED(__super::Initialize_Prototype(pOwnerTree)))
		return E_FAIL;

	if (nullptr == m_pOwner)
		m_pOwner = static_cast<CNayitba*>(m_pOwnerTree->GetOwner());

	if (nullptr == m_pBlackBoard)
		m_pBlackBoard = static_cast<CBossBlackBoard*>(m_pOwnerTree->GetBlackBoard());

	m_pGameManager = CGameManager::GetInstance();
	Safe_AddRef(m_pGameManager);

	m_fMaxDelayTime = 3.5f;
	return S_OK;
}

CBehaviorNode::NODE_STATE CTask_ScarletAttack::Update(_float fTimeDelta)
{
	CBossBlackBoard::BOSS_STATE eCurState = m_pBlackBoard->GetCurState();
	CBossBlackBoard::BOSS_STATE ePreState = m_pBlackBoard->GetPreState();

	return NODE_STATE::RUNNING;
}

_bool CTask_ScarletAttack::SelectPattern()
{
	return _bool();
}

_bool CTask_ScarletAttack::AttackMoveAction(_float fTimeDelta)
{
	return _bool();
}

_bool CTask_ScarletAttack::Compute_AttackCoolTime(_bool bIsForce)
{
	return _bool();
}

void CTask_ScarletAttack::AttackLerpMove(_float fTimeDelta)
{
}

void CTask_ScarletAttack::AttackADDMove(_float fTimeDelta)
{
}

void CTask_ScarletAttack::LookAtPoint(_float fTimeDelta)
{
}

void CTask_ScarletAttack::ResetAttackTask(_bool bIsCoolTime)
{
}

CTask_ScarletAttack* CTask_ScarletAttack::Create(CBehaviorTree* pOwnerTree)
{
	return nullptr;
}

void CTask_ScarletAttack::Free()
{
}
