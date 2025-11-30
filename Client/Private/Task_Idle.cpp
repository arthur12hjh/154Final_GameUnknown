#include "pch.h"
#include "Task_Idle.h"

#include "GameInstance.h"
#include "BossBlackBoard.h"
#include "GorillaBehaviorTree.h"
#include "Nayitba.h"

CTask_Idle::CTask_Idle() : CTask()
{
}

HRESULT CTask_Idle::Initialize_Prototype(CBehaviorTree* pOwnerTree)
{
	if (FAILED(__super::Initialize_Prototype(pOwnerTree)))
		return E_FAIL;

	if (nullptr == m_pOwner)
		m_pOwner = static_cast<CNayitba*>(m_pOwnerTree->GetOwner());

	if (nullptr == m_pBlackBoard)
		m_pBlackBoard = static_cast<CBossBlackBoard*>(m_pOwnerTree->GetBlackBoard());

	return S_OK;
}

CBehaviorNode::NODE_STATE CTask_Idle::Update(_float fTimeDelta)
{
	// 여기서 블렉보드 또는 다른곳의 상태가 바뀌면 Complete 호출해서 사용
	string szAnimName = m_pBlackBoard->GetBossDefaultInfo()->szAnimationName;
	m_pBlackBoard->SetCurState(CBossBlackBoard::BOSS_STATE::IDLE);

	szAnimName += "_BattleIdle01";
	m_pOwner->Set_Animation(szAnimName.c_str(), true);
	m_pOwner->Play_Animation(fTimeDelta);

	return NODE_STATE::COMPLETE;
}

CTask_Idle* CTask_Idle::Create(CBehaviorTree* pOwnerTree)
{
	CTask_Idle* pTask_Idle = new CTask_Idle();
	if (FAILED(pTask_Idle->Initialize_Prototype(pOwnerTree)))
	{
		Safe_Release(pTask_Idle);
		MSG_BOX("Create Fail : Task Idle");
	}
	return pTask_Idle;
}

void CTask_Idle::Free()
{
	__super::Free();

	Safe_Release(m_pBlackBoard);
}
