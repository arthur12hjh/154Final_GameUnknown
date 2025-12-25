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

	m_fAnimationSpeed = 2.f;
	AttackActionAmount(fTimeDelta);
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

void CTask_ScarletAttack::SelectAttackData()
{
	if (m_pSkillData.empty())
		return;

	auto pSkillData = m_pSkillData.front();
	m_pSkillData.pop();

	m_pBlackBoard->SetAttackData(pSkillData);
	m_pOwner->Set_Animation(pSkillData->szAnimationName, false, 1.f, 0.f, true);
}

_bool CTask_ScarletAttack::SelectPattern(_bool bIsRandom)
{
	m_pBlackBoard->SetCurState(CBossBlackBoard::BOSS_STATE::ATTACK);
	if (false == m_pBlackBoard->IsPhaseLastAttack())
	{
		if (m_pBlackBoard->bIsEnableEntarnceAttack())
		{
			EntranceAttack();
		}
		else
		{
			CBossBlackBoard::BOSS_PAHSE ePhase = m_pBlackBoard->Get_BossPhase();
			if (false == m_pBlackBoard->IsParryAttack())
			{
	/*			m_pSkillData.push(m_pGameManager->Find_SkillData(42));
				SelectAttackData();*/
				if (CBossBlackBoard::BOSS_PAHSE::SECOND == ePhase)
					SecondPhaseNormalAttack();
				else
					NormalAttackPattern();
			}
			else
			{
				LinkAttackPattern();
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

	if (22 == pSkill_Data->iSkillID)
	{
		if (fDistance >= m_pBlackBoard->GetBossDefaultInfo()->fAttackRange)
			bIsSelectAttack = false;
	}
	else if (27 == pSkill_Data->iSkillID || 30 == pSkill_Data->iSkillID)
	{
		if (fDistance >= m_pBlackBoard->GetBossDefaultInfo()->fAttackRange * 1.5f)
			bIsSelectAttack = false;
	}
	else if (23 == pSkill_Data->iSkillID)
	{
		if (fDistance >= m_pBlackBoard->GetBossDefaultInfo()->fAttackRange * 1.3f)
			m_pSkillData.push(m_pGameManager->Find_SkillData(22));
	}
	else if (25 == pSkill_Data->iSkillID || 26 == pSkill_Data->iSkillID)
	{
		if (fDistance < m_pBlackBoard->GetBossDefaultInfo()->fAttackRange * 2.f)
			bIsSelectAttack = false;
	}
	
	if (bIsSelectAttack)
	{
		m_pSkillData.push(pSkill_Data);
		SelectAttackData();
	}
	else
		NormalAttackPattern();
}

void CTask_ScarletAttack::LinkAttackPattern()
{
	
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
		m_pSkillData.push(m_pGameManager->Find_SkillData(42)); // Link Break Chance Attack
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

_bool CTask_ScarletAttack::AttackActionAmount(_float fTimeDelta)
{
	auto pNaytibaStaticData = m_pOwner->GetStaticMonsterData();
	if (nullptr == pNaytibaStaticData)
		return false;

	_bool  bIsMove{ false }, bIsLerpMove{ false };

	_vector vOwnerPos{}, vTempOwnerPos{};
	vTempOwnerPos = vOwnerPos = m_pOwner->GetTransform()->Get_State(STATE::POSITION);

	_vector vTargetPos{}, vTempTargetPos{};
	vTargetPos = vTempTargetPos = m_pTarget->GetTransform()->Get_State(STATE::POSITION);

	vOwnerPos.m128_f32[1] = vTargetPos.m128_f32[1] = 0.f;
	_vector vDir = XMVector3Normalize(vTargetPos - vOwnerPos);
	_vector vReverseDir = -1 * vDir;

	m_bIsLookAtPoint = true;
	auto pAttackData = m_pBlackBoard->GetAttackData();
	_float fSpeed = m_pBlackBoard->GetBossDefaultInfo()->fMoveSpeed;

	if (pAttackData)
	{
		_float fAnimationRatio = m_pOwner->Get_AnimationRatio();
		switch (pAttackData->iSkillID)
		{
		case 20:  // AirDashCut
		{
			// Length : 468
			// Move Frame : 66 ~ 86
			if (0.14f <= fAnimationRatio && 0.183f >= fAnimationRatio)
			{
				m_fMoveAnimMaxRatio = 0.183f;
				m_fLerpSpeed = 5.f;

				if (0.155f > fAnimationRatio)
					XMStoreFloat3(&m_fAttackMovePoint, vTempTargetPos + vReverseDir);
				bIsLerpMove = true;
			}
			if (0.224f <= fAnimationRatio && 0.262f >= fAnimationRatio)
			{
				// Move Frame : 105 ~ 123
				m_fMoveAnimMaxRatio = 0.262f;
				m_fLerpSpeed = 10.f;

				if (0.239f > fAnimationRatio)
					XMStoreFloat3(&m_fAttackMovePoint, vTempOwnerPos + m_pOwner->GetTransform()->Get_State(STATE::LOOK) * -10.f);

				m_bIsLookAtPoint = false;
				bIsLerpMove = true;
			}
			if (0.318f <= fAnimationRatio && 0.34f >= fAnimationRatio)
			{
				// Move Frame : 149 ~ 158
				m_fMoveAnimMaxRatio = 0.35f;
				m_fLerpSpeed = 5.f;

				if(0.32f > fAnimationRatio)
					XMStoreFloat3(&m_fAttackMovePoint, vTempTargetPos + vDir * 30.f);

				bIsLerpMove = true;
			}
			if (0.35f <= fAnimationRatio && 0.5f >= fAnimationRatio)
			{
				LookAtPoint(fTimeDelta);
				m_fLerpSpeed = 5.f;
				// Move Frame : 224 ~ 234
				if (0.46f < fAnimationRatio)
				{
					m_fMoveAnimMaxRatio = 0.5f;
					

					if (0.475f > fAnimationRatio)
						XMStoreFloat3(&m_fAttackMovePoint, vTempTargetPos + vReverseDir);
					bIsLerpMove = true;
				}
					
			}
		}
		break;
		case 21 :  //Attack Approach
		{
			// Length : 228
			// Move Frame : 21 ~ 50
			if (0.1f <= fAnimationRatio && 0.3f >= fAnimationRatio)
			{
				m_fMoveAnimMaxRatio = 0.2f;

				if (0.175f > fAnimationRatio)
					XMStoreFloat3(&m_fAttackMovePoint, vTempTargetPos + vReverseDir);
				m_fLerpSpeed = 5.f;
				m_fAnimationSpeed = 3.f;
				bIsLerpMove = true;
			}
			if (0.46f <= fAnimationRatio && 0.5f >= fAnimationRatio)
			{
				// Move Frame : 105 ~ 116
				m_fMoveAnimMaxRatio = 0.5f;
				m_fLerpSpeed = 7.f;
				m_fAnimationSpeed = 1.5f;

				if (0.475f > fAnimationRatio)
					XMStoreFloat3(&m_fAttackMovePoint, vTempOwnerPos + vDir * 2.f);

				bIsLerpMove = true;
			}
		}
			break;
		case 22:  //Swing Combo 1
		{
			// Length : 48
			m_fMoveAnimMaxRatio = 1.f;
			XMStoreFloat3(&m_vDir, m_pOwner->GetTransform()->Get_State(STATE::LOOK) * 3.f);
			bIsMove = true;

			if (7.f > m_pBlackBoard->GetTargetDistance())
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
			// 25(Swing Fast), 26(Swing Triple) ������ ���Ÿ� �����̴�.
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
			// Move Frame :  72 ~ 104
			if (0.15f <= fAnimationRatio && 0.23f >= fAnimationRatio)
			{
				m_fMoveAnimMaxRatio = 0.23f;
				if (0.17f > fAnimationRatio)
					XMStoreFloat3(&m_fAttackMovePoint, vTempTargetPos + vReverseDir);

				m_fLerpSpeed = 5.f;
				bIsLerpMove = true;
			}
			if (0.36f <= fAnimationRatio && 0.39f >= fAnimationRatio) // Move Frame : 161 ~ 172
			{
				m_fMoveAnimMaxRatio = 0.39f;
				if (0.375f > fAnimationRatio)
					XMStoreFloat3(&m_fAttackMovePoint, vTempTargetPos + vReverseDir);

				m_fLerpSpeed = 7.f;
				bIsLerpMove = true;
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
				m_fLerpSpeed = 5.f;
				if (0.1f > fAnimationRatio)
				{
					_vector vDir = XMVector3Normalize(m_pOwner->GetTransform()->Get_State(STATE::LOOK)) * -10.f;
					XMStoreFloat3(&m_fAttackMovePoint, (vTempOwnerPos + vDir));
				}

				m_bIsLookAtPoint = false;
				bIsLerpMove = true;
			}
		}
		break;
		case 29: // Jump Stamp Slash
		{
			// Length : 390
			// Move Frame : 0 ~ 90
			if (0.09f <= fAnimationRatio && 0.16f >= fAnimationRatio)
			{
				// Move Frame : 37 ~ 63
				m_fMoveAnimMaxRatio = 0.16f;
				m_fLerpSpeed = 5.f;

				if (0.15f > fAnimationRatio)
					XMStoreFloat3(&m_fAttackMovePoint, (vTempTargetPos + vReverseDir));
				m_fAnimationSpeed = 3.f;
				bIsLerpMove = true;
			}
			if (0.24f <= fAnimationRatio && 0.3f >= fAnimationRatio) // Back Step
			{
				// Move Frame : 97 ~ 120
				m_fMoveAnimMaxRatio = 0.3f;
				m_fLerpSpeed = 7.f;
				m_bIsLookAtPoint = false;

				if (0.255f > fAnimationRatio)
					XMStoreFloat3(&m_fAttackMovePoint, (vTempOwnerPos + m_pOwner->GetTransform()->Get_State(STATE::LOOK) * -10.f));
				
				m_bIsLookAtPoint = false;
				bIsLerpMove = true;
			}
			if (0.41f <= fAnimationRatio && 0.45f >= fAnimationRatio)
			{
				// Move Frame : 160 ~ 178
				// Move Frame : 0 ~ 90
				m_fMoveAnimMaxRatio = 0.45f;
				m_fLerpSpeed = 5.f;

				if (0.415f > fAnimationRatio)
					XMStoreFloat3(&m_fAttackMovePoint, (vTempTargetPos + vReverseDir));
				bIsLerpMove = true;
			}
			if (0.65f <= fAnimationRatio && 0.74f >= fAnimationRatio)
			{
				// Move Frame : 255 ~ 290
				m_fMoveAnimMaxRatio = 0.74f;
				m_fLerpSpeed = 7.f;

				if (0.665f > fAnimationRatio)
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
				XMStoreFloat3(&m_vDir, m_pOwner->GetTransform()->Get_State(STATE::LOOK));
				bIsMove = true;
			}
			if (0.57f <= fAnimationRatio && 0.61f >= fAnimationRatio)
			{
				// Move Frame : 144 ~ 154
				m_fMoveAnimMaxRatio = 0.61f;

				if (0.585f > fAnimationRatio)
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
			if (0.3f <= fAnimationRatio && 0.41f >= fAnimationRatio)
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
			if (0.28f <= fAnimationRatio && 0.3f >= fAnimationRatio)
			{
				// Move Frame : 135 ~ 147
				m_fMoveAnimMaxRatio = 0.3f;
				XMStoreFloat3(&m_vDir, m_pOwner->GetTransform()->Get_State(STATE::LOOK));
				bIsMove = true;
			}
			if (0.44f <= fAnimationRatio && 0.47f >= fAnimationRatio)
			{
				// Move Frame : 256 ~ 265
				m_fMoveAnimMaxRatio = 0.47f;
				m_fLerpSpeed = 7.f;

				if (0.45f > fAnimationRatio)
					XMStoreFloat3(&m_fAttackMovePoint, (vTempOwnerPos + m_pOwner->GetTransform()->Get_State(STATE::LOOK) * -13.f));

				m_bIsLookAtPoint = false;
				bIsLerpMove = true;
			}
			if (0.53f <= fAnimationRatio && 0.55f >= fAnimationRatio)
			{
				// Move Frame : 256 ~ 265
				m_fMoveAnimMaxRatio = 0.55f;
				m_fLerpSpeed = 5.f;
				m_bIsLookAtPoint = false;
				bIsLerpMove = true;

				if (0.545f > fAnimationRatio)
					XMStoreFloat3(&m_fAttackMovePoint, (vTempTargetPos + vReverseDir));
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

				if (0.295f > fAnimationRatio)
					XMStoreFloat3(&m_fAttackMovePoint, vTempTargetPos + vReverseDir);
				m_fLerpSpeed = 7.f;
				bIsLerpMove = true;
			}
		}
		break;
		case 34: // PhaseChagne1L_Attack1
		{
			// Length : 63
			// Move Frame : 18 ~ 40
			if (0.28f <= fAnimationRatio && 0.63f >= fAnimationRatio)
			{
				m_fMoveAnimMaxRatio = 0.63f;

				if (0.295f > fAnimationRatio)
					XMStoreFloat3(&m_fAttackMovePoint, vTempTargetPos + vReverseDir);
				m_fLerpSpeed = 5.f;
				bIsLerpMove = true;
			}
		}
		break;
		case 35: // PhaseChagne1L_Attack2
		{
			// Length : 87
			// Move Frame : 0 ~ 40
			if (0.46f >= fAnimationRatio)
			{
				m_fMoveAnimMaxRatio = 0.46f;

				if (0.15f > fAnimationRatio)
					XMStoreFloat3(&m_fAttackMovePoint, vTempTargetPos + vReverseDir);
				m_fLerpSpeed = 5.f;
				bIsLerpMove = true;
			}
		}
		break;
		case 36: // PhaseChagne1L_Attack3
		{
			// Length : 117
			// Move Frame : 18 ~ 40
			if (0.15f <= fAnimationRatio && 0.34f >= fAnimationRatio)
			{
				m_fMoveAnimMaxRatio = 0.34f;

				if (0.165f > fAnimationRatio)
					XMStoreFloat3(&m_fAttackMovePoint, vTempTargetPos + vReverseDir);
				m_fLerpSpeed = 5.f;
				bIsLerpMove = true;
			}
		}
		break;
		case 37: // PhaseChagne1L_Attack4
		{
			// Length : 153
			// Move Frame : 0 ~ 20
			if (0.13f >= fAnimationRatio)
			{
				m_fMoveAnimMaxRatio = 0.13;

				if (0.05f > fAnimationRatio)
					XMStoreFloat3(&m_fAttackMovePoint, vTempTargetPos + vReverseDir);
				m_fLerpSpeed = 5.f;
				bIsLerpMove = true;
			}
			else if (0.52f <= fAnimationRatio && 0.78f >= fAnimationRatio)
			{
				// Move Frame : 80 ~ 120
				m_fMoveAnimMaxRatio = 0.78f;
				if (0.53f > fAnimationRatio)
					XMStoreFloat3(&m_fAttackMovePoint, vTempTargetPos + vReverseDir);

				m_fLerpSpeed = 5.f;
				bIsLerpMove = true;
			}
		}
		break;
		case 38: // BlinkShot1
		{
			// Length : 78
			// Move Frame : 60 ~ 78
			if (0.76f <= fAnimationRatio && 1.f >= fAnimationRatio)
			{
				m_fMoveAnimMaxRatio = 1.f;
				if (0.79f > fAnimationRatio)
				{
					_vector vRight = m_pOwner->GetTransform()->Get_State(STATE::RIGHT);
					_vector vLook = m_pOwner->GetTransform()->Get_State(STATE::LOOK);
					_vector vPointDir = vLook + vRight;

					XMStoreFloat3(&m_fAttackMovePoint, vTempOwnerPos + vPointDir * -10.f );
				}
				m_fLerpSpeed = 10.f;
				m_bIsLookAtPoint = false;
				bIsLerpMove = true;
			}
		}
		break;
		case 41: // CounterMoveAttack
		{
			// Length : 129
			// Move Frame : 0 ~ 20
			if (0.15f >= fAnimationRatio)
			{
				m_fMoveAnimMaxRatio = 0.15f;
				m_fLerpSpeed = 2.f;
				if (0.1f > fAnimationRatio)
					XMStoreFloat3(&m_fAttackMovePoint, vTempOwnerPos + m_pOwner->GetTransform()->Get_State(STATE::RIGHT) * -4.f);
				
				m_bIsLookAtPoint = false;
				bIsLerpMove = true;
			}
			if (0.46f <= fAnimationRatio && 0.55f >= fAnimationRatio)
			{
				// Move Frame : 60 ~ 70
				m_fMoveAnimMaxRatio = 0.55f;
				m_fLerpSpeed = 5.f;
				if (0.49f > fAnimationRatio)
					XMStoreFloat3(&m_fAttackMovePoint, vTempOwnerPos + m_pOwner->GetTransform()->Get_State(STATE::LOOK) * 2.f);
				bIsLerpMove = true;
			}
		}
		break;
		case 42: // LinkBreakChanceAttack
		{
			// Length : 282
			// Move Frame : 80 ~ 100
			if (0.28f <= fAnimationRatio && 0.35f >= fAnimationRatio)
			{
				// Move Frame : 60 ~ 70
				m_fMoveAnimMaxRatio = 0.35f;
				if (0.29f > fAnimationRatio)
					XMStoreFloat3(&m_fAttackMovePoint, vTempTargetPos + vReverseDir * 13.f);
				m_fLerpSpeed = 7.f;

				m_bIsLookAtPoint = false;
				bIsLerpMove = true;
			}
		}
		break;
		case 44: // AirDashSpaceCut
		{
			// Length : 489
			// Move Frame : 60 ~ 85
			if (0.16f <= fAnimationRatio && 0.173f >= fAnimationRatio)
			{
				m_fMoveAnimMaxRatio = 0.173f;
				if (0.16f > fAnimationRatio)
					XMStoreFloat3(&m_fAttackMovePoint, vTempTargetPos + vReverseDir);
				m_fLerpSpeed = 7.f;
				bIsLerpMove = true;
			}
			if (0.224f <= fAnimationRatio && 0.286f >= fAnimationRatio)
			{
				// Move Frame : 110 ~ 140
				m_fMoveAnimMaxRatio = 0.286f;
				if (0.286f > fAnimationRatio)
					XMStoreFloat3(&m_fAttackMovePoint, vTempOwnerPos * m_pOwner->GetTransform()->Get_State(STATE::LOOK) * -20.f);

				m_fLerpSpeed = 7.f;
				bIsLerpMove = true;
			}
			if (0.53f <= fAnimationRatio && 0.556f >= fAnimationRatio)
			{
				// Move Frame : 260 ~ 272
				m_fMoveAnimMaxRatio = 0.556f;
				if (0.24f > fAnimationRatio)
					XMStoreFloat3(&m_fAttackMovePoint, vTempTargetPos + vReverseDir);
				
				m_fLerpSpeed = 7.f;
				bIsLerpMove = true;
			}
		}
		break;
		case 45: // AirDashSpaceCut
		{
			// Length : 489
			// Move Frame : 67 ~ 85
			if (0.137f <= fAnimationRatio && 0.173f >= fAnimationRatio)
			{
				m_fMoveAnimMaxRatio = 0.173f;
				if (0.14f > fAnimationRatio)
					XMStoreFloat3(&m_fAttackMovePoint, vTempTargetPos + vReverseDir);
				m_fLerpSpeed = 7.f;
				bIsLerpMove = true;
			}
			if (0.235f <= fAnimationRatio && 0.276f >= fAnimationRatio)
			{
				// Move Frame : 148 ~ 158
				m_fMoveAnimMaxRatio = 0.276f;
				if (0.24f > fAnimationRatio)
					XMStoreFloat3(&m_fAttackMovePoint, vTempTargetPos + vReverseDir);

				m_fLerpSpeed = 7.f;
				bIsLerpMove = true;
			}
			if (0.525f <= fAnimationRatio && 0.55f >= fAnimationRatio)
			{
				// Move Frame : 217 ~ 235
				m_fMoveAnimMaxRatio = 0.55f;
				m_fLerpSpeed = 7.f;
				if (0.53f > fAnimationRatio)
					XMStoreFloat3(&m_fAttackMovePoint, vTempOwnerPos + m_pOwner->GetTransform()->Get_State(STATE::LOOK));
				
				bIsLerpMove = true;
			}
		}
		break;
		case 46: // AirDashSpaceCut
		{
			// Length : 489
			// Move Frame : 67 ~ 85
			if (0.137f <= fAnimationRatio && 0.173f >= fAnimationRatio)
			{
				m_fMoveAnimMaxRatio = 0.173f;
				if (0.14f > fAnimationRatio)
					XMStoreFloat3(&m_fAttackMovePoint, vTempTargetPos + vReverseDir);
				m_fLerpSpeed = 5.f;
				bIsLerpMove = true;
			}
			if (0.235f <= fAnimationRatio && 0.276f >= fAnimationRatio)
			{
				// Move Frame : 148 ~ 158
				m_fMoveAnimMaxRatio = 0.276f;
				if (0.24f > fAnimationRatio)
					XMStoreFloat3(&m_fAttackMovePoint, vTempTargetPos + vReverseDir);

				m_fLerpSpeed = 5.f;
				bIsLerpMove = true;
			}
			if (0.525f <= fAnimationRatio && 0.55f >= fAnimationRatio)
			{
				// Move Frame : 217 ~ 235
				m_fMoveAnimMaxRatio = 0.55f;
				m_fLerpSpeed = 7.f;
				if (0.53f > fAnimationRatio)
					XMStoreFloat3(&m_fAttackMovePoint, vTempTargetPos + vReverseDir);
				bIsLerpMove = true;
			}
		}
		break;
		case 48: // Move BackS
		{
			// Length : 54
			// Move Frame : 17 ~ 45
			if (0.15f <= fAnimationRatio && 0.83f >= fAnimationRatio)
			{
				m_fMoveAnimMaxRatio = 0.83f;

				if (0.18f > fAnimationRatio)
					XMStoreFloat3(&m_fAttackMovePoint, vTempOwnerPos + m_pOwner->GetTransform()->Get_State(STATE::LOOK) * -30.f);
				m_fLerpSpeed = 3.f;

				m_bIsLookAtPoint = false;
				bIsLerpMove = true;
			}
		}
		break;
		case 49: // Move SideL
		{
			// Length : 54
			// Move Frame : 5 ~ 45
			if (0.092f <= fAnimationRatio && 0.83f >= fAnimationRatio)
			{
				m_fMoveAnimMaxRatio = 0.83f;

				if (0.14f > fAnimationRatio)
					XMStoreFloat3(&m_fAttackMovePoint, vTempOwnerPos + m_pOwner->GetTransform()->Get_State(STATE::RIGHT) * -15.f);
				m_fLerpSpeed = 2.f;
				m_bIsLookAtPoint = false;
				bIsLerpMove = true;
			}
		}
		break;
		case 50: // Move SideR
		{
			// Length : 54
			// Move Frame : 5 ~ 45
			if (0.092f <= fAnimationRatio && 0.83f >= fAnimationRatio)
			{
				m_fMoveAnimMaxRatio = 0.83f;

				if (0.14f > fAnimationRatio)
					XMStoreFloat3(&m_fAttackMovePoint, vTempOwnerPos + m_pOwner->GetTransform()->Get_State(STATE::RIGHT) * 15.f);
				m_fLerpSpeed = 2.f;
				m_bIsLookAtPoint = false;
				bIsLerpMove = true;
			}
		}
		break;
		case 51: // PhaseChange2_Attack
		{
			// Length : 132
			// Move Frame : 5 ~ 45
			if (0.092f <= fAnimationRatio && 0.83f >= fAnimationRatio)
			{
				m_fMoveAnimMaxRatio = 0.83f;

				if (0.14f > fAnimationRatio)
					XMStoreFloat3(&m_fAttackMovePoint, vTempOwnerPos + m_pOwner->GetTransform()->Get_State(STATE::RIGHT) * 15.f);
				m_fLerpSpeed = 2.f;
				m_bIsLookAtPoint = false;
				bIsLerpMove = true;
			}
		}
		break;
		case 52: // Stinger
		{
			// Length : 255
			// Move Frame : 0 ~ 36
			if (0.14f >= fAnimationRatio)
			{
				// Move Frame : 47 ~ 97
				m_fMoveAnimMaxRatio = 0.14f;

				if (0.07f > fAnimationRatio)
					XMStoreFloat3(&m_fAttackMovePoint, vTempOwnerPos + m_pOwner->GetTransform()->Get_State(STATE::RIGHT) * 5.f);
				m_fLerpSpeed = 3.f;
				m_bIsLookAtPoint = false;
				bIsLerpMove = true;
			}
			if (0.18f <= fAnimationRatio && 0.38f >= fAnimationRatio)
			{
				// Move Frame : 47 ~ 97
				m_fMoveAnimMaxRatio = 0.38f;

				if (0.2f > fAnimationRatio)
					XMStoreFloat3(&m_fAttackMovePoint, vTempTargetPos + vReverseDir);
				m_fLerpSpeed = 2.f;
				bIsLerpMove = true;
			}
		}
		break;
		case 53: // AreaCombo
		{
			// Length : 591
			// Move Frame : 130 ~ 156
			if (0.22f <= fAnimationRatio && 0.26f >= fAnimationRatio)
			{
				m_fMoveAnimMaxRatio = 0.26f;

				if (0.235f > fAnimationRatio)
					XMStoreFloat3(&m_fAttackMovePoint, vTempTargetPos + vReverseDir);
				m_fLerpSpeed = 2.f;
				bIsLerpMove = true;
			}
			if (0.365f <= fAnimationRatio && 0.41f >= fAnimationRatio) // Back
			{
				// Move Frame : 216 ~ 245
				m_fMoveAnimMaxRatio = 0.41f;

				if (0.38f > fAnimationRatio)
				{
					_vector vRight = m_pOwner->GetTransform()->Get_State(STATE::RIGHT);
					_vector vLook = m_pOwner->GetTransform()->Get_State(STATE::LOOK);
					_vector vMoveDir = vRight + (vLook * -1.f);

					XMStoreFloat3(&m_fAttackMovePoint, vTempOwnerPos + vMoveDir * 10.f);
				}
				m_fLerpSpeed = 2.f;
				m_bIsLookAtPoint = false;
				bIsLerpMove = true;
			}
			if (0.46f <= fAnimationRatio && 0.5f >= fAnimationRatio)
			{
				// Move Frame : 272 ~ 300
				m_fMoveAnimMaxRatio = 0.5f;

				if (0.475f > fAnimationRatio)
					XMStoreFloat3(&m_fAttackMovePoint, vTempOwnerPos + m_pOwner->GetTransform()->Get_State(STATE::RIGHT) * 3.f);
				m_fLerpSpeed = 2.f;
				m_bIsLookAtPoint = false;
				bIsLerpMove = true;
			}
			if (0.52f <= fAnimationRatio && 0.62f >= fAnimationRatio)
			{
				// Move Frame : 310 ~ 324
				m_fMoveAnimMaxRatio = 0.57f;

				if (0.522f > fAnimationRatio)
					XMStoreFloat3(&m_fAttackMovePoint, vTempTargetPos + vDir * 10.f);

				m_bIsLookAtPoint = false;
				m_fLerpSpeed = 10.f;
				bIsLerpMove = true;
				
			}
		}
		break;
		case 54: // AreaSlash
		{
			// Length : 147
			// Move Frame : 5 ~ 45
			if (0.25f <= fAnimationRatio && 0.34f >= fAnimationRatio)
			{
				// Move Frame : 310 ~ 324
				m_fMoveAnimMaxRatio = 0.34f;

				if (0.27f > fAnimationRatio)
					XMStoreFloat3(&m_fAttackMovePoint, vTempTargetPos + vDir * 15.f);
				m_fLerpSpeed = 7.f;
				m_bIsLookAtPoint = false;
				bIsLerpMove = true;
			}
		}
		break;
		case 55: // AreaSlash2
		{
			// Length : 354
			// Move Frame : 27 ~ 50
			if (0.076f <= fAnimationRatio && 0.14f >= fAnimationRatio)
			{
				m_fMoveAnimMaxRatio = 0.14f;

				if (0.08f > fAnimationRatio)
					XMStoreFloat3(&m_fAttackMovePoint, vTempTargetPos + vDir * 15.f);
				m_fLerpSpeed = 7.f;
				m_bIsLookAtPoint = false;
				bIsLerpMove = true;
			}
			if (0.4f <= fAnimationRatio && 0.5f >= fAnimationRatio)
			{
				// Move Frame : 136 ~ 180
				m_fMoveAnimMaxRatio = 0.5f;

				if (0.43f > fAnimationRatio)
					XMStoreFloat3(&m_fAttackMovePoint, vTempTargetPos + vReverseDir);
				m_fLerpSpeed = 3.f;
				bIsLerpMove = true;
			}
			if (0.6f <= fAnimationRatio && 0.73f >= fAnimationRatio)
			{
				// Move Frame : 215 ~ 250
				m_fMoveAnimMaxRatio = 0.73f;
				XMStoreFloat3(&m_vDir, vDir);
				m_fLerpSpeed = 3.f;
				bIsMove = true;
			}
		}
		break;
		}
	}

	if (bIsLerpMove)
		AttackLerpMove(fTimeDelta);
	else if (bIsMove)
		AttackADDMove(fTimeDelta, fSpeed);

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

	_float fRatio = Clamp<_float>(fTimeDelta * m_fLerpSpeed, 0.f, 1.f);
	_vector vLerpPos = XMVectorLerp(vOwnerPos, XMLoadFloat3(&m_fAttackMovePoint), fRatio);

	if (m_bIsLookAtPoint)
		m_pOwner->GetTransform()->LookAt_Lerp(vLerpPos, fTimeDelta, m_fLerpSpeed);
	else
		LookAtPoint(fTimeDelta);

	if (fDistance <= 3.5f)
		return;
	m_pOwner->GetTransform()->Set_State(STATE::POSITION, vLerpPos);
}

void CTask_ScarletAttack::AttackADDMove(_float fTimeDelta, _float fSpeed)
{
	m_pBlackBoard->SetTargetDistacne();
	_float fDistance = m_pBlackBoard->GetTargetDistance();
	auto pSkill_Data = m_pBlackBoard->GetAttackData();
	LookAtPoint(fTimeDelta);

	if (nullptr == pSkill_Data || 3.f >= fDistance - pSkill_Data->fRange)
		return;
	
	_float fAnimPlayRatio = m_pOwner->Get_AnimationRatio();
	fSpeed = fSpeed * (fDistance / pSkill_Data->fRange) * (m_fMoveAnimMaxRatio / fAnimPlayRatio);
	fSpeed = Clamp<_float>(fSpeed, 0.f, 10.f);
	m_pOwner->GetTransform()->Move_Direction(fTimeDelta, XMLoadFloat3(&m_vDir), fSpeed);
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

}

void CTask_ScarletAttack::Clear_ScarletAttackTask()
{
	if (m_pBlackBoard->IsPhaseLastAttack())
		m_pBlackBoard->SetPhaseLastAttack(false);

	if (m_pBlackBoard->bIsEnableEntarnceAttack())
		m_pBlackBoard->SetEntarnceAttack(false);
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