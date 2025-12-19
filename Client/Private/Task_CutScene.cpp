#include "pch.h"
#include "Task_CutScene.h"

#include "BehaviorTree.h"
#include "ScarletBlackBoard.h"
#include "Nayitba.h"

CTask_CutScene::CTask_CutScene() : CTask()
{
}

HRESULT CTask_CutScene::Initialize_Prototype(CBehaviorTree* pOwnerTree)
{
    if (FAILED(__super::Initialize_Prototype(pOwnerTree)))
        return E_FAIL;

	m_pNaytiba = static_cast<CNayitba*>(m_pOwnerTree->GetOwner());
	auto pNaytibaInitData = m_pNaytiba->GetStaticMonsterData();

	m_pBlackBoard = static_cast<CScarletBlackBoard*>(pOwnerTree->GetBlackBoard());
	return S_OK;
}

CBehaviorNode::NODE_STATE CTask_CutScene::Update(_float fTimeDelta)
{
	// 여기서 현재 컷씬에 맞는 데이터를 받아와서
	// 플레이어랑 재생한다.

	// 나중에 컷씬 데이터로 수정 하고 컷씬의 종료 이벤트를 받아서
	// 컷씬의 종료를 판단하겠습니다.
	// 
	// 이거 그냥 그런가보다 하시면 됩니다.
	CScarletBlackBoard::BOSS_PAHSE ePhase = m_pBlackBoard->Get_BossPhase();
	_uint iCrrentPhaseIndex = ENUM_CLASS(ePhase);

	// 나중에 수정할 코드
	WCHAR szMessage[MAX_PATH] = {};
	wsprintf(szMessage, TEXT("%d 번째 컷씬 재생중입니다"), iCrrentPhaseIndex);
	MSG_BOX_TEXT(szMessage);

	// 대충 컷씬이 끝나면 여기서 End 함수를 호출해서 할거임 지금은 그냥 Idle로만 바꿀예정
	m_pBlackBoard->Set_BossPhase(CScarletBlackBoard::BOSS_PAHSE(iCrrentPhaseIndex + 1));

	m_pBlackBoard->SetCurState(CScarletBlackBoard::BOSS_STATE::IDLE);
	m_pBlackBoard->SetEntarnceAttack(true);
	m_pNaytiba->RecoveryPoint(RECOVERY_TYPE::RECOVERY_SHILED);
	m_pBlackBoard->Reset_State();

	return NODE_STATE::COMPLETE;
	//return NODE_STATE::RUNNING;
}

CTask_CutScene* CTask_CutScene::Create(CBehaviorTree* pOwnerTree)
{
	CTask_CutScene* pCut_Scene = new CTask_CutScene();
	if (FAILED(pCut_Scene->Initialize_Prototype(pOwnerTree)))
	{
		Safe_Release(pCut_Scene);
		MSG_BOX("Create Fail : Cut Scene");
	}
	return pCut_Scene;
}

void CTask_CutScene::Free()
{
	__super::Free();
	
	Safe_Release(m_pBlackBoard);
}
