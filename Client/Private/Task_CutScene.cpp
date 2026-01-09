#include "pch.h"
#include "Task_CutScene.h"

#include "GameInstance.h"
#include "BehaviorTree.h"
#include "ScarletBlackBoard.h"
#include "Nayitba.h"

#include "GameManager.h"

CTask_CutScene::CTask_CutScene() : CTask()
{
}

HRESULT CTask_CutScene::Initialize_Prototype(CBehaviorTree* pOwnerTree)
{
    if (FAILED(__super::Initialize_Prototype(pOwnerTree)))
        return E_FAIL;

	m_pNaytiba = static_cast<CNaytiba*>(m_pOwnerTree->GetOwner());
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
	_uint iCurrentPhaseIndex = ENUM_CLASS(ePhase);

	if (false == m_pBlackBoard->Is_PlayPhaseChangeCutScene())
	{
		auto pGameManger = CGameManager::GetInstance();
		m_pBlackBoard->Set_PlayCutScene();
		switch (iCurrentPhaseIndex)
		{
		case 0:
			if (8 == m_pNaytiba->GetMonsterID())
			{
				m_pGameInstance->Manager_PlayBGM(TEXT("BGM_BOSS_SCARLET_PHASECHANGE.wav"), 3.f);
				pGameManger->Play_Cinematic(142, [&]() { FinishedCutScene(); });

			}
			else if (1 == m_pNaytiba->GetMonsterID())
			{
				m_pGameInstance->Manager_PlayBGM(TEXT("BGM_XION_BOSS_RAVENBEAST_FINISH.wav"), 3.f);
				pGameManger->Play_Cinematic(125, [&]() { FinishedCutScene(); });
			}
			break;
		case 1:
			m_pGameInstance->Manager_PlayBGM(TEXT("BGM_BOSS_SCARLET_FINISH.wav"), 3.f);
			if (8 == m_pNaytiba->GetMonsterID())
				pGameManger->Play_Cinematic(143, [&]() { FinishedCutScene(); });
			
			break;
		}
	}

	if (m_bIsPlayEnd)
	{
		m_bIsPlayEnd = false;
		return NODE_STATE::COMPLETE;
	}

	return NODE_STATE::RUNNING;
}

void CTask_CutScene::FinishedCutScene()
{
	CScarletBlackBoard::BOSS_PAHSE ePhase = m_pBlackBoard->Get_BossPhase();
	_uint iCurrentPhaseIndex = ENUM_CLASS(ePhase);

	auto ePreState = m_pBlackBoard->GetPreState();
	// 대충 컷씬이 끝나면 여기서 End 함수를 호출해서 할거임 지금은 그냥 Idle로만 바꿀예정
	if (m_pBlackBoard->IsLastPhase())
	{
		if(1 == m_pNaytiba->GetMonsterID())
			m_pNaytiba->Excution();
		else
			m_pNaytiba->Set_Dead(true);
	}
	else
	{
		m_pBlackBoard->Set_BossPhase(CScarletBlackBoard::BOSS_PAHSE(iCurrentPhaseIndex + 1));
		m_pBlackBoard->SetCurState(CScarletBlackBoard::BOSS_STATE::IDLE);
		m_pBlackBoard->SetEntarnceAttack(true);

		CGameManager::GetInstance()->Play_BossBGM(m_pNaytiba->GetMonsterID(), iCurrentPhaseIndex + 1);
		if (CBossBlackBoard::BOSS_STATE::GROGGY == ePreState)
			m_pNaytiba->RecoveryPoint(RECOVERY_TYPE::RECOVERY_STEMINA);
		m_pNaytiba->RecoveryPoint(RECOVERY_TYPE::RECOVERY_SHILED);
		m_pBlackBoard->Reset_State();
	}


	m_bIsPlayEnd = true;
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
