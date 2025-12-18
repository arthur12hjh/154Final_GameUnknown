#include "pch.h"
#include "Deco_BossPhase.h"

#include "BehaviorTree.h"
#include "BossBlackBoard.h"
#include "Nayitba.h"

CDeco_BossPhase::CDeco_BossPhase() : CDecorator()
{
}

HRESULT CDeco_BossPhase::Initialize_Prototype(CBehaviorTree* pOwnerTree, const vector<pair<_float, _bool>>& ChangePhaseRatio)
{
	if (FAILED(__super::Initialize_Prototype(pOwnerTree)))
		return E_FAIL;

	auto pNaytiba = static_cast<CNayitba*>(m_pOwnerTree->GetOwner());
	auto pNaytibaInitData = pNaytiba->GetStaticMonsterData();

	m_iBossMaxHealth = pNaytibaInitData->iMaxHealth;
	m_iNumPhases = pNaytibaInitData->iNumPhase;

	m_pBlackBoard = static_cast<CBossBlackBoard*>(pOwnerTree->GetBlackBoard());
	m_ChangePahseRatio.reserve(m_iNumPhases);
	m_ChangePahseRatio = ChangePhaseRatio;
	return S_OK;
}

CBehaviorNode::NODE_STATE CDeco_BossPhase::Update(_float fTimeDelta)
{
	// 여기서 페이즈 체력 보고 다음 페이즈로
	CBossBlackBoard::BOSS_PAHSE ePhase = m_pBlackBoard->Get_BossPhase();
	CBossBlackBoard::BOSS_STATE eBossState = m_pBlackBoard->GetCurState();
	_uint iCurrentPhaseIndex = ENUM_CLASS(ePhase);

	if (iCurrentPhaseIndex >= m_iNumPhases)
		return NODE_STATE::FAIL;

	if (CBossBlackBoard::BOSS_STATE::CUTSCENE != eBossState)
	{
		_float CurrentHealthRatio = (_float)m_pBlackBoard->GetBossInfo()->iCurrentHealth / (_float)m_iBossMaxHealth;
		if (CurrentHealthRatio <= m_ChangePahseRatio[iCurrentPhaseIndex].first)
		{
			// 페이즈 전환 컷씬 재생
			if (false == m_ChangePahseRatio[iCurrentPhaseIndex].second)
			{
				// 컷씬끝나고 페이즈 세팅해야할거같음
				//m_pBlackBoard->Set_BossPhase(CBossBlackBoard::BOSS_PAHSE(iCurrentPhaseIndex + 1));

				m_pBlackBoard->SetCurState(CBossBlackBoard::BOSS_STATE::CUTSCENE);
				m_ChangePahseRatio[iCurrentPhaseIndex].second = true;
			}
		}
	}

	// 컷씬이 재생되어야하고 처음 페이즈가 바뀌었다면 이때 재생해야할것
	if (CBossBlackBoard::BOSS_STATE::CUTSCENE == m_pBlackBoard->GetCurState())
		return NODE_STATE::COMPLETE;

	return NODE_STATE::FAIL;
}

CDeco_BossPhase* CDeco_BossPhase::Create(CBehaviorTree* pOwnerTree, const vector<pair<_float, _bool>>& ChangePhaseRatio)
{
	CDeco_BossPhase* pPhaseChange = new CDeco_BossPhase();
	if (FAILED(pPhaseChange->Initialize_Prototype(pOwnerTree, ChangePhaseRatio)))
	{
		Safe_Release(pPhaseChange);
		MSG_BOX("Create Fail : Phase Change");
	}
	return pPhaseChange;
}

void CDeco_BossPhase::Free()
{
	__super::Free();

	Safe_Release(m_pBlackBoard);
}
