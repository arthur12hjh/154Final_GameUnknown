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
		m_pOwner = static_cast<CNaytiba*>(m_pOwnerTree->GetOwner());

	if (nullptr == m_pBlackBoard)
		m_pBlackBoard = static_cast<CScarletBlackBoard*>(m_pOwnerTree->GetBlackBoard());

	m_pGameManager = CGameManager::GetInstance();
	Safe_AddRef(m_pGameManager);

	m_fMaxDelayTime = 3.5f;
	return S_OK;
}

CBehaviorNode::NODE_STATE CTask_ScarletAttack::Update(_float fTimeDelta)
{
	_bool bIsTaskFinished = false;
	CBossBlackBoard::BOSS_STATE eCurState = m_pBlackBoard->GetCurState();
	CBossBlackBoard::BOSS_STATE ePreState = m_pBlackBoard->GetPreState();

	if (CBossBlackBoard::BOSS_STATE::HIT == eCurState ||
		CBossBlackBoard::BOSS_STATE::GROGGY == eCurState ||
		CBossBlackBoard::BOSS_STATE::INTERACTION_ATTACK == eCurState)
	{
		if (CBossBlackBoard::BOSS_STATE::INTERACTION_ATTACK != eCurState)
			ResetAttackTask();
		else
			ResetAttackTask(false);
		
		m_pBlackBoard->AccAttackDelay(fTimeDelta);
		return NODE_STATE::FAIL;
	}

	m_pTarget = m_pBlackBoard->GetTarget();
	m_pBlackBoard->SetTargetDistacne();
	if (CBossBlackBoard::BOSS_STATE::HIT == ePreState ||
		CBossBlackBoard::BOSS_STATE::GROGGY == ePreState ||
		CBossBlackBoard::BOSS_STATE::INTERACTION_ATTACK == ePreState)
	{
		ResetAttackTask(false);
		m_pBlackBoard->SetCurState(CBossBlackBoard::BOSS_STATE::ATTACK);
		return NODE_STATE::RUNNING;
	}

	if (nullptr == m_pBlackBoard->GetAttackData())
	{
		if (false == SelectPattern())
			return NODE_STATE::FAIL;
	}
	else
	{
		ActionAmount(fTimeDelta);
		if (42 == m_pBlackBoard->GetAttackData()->iSkillID)
			m_fAnimationSpeed = 1.2f;
	}

	_bool bIsFinished = m_pOwner->Play_Animation(fTimeDelta * m_fAnimationSpeed);
	if (bIsFinished)
	{
		if (m_pSkillData.empty())
		{
			m_fMaxDelayTime = 3.5f;
		
			if (10.f > m_pBlackBoard->GetTargetDistance())
			{
				if (!m_pBlackBoard->IsPhaseLastAttack() && !m_pBlackBoard->bIsEnableEntarnceAttack())
				{
					_float fRandom = m_pGameInstance->Random(0.f, 100.f);
					if (30.f > fRandom)
					{
						BackStepPattern();
					}
					else
					{
						m_fMaxDelayTime = 1.7f;
						bIsTaskFinished = true;
					}
				}
				else
				{
					
					bIsTaskFinished = true;
				}
			}
			else
				bIsTaskFinished = true;

			Clear_ScarletAttackTask();
		}
		else
			SelectAttackData();
	}

	if (bIsTaskFinished)
	{
		m_pBlackBoard->SetCurState(CBossBlackBoard::BOSS_STATE::IDLE);
		m_pBlackBoard->SetAttackData(nullptr);
		Compute_AttackCoolTime();

		return NODE_STATE::COMPLETE;
	}

	return NODE_STATE::RUNNING;
}

void CTask_ScarletAttack::ActionAmount(_float fTimeDelta)
{
	// 여기서 특정 스킬 행동 초기화해야하면 하자
	CancelSkillData();

	if (m_TimeLineDatas.empty())
		return;

	auto& pDesc = m_TimeLineDatas.front();
	_float	fAnimationRatio = m_pOwner->Get_AnimationRatio();

	if (fAnimationRatio >= pDesc.EndTime)
	{
		m_TimeLineDatas.pop();
		if (m_TimeLineDatas.empty())
			return;

		pDesc = m_TimeLineDatas.front();
	}

	if (pDesc.BeginTime <= fAnimationRatio &&  fAnimationRatio <= pDesc.EndTime)
	{
		if (false == pDesc.bIsEnter)
		{
			_vector vOwnerPos{}, vTargetPos{};
			vOwnerPos = m_pOwner->GetTransform()->Get_State(STATE::POSITION);
			vTargetPos = m_pTarget->GetTransform()->Get_State(STATE::POSITION);

			switch (pDesc.eDirection)
			{
			case DIRECTION::LEFT:
				XMStoreFloat3(&m_vDir, -1.f * m_pOwner->GetTransform()->Get_State(STATE::RIGHT));
				m_bIsLookAtPoint = false;
				break;

			case DIRECTION::RIGHT:
				XMStoreFloat3(&m_vDir, m_pOwner->GetTransform()->Get_State(STATE::RIGHT));
				m_bIsLookAtPoint = false;
				break;

			case DIRECTION::RIGHT_BACK:
			{
				_vector vRight = m_pOwner->GetTransform()->Get_State(STATE::RIGHT);
				_vector vFront = m_pOwner->GetTransform()->Get_State(STATE::LOOK);
				XMStoreFloat3(&m_vDir, vRight + -1.f * vFront);
				m_bIsLookAtPoint = false;
			}
			break;

			case DIRECTION::LEFT_BACK:
			{
				_vector vRight = m_pOwner->GetTransform()->Get_State(STATE::RIGHT);
				_vector vFront = m_pOwner->GetTransform()->Get_State(STATE::LOOK);
				XMStoreFloat3(&m_vDir, -1.f * (vRight + vFront));
				m_bIsLookAtPoint = false;
			}
			break;
			case DIRECTION::BACK:
			{
				_vector vTempOwnerPos = vOwnerPos;
				_vector vTempTargetPost = vTargetPos;
				vTempOwnerPos.m128_f32[1] = vTempTargetPost.m128_f32[1] = 0.f;
				XMStoreFloat3(&m_vDir, -1 * XMVector3Normalize(vTempTargetPost - vTempOwnerPos));
				m_bIsLookAtPoint = false;
			}
			break;
			case DIRECTION::FRONT:
			{
				_vector vTempOwnerPos = vOwnerPos;
				_vector vTempTargetPost = vTargetPos;
				vTempOwnerPos.m128_f32[1] = vTempTargetPost.m128_f32[1] = 0.f;
				XMStoreFloat3(&m_vDir, XMVector3Normalize(vTempTargetPost - vTempOwnerPos));
				m_bIsLookAtPoint = true;
			}
			break;
			}

			if (pDesc.bIsTarget)
			{
				_vector vPoint = XMLoadFloat3(&m_vDir) * pDesc.fRange + vTargetPos;
				XMStoreFloat3(&m_fAttackMovePoint, vPoint);
			}
			else
			{
				_vector vPoint = XMLoadFloat3(&m_vDir) * pDesc.fRange + vOwnerPos;
				XMStoreFloat3(&m_fAttackMovePoint, vPoint);
			}
			
			m_fLerpSpeed = pDesc.fSpeed;
			m_fAnimationSpeed = pDesc.fAnimationSpeed;
			pDesc.bIsEnter = true;
		}
	
		AttackLerpMove(fTimeDelta);
	}
}

void CTask_ScarletAttack::SelectAttackData()
{
	if (m_pSkillData.empty())
		return;

	auto pSkillData = m_pSkillData.front();
	m_pSkillData.pop();

	SelectAttackMoveData(pSkillData->iSkillID);
	m_pBlackBoard->SetAttackData(pSkillData);

	pSkillData->eProPerty & SKILL_PROPERTY::SUPERARMOR ? m_pBlackBoard->SetSuperAmor(true) : m_pBlackBoard->SetSuperAmor(false);
	m_pOwner->Set_Animation(pSkillData->szAnimationName, false, 1.f, 0.f, true);
}

_bool CTask_ScarletAttack::SelectPattern(_bool bIsRandom)
{
	m_pBlackBoard->SetCurState(CBossBlackBoard::BOSS_STATE::ATTACK);
	//m_pSkillData.push(m_pGameManager->Find_SkillData(28));
	//SelectAttackData();

	EntranceAttack();
	if (false == m_pBlackBoard->IsPhaseLastAttack())
	{
		if (m_pBlackBoard->bIsEnableEntarnceAttack())
		{
			EntranceAttack();
		}
		else if (m_pBlackBoard->bIsReflectExcution())
		{
			m_pSkillData.push(m_pGameManager->Find_SkillData(40));
			SelectAttackData();
		}
		else
		{
			CBossBlackBoard::BOSS_PAHSE ePhase = m_pBlackBoard->Get_BossPhase();
			if (false == m_pBlackBoard->IsParryAttack())
			{
				if (CBossBlackBoard::BOSS_PAHSE::SECOND == ePhase)
					SecondPhaseNormalAttack();
				else
					NormalAttackPattern();
			}
		}
	}
	else
	{
		m_pSkillData.push(m_pGameManager->Find_SkillData(34));
		m_pSkillData.push(m_pGameManager->Find_SkillData(35));
		m_pSkillData.push(m_pGameManager->Find_SkillData(36));
		m_pSkillData.push(m_pGameManager->Find_SkillData(51));
		m_pSkillData.push(m_pGameManager->Find_SkillData(37));
		SelectAttackData();
	}

	return true;
}

void CTask_ScarletAttack::NormalAttackPattern()
{
	_bool bIsSelectAttack = true;
	_float fDistance = m_pBlackBoard->GetTargetDistance();
	auto pSkill_Data = m_pOwner->GetSkillData(true, ENUM_CLASS(SKILL_TYPE::DEFAULT_SKILL));

	switch(pSkill_Data->iSkillID)
	{
	case 27 : case 30: case 45: case 46: case 52: 	case 23:
		if (fDistance >= pSkill_Data->fRange)
			m_pSkillData.push(m_pGameManager->Find_SkillData(22));
		break;

	case 25: case 26:
		if (fDistance <= m_pBlackBoard->GetBossDefaultInfo()->fAttackRange * 4.f)
			bIsSelectAttack = false;
		break;
	}
	
	if (bIsSelectAttack)
	{
		m_pSkillData.push(pSkill_Data);
		SelectAttackData();
	}
	else
		NormalAttackPattern();
}

void CTask_ScarletAttack::EntranceAttack()
{
	CBossBlackBoard::BOSS_PAHSE ePhase = m_pBlackBoard->Get_BossPhase();
	switch (ePhase)
	{
	case CBossBlackBoard::BOSS_PAHSE::FIRST:
	{
		auto pPhaseSkillData = m_pGameManager->Find_SkillData(20);
		_float fDistance = m_pBlackBoard->GetTargetDistance();

		if(fDistance > pPhaseSkillData->fRange * 2.f)
			m_pSkillData.push(m_pGameManager->Find_SkillData(22));
		m_pSkillData.push(pPhaseSkillData);
	}
	break;

	case CBossBlackBoard::BOSS_PAHSE::SECOND:
		m_pSkillData.push(m_pGameManager->Find_SkillData(56));
		m_pSkillData.push(m_pGameManager->Find_SkillData(34));
		m_pSkillData.push(m_pGameManager->Find_SkillData(35));
		m_pSkillData.push(m_pGameManager->Find_SkillData(36));
		m_pSkillData.push(m_pGameManager->Find_SkillData(37));
		break;
	}

	SelectAttackData();
}

void CTask_ScarletAttack::BackStepPattern()
{
	_float fRandom = m_pGameInstance->Random(0.f, 100.f);
	if (60.f < fRandom)
		m_pSkillData.push(m_pGameManager->Find_SkillData(48));
	else
		m_pSkillData.push(m_pGameManager->Find_SkillData(28));

	SelectAttackData();
}

void CTask_ScarletAttack::CancelSkillData()
{
	_float fAnimationRatio = m_pOwner->Get_AnimationRatio();
	_float fDistance = m_pBlackBoard->GetTargetDistance();
	auto pSkillData = m_pBlackBoard->GetAttackData();

	if (22 == pSkillData->iSkillID)
	{
		auto NextSkillData = m_pSkillData.front();
		if (NextSkillData->fRange >= fDistance)
			SelectAttackData();
	}
	else if(20 == pSkillData->iSkillID)
	{
		if (m_pBlackBoard->bIsEnableEntarnceAttack())
		{
			if (0.66f <= fAnimationRatio)
			{
				m_pBlackBoard->SetEntarnceAttack(false);
				m_pBlackBoard->SetSuperAmor(false);
			}
		}
	}
}

void CTask_ScarletAttack::SecondPhaseNormalAttack()
{
	_float fRandom = m_pGameInstance->Random(0.f, 100.f);
	if (50.f < fRandom)
		NormalAttackPattern();
	else
	{
		SecondPhaseAttack();
	}
}

void CTask_ScarletAttack::SecondPhaseAttack()
{
	_float iRandom = m_pGameInstance->Random(0.f, 100.f);
	if (30 > iRandom)
	{
		m_pSkillData.push(m_pGameManager->Find_SkillData(38)); // Blink Shot1
		m_pSkillData.push(m_pGameManager->Find_SkillData(39)); // Blink Shot2
	}
	else if (60 > iRandom)
	{
		m_pSkillData.push(m_pGameManager->Find_SkillData(53)); // AreaCombo
	}
	else if (90 > iRandom)
	{
		m_pSkillData.push(m_pGameManager->Find_SkillData(55)); // AreaSlash2
	}
	else if (93 > iRandom)
	{
		_float fDistance = m_pBlackBoard->GetTargetDistance();

		if(fDistance >= 10.f)
			m_pSkillData.push(m_pGameManager->Find_SkillData(42)); // Link Break Chance Attack
		else
			m_pSkillData.push(m_pGameManager->Find_SkillData(44)); // Air Dash SpaceCut
	}
	else if (96 > iRandom)
	{
		m_pSkillData.push(m_pGameManager->Find_SkillData(44)); // Air Dash SpaceCut
	}
	else
	{
		m_pSkillData.push(m_pGameManager->Find_SkillData(45)); // Blink Combo1
		m_pSkillData.push(m_pGameManager->Find_SkillData(46)); // Blink Combo2
	}

	SelectAttackData();
}

void CTask_ScarletAttack::SelectAttackMoveData(_uint iID)
{
	MOTION_TIME_DESC Desc = {};

	while (!m_TimeLineDatas.empty())
		m_TimeLineDatas.pop();

	switch (iID)
	{
	case 20:
#pragma region AIR_DASH_CUT_MOTION
	{
		Desc.BeginTime = 0.14f;
		Desc.EndTime = 0.183f;
		Desc.fRange = 1.f;
		Desc.fSpeed = 5.f;
		Desc.fAnimationSpeed = 1.5f;

		Desc.eDirection = DIRECTION::FRONT;
		Desc.bIsTarget = true;
		m_TimeLineDatas.emplace(Desc);

		Desc.BeginTime = 0.224f;
		Desc.EndTime = 0.28f;
		Desc.fRange = 20.f;
		Desc.fSpeed = 10.f;
		Desc.fAnimationSpeed = 1.5f;

		Desc.eDirection = DIRECTION::BACK;
		Desc.bIsTarget = false;
		m_TimeLineDatas.emplace(Desc);

		Desc.BeginTime = 0.33f;
		Desc.EndTime = 0.4f;
		Desc.fRange = 15.f;
		Desc.fSpeed = 10.f;
		Desc.fAnimationSpeed = 1.5f;

		Desc.eDirection = DIRECTION::FRONT;
		Desc.bIsTarget = true;
		m_TimeLineDatas.emplace(Desc);

		Desc.BeginTime = 0.45f;
		Desc.EndTime = 0.55f;
		Desc.fRange = 1.f;
		Desc.fSpeed = 5.f;
		Desc.fAnimationSpeed = 1.5f;

		Desc.eDirection = DIRECTION::FRONT;
		Desc.bIsTarget = true;
		m_TimeLineDatas.emplace(Desc);
	}
	break;
#pragma endregion
	
	case 21 :
#pragma region ATTACK_APPROACH
	{
		Desc.BeginTime = 0.1f;
		Desc.EndTime = 0.3f;
		Desc.fRange = 1.f;
		Desc.fSpeed = 5.f;
		Desc.fAnimationSpeed = 3.f;

		Desc.eDirection = DIRECTION::FRONT;
		Desc.bIsTarget = true;
		m_TimeLineDatas.emplace(Desc);

		Desc.BeginTime = 0.43f;
		Desc.EndTime = 0.5f;
		Desc.fRange = 2.f;
		Desc.fSpeed = 7.f;
		Desc.fAnimationSpeed = 2.f;

		Desc.eDirection = DIRECTION::FRONT;
		Desc.bIsTarget = false;
		m_TimeLineDatas.emplace(Desc);
	}
	break;
#pragma endregion

	case 22: 
#pragma region SWING COMBO1
	{
		Desc.BeginTime = 0.2f;
		Desc.EndTime = 1.f;
		Desc.fRange = 1.f;
		Desc.fSpeed = 2.f;
		Desc.fAnimationSpeed = 2.f;

		Desc.eDirection = DIRECTION::FRONT;
		Desc.bIsTarget = true;
		m_TimeLineDatas.emplace(Desc);
	}
	break;
#pragma endregion
	
	case 23 :
#pragma region SWING COMBO2
	{
		Desc.BeginTime = 0.f;
		Desc.EndTime = 0.26f;
		Desc.fRange = 3.f;
		Desc.fSpeed = 5.f;
		Desc.fAnimationSpeed = 2.f;

		Desc.eDirection = DIRECTION::FRONT;
		Desc.bIsTarget = true;
		m_TimeLineDatas.emplace(Desc);
	}
	break;
#pragma endregion

	case 24: 
#pragma region SWING
	{
		// 25(Swing Fast), 26(Swing Triple) ������ ���Ÿ� �����̴�.
		// 
		// Length : 72
		// Move Frame :  0 ~ 40
		Desc.BeginTime = 0.f;
		Desc.EndTime = 0.55f;
		Desc.fRange = 15.f;
		Desc.fSpeed = 5.f;
		Desc.fAnimationSpeed = 2.f;

		Desc.eDirection = DIRECTION::BACK;
		Desc.bIsTarget = false;
		m_TimeLineDatas.emplace(Desc);
	}
	break;
#pragma endregion
	
	case 27:
#pragma region AIR SPIN
		Desc.BeginTime = 0.15f;
		Desc.EndTime = 0.23f;
		Desc.fRange = 5.f;
		Desc.fSpeed = 5.f;
		Desc.fAnimationSpeed = 2.f;

		Desc.eDirection = DIRECTION::FRONT;
		Desc.bIsTarget = false;
		m_TimeLineDatas.emplace(Desc);

		Desc.BeginTime = 0.36f;
		Desc.EndTime = 0.39f;
		Desc.fRange = 1.f;
		Desc.fSpeed = 7.f;
		Desc.fAnimationSpeed = 2.f;

		Desc.eDirection = DIRECTION::FRONT;
		Desc.bIsTarget = true;
		m_TimeLineDatas.emplace(Desc);
#pragma endregion

	case 28:
#pragma region MOVE BACK SHOT2
	{
		Desc.BeginTime = 0.1f;
		Desc.EndTime = 0.35f;
		Desc.fRange = 20.f;
		Desc.fSpeed = 7.f;
		Desc.fAnimationSpeed = 2.f;

		Desc.eDirection = DIRECTION::BACK;
		Desc.bIsTarget = false;
		m_TimeLineDatas.emplace(Desc);
	}
	break;
#pragma endregion

	case 29 :
#pragma region JUMP STAMP SLASH
		Desc.BeginTime = 0.09f;
		Desc.EndTime = 0.16f;
		Desc.fRange = 1.f;
		Desc.fSpeed = 5.f;
		Desc.fAnimationSpeed = 3.f;

		Desc.eDirection = DIRECTION::FRONT;
		Desc.bIsTarget = true;
		m_TimeLineDatas.emplace(Desc);

		Desc.BeginTime = 0.24f;
		Desc.EndTime = 0.3f;
		Desc.fRange = 10.f;
		Desc.fSpeed = 7.f;
		Desc.fAnimationSpeed = 2.f;

		Desc.eDirection = DIRECTION::BACK;
		Desc.bIsTarget = false;
		m_TimeLineDatas.emplace(Desc);

		Desc.BeginTime = 0.41f;
		Desc.EndTime = 0.45f;
		Desc.fRange = 1.f;
		Desc.fSpeed = 5.f;
		Desc.fAnimationSpeed = 2.f;

		Desc.eDirection = DIRECTION::FRONT;
		Desc.bIsTarget = true;
		m_TimeLineDatas.emplace(Desc);

		Desc.BeginTime = 0.65f;
		Desc.EndTime = 0.74f;
		Desc.fRange = 1.f;
		Desc.fSpeed = 7.f;
		Desc.fAnimationSpeed = 2.f;

		Desc.eDirection = DIRECTION::FRONT;
		Desc.bIsTarget = true;
		m_TimeLineDatas.emplace(Desc);
#pragma endregion

	case 30:
#pragma region KICK STAMP
		Desc.BeginTime = 0.25f;
		Desc.EndTime = 0.32f;
		Desc.fRange = 1.f;
		Desc.fSpeed = 5.f;
		Desc.fAnimationSpeed = 2.f;

		Desc.eDirection = DIRECTION::FRONT;
		Desc.bIsTarget = false;
		m_TimeLineDatas.emplace(Desc);

		Desc.BeginTime = 0.57f;
		Desc.EndTime = 0.61f;
		Desc.fRange = 1.f;
		Desc.fSpeed = 5.f;
		Desc.fAnimationSpeed = 2.f;

		Desc.eDirection = DIRECTION::FRONT;
		Desc.bIsTarget = true;
		m_TimeLineDatas.emplace(Desc);
#pragma endregion

	case 31:
#pragma region PARRY RANGE DASH
	{
		Desc.BeginTime = 0.05f;
		Desc.EndTime = 0.2f;
		Desc.fRange = 15.f;
		Desc.fSpeed = 7.f;
		Desc.fAnimationSpeed = 2.f;

		Desc.eDirection = DIRECTION::LEFT;
		Desc.bIsTarget = false;
		m_TimeLineDatas.emplace(Desc);

		Desc.BeginTime = 0.3f;
		Desc.EndTime = 0.4f;
		Desc.fRange = 1.f;
		Desc.fSpeed = 5.f;
		Desc.fAnimationSpeed = 2.f;

		Desc.eDirection = DIRECTION::FRONT;
		Desc.bIsTarget = true;
		m_TimeLineDatas.emplace(Desc);
	}
	break;
#pragma endregion
	
	case 32:
#pragma region BACK DASH SPACE CUT
	{
		Desc.BeginTime = 0.17f;
		Desc.EndTime = 0.26f;
		Desc.fRange = -4.f;
		Desc.fSpeed = 5.f;
		Desc.fAnimationSpeed = 2.f;

		Desc.eDirection = DIRECTION::FRONT;
		Desc.bIsTarget = true;
		m_TimeLineDatas.emplace(Desc);

		Desc.BeginTime = 0.28f;
		Desc.EndTime = 0.3f;
		Desc.fRange = 4.f;
		Desc.fSpeed = 5.f;
		Desc.fAnimationSpeed = 2.f;

		Desc.eDirection = DIRECTION::FRONT;
		Desc.bIsTarget = true;
		m_TimeLineDatas.emplace(Desc);

		Desc.BeginTime = 0.44f;
		Desc.EndTime = 0.5f;
		Desc.fRange = 20.f;
		Desc.fSpeed = 7.f;
		Desc.fAnimationSpeed = 1.5f;

		Desc.eDirection = DIRECTION::BACK;
		Desc.bIsTarget = false;
		m_TimeLineDatas.emplace(Desc);

		Desc.BeginTime = 0.53f;
		Desc.EndTime = 0.55f;
		Desc.fRange = 1.f;
		Desc.fSpeed = 5.f;
		Desc.fAnimationSpeed = 1.5f;

		Desc.eDirection = DIRECTION::FRONT;
		Desc.bIsTarget = true;
		m_TimeLineDatas.emplace(Desc);
	}
	break;
#pragma endregion

	case 33:
#pragma region BLINK CUT
	{
		Desc.BeginTime = 0.28f;
		Desc.EndTime = 0.44f;
		Desc.fRange = 1.f;
		Desc.fSpeed = 7.f;
		Desc.fAnimationSpeed = 2.f;

		Desc.eDirection = DIRECTION::FRONT;
		Desc.bIsTarget = true;
		m_TimeLineDatas.emplace(Desc);
	}
	break;
#pragma endregion

	case 34:
#pragma region PHASE CHANGE ATTACK 1
	{
		Desc.BeginTime = 0.28f;
		Desc.EndTime = 0.63f;
		Desc.fRange = 1.f;
		Desc.fSpeed = 5.f;
		Desc.fAnimationSpeed = 2.f;

		Desc.eDirection = DIRECTION::FRONT;
		Desc.bIsTarget = true;
		m_TimeLineDatas.emplace(Desc);
	}
	break;
#pragma endregion

	case 35:
#pragma region PHASE CHANGE ATTACK 2
	{
		Desc.BeginTime = 0.f;
		Desc.EndTime = 0.46f;
		Desc.fRange = 1.f;
		Desc.fSpeed = 5.f;
		Desc.fAnimationSpeed = 2.f;

		Desc.eDirection = DIRECTION::FRONT;
		Desc.bIsTarget = true;
		m_TimeLineDatas.emplace(Desc);
	}
	break;
#pragma endregion

	case 36:
#pragma region PHASE CHANGE ATTACK 3
	{
		Desc.BeginTime = 0.15f;
		Desc.EndTime = 0.34f;
		Desc.fRange = 1.f;
		Desc.fSpeed = 5.f;
		Desc.fAnimationSpeed = 2.f;

		Desc.eDirection = DIRECTION::FRONT;
		Desc.bIsTarget = true;
		m_TimeLineDatas.emplace(Desc);
	}
	break;
#pragma endregion

	case 37:
#pragma region PHASE CHANGE ATTACK 4
	{
		Desc.BeginTime = 0.f;
		Desc.EndTime = 0.13f;
		Desc.fRange = 1.f;
		Desc.fSpeed = 5.f;
		Desc.fAnimationSpeed = 2.f;

		Desc.eDirection = DIRECTION::FRONT;
		Desc.bIsTarget = true;
		m_TimeLineDatas.emplace(Desc);

		Desc.BeginTime = 0.52f;
		Desc.EndTime = 0.78f;
		Desc.fRange = 1.f;
		Desc.fSpeed = 5.f;
		Desc.fAnimationSpeed = 2.f;

		Desc.eDirection = DIRECTION::FRONT;
		Desc.bIsTarget = true;
		m_TimeLineDatas.emplace(Desc);
	}
	break;
#pragma endregion

	case 38 :	
#pragma region BLINK SHOT1
	{
		Desc.BeginTime = 0.76f;
		Desc.EndTime = 1.f;
		Desc.fRange = 15.f;
		Desc.fSpeed = 10.f;
		Desc.fAnimationSpeed = 2.f;

		Desc.eDirection = DIRECTION::LEFT_BACK;
		Desc.bIsTarget = false;
		m_TimeLineDatas.emplace(Desc);
	}
	break;
#pragma endregion

	case 40:
#pragma region GroggyCounter
	{
		Desc.BeginTime = 0.16f;
		Desc.EndTime = 0.21f;
		Desc.fRange = 3.f;
		Desc.fSpeed = 5.f;
		Desc.fAnimationSpeed = 2.f;

		Desc.eDirection = DIRECTION::BACK;
		Desc.bIsTarget = false;
		m_TimeLineDatas.emplace(Desc);

		Desc.BeginTime = 0.26f;
		Desc.EndTime = 0.32f;
		Desc.fRange = 1.f;
		Desc.fSpeed = 10.f;
		Desc.fAnimationSpeed = 2.f;

		Desc.eDirection = DIRECTION::FRONT;
		Desc.bIsTarget = true;
		m_TimeLineDatas.emplace(Desc);
	}
	break;
#pragma endregion

	case 41:
#pragma region COUNTER MOVE ATTACK
	{
		Desc.BeginTime = 0.f;
		Desc.EndTime = 0.3f;
		Desc.fRange = 6.f;
		Desc.fSpeed = 4.f;
		Desc.fAnimationSpeed = 2.f;

		Desc.eDirection = DIRECTION::LEFT;
		Desc.bIsTarget = false;
		m_TimeLineDatas.emplace(Desc);

		Desc.BeginTime = 0.46f;
		Desc.EndTime = 0.55f;
		Desc.fRange = 3.f;
		Desc.fSpeed = 5.f;
		Desc.fAnimationSpeed = 2.f;

		Desc.eDirection = DIRECTION::FRONT;
		Desc.bIsTarget = false;
		m_TimeLineDatas.emplace(Desc);
	}
	break;
#pragma endregion

	case 42:
#pragma region LINK BREAK CHANCE ATTACK
	{
		Desc.BeginTime = 0.28f;
		Desc.EndTime = 0.35f;
		Desc.fRange = -5.f;
		Desc.fSpeed = 5.f;
		Desc.fAnimationSpeed = 2.f;

		Desc.eDirection = DIRECTION::FRONT;
		Desc.bIsTarget = true;
		m_TimeLineDatas.emplace(Desc);
	}
	break;

#pragma endregion

	case 44:
#pragma region AIR DASH SAPCE CUT
	{
		Desc.BeginTime = 0.14f;
		Desc.EndTime = 0.18f;
		Desc.fRange = 1.f;
		Desc.fSpeed = 7.f;
		Desc.fAnimationSpeed = 2.f;

		Desc.eDirection = DIRECTION::FRONT;
		Desc.bIsTarget = true;
		m_TimeLineDatas.emplace(Desc);

		Desc.BeginTime = 0.2f;
		Desc.EndTime = 0.3f;
		Desc.fRange = 20.f;
		Desc.fSpeed = 7.f;
		Desc.fAnimationSpeed = 2.f;

		Desc.eDirection = DIRECTION::BACK;
		Desc.bIsTarget = false;
		m_TimeLineDatas.emplace(Desc);

		Desc.BeginTime = 0.53f;
		Desc.EndTime = 0.6f;
		Desc.fRange = 1.f;
		Desc.fSpeed = 7.f;
		Desc.fAnimationSpeed = 2.f;

		Desc.eDirection = DIRECTION::FRONT;
		Desc.bIsTarget = true;
		m_TimeLineDatas.emplace(Desc);
	}
	break;
#pragma endregion

	case 45:
#pragma region BLINK COMBO 1
	{
		Desc.BeginTime = 0.137f;
		Desc.EndTime = 0.17f;
		Desc.fRange = 2.f;
		Desc.fSpeed = 4.f;
		Desc.fAnimationSpeed = 2.f;

		Desc.eDirection = DIRECTION::FRONT;
		Desc.bIsTarget = true;
		m_TimeLineDatas.emplace(Desc);

		Desc.BeginTime = 0.235f;
		Desc.EndTime = 0.276f;
		Desc.fRange = 1.f;
		Desc.fSpeed = 7.f;
		Desc.fAnimationSpeed = 2.f;

		Desc.eDirection = DIRECTION::FRONT;
		Desc.bIsTarget = false;
		m_TimeLineDatas.emplace(Desc);

		Desc.BeginTime = 0.525f;
		Desc.EndTime = 0.55f;
		Desc.fRange = 1.f;
		Desc.fSpeed = 7.f;
		Desc.fAnimationSpeed = 2.f;

		Desc.eDirection = DIRECTION::FRONT;
		Desc.bIsTarget = false;
		m_TimeLineDatas.emplace(Desc);
	}
	break;
#pragma endregion

	case 46:
#pragma region BLINK COMBO 2
	{
		Desc.BeginTime = 0.137f;
		Desc.EndTime = 0.173f;
		Desc.fRange = 1.f;
		Desc.fSpeed = 5.f;
		Desc.fAnimationSpeed = 2.f;

		Desc.eDirection = DIRECTION::FRONT;
		Desc.bIsTarget = true;
		m_TimeLineDatas.emplace(Desc);

		Desc.BeginTime = 0.235f;
		Desc.EndTime = 0.276f;
		Desc.fRange = 1.f;
		Desc.fSpeed = 5.f;
		Desc.fAnimationSpeed = 2.f;

		Desc.eDirection = DIRECTION::FRONT;
		Desc.bIsTarget = true;
		m_TimeLineDatas.emplace(Desc);

		Desc.BeginTime = 0.525f;
		Desc.EndTime = 0.55f;
		Desc.fRange = 5.f;
		Desc.fSpeed = 5.f;
		Desc.fAnimationSpeed = 2.f;

		Desc.eDirection = DIRECTION::FRONT;
		Desc.bIsTarget = false;
		m_TimeLineDatas.emplace(Desc);
	}
	break;
#pragma endregion

	case 48:
#pragma region MOVE BACK
	{
		Desc.BeginTime = 0.15f;
		Desc.EndTime = 0.83f;
		Desc.fRange = 30.f;
		Desc.fSpeed = 3.f;
		Desc.fAnimationSpeed = 2.f;

		Desc.eDirection = DIRECTION::BACK;
		Desc.bIsTarget = false;
		m_TimeLineDatas.emplace(Desc);
	}
	break;
#pragma endregion

	case 49:
#pragma region MOVE SIDE LEFT
	{
		Desc.BeginTime = 0.092f;
		Desc.EndTime = 0.83f;
		Desc.fRange = 15.f;
		Desc.fSpeed = 3.f;
		Desc.fAnimationSpeed = 2.f;

		Desc.eDirection = DIRECTION::LEFT;
		Desc.bIsTarget = false;
		m_TimeLineDatas.emplace(Desc);
	}
	break;
#pragma endregion

	case 50:
#pragma region MOVE SIDE RIGHT
	{
		Desc.BeginTime = 0.092f;
		Desc.EndTime = 0.83f;
		Desc.fRange = 15.f;
		Desc.fSpeed = 3.f;
		Desc.fAnimationSpeed = 2.f;

		Desc.eDirection = DIRECTION::RIGHT;
		Desc.bIsTarget = false;
		m_TimeLineDatas.emplace(Desc);
	}
	break;
#pragma endregion

	case 51:
#pragma region PHASE CHANGE2 ATTACK
	{
		Desc.BeginTime = 0.092f;
		Desc.EndTime = 0.83f;
		Desc.fRange = 20.f;
		Desc.fSpeed = 3.f;
		Desc.fAnimationSpeed = 2.f;

		Desc.eDirection = DIRECTION::RIGHT;
		Desc.bIsTarget = false;
		m_TimeLineDatas.emplace(Desc);
	}
	break;
#pragma endregion

	case 52:
#pragma region STINGGER
	{
		Desc.BeginTime = 0.f;
		Desc.EndTime = 0.14f;
		Desc.fRange = 10.f;
		Desc.fSpeed = 3.f;
		Desc.fAnimationSpeed = 2.f;

		Desc.eDirection = DIRECTION::RIGHT;
		Desc.bIsTarget = false;
		m_TimeLineDatas.emplace(Desc);

		Desc.BeginTime = 0.18f;
		Desc.EndTime = 0.38f;
		Desc.fRange = 1.f;
		Desc.fSpeed = 3.f;
		Desc.fAnimationSpeed = 2.f;

		Desc.eDirection = DIRECTION::FRONT;
		Desc.bIsTarget = true;
		m_TimeLineDatas.emplace(Desc);
	}
	break;
#pragma endregion

	case 53:
#pragma region AREA COMBO
	{
		Desc.BeginTime = 0.22f;
		Desc.EndTime = 0.26f;
		Desc.fRange = 1.f;
		Desc.fSpeed = 3.f;
		Desc.fAnimationSpeed = 2.f;

		Desc.eDirection = DIRECTION::FRONT;
		Desc.bIsTarget = true;
		m_TimeLineDatas.emplace(Desc);

		Desc.BeginTime = 0.365f;
		Desc.EndTime = 0.41f;
		Desc.fRange = 10.f;
		Desc.fSpeed = 3.f;
		Desc.fAnimationSpeed = 2.f;

		Desc.eDirection = DIRECTION::RIGHT_BACK;
		Desc.bIsTarget = false;
		m_TimeLineDatas.emplace(Desc);

		Desc.BeginTime = 0.46f;
		Desc.EndTime = 0.5f;
		Desc.fRange = 5.f;
		Desc.fSpeed = 3.f;
		Desc.fAnimationSpeed = 2.f;

		Desc.eDirection = DIRECTION::RIGHT;
		Desc.bIsTarget = false;
		m_TimeLineDatas.emplace(Desc);

		Desc.BeginTime = 0.52f;
		Desc.EndTime = 0.62f;
		Desc.fRange = 12.f;
		Desc.fSpeed = 10.f;
		Desc.fAnimationSpeed = 2.f;

		Desc.eDirection = DIRECTION::FRONT;
		Desc.bIsTarget = true;
		m_TimeLineDatas.emplace(Desc);
	}
	break;
#pragma endregion

	case 54:
#pragma region AREA SLASH
	{
		Desc.BeginTime = 0.25f;
		Desc.EndTime = 0.34f;
		Desc.fRange = 15.f;
		Desc.fSpeed = 7.f;
		Desc.fAnimationSpeed = 2.f;

		Desc.eDirection = DIRECTION::FRONT;
		Desc.bIsTarget = true;
		m_TimeLineDatas.emplace(Desc);
	}
	break;
#pragma endregion

	case 55:
#pragma region AREA SLASH 2
	{
		Desc.BeginTime = 0.076f;
		Desc.EndTime = 0.14f;
		Desc.fRange = 15.f;
		Desc.fSpeed = 7.f;
		Desc.fAnimationSpeed = 2.f;

		Desc.eDirection = DIRECTION::FRONT;
		Desc.bIsTarget = true;
		m_TimeLineDatas.emplace(Desc);

		Desc.BeginTime = 0.4f;
		Desc.EndTime = 0.5f;
		Desc.fRange = 1.f;
		Desc.fSpeed = 3.f;
		Desc.fAnimationSpeed = 2.f;

		Desc.eDirection = DIRECTION::FRONT;
		Desc.bIsTarget = true;
		m_TimeLineDatas.emplace(Desc);

		Desc.BeginTime = 0.6f;
		Desc.EndTime = 0.73f;
		Desc.fRange = 6.f;
		Desc.fSpeed = 3.f;
		Desc.fAnimationSpeed = 2.f;

		Desc.eDirection = DIRECTION::FRONT;
		Desc.bIsTarget = false;
		m_TimeLineDatas.emplace(Desc);
	}
	break;
#pragma endregion
	}
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

	_float fRatio = Clamp<_float>(fTimeDelta * m_fLerpSpeed, 0.f, 1.f);
	_vector vLerpPos = XMVectorLerp(vOwnerPos, XMLoadFloat3(&m_fAttackMovePoint), fRatio);

	LookAtPoint(fTimeDelta);
	if (fDistance <= 3.5f)
		return;
	m_pOwner->GetTransform()->Set_State(STATE::POSITION, vLerpPos);
}

void CTask_ScarletAttack::LookAtPoint(_float fTimeDelta)
{
	_vector vOwnerPos{}, vTempOwnerPos{}, vTargetPos{};
	vOwnerPos = vTempOwnerPos = m_pOwner->GetTransform()->Get_State(STATE::POSITION);
	vTargetPos = m_pTarget->GetTransform()->Get_State(STATE::POSITION);
	vTempOwnerPos.m128_f32[1] = vTargetPos.m128_f32[1] = 0.f;

	_vector vDir = XMVectorSetY(XMVector3Normalize(vTargetPos - vTempOwnerPos), 0.f);
	if (XMVector3Equal(vDir, XMVectorZero()))
		return;

	m_pOwner->GetTransform()->LookAt_Lerp(vOwnerPos + vDir, fTimeDelta, 20.f);
}

void CTask_ScarletAttack::ResetAttackTask(_bool bIsCoolTime)
{
	if (bIsCoolTime)
		Compute_AttackCoolTime(true);

	while (!m_pSkillData.empty())
		m_pSkillData.pop();

	while (!m_TimeLineDatas.empty())
		m_TimeLineDatas.pop();
}

void CTask_ScarletAttack::Clear_ScarletAttackTask()
{
	if (m_pBlackBoard->IsPhaseLastAttack())
		m_pBlackBoard->SetPhaseLastAttack(false);

	if (m_pBlackBoard->bIsEnableEntarnceAttack())
		m_pBlackBoard->SetEntarnceAttack(false);

	if (m_pBlackBoard->bIsReflectExcution())
		m_pBlackBoard->SetRefelctExcution(false);
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