#include "pch.h"
#include "Task_GorillaAttack.h"

#include "GameManager.h"
#include "GameInstance.h"
#include "BossBlackBoard.h"
#include "GorillaBehaviorTree.h"
#include "Nayitba.h"

CTask_GorillaAttack::CTask_GorillaAttack() : CTask()
{
}

HRESULT CTask_GorillaAttack::Initialize_Prototype(CBehaviorTree* pOwnerTree)
{
	if (FAILED(__super::Initialize_Prototype(pOwnerTree)))
		return E_FAIL;

	if (nullptr == m_pOwner)
		m_pOwner = static_cast<CNayitba*>(m_pOwnerTree->GetOwner());

	if (nullptr == m_pBlackBoard)
		m_pBlackBoard = static_cast<CBossBlackBoard*>(m_pOwnerTree->GetBlackBoard());

	m_pGameManager = CGameManager::GetInstance();
	Safe_AddRef(m_pGameManager);

	m_fMaxDelayTime = 5.5f;
	return S_OK;
}

CBehaviorNode::NODE_STATE CTask_GorillaAttack::Update(_float fTimeDelta)
{
	if (nullptr == m_pBlackBoard->GetAttackData())
	{
		if (false == SelectPattern())
			return NODE_STATE::FAIL;

		m_bIsAttackStartLerp = true;
	}

	CBossBlackBoard::BOSS_STATE eCurState = m_pBlackBoard->GetCurState();
	if (CBossBlackBoard::BOSS_STATE::HIT == eCurState)
	{
		while (!m_pSkillData.empty())
			m_pSkillData.pop();

		Compute_AttackCoolTime(true);
		m_pBlackBoard->SetAttackData(nullptr);
		return NODE_STATE::FAIL;
	}

	if (m_bIsAttackStartLerp)
		LookAtPoint(fTimeDelta);

	// 일단 여기서 고릴라 공격에대한 이동 처리
	AttackMoveAction(fTimeDelta);
	m_pBlackBoard->SetCurState(CBossBlackBoard::BOSS_STATE::ATTACK);

	_bool bIsFinished = m_pOwner->Play_Animation(fTimeDelta);
	if (bIsFinished)
	{
		if (m_pSkillData.empty())
		{
			Compute_AttackCoolTime();
			m_pBlackBoard->SetCurState(CBossBlackBoard::BOSS_STATE::IDLE);
			m_pBlackBoard->SetAttackData(nullptr);
			return NODE_STATE::COMPLETE;
		}
		else
			SelectAttack();
	}

	return NODE_STATE::RUNNING;
}

void CTask_GorillaAttack::SelectRandomPattern()
{
	// 여기서 거리가 일단 멀어지면 날라오거나
	// 돌을 던지는 패턴을 하자
	if(50 >= m_pGameInstance->Random(0.f, 100.f))
		m_pSkillData.push(m_pOwner->GetSkillData(true, ENUM_CLASS(SKILL_TYPE::DEFAULT_SKILL)));
	else
		m_pSkillData.push(m_pOwner->GetSkillData(true, ENUM_CLASS(SKILL_TYPE::BETA_SKILL)));

	SelectAttack();
}

_bool CTask_GorillaAttack::SelectPattern()
{
	m_pBlackBoard->SetTargetDistacne();
	_float fDistance = m_pBlackBoard->GetTargetDistance();

	if (fDistance <= m_pOwner->GetMonsterData().fAttackRange * 1.5f)
	{
		_float fRandomIndex = m_pGameInstance->Random(0.f, 100.f);

		/*if (30.f > fRandomIndex)
			BboyStepPattern();
		else*/ if (30.f > fRandomIndex)
		{
			m_CurPatternIndex = 1;
		}
		else
		{
			m_PrePatternIndex = 0;
			if(65 > fRandomIndex)
				m_CurPatternIndex = 2;
			else
				m_CurPatternIndex = 3;
		}
	}
	else
	{
		m_PrePatternIndex = 0;
		m_CurPatternIndex = 4;
	}

	if (m_PrePatternIndex != m_CurPatternIndex || 0 == m_PrePatternIndex)
	{
		m_PrePatternIndex = m_CurPatternIndex;
		switch (m_CurPatternIndex)
		{
		case 1 :
			BackStepPattern();
			break;
		case 2:
			SelectRandomPattern();
			break;
		case 3:
			MoveAttackPattern();
			break;
		case 4:
			CrushPattern();
			break;
		}
	}
	else
		SelectPattern();

	if (nullptr == m_pBlackBoard->GetAttackData())
		return false;

	return true;
}

void CTask_GorillaAttack::SelectAttack(_bool bIsForce)
{
	if (!m_pSkillData.empty())
	{
		m_pBlackBoard->SetAttackData(m_pSkillData.front());
		m_pSkillData.pop();

		auto pSkill_Data = m_pBlackBoard->GetAttackData();
		m_pOwner->Set_Animation(pSkill_Data->szAnimationName, false, 1.0f, 0.12f, true);
	}

	if (3 == m_CurPatternIndex && m_pSkillData.empty())
		m_bIsAttackStartLerp = true;
}

void CTask_GorillaAttack::CrushPattern()
{
	m_pSkillData.push(m_pGameManager->Find_SkillData(2));
	m_pSkillData.push(m_pGameManager->Find_SkillData(12));
	SelectAttack();
}

void CTask_GorillaAttack::MoveAttackPattern()
{
	_float fRandomIndex = m_pGameInstance->Random(0.f, 100.f);
	if (50.f > fRandomIndex)
		m_pSkillData.push(m_pGameManager->Find_SkillData(7));
	else
		m_pSkillData.push(m_pGameManager->Find_SkillData(8));

	m_pSkillData.push(m_pGameManager->Find_SkillData(9));
	SelectAttack();
}

void CTask_GorillaAttack::BackStepPattern()
{
	m_pSkillData.push(m_pGameManager->Find_SkillData(10));

	_float fRandomIndex = m_pGameInstance->Random(0.f, 100.f);
	if (40.f < fRandomIndex)
	{
		CrushPattern();
		
	}
	else
	{
		//돌을 던져봐요~
		//영차!영차!
		//
		//높이 던져봐요~
		//영차!영차!
		//
		//굴러가는 돌~
		//데굴 데굴 굴러가요~
		m_pSkillData.push(m_pGameManager->Find_SkillData(13));
		SelectAttack();
	}
}

// Return SKill
//void CTask_GorillaAttack::BboyStepPattern()
//{
//	auto pSkillData = m_pGameManager->Find_SkillData(12);
//	m_pSkillData.push(pSkillData);
//	m_pSkillData.push(pSkillData);
//	m_pSkillData.push(pSkillData);
//	m_pSkillData.push(pSkillData);
//
//	SelectAttack();
//}

_bool CTask_GorillaAttack::AttackMoveAction(_float fTimeDelta)
{
	// 블랙보드에서 선택된 타겟을 가져오고 거리가 가까우면 이녀석은 거리를 좁히는 이동은 하지않는다.
	// 블랙 보드에서 거리를 받아와서 하자
	auto pNaytibaStaticData = m_pOwner->GetStaticMonsterData();
	if (nullptr == pNaytibaStaticData)
		return false;

	m_pBlackBoard->SetTargetDistacne();

	m_fMoveSpeed = m_pOwner->GetMonsterData().fMoveSpeed;
	_float fDistance = m_pBlackBoard->GetTargetDistance();
	if (0 > fDistance)
		return false;

	// 여기서 선택된 스킬에 대한 정보를 처리한다.
	_bool  bIsMove{ false }, bIsLerpMove{ false };
	m_bIsLookAtPoint = true;

	_float fAnimationRatio = m_pOwner->Get_AnimationRatio();

	// 플레이어와 몬스터의 거리기반으로 이동시키는 걸로하자 공격할때
	// 타이밍에 맞춰서 최소거리 유지
	_vector vOwnerPos = m_pOwner->GetTransform()->Get_State(STATE::POSITION);
	_vector vTargetPos{}, vTempTargetPos{};
	vTargetPos = vTempTargetPos = m_pBlackBoard->GetTarget()->GetTransform()->Get_State(STATE::POSITION);

	vOwnerPos.m128_f32[1] = vTargetPos.m128_f32[1] = 0.f;
	_vector vDir = XMVector3Normalize(vTargetPos - vOwnerPos);
	_vector vReverseDir = -1 * vDir;
	
	auto pAttackData = m_pBlackBoard->GetAttackData();
	if (pAttackData)
	{
		switch (pAttackData->iSkillID)
		{
		case 2 : 
		{
			// M_Gorilla_S12_Crush
			// Max Frame : 90
			// Farme: 30 ~50

			if (0.33f <= fAnimationRatio && 0.55f >= fAnimationRatio)
			{
				XMStoreFloat3(&m_fAttackMovePoint, vTempTargetPos + vReverseDir * pNaytibaStaticData->fAttackRange);
				if (pNaytibaStaticData->fAttackRange > fDistance)
					m_fLerpSpeed = 0.f;
				else
					m_fLerpSpeed = 5.f;
				bIsLerpMove = true;
			}
		}
		break;
		case 3:
		{
			// M_Gorilla_S13_RightSwing
			// Max Frame : 99
			// Frame : 20 ~35
			// Frame : 45 ~55

			if ((0.2f <= fAnimationRatio && 0.35f >= fAnimationRatio) ||
				(0.45f <= fAnimationRatio && 0.55f >= fAnimationRatio))
			{
				XMStoreFloat3(&m_fAttackMoveDir, vDir);
				if (pNaytibaStaticData->fAttackRange > fDistance)
					m_fMoveSpeed = 0.f;
				bIsMove = true;
			}
		}
		break;
		case 5:
		{
			// M_Gorilla_S15_FowardMoveBlow
			// Max Frame : 120
			// Frame : 0 ~40
			if (0.30f >= fAnimationRatio)
			{
				XMStoreFloat3(&m_fAttackMoveDir, vDir);
				if (pNaytibaStaticData->fAttackRange > fDistance)
					m_fMoveSpeed = 0.f;
				bIsMove = true;
			}
		}
		break;
		case 6:
		{
			// M_Gorilla_S07_ZigzagMoveAttack
			// Max Frame : 115
			// Frame : 30 ~ 48
			// Frame : 63 ~ 80

			if ((0.26f <= fAnimationRatio && 0.41f >= fAnimationRatio) ||
				(0.54f <= fAnimationRatio && 0.69f >= fAnimationRatio))
			{
				XMStoreFloat3(&m_fAttackMoveDir, vDir);
				if (pNaytibaStaticData->fAttackRange > fDistance)
					m_fMoveSpeed = 0.f;
				bIsMove = true;
			}
		}
		break;
		case 7:
		{
			// M_Gorilla_S02_ShortMoveLeft
			// Max Frame : 30
			// Frame : 20 ~ 25
			if (0.8f >= fAnimationRatio)
			{
				XMStoreFloat3(&m_fAttackMovePoint, vOwnerPos + m_pOwner->GetTransform()->Get_State(STATE::RIGHT) * -7.f);
				m_fLerpSpeed = 5.f;
				m_bIsLookAtPoint = false;
				bIsLerpMove = true;
			}
		}
		break;
		case 8:
		{
			// M_Gorilla_S02_ShortMoveRight
			// Max Frame : 30
			// Frame : 20 ~ 25
			if (0.8f >= fAnimationRatio)
			{
				XMStoreFloat3(&m_fAttackMovePoint, vOwnerPos + m_pOwner->GetTransform()->Get_State(STATE::RIGHT) * 7.f);
				m_fLerpSpeed = 5.f;
				m_bIsLookAtPoint = false;
				bIsLerpMove = true;
			}
		}
		break;	
		case 10:
		{
			// 이건 여기서 무빙 위치를 다시 만들어주자
			// M_Gorilla_S06_BlowMoveBack
			// Max Frame : 66
			// Frame : 20 ~ 43
			if (0.3f <= fAnimationRatio && 0.65f >= fAnimationRatio)
			{
				XMStoreFloat3(&m_fAttackMovePoint, vOwnerPos + vReverseDir * 13.f);
				m_bIsLookAtPoint = false;
				bIsLerpMove = true;
			}
		}
		break;
		case 11:
		{
			// M_Gorilla_S09_LeftSwingUpperCombo
			// Max Frame : 135
			// Frame : 20 ~ 30
			// Frame : 36 ~ 46
			// Frame : 54 ~ 64
			// Frame : 108 ~ 120

			if ((0.15f <= fAnimationRatio && 0.23f >= fAnimationRatio) ||
				(0.26f <= fAnimationRatio && 0.34f >= fAnimationRatio) ||
				(0.4f <= fAnimationRatio && 0.47f >= fAnimationRatio)  ||
				(0.77f <= fAnimationRatio && 0.88f >= fAnimationRatio))
			{
				XMStoreFloat3(&m_fAttackMoveDir, vDir);
				if (pNaytibaStaticData->fAttackRange > fDistance)
					m_fMoveSpeed = 0.f;

				bIsMove = true;	
			}
		}
		break;
		case 12:
		{
			// M_Gorilla_S09_LeftSwingUpperCombo
			// Max Frame : 135
			// Frame : 40 ~ 50

			if (0.53f <= fAnimationRatio && 0.66f >= fAnimationRatio)
			{
				XMStoreFloat3(&m_fAttackMoveDir, vDir);
				if (pNaytibaStaticData->fAttackRange > fDistance)
					m_fMoveSpeed = 0.f;

				bIsMove = true;
			}
		}
		break;
		}
	}

	m_fLerpSpeed = 3.f;

	if(bIsLerpMove)
		AttackLerpMove(fTimeDelta);
	if (bIsMove)
		AttackADDMove(fTimeDelta);

	return true;
}

_bool CTask_GorillaAttack::Compute_AttackCoolTime(_bool bIsForce)
{
	m_pBlackBoard->SetAttackDelay(m_pGameInstance->Random(3.5f, m_fMaxDelayTime));
	return true;
}

void CTask_GorillaAttack::AttackLerpMove(_float fTimeDelta)
{
	_vector vOwnerPos = m_pOwner->GetTransform()->Get_State(STATE::POSITION);
	_vector vLerpPos = XMVectorLerp(vOwnerPos, XMLoadFloat3(&m_fAttackMovePoint), fTimeDelta * m_fLerpSpeed);
	
	if(m_bIsLookAtPoint)
		m_pOwner->GetTransform()->LookAt_Lerp(vLerpPos, fTimeDelta, 5.f);

	m_pOwner->GetTransform()->Set_State(STATE::POSITION, vLerpPos);
}

void CTask_GorillaAttack::AttackADDMove(_float fTimeDelta)
{
	// 거리기반으로 속도 조절해보자
	m_pOwner->GetTransform()->Move_Direction(fTimeDelta, XMLoadFloat3(&m_fAttackMoveDir), m_fMoveSpeed);

}

void CTask_GorillaAttack::LookAtPoint(_float fTimeDelta)
{
	_vector vOwnerPos{}, vTempOwnerPos{};
	vOwnerPos = vTempOwnerPos = m_pOwner->GetTransform()->Get_State(STATE::POSITION);

	auto pTarget = m_pBlackBoard->GetTarget();
	_vector vTargetPos = pTarget->GetTransform()->Get_State(STATE::POSITION);

	vOwnerPos.m128_f32[1] = vTargetPos.m128_f32[1] = 0.f;
	_vector vDir = XMVector3Normalize(vTargetPos - vOwnerPos);

	m_pOwner->GetTransform()->LookAt_Lerp(vOwnerPos + vDir, fTimeDelta, 5.f);

	if (m_vLerpTime.x > m_vLerpTime.y)
	{
		m_bIsAttackStartLerp = false;
		m_vLerpTime.x = 0.f;
	}
	else
		m_vLerpTime.x += fTimeDelta;
}

CTask_GorillaAttack* CTask_GorillaAttack::Create(CBehaviorTree* pOwnerTree)
{
	CTask_GorillaAttack* pTask_GorillaAttack = new CTask_GorillaAttack();
	if (FAILED(pTask_GorillaAttack->Initialize_Prototype(pOwnerTree)))
	{
		Safe_Release(pTask_GorillaAttack);
		MSG_BOX("Create Fail : Task Gorilla Attack");
	}
	return pTask_GorillaAttack;
}

void CTask_GorillaAttack::Free()
{
	__super::Free();

	Safe_Release(m_pBlackBoard);
	Safe_Release(m_pGameManager);
}
