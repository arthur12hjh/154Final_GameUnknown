#include "pch.h"
#include "Task_ScarletMove.h"

#include "GameInstance.h"
#include "BossBlackBoard.h"
#include "BehaviorTree.h"
#include "Nayitba.h"

CTask_ScarletMove::CTask_ScarletMove()
{
}

HRESULT CTask_ScarletMove::Initialize_Prototype(CBehaviorTree* pOwnerTree)
{
	if (FAILED(__super::Initialize_Prototype(pOwnerTree)))
		return E_FAIL;

	if (nullptr == m_pOwner)
		m_pOwner = static_cast<CNayitba*>(m_pOwnerTree->GetOwner());

	if (nullptr == m_pBlackBoard)
		m_pBlackBoard = static_cast<CBossBlackBoard*>(m_pOwnerTree->GetBlackBoard());

	m_fSpeed = m_pBlackBoard->GetBossDefaultInfo()->fMoveSpeed;
	return S_OK;
}

CBehaviorNode::NODE_STATE CTask_ScarletMove::Update(_float fTimeDelta)
{
	CBossBlackBoard::BOSS_STATE eCurState = m_pBlackBoard->GetCurState();
	if (CBossBlackBoard::BOSS_STATE::GROGGY == eCurState)
		return NODE_STATE::COMPLETE;

	// 블랙보드에 목표 지점 또는 타겟이 있을때 이동할 녀석
	if (CBossBlackBoard::BOSS_STATE::MOVE == m_pBlackBoard->GetCurState() &&
		(m_pBlackBoard->GetCurState() == m_pBlackBoard->GetPreState()))
	{
		switch (m_eDirection)
		{
		case DIRECTION::FRONT:
			XMStoreFloat3(&m_vMoveDir, m_pOwner->GetTransform()->Get_State(STATE::LOOK));
			break;
		case DIRECTION::LEFT:
			XMStoreFloat3(&m_vMoveDir, -1 * m_pOwner->GetTransform()->Get_State(STATE::RIGHT));
			break;
		case DIRECTION::RIGHT:
			XMStoreFloat3(&m_vMoveDir, m_pOwner->GetTransform()->Get_State(STATE::RIGHT));
			break;
		case DIRECTION::BACK:
			XMStoreFloat3(&m_vMoveDir, -1 * m_pOwner->GetTransform()->Get_State(STATE::LOOK));
			break;
		}

		auto pTarget = m_pBlackBoard->GetTarget();
		if (nullptr == pTarget)
			return NODE_STATE::FAIL;

		auto pNaytibaDefaultInfo = m_pBlackBoard->GetBossDefaultInfo();
		_vector vOwnerPos{}, vTempOwnerPos{};
		vTempOwnerPos = vOwnerPos = m_pOwner->GetTransform()->Get_State(STATE::POSITION);
		_vector vTargetPos = pTarget->GetTransform()->Get_State(STATE::POSITION);

		vTempOwnerPos.m128_f32[1] = vTargetPos.m128_f32[1] = 0.f;
		_float fDistance = XMVectorGetX(XMVector3Length(vTargetPos - vTempOwnerPos));
		_vector vDir = XMVector3Normalize(vTargetPos - vTempOwnerPos);

		_vector vOwnerLook = m_pOwner->GetTransform()->Get_State(STATE::LOOK);
		_float fScalar = XMVectorGetX(XMVector3Dot(vOwnerLook, vDir));

		if (0.99f > fScalar)
			m_pOwner->GetTransform()->LookAt_Lerp(vOwnerPos + vDir, fTimeDelta, 3.0f);
		else
			m_pOwner->GetTransform()->LookAt(vOwnerPos + vDir);

		m_pOwner->GetTransform()->Move_Direction(fTimeDelta, XMLoadFloat3(&m_vMoveDir), m_fSpeed);
		_bool bIsFinished = m_pOwner->Play_Animation(fTimeDelta);
	}
	else
	{
		Refresh_MovePoint();
		m_pBlackBoard->SetCurState(CBossBlackBoard::BOSS_STATE::MOVE);
	}

	return NODE_STATE::COMPLETE;
}

void CTask_ScarletMove::Refresh_MovePoint()
{
	auto pNaytibaDefaultInfo = m_pBlackBoard->GetBossDefaultInfo();
	m_pBlackBoard->SetTargetDistacne();
	_float fDistance = m_pBlackBoard->GetTargetDistance();
	_float fATKRange = m_pBlackBoard->GetBossInfo()->fAttackRange;

	m_szAnimationName = pNaytibaDefaultInfo->szAnimationName;
	_float fRandomIndex = m_pGameInstance->Random(0.f, 100.f);
	m_fSpeed = m_pBlackBoard->GetBossInfo()->fMoveSpeed * 0.5f;
	if (40.f > fRandomIndex)
	{
		m_eDirection = DIRECTION::LEFT;
		m_szAnimationName += "_Caution_Lw";
	}
	else
	{
		m_eDirection = DIRECTION::RIGHT;
		m_szAnimationName += "_Caution_Rw";
	}
	m_bIsCaution = true;

	m_pOwner->Set_Animation(m_szAnimationName.c_str(), true, 1.f, 0.f);
}

CTask_ScarletMove* CTask_ScarletMove::Create(CBehaviorTree* pOwnerTree)
{
	CTask_ScarletMove* pTask_ScarletMove = new CTask_ScarletMove();
	if (FAILED(pTask_ScarletMove->Initialize_Prototype(pOwnerTree)))
	{
		Safe_Release(pTask_ScarletMove);
		MSG_BOX("Create Fail : Task ScarletMove");
	}
	return pTask_ScarletMove;
}

void CTask_ScarletMove::Free()
{
	__super::Free();

	Safe_Release(m_pBlackBoard);
}
