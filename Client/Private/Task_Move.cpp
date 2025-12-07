#include "pch.h"
#include "Task_Move.h"

#include "GameInstance.h"
#include "BossBlackBoard.h"
#include "BehaviorTree.h"
#include "Nayitba.h"

CTask_Move::CTask_Move() : CTask()
{
}

HRESULT CTask_Move::Initialize_Prototype(CBehaviorTree* pOwnerTree)
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

CBehaviorNode::NODE_STATE CTask_Move::Update(_float fTimeDelta)
{
	CBossBlackBoard::BOSS_STATE eCurState = m_pBlackBoard->GetCurState();
	if (CBossBlackBoard::BOSS_STATE::GROGGY == eCurState)
		return NODE_STATE::COMPLETE;

	// 블랙보드에 목표 지점 또는 타겟이 있을때 이동할 녀석
	
	if (m_pBlackBoard->GetCurState() == m_pBlackBoard->GetPreState())
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
		}

		auto pTarget = m_pBlackBoard->GetTarget();
		if (nullptr == pTarget)
			return NODE_STATE::FAIL;

		_vector vOwnerPos{}, vTempOwnerPos{};
		vTempOwnerPos = vOwnerPos = m_pOwner->GetTransform()->Get_State(STATE::POSITION);
		_vector vTargetPos = pTarget->GetTransform()->Get_State(STATE::POSITION);

		vTempOwnerPos.m128_f32[1] = vTargetPos.m128_f32[1] = 0.f;
		_vector vDir = XMVector3Normalize(vTargetPos - vTempOwnerPos);

		_vector vOwnerLook = m_pOwner->GetTransform()->Get_State(STATE::LOOK);
		_float fScalar = acosf(XMVectorGetX(XMVector3Dot(vOwnerLook, vDir)));

		if (0.1f < fabsf(fScalar))
			m_pOwner->GetTransform()->LookAt_Lerp(vOwnerPos + vDir, fTimeDelta, 5.0f);
		else
			m_pOwner->GetTransform()->LookAt(vOwnerPos + vDir);
		m_pOwner->GetTransform()->Move_Direction(fTimeDelta, XMLoadFloat3(&m_vMoveDir), m_fSpeed);

		_bool bIsFinished = m_pOwner->Play_Animation(fTimeDelta);
		if (false == m_bIsCaution)
		{
			if (bIsFinished)
			{
				m_szAnimationName = m_pBlackBoard->GetBossDefaultInfo()->szAnimationName;
				m_iAnimSection++;
				switch (m_iAnimSection)
				{
				case 1 :
					m_szAnimationName += "_Run_L";
					m_pOwner->Set_Animation(m_szAnimationName.c_str(), true);
					break;
				case 2:
					m_szAnimationName += "_Run_E";
					m_pOwner->Set_Animation(m_szAnimationName.c_str(), false);
					m_iAnimSection = 0.f;
					break;
				}
			}
		}
	}
	else
	{
		Refresh_MovePoint();
		m_pBlackBoard->SetCurState(CBossBlackBoard::BOSS_STATE::MOVE);
	}

	return NODE_STATE::COMPLETE;
}

void CTask_Move::Refresh_MovePoint()
{
	_float fRandomIndex = m_pGameInstance->Random(0.f, 100.f);

	m_pBlackBoard->SetTargetDistacne();
	_float fDistance = m_pBlackBoard->GetTargetDistance();
	_float fATKRange = m_pBlackBoard->GetBossInfo()->fAttackRange;

	m_szAnimationName = m_pBlackBoard->GetBossDefaultInfo()->szAnimationName;
	if (fDistance < fATKRange * 1.5f)
	{
		if (50 > fRandomIndex)
		{
			m_eDirection = DIRECTION::LEFT;
			m_szAnimationName += "_Caution_Lw";
			m_pOwner->Set_Animation(m_szAnimationName.c_str());
			m_fSpeed = m_pBlackBoard->GetBossInfo()->fMoveSpeed;
		}
		else
		{
			m_eDirection = DIRECTION::RIGHT;
			m_szAnimationName += "_Caution_Rw";
			m_pOwner->Set_Animation(m_szAnimationName.c_str());
			m_fSpeed = m_pBlackBoard->GetBossInfo()->fMoveSpeed;
		}
		m_bIsCaution = true;
	}
	else
	{
		// 여기서 달리기 하자
		m_szAnimationName += "_Run_S";
		m_eDirection = DIRECTION::FRONT;
		m_pOwner->Set_Animation(m_szAnimationName.c_str(), false);
		m_iAnimSection = 0;
		m_bIsCaution = false;
	}
	
}

CTask_Move* CTask_Move::Create(CBehaviorTree* pOwnerTree)
{
	CTask_Move* pTask_Move = new CTask_Move();
	if (FAILED(pTask_Move->Initialize_Prototype(pOwnerTree)))
	{
		Safe_Release(pTask_Move);
		MSG_BOX("Create Fail : Task Move");
	}
	return pTask_Move;
}

void CTask_Move::Free()
{
	__super::Free();

	Safe_Release(m_pBlackBoard);
}
