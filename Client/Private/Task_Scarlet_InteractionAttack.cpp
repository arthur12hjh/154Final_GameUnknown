#include "pch.h"
#include "Task_Scarlet_InteractionAttack.h"

#include "GameManager.h"
#include "GameInstance.h"
#include "ScarletBlackBoard.h"
#include "ScarletBehaviorTree.h"
#include "Nayitba.h"

CTask_Scarlet_InteractionAttack::CTask_Scarlet_InteractionAttack() : CTask()
{
}

HRESULT CTask_Scarlet_InteractionAttack::Initialize_Prototype(CBehaviorTree* pOwnerTree)
{
	if (FAILED(__super::Initialize_Prototype(pOwnerTree)))
		return E_FAIL;

	if (nullptr == m_pOwner)
		m_pOwner = static_cast<CNayitba*>(m_pOwnerTree->GetOwner());

	if (nullptr == m_pBlackBoard)
		m_pBlackBoard = static_cast<CScarletBlackBoard*>(m_pOwnerTree->GetBlackBoard());

	m_pGameManager = CGameManager::GetInstance();
	Safe_AddRef(m_pGameManager);
	return S_OK;
}

CBehaviorNode::NODE_STATE CTask_Scarlet_InteractionAttack::Update(_float fTimeDelta)
{
	_bool bIsTaskFinished = false;
	CBossBlackBoard::BOSS_STATE eCurState = m_pBlackBoard->GetCurState();
	CBossBlackBoard::BOSS_STATE ePreState = m_pBlackBoard->GetPreState();
	
	if (CBossBlackBoard::BOSS_STATE::INTERACTION_ATTACK == eCurState)
	{
		if (eCurState != ePreState)
			SelectAnimationData();
	}
	else
		return NODE_STATE::FAIL;

	_bool bIsFinished = m_pOwner->Play_Animation(fTimeDelta * m_fAnimationSpeed);
	if (bIsFinished)
	{
		m_pBlackBoard->SetCurState(CBossBlackBoard::BOSS_STATE::IDLE);
		m_pBlackBoard->SetAttackData(nullptr);

		m_pBlackBoard->ClearAttackTimer();
		m_pBlackBoard->SetAttackDelay(m_pGameInstance->Random(1.5f, 2.5f));
		return NODE_STATE::COMPLETE;
	}

	return NODE_STATE::RUNNING;
}

_bool CTask_Scarlet_InteractionAttack::SelectAnimationData()
{
	auto pSkillData = m_pBlackBoard->GetAttackData();

	if (nullptr == pSkillData)
		return false;

	const CHARACTER_SKILL_DESC* pSkillDesc = nullptr;
	if (42 == pSkillData->iSkillID)
	{
		pSkillDesc = m_pGameManager->Find_SkillData(43);
	}
	else if (37 == pSkillData->iSkillID)
	{
		// 이건 나중에 컷씬재생 이런식으로 가자
	}

	if (pSkillDesc)
	{
		m_pBlackBoard->SetCurState(CBossBlackBoard::BOSS_STATE::INTERACTION_ATTACK);
		m_pOwner->Set_Animation(pSkillDesc->szAnimationName, false, 1.f, 0.12f, true);
	}
	else
		return false;

	return true;
}

CTask_Scarlet_InteractionAttack* CTask_Scarlet_InteractionAttack::Create(CBehaviorTree* pOwnerTree)
{
	CTask_Scarlet_InteractionAttack* pTask_Scarlet_InteractionAttack = new CTask_Scarlet_InteractionAttack();
	if (FAILED(pTask_Scarlet_InteractionAttack->Initialize_Prototype(pOwnerTree)))
	{
		Safe_Release(pTask_Scarlet_InteractionAttack);
		MSG_BOX("Create Fail : Task Scarlet InteractionAttack");
	}
	return pTask_Scarlet_InteractionAttack;
}

void CTask_Scarlet_InteractionAttack::Free()
{
	__super::Free();

	Safe_Release(m_pBlackBoard);
}
