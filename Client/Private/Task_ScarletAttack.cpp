#include "pch.h"
#include "Task_ScarletAttack.h"

#include "GameManager.h"
#include "GameInstance.h"
#include "ScarletBlackBoard.h"
#include "ScarletBehaviorTree.h"
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
		m_pBlackBoard = static_cast<CScarletBlackBoard*>(m_pOwnerTree->GetBlackBoard());

	m_pGameManager = CGameManager::GetInstance();
	Safe_AddRef(m_pGameManager);

	m_fMaxDelayTime = 3.5f;
	return S_OK;
}

CBehaviorNode::NODE_STATE CTask_ScarletAttack::Update(_float fTimeDelta)
{
	CBossBlackBoard::BOSS_STATE eCurState = m_pBlackBoard->GetCurState();
	CBossBlackBoard::BOSS_STATE ePreState = m_pBlackBoard->GetPreState();

	
	if (CBossBlackBoard::BOSS_STATE::HIT == eCurState ||
		CBossBlackBoard::BOSS_STATE::GROGGY == eCurState)
	{
		ResetAttackTask();
		m_pBlackBoard->AccAttackDelay(fTimeDelta);
		return NODE_STATE::FAIL;
	}

	// 일단 여기서 고릴라 공격에대한 이동 처리
	m_pTarget = m_pBlackBoard->GetTarget();
	m_pBlackBoard->SetTargetDistacne();
	m_pBlackBoard->SetCurState(CBossBlackBoard::BOSS_STATE::ATTACK);
	if (CBossBlackBoard::BOSS_STATE::HIT == ePreState ||
		CBossBlackBoard::BOSS_STATE::GROGGY == ePreState)
	{
		ResetAttackTask(false);
		return NODE_STATE::RUNNING;
	}

	if (nullptr == m_pBlackBoard->GetAttackData())
	{
		if (false == SelectPattern())
			return NODE_STATE::FAIL;
	}

	AttackActionAmount(fTimeDelta);
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
			SelectAttackData();
	}

	return NODE_STATE::RUNNING;
}

void CTask_ScarletAttack::SelectAttackData()
{
	if (m_pSkillData.empty())
		return;

	auto pSkillData = m_pSkillData.front();
	m_pSkillData.pop();

	m_pBlackBoard->SetAttackData(pSkillData);
	m_pOwner->Set_Animation(pSkillData->szAnimationName, false, 1.5f, 0.12f, true);
}

_bool CTask_ScarletAttack::SelectPattern(_bool bIsRandom)
{
	// 거리 기반으로
	// ApproachAttackPattern or NormalAttackPattern 중에서 선택
	// Hit 중에 패링 상태 공격으로 넘어왔을때 LinkAttackPattern을 재생

	if (m_pBlackBoard->bIsEnableEntarnceAttack())
	{
		// 페이즈 최초 조우 패턴
		EntranceAttack();
	}
	else
	{
		// 기본적인 패턴들 하는거
		CBossBlackBoard::BOSS_PAHSE ePhase = m_pBlackBoard->Get_BossPhase();
		if (false == m_pBlackBoard->IsParryAttack())
		{
			if (CBossBlackBoard::BOSS_PAHSE::SECOND == ePhase)
				SecondPhaseNormalAttack();
			else
				NormalAttackPattern();
		}
		else
		{
			// 연계 패턴 
			LinkAttackPattern();
		}
	}
	
	return true;
}

void CTask_ScarletAttack::NormalAttackPattern()
{
	auto pSkill_Data = m_pOwner->GetSkillData(true, ENUM_CLASS(SKILL_TYPE::DEFAULT_SKILL));
	if (23 == pSkill_Data->iSkillID)
	{
		_float fDistance = m_pBlackBoard->GetTargetDistance();
		if (fDistance >= m_pBlackBoard->GetBossDefaultInfo()->fAttackRange)
			m_pSkillData.push(m_pGameManager->Find_SkillData(22));
	}
	
	m_pSkillData.push(pSkill_Data);
	SelectAttackData();
}

void CTask_ScarletAttack::LinkAttackPattern()
{
	// 여기는 강제로 그로기상태에서 강제로 들어왔을때 실행할 액션
	
}

void CTask_ScarletAttack::EntranceAttack()
{
	CBossBlackBoard::BOSS_PAHSE ePhase = m_pBlackBoard->Get_BossPhase();
	switch (ePhase)
	{
	case CBossBlackBoard::BOSS_PAHSE::FIRST:
		m_pSkillData.push(m_pGameManager->Find_SkillData(20));
		break;
	case CBossBlackBoard::BOSS_PAHSE::SECOND:
		m_pSkillData.push(m_pGameManager->Find_SkillData(33));
		m_pSkillData.push(m_pGameManager->Find_SkillData(34));
		m_pSkillData.push(m_pGameManager->Find_SkillData(35));
		m_pSkillData.push(m_pGameManager->Find_SkillData(36));
		break;
	}

	SelectAttackData();
	m_pBlackBoard->SetEntarnceAttack(false);
}

void CTask_ScarletAttack::SecondPhaseNormalAttack()
{
	_float fRandom = m_pGameInstance->Random(0.f, 100.f);
	if (50.f < fRandom)
		NormalAttackPattern();
	else
	{
		auto pSkill_Data = m_pOwner->GetSkillData(true, ENUM_CLASS(SKILL_TYPE::SECOND_PHASE_SKILL));
		m_pSkillData.push(pSkill_Data);

		if (45 == pSkill_Data->iSkillID)
			m_pSkillData.push(m_pGameManager->Find_SkillData(46));
		SelectAttackData();
	}
}

void CTask_ScarletAttack::SecondPhaseSpecialAttack()
{
	// 여기서 기술 4개 연계해서 넣고 그다음 애니메이션 재생할 예정
	m_pSkillData.push(m_pGameManager->Find_SkillData(36));
	m_pSkillData.push(m_pGameManager->Find_SkillData(37));
	m_pSkillData.push(m_pGameManager->Find_SkillData(38));
	m_pSkillData.push(m_pGameManager->Find_SkillData(39));

	// 이거 4개 실행하고 성공실패 판단해서 Fail 인지 Success 인지 판단
	SelectAttackData();
}

_bool CTask_ScarletAttack::AttackActionAmount(_float fTimeDelta)
{
	// 블랙보드에서 선택된 타겟을 가져오고 거리가 가까우면 이녀석은 거리를 좁히는 이동은 하지않는다.
	// 블랙 보드에서 거리를 받아와서 하자
	auto pNaytibaStaticData = m_pOwner->GetStaticMonsterData();
	if (nullptr == pNaytibaStaticData)
		return false;

	// 여기서 선택된 스킬에 대한 정보를 처리한다.
	_bool  bIsMove{ false }, bIsLerpMove{ false };
	// 플레이어와 몬스터의 거리기반으로 이동시키는 걸로하자 공격할때
	// 타이밍에 맞춰서 최소거리 유지
	_vector vOwnerPos{}, vTempOwnerPos{};
	vTempOwnerPos = vOwnerPos = m_pOwner->GetTransform()->Get_State(STATE::POSITION);

	_vector vTargetPos{}, vTempTargetPos{};
	vTargetPos = vTempTargetPos = m_pTarget->GetTransform()->Get_State(STATE::POSITION);

	vOwnerPos.m128_f32[1] = vTargetPos.m128_f32[1] = 0.f;
	_vector vDir = XMVector3Normalize(vTargetPos - vOwnerPos);
	_vector vReverseDir = -1 * vDir;

	m_bIsLookAtPoint = true;
	auto pAttackData = m_pBlackBoard->GetAttackData();
	if (pAttackData)
	{
		_float fAnimationRatio = m_pOwner->Get_AnimationRatio();
		switch (pAttackData->iSkillID)
		{
		case 20:  // AirDashCut
		{
			// Length : 468
			// Move Frame : 59 ~ 80
			if (0.12f <= fAnimationRatio && 0.17f >= fAnimationRatio)
			{
				m_fMoveAnimMaxRatio = 0.17f;
				m_fLerpSpeed = 5.f;
				XMStoreFloat3(&m_fAttackMovePoint, vTempTargetPos + vReverseDir);
				bIsLerpMove = true;
			}
			else if (0.3f <= fAnimationRatio && 0.33f >= fAnimationRatio)
			{
				// Move Frame : 145 ~ 155
				m_fMoveAnimMaxRatio = 0.33f;
				m_fLerpSpeed = 5.f;

				if(0.31f > fAnimationRatio)
					XMStoreFloat3(&m_fAttackMovePoint, vTempTargetPos + vReverseDir);

				bIsLerpMove = true;
			}
			else if (0.33f <= fAnimationRatio && 0.41f >= fAnimationRatio)
			{
				// Move Frame : 145 ~ 155
				m_fMoveAnimMaxRatio = 0.41f;
				m_fLerpSpeed = 7.f;

				if (0.34f > fAnimationRatio)
					XMStoreFloat3(&m_fAttackMovePoint, vTempTargetPos + vReverseDir);

				bIsLerpMove = true;
			}
		}
		break;
		case 21 :  //Attack Approach
		{
			// Length : 228
			// Move Frame : 
			if(0.21f >= fAnimationRatio)
			{
				m_fMoveAnimMaxRatio = 0.21f;

				if (0.05f > fAnimationRatio)
					XMStoreFloat3(&m_fAttackMovePoint, vTempTargetPos + vReverseDir);
				m_fLerpSpeed = 5.f;
				bIsLerpMove = true;
			}
		}
			break;
		case 22:  //Swing Combo 1
		{
			// Length : 48
			m_fMoveAnimMaxRatio = 1.f;
			bIsMove = true;

			if (3.f > m_pBlackBoard->GetTargetDistance())
			{
				SelectAttackData();
				bIsMove = false;
			}
		}
		break;
		case 23:  //Swing Combo 2
		{
			// Length : 252
			// Move Frame :  0 ~ 67
			if (0.26f >= fAnimationRatio)
			{
				m_fMoveAnimMaxRatio = 0.26f;

				if (0.05f > fAnimationRatio)
					XMStoreFloat3(&m_fAttackMovePoint, vTempTargetPos + vReverseDir);
				bIsLerpMove = true;
			}
		}
		break;
		case 24:  // Swing
		{
			// 25(Swing Fast), 26(Swing Triple) 패턴은 원거리 패턴이다.
			// 
			// Length : 72
			// Move Frame :  0 ~ 40
			if (0.55f >= fAnimationRatio)
			{
				m_fMoveAnimMaxRatio = 0.55f;
				if (0.05f > fAnimationRatio)
					XMStoreFloat3(&m_fAttackMovePoint, (vTempOwnerPos + m_pOwner->GetTransform()->Get_State(STATE::LOOK) * -10.f));

				m_fLerpSpeed = 5.f;
				m_bIsLookAtPoint = false;
				bIsLerpMove = true;
			}
		}
		break;
		case 27: // Air Spin
		{
			// Length : 447
			// Move Frame :  70 ~ 104
			if (0.15f <= fAnimationRatio && 0.23f >= fAnimationRatio)
			{
				m_fMoveAnimMaxRatio = 0.23f;
				bIsMove = true;
			}
			else if (0.32f <= fAnimationRatio && 0.44f >= fAnimationRatio) // Move Frame : 145 ~ 200
			{
				m_fMoveAnimMaxRatio = 0.44f;
				bIsMove = true;
			}
		}
		break;
		case 28 : // MoveBackShot2
		{
			// Length : 315
			// Move Frame : 0 ~ 90
			if (0.28f >= fAnimationRatio)
			{
				m_fMoveAnimMaxRatio = 0.28f;
				m_fLerpSpeed = 2.f;
				if (0.05f > fAnimationRatio)
					XMStoreFloat3(&m_fAttackMovePoint, (vTempOwnerPos + m_pOwner->GetTransform()->Get_State(STATE::LOOK) * -5.f));

				bIsLerpMove = true;
			}
		}
		break;
		case 29: // Jump Stamp Slash
		{
			// Length : 390
			// Move Frame : 0 ~ 90
			if (0.28f >= fAnimationRatio)
			{
				// Move Frame : 0 ~ 90
				m_fMoveAnimMaxRatio = 0.28f;
				m_fLerpSpeed = 5.f;

				if (0.25f > fAnimationRatio)
					XMStoreFloat3(&m_fAttackMovePoint, (vTempTargetPos + vReverseDir));
				bIsLerpMove = true;
			}
			else if (0.24f <= fAnimationRatio && 0.3f >= fAnimationRatio) // Back Step
			{
				// Move Frame : 0 ~ 90
				m_fMoveAnimMaxRatio = 0.3f;
				m_fLerpSpeed = 7.f;
				m_bIsLookAtPoint = false;

				if (0.25f > fAnimationRatio)
					XMStoreFloat3(&m_fAttackMovePoint, (vTempOwnerPos + m_pOwner->GetTransform()->Get_State(STATE::LOOK) * -10.f));
				bIsLerpMove = true;
			}
			else if (0.33f <= fAnimationRatio && 0.41f >= fAnimationRatio)
			{
				// Move Frame : 132 ~ 160
				// Move Frame : 0 ~ 90
				m_fMoveAnimMaxRatio = 0.41f;
				m_fLerpSpeed = 5.f;

				if (0.34f > fAnimationRatio)
					XMStoreFloat3(&m_fAttackMovePoint, (vTempTargetPos + vReverseDir));
				bIsLerpMove = true;
			}
			else if (0.64f <= fAnimationRatio && 0.75f >= fAnimationRatio)
			{
				// Move Frame : 250 ~ 296
				m_fMoveAnimMaxRatio = 0.75f;
				m_fLerpSpeed = 5.f;

				if (0.65f > fAnimationRatio)
					XMStoreFloat3(&m_fAttackMovePoint, (vTempTargetPos + vReverseDir));
				bIsLerpMove = true;
			}
		}
		break;
		case 30 : // Kick Stamp
		{
			// Length : 252
			// Move Frame : 63 ~ 83
			if (0.25f <= fAnimationRatio && 0.32f >= fAnimationRatio)
			{
				m_fMoveAnimMaxRatio = 0.32f;
				bIsMove = true;
			}
			else if (0.57f <= fAnimationRatio && 0.61f >= fAnimationRatio)
			{
				// Move Frame : 144 ~ 154
				m_fMoveAnimMaxRatio = 0.61f;

				if (0.58f > fAnimationRatio)
					XMStoreFloat3(&m_fAttackMovePoint, vTempTargetPos + vReverseDir);
				bIsLerpMove = true;
			}
		}
		break;
		case 31: // ParryRangeDash
		{
			// Length : 192
			// Move Frame : 10 ~ 40
			if (0.05f <= fAnimationRatio && 0.2f >= fAnimationRatio)
			{
				m_fMoveAnimMaxRatio = 0.2f;
				m_fLerpSpeed = 7.f;
				if (0.08f > fAnimationRatio)
				{
					_vector vRight = m_pOwner->GetTransform()->Get_State(STATE::RIGHT);
					XMStoreFloat3(&m_fAttackMovePoint, vTempOwnerPos + vRight * -10.f);
				}
					
				bIsLerpMove = true;
			}
			else if (0.3f <= fAnimationRatio && 0.41f >= fAnimationRatio)
			{
				// Move Frame : 60 ~ 80
				m_fMoveAnimMaxRatio = 0.41f;
				m_fLerpSpeed = 7.f;
				if (0.35f > fAnimationRatio)
					XMStoreFloat3(&m_fAttackMovePoint, vTempTargetPos + vReverseDir);
				bIsLerpMove = true;
			}
		}
		break;
		case 32: // BackDashSpaceCut
		{
			// Length : 480
			// Move Frame : 85 ~ 125
			if (0.17f <= fAnimationRatio && 0.26f >= fAnimationRatio)
			{
				m_fMoveAnimMaxRatio = 0.26f;

				if (0.18f > fAnimationRatio)
					XMStoreFloat3(&m_fAttackMovePoint, vTempTargetPos + vReverseDir * 4.f);
				bIsLerpMove = true;
			}
			else if (0.28f <= fAnimationRatio && 0.3f >= fAnimationRatio)
			{
				// Move Frame : 135 ~ 147
				m_fMoveAnimMaxRatio = 0.3f;
				bIsMove = true;
			}
			else if (0.44f <= fAnimationRatio && 0.47f >= fAnimationRatio)
			{
				// Move Frame : 256 ~ 265
				m_fMoveAnimMaxRatio = 0.47f;
				m_fLerpSpeed = 7.f;

				if (0.45f > fAnimationRatio)
					XMStoreFloat3(&m_fAttackMovePoint, (vOwnerPos + m_pOwner->GetTransform()->Get_State(STATE::LOOK) * -13.f));

				m_bIsLookAtPoint = false;
				bIsLerpMove = true;
			}
			else if (0.53f <= fAnimationRatio && 0.55f >= fAnimationRatio)
			{
				// Move Frame : 256 ~ 265
				m_fMoveAnimMaxRatio = 0.55f;
				m_fLerpSpeed = 5.f;
				m_bIsLookAtPoint = false;
				bIsLerpMove = true;

				if (0.54f > fAnimationRatio)
					XMStoreFloat3(&m_fAttackMovePoint, (vOwnerPos + m_pOwner->GetTransform()->Get_State(STATE::LOOK) * 3.f));
			}
		}
		break;
		case 33: // BlinkCut
		{
			// Length : 150
			// Move Frame : 42 ~ 66
			if (0.28f <= fAnimationRatio && 0.44f >= fAnimationRatio)
			{
				m_fMoveAnimMaxRatio = 0.44f;

				if (0.29f > fAnimationRatio)
					XMStoreFloat3(&m_fAttackMovePoint, vTempTargetPos + vReverseDir);
				bIsLerpMove = true;
			}
		}
		break;
		}
	}

	if (bIsLerpMove)
		AttackLerpMove(fTimeDelta);
	else if (bIsMove)
		AttackADDMove(fTimeDelta);

	return true;
}

_bool CTask_ScarletAttack::Compute_AttackCoolTime(_bool bIsForce)
{
	m_pBlackBoard->ClearAttackTimer();
	m_pBlackBoard->SetAttackDelay(m_pGameInstance->Random(1.5f, m_fMaxDelayTime));
	return true;
}

void CTask_ScarletAttack::AttackLerpMove(_float fTimeDelta)
{
	_vector vOwnerPos = m_pOwner->GetTransform()->Get_State(STATE::POSITION);
	_float fDistance = XMVectorGetX(XMVector3Length(vOwnerPos - XMLoadFloat3(&m_fAttackMovePoint)));
	_vector vLerpPos = XMVectorLerp(vOwnerPos, XMLoadFloat3(&m_fAttackMovePoint), fTimeDelta * m_fLerpSpeed);

	if (fDistance <= 3.5f)
		return;

	if (m_bIsLookAtPoint)
		m_pOwner->GetTransform()->LookAt_Lerp(vLerpPos, fTimeDelta, 5.f);
	else
		LookAtPoint(fTimeDelta);

	m_pOwner->GetTransform()->Set_State(STATE::POSITION, vLerpPos);
}

void CTask_ScarletAttack::AttackADDMove(_float fTimeDelta)
{
	m_pBlackBoard->SetTargetDistacne();
	_float fDistance = m_pBlackBoard->GetTargetDistance();
	auto pSkill_Data = m_pBlackBoard->GetAttackData();
	LookAtPoint(fTimeDelta);

	if (nullptr == pSkill_Data || 3.f >= fDistance - pSkill_Data->fRange)
		return;
	
	_float fAnimPlayRatio = m_pOwner->Get_AnimationRatio();
	_float fSpeed = m_pBlackBoard->GetBossDefaultInfo()->fMoveSpeed * (fDistance / pSkill_Data->fRange) * (m_fMoveAnimMaxRatio / fAnimPlayRatio);
	fSpeed = Clamp<_float>(fSpeed, 0.f, 10.f);
	m_pOwner->GetTransform()->Move_Direction(fTimeDelta, m_pOwner->GetTransform()->Get_State(STATE::LOOK), fSpeed);
}

void CTask_ScarletAttack::LookAtPoint(_float fTimeDelta)
{
	_vector vOwnerPos{}, vTempOwnerPos{}, vTargetPos{};
	vOwnerPos = vTempOwnerPos = m_pOwner->GetTransform()->Get_State(STATE::POSITION);
	vTargetPos = m_pTarget->GetTransform()->Get_State(STATE::POSITION);
	vTempOwnerPos.m128_f32[1] = vTargetPos.m128_f32[1] = 0.f;

	_vector vDir = XMVector3Normalize(vTargetPos - vTempOwnerPos);
	m_pOwner->GetTransform()->LookAt_Lerp(vOwnerPos + vDir, fTimeDelta, 5.f);
}

void CTask_ScarletAttack::ResetAttackTask(_bool bIsCoolTime)
{
	if (bIsCoolTime)
		Compute_AttackCoolTime(true);

	while (!m_pSkillData.empty())
		m_pSkillData.pop();

}

CTask_ScarletAttack* CTask_ScarletAttack::Create(CBehaviorTree* pOwnerTree)
{
	CTask_ScarletAttack* pTask_ScarletAttack = new CTask_ScarletAttack();
	if (FAILED(pTask_ScarletAttack->Initialize_Prototype(pOwnerTree)))
	{
		Safe_Release(pTask_ScarletAttack);
		MSG_BOX("Create Fail : Task Scarlet Attack");
	}
	return pTask_ScarletAttack;
}

void CTask_ScarletAttack::Free()
{
	__super::Free();

	Safe_Release(m_pBlackBoard);
}
