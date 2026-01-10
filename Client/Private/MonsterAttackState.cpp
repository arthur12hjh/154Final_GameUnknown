#include "pch.h"
#include "MonsterAttackState.h"

#include "GameInstance.h"
#include "MonsterStateMimesis.h"
#include "Player.h"
#include "Nayitba.h"

CMonsterAttackState::CMonsterAttackState() :
	CState()
{
	m_iStateID = 1;
}

HRESULT CMonsterAttackState::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CMonsterAttackState::Start(void* pArg, CState* pPreState)
{
	auto pEntity = static_cast<CNaytiba*>(m_pOwner);
	m_StaticMonsterData = pEntity->GetStaticMonsterData();
	auto pOwnerInfo = &pEntity->GetMonsterData();

	m_fMoveSpeed = m_StaticMonsterData->fMoveSpeed;
	MONSTER_ATTACK_DESC* pDesc = static_cast<MONSTER_ATTACK_DESC*>(pArg);
	m_AttackCompletedFunc = pDesc->AttackCompletedFunc;
	m_pTarget = pDesc->pTarget;

	_bool bIsRandomAttack = true;
	if (AI_TYPE::PASSIVE == m_StaticMonsterData->eAI_Type)
	{
		auto pMimesisState = dynamic_cast<CMonsterStateMimesis*>(pPreState);
		if (nullptr != pMimesisState)
		{
			m_pSkillData = pEntity->GetSkillData(false, ENUM_CLASS(SKILL_TYPE::MIMESIS_SKILL));
			if (nullptr == m_pSkillData)
			{
				m_bIsFinished = true;
				return;
			}

			m_szAnimationName = m_pSkillData->szAnimationName;
			bIsRandomAttack = false;
		}
	}

	_vector vOwnerPos = {};
	SearchTargetDistance();

	if (bIsRandomAttack)
	{
		m_pSkillData = pEntity->GetSkillData(ENUM_CLASS(SKILL_TYPE::DEFAULT_SKILL), false);
		if (m_fDistance < m_pSkillData->fRange * 0.5f)
		{
			while (633 == m_pSkillData->iSkillID)
				m_pSkillData = pEntity->GetSkillData(ENUM_CLASS(SKILL_TYPE::DEFAULT_SKILL), false);
		}
	}

	ReadySetting();
	pEntity->SetAttackData(m_pSkillData);
	if (m_fDistance >= m_pSkillData->fRange && 0 != pEntity->GetStaticMonsterData()->fMoveSpeed)
	{
		m_szAnimationName = pEntity->GetStaticMonsterData()->szAnimationName;
		m_szAnimationName += "_Run_L";
		pEntity->Set_Animation(m_szAnimationName.c_str(), true, 1.f, m_StaticMonsterData->fLerpRatio);

		if (10.f > m_fDistance)
		{
			m_fPlayRatio = 1.f;
			m_fMoveSpeed = m_StaticMonsterData->fMoveSpeed;
		}
		else
		{
			m_fPlayRatio = 1.3f;
			m_fMoveSpeed = m_StaticMonsterData->fMoveSpeed * 1.3f;
		}


		m_bIsMoveAction = true;
	}
	else
		pEntity->Set_Animation(m_pSkillData->szAnimationName, false, 1.f, m_StaticMonsterData->fLerpRatio, true);

	m_bIsEnableChange = false;
}

void CMonsterAttackState::Update(_float fTimeDelta)
{
	// 여기서 공격 분기
	auto pEntity = static_cast<CNaytiba*>(m_pOwner);
	SearchTargetDistance();

	if (m_fDistance <= m_pSkillData->fRange && m_bIsMoveAction)
	{
		pEntity->Set_Animation(m_pSkillData->szAnimationName, false, 1.f, m_StaticMonsterData->fLerpRatio, true);
		m_bIsMoveAction = false;
	}
	
	if(m_bIsMoveAction)
	{
		m_pOwner->GetTransform()->LookAt(LerpRotation(fTimeDelta, 2.f));
		m_pOwner->GetTransform()->Move_Direction(fTimeDelta, m_pOwner->GetTransform()->Get_State(STATE::LOOK), m_fMoveSpeed);
	}
	else
	{
		m_fPlayRatio = 2.f;
		if (m_bIsPattern)
		{
			switch (m_StaticMonsterData->iMonsetID)
			{
			case 2:
				BeholderPattern(fTimeDelta);
				break;
			case 3:
				BanaclePattern(fTimeDelta);
				break;
			case 4:
				StatueAPattern(fTimeDelta);
				break;
			case 5:
				StatueBPattern(fTimeDelta);
				break;
			case 6:
				SunFlowerPattern(fTimeDelta);
				break;
			case 7:
				Minion11Pattern(fTimeDelta);
				break;
			case 9:
				TentaclePattern(fTimeDelta);
				break;
			case 10:
				DroidTurretPattern(fTimeDelta);
				break;
			}
		}
	}

	m_bIsFinished = pEntity->Play_Animation(fTimeDelta * m_fPlayRatio);
	if (m_bIsFinished)
	{
		m_AttackCompletedFunc(0.f);
		m_bIsEnableChange = true;
	}
}

void CMonsterAttackState::End()
{
	auto pEntity = static_cast<CNaytiba*>(m_pOwner);
	
	pEntity->SetAttackData(nullptr);
}

void CMonsterAttackState::ReadySetting()
{
	auto pEntity = static_cast<CNaytiba*>(m_pOwner);
	auto pOwnerInfo = &pEntity->GetMonsterData();

	switch (m_StaticMonsterData->iMonsetID)
	{
	case 2:
	case 3:
	case 4:
	case 5:
	case 6:
	case 7:
	case 9:
	case 10:
	{
		_vector vOwnerPos = m_pOwner->GetTransform()->Get_State(STATE::POSITION);
		_vector vTargetPos = m_pTarget->GetTransform()->Get_State(STATE::POSITION);

		vOwnerPos.m128_f32[1] = vTargetPos.m128_f32[1] = 0.f;
		_vector vDir = XMVector3Normalize(vTargetPos - vOwnerPos);
		XMStoreFloat3(&m_vMoveDir, vDir);

		_float fLength = XMVectorGetX(XMVector3Length(vTargetPos - vOwnerPos));
		_float fRangeRatio = fLength / pOwnerInfo->fAttackRange;
		if (0.4f >= fRangeRatio)
		{
			m_fMoveSpeed *= 0.3f;
			m_fPlayRatio = 2.f;
		}
		else if (0.8f >= fRangeRatio)
		{
			m_fMoveSpeed *= 0.5f;
			m_fPlayRatio = 2.f; 
		}
		else
		{
			m_fMoveSpeed *= 0.8f;
			m_fPlayRatio = 2.f;
		}

		m_bIsPattern = true;
	}
	break;
	}
}

void CMonsterAttackState::BeholderPattern(_float fTimeDelta)
{
	auto pEntity = static_cast<CNaytiba*>(m_pOwner);
	_float fAnimPlayRatio = pEntity->Get_AnimationRatio();
	_float fSpeed = m_StaticMonsterData->fMoveSpeed;
	_bool bMoveAction = false;
	DIRECTION eType = DIRECTION::FRONT;

	if (512 == m_pSkillData->iSkillID)
	{
		if (0.1f <= fAnimPlayRatio && 0.3 >= fAnimPlayRatio)
		{
			m_fMoveAnimMaxRatio = 0.3f;
			bMoveAction = true;
		}
	}
	else if (513 == m_pSkillData->iSkillID)
	{
		if (0.05f <= fAnimPlayRatio && 0.45f >= fAnimPlayRatio)
		{
			m_fMoveAnimMaxRatio = 0.45f;
			bMoveAction = true;
		}
	}
	else if(511 == m_pSkillData->iSkillID)
	{
		if (0.13f >= fAnimPlayRatio)
		{
			m_fMoveAnimMaxRatio = 0.3f;
			fSpeed = 5.f;
			eType = DIRECTION::BACK;
			bMoveAction = true;
		}

		if (0.25f <= fAnimPlayRatio && 0.46f >= fAnimPlayRatio)
		{
			m_fMoveAnimMaxRatio = 0.45f;
			bMoveAction = true;
		}
		else if (0.46f <= fAnimPlayRatio && 0.6f >= fAnimPlayRatio)
		{
			m_fMoveAnimMaxRatio = 0.6f;
			fSpeed = m_fMoveSpeed - 1.f;
			bMoveAction = true;
		}
	}

	if (bMoveAction)
		LerpMoveAction(fTimeDelta, fSpeed, eType);
}

void CMonsterAttackState::BanaclePattern(_float fTimeDelta)
{
	auto pEntity = static_cast<CNaytiba*>(m_pOwner);
	_float fAnimPlayRatio = pEntity->Get_AnimationRatio();
	_float fSpeed = m_StaticMonsterData->fMoveSpeed;
	_bool bMoveAction = false;

	if (541 == m_pSkillData->iSkillID)
	{
		// Punch Attack
		if (0.2f >= fAnimPlayRatio)
		{
			// 이동 프레임 : 0 ~ 60
			m_fMoveAnimMaxRatio = 0.2f;
			bMoveAction = true;
		}
	}
	else if (542 == m_pSkillData->iSkillID)
	{
		// TraceAttack
		if (0.28f >= fAnimPlayRatio)
		{	
			// 이동 프레임 : 0 ~ 60
			m_fMoveAnimMaxRatio = 0.28f;
			bMoveAction = true;
		}
	}
	else if (543 == m_pSkillData->iSkillID)
	{
		// Combo Attack
		if (0.2f >= fAnimPlayRatio)
		{	
			// 이동 프레임 : 0 ~ 60
			m_fMoveAnimMaxRatio = 0.2f;
			bMoveAction = true;
		}
		else if (0.34f <= fAnimPlayRatio && 0.46f >= fAnimPlayRatio)
		{	
			// 이동 프레임 : 100 ~ 120
			m_fMoveAnimMaxRatio = 0.41f;
			bMoveAction = true;
		}
		else if (0.55f <= fAnimPlayRatio && 0.62f >= fAnimPlayRatio)
		{	
			// 이동 프레임 : 160 ~ 180
			m_fMoveAnimMaxRatio = 0.62f;
			bMoveAction = true;
		}
	}

	if (bMoveAction)
		LerpMoveAction(fTimeDelta, fSpeed);
}

void CMonsterAttackState::StatueAPattern(_float fTimeDelta)
{
	auto pEntity = static_cast<CNaytiba*>(m_pOwner);
	_float fAnimPlayRatio = pEntity->Get_AnimationRatio();
	_vector vOwnerPos = m_pOwner->GetTransform()->Get_State(STATE::POSITION);

	_float fSpeed = m_StaticMonsterData->fMoveSpeed;
	_bool bMoveAction = false;

	if (nullptr == m_pSkillData)
		return;

	if (521 == m_pSkillData->iSkillID)
	{
		//잡기 공격
		if (0.15f <= fAnimPlayRatio && 0.4 >= fAnimPlayRatio)
		{
			m_fMoveAnimMaxRatio = 0.4f;
			bMoveAction = true;
		}
	}
	else if (522 == m_pSkillData->iSkillID)
	{
		// Slash 공격
		// 0 ~ 40 프레임
		if (0.36f >= fAnimPlayRatio)
		{
			m_fMoveAnimMaxRatio = 0.36f;
			bMoveAction = true;
		}
	}
	else if (523 == m_pSkillData->iSkillID)
	{
		// Rush Slash 공격
		// 30 ~ 80 프레임
		if (0.15f <= fAnimPlayRatio && 0.4f >= fAnimPlayRatio)
		{
			m_fMoveAnimMaxRatio = 0.4f;
			bMoveAction = true;
		}
	}
	else if (524 == m_pSkillData->iSkillID)
	{
		// Double Slash
		// 20 ~ 42 프레임
		if (0.07f <= fAnimPlayRatio && 0.15f >= fAnimPlayRatio)
		{
			m_fMoveAnimMaxRatio = 0.15f;
			fSpeed = m_fMoveSpeed - 1.f;
			bMoveAction = true;
		}
		// 44 ~ 90 프레임
		else if (0.16f <= fAnimPlayRatio && 0.32f >= fAnimPlayRatio)
		{
			m_fMoveAnimMaxRatio = 0.32f;
			fSpeed = m_fMoveSpeed - 1.f;
			bMoveAction = true;
		}
		// 109 ~ 140 프레임
		else if (0.4f <= fAnimPlayRatio && 0.51f >= fAnimPlayRatio)
		{
			m_fMoveAnimMaxRatio = 0.51f;
			//m_fRootMotionRatio = 1.f;
		}
	}
	else if (525 == m_pSkillData->iSkillID)
	{
		// 기습 공격
		// 6 ~ 20 프레임 회전
		if (0.38f <= fAnimPlayRatio && 0.5f >= fAnimPlayRatio)
		{
			m_fMoveAnimMaxRatio = 0.5f;
			fSpeed = m_fMoveSpeed;
			bMoveAction = true;
		}
	}

	if (bMoveAction)
		LerpMoveAction(fTimeDelta, fSpeed);
}

void CMonsterAttackState::StatueBPattern(_float fTimeDelta)
{
	auto pEntity = static_cast<CNaytiba*>(m_pOwner);
	_float fAnimPlayRatio = pEntity->Get_AnimationRatio();
	_vector vOwnerPos = m_pOwner->GetTransform()->Get_State(STATE::POSITION);

	_float fSpeed = m_StaticMonsterData->fMoveSpeed;
	_bool bMoveAction = false;

	if (531 == m_pSkillData->iSkillID)
	{
		// Explosion
		// 10 ~ 30 프레임
		if (0.05f <= fAnimPlayRatio && 0.12 >= fAnimPlayRatio)
		{
			m_fMoveAnimMaxRatio = 0.12f;
			bMoveAction = true;
		}

		// 40 ~ 70 프레임
		if (0.15f <= fAnimPlayRatio && 0.27 >= fAnimPlayRatio)
		{
			m_fMoveAnimMaxRatio = 0.27f;
			bMoveAction = true;
		}
	}
	else if (534 == m_pSkillData->iSkillID)
	{
		// Rush Slash
		// 60 ~ 110 프레임
		if (0.12f <= fAnimPlayRatio && 0.24f >= fAnimPlayRatio)
		{
			m_fMoveAnimMaxRatio = 0.24f;
			bMoveAction = true;
		}

		if (0.4f <= fAnimPlayRatio && 0.5f >= fAnimPlayRatio)
		{
			m_fMoveAnimMaxRatio = 0.4f;
			bMoveAction = true;
		}
	}
	else if (532 == m_pSkillData->iSkillID)
	{
		// Slash
		// 20 ~ 40
		if (0.13f <= fAnimPlayRatio && 0.2f >= fAnimPlayRatio)
		{
			m_fMoveAnimMaxRatio = 0.2f;
			bMoveAction = true;
		}
	}
	else if (533 == m_pSkillData->iSkillID)
	{
		// Rush Slash
		// 30 ~ 80
		if (0.15f <= fAnimPlayRatio && 0.4f >= fAnimPlayRatio)
		{
			m_fMoveAnimMaxRatio = 0.4f;
			bMoveAction = true;
		}
	}
	else if (535 == m_pSkillData->iSkillID)
	{
		// Slash Triple
		// 65 ~ 120
		if (0.22f <= fAnimPlayRatio && 0.3f >= fAnimPlayRatio)
		{
			m_fMoveAnimMaxRatio = 0.3f;
			bMoveAction = true;
		}
		// 125 ~ 170
		else if (0.46f <= fAnimPlayRatio && 0.65f >= fAnimPlayRatio)
		{
			m_fMoveAnimMaxRatio = 0.65f;
			bMoveAction = true;
		}

	}
	else if (536 == m_pSkillData->iSkillID)
	{
		// 기습 공격
		// 55 ~ 65 회전
		// 125 ~ 170
		if (0.3f <= fAnimPlayRatio && 0.35f >= fAnimPlayRatio)
		{
			m_fMoveAnimMaxRatio = 0.35f;
			fSpeed = m_fMoveSpeed - 1.f;
			bMoveAction = true;
		}
	}

	if (bMoveAction)
		LerpMoveAction(fTimeDelta, fSpeed);
}

void CMonsterAttackState::SunFlowerPattern(_float fTimeDelta)
{
	auto pEntity = static_cast<CNaytiba*>(m_pOwner);
	_float fAnimPlayRatio = pEntity->Get_AnimationRatio();
	_vector vOwnerPos = m_pOwner->GetTransform()->Get_State(STATE::POSITION);

	_float fSpeed = m_StaticMonsterData->fMoveSpeed;
	_bool bMoveAction = false;

	if (631 == m_pSkillData->iSkillID)
	{
		// 0 ~ 42
		if (0.58f <= fAnimPlayRatio && 0.7f >= fAnimPlayRatio)
		{
			m_fMoveAnimMaxRatio = 0.7f;
			fSpeed = m_fMoveSpeed - 1.f;
			bMoveAction = true;
		}
	}
	else if (632 == m_pSkillData->iSkillID)
	{
		
		// 0 ~75
		if (0.25f >= fAnimPlayRatio)
		{
			m_fMoveAnimMaxRatio = 0.25f;
			fSpeed = m_fMoveSpeed - 1.f;
			bMoveAction = true;
		}

		// 0 ~ 75 회전
		// 120 ~ 230
		if (0.4f <= fAnimPlayRatio && 0.76f >= fAnimPlayRatio)
		{
			m_fMoveAnimMaxRatio = 0.76f;
			fSpeed = m_fMoveSpeed - 1.f;
			bMoveAction = true;
		}
	}

	if (bMoveAction)
		LerpMoveAction(fTimeDelta, fSpeed);
}

void CMonsterAttackState::Minion11Pattern(_float fTimeDelta)
{
	auto pEntity = static_cast<CNaytiba*>(m_pOwner);
	_float fAnimPlayRatio = pEntity->Get_AnimationRatio();
	_vector vOwnerPos = m_pOwner->GetTransform()->Get_State(STATE::POSITION);

	_float fSpeed = m_StaticMonsterData->fMoveSpeed;
	_bool bMoveAction = false;

	if (732 == m_pSkillData->iSkillID)
	{
		// 150 ~ 175
		if (0.58f <= fAnimPlayRatio && 0.7f >= fAnimPlayRatio)
		{
			m_fMoveAnimMaxRatio = 0.7f;
			fSpeed = m_fMoveSpeed - 1.f;
			bMoveAction = true;
		}
	}

	if (bMoveAction)
		LerpMoveAction(fTimeDelta, fSpeed);
}

void CMonsterAttackState::TentaclePattern(_float fTimeDelta)
{
	auto pEntity = static_cast<CNaytiba*>(m_pOwner);
	_float fAnimPlayRatio = pEntity->Get_AnimationRatio();
	_vector vOwnerPos = m_pOwner->GetTransform()->Get_State(STATE::POSITION);
	_bool bMoveAction = false;

	if (741 == m_pSkillData->iSkillID)
	{
		// 0 ~ 40
		if (0.5f >= fAnimPlayRatio)
		{
			m_fMoveAnimMaxRatio = 0.5f;
			m_fPlayRatio = 1.f;
			bMoveAction = true;
		}
	}
	else if (742 == m_pSkillData->iSkillID)
	{
		// 0 ~ 40
		if (0.34f >= fAnimPlayRatio)
		{
			m_fMoveAnimMaxRatio = 0.34f;
			bMoveAction = true;
		}
	}
	else if (743 == m_pSkillData->iSkillID)
	{
		// 0 ~ 40
		if (0.17f >= fAnimPlayRatio)
		{
			m_fMoveAnimMaxRatio = 0.17f;
			bMoveAction = true;
		}
	}

	if (bMoveAction)
		LerpMoveAction(fTimeDelta, 0.f);
}

void CMonsterAttackState::DroidTurretPattern(_float fTimeDelta)
{
	auto pEntity = static_cast<CNaytiba*>(m_pOwner);
	_float fAnimPlayRatio = pEntity->Get_AnimationRatio();
	_vector vOwnerPos = m_pOwner->GetTransform()->Get_State(STATE::POSITION);
	DIRECTION eType = DIRECTION::FRONT;
	_bool bMoveAction = false;

	if (751 == m_pSkillData->iSkillID)
	{
		// 0 ~ 40
		if (0.5f >= fAnimPlayRatio)
		{
			m_fMoveAnimMaxRatio = 0.5f;
			m_fPlayRatio = 1.f;
			bMoveAction = true;
		}
	}
	else if (753 == m_pSkillData->iSkillID)
	{
		// 0 ~ 122
		if (0.3f >= fAnimPlayRatio)
		{
			m_fMoveAnimMaxRatio = 0.3f;
			eType = DIRECTION::BACK;
			bMoveAction = true;
		}
	}

	if (bMoveAction)
		LerpMoveAction(fTimeDelta, m_fMoveSpeed, eType);
}

void CMonsterAttackState::SearchTargetDistance()
{
	_vector vOwnerPos = m_pOwner->GetTransform()->Get_State(STATE::POSITION);
	_vector vTargetPos = m_pTarget->GetTransform()->Get_State(STATE::POSITION);

	vOwnerPos.m128_f32[1] = vTargetPos.m128_f32[1] = 0.f;
	m_fDistance =	XMVectorGetX(XMVector3Length(vTargetPos - vOwnerPos));
}

void CMonsterAttackState::LerpMoveAction(_float fTimeDelta, _float fSpeed, DIRECTION eType)
{
	auto pEntity = static_cast<CNaytiba*>(m_pOwner);
	_float fAnimPlayRatio = pEntity->Get_AnimationRatio();

	_bool bIsLookAt = true;
	auto pPlayer = dynamic_cast<CPlayer*>(m_pTarget);
	if (pPlayer)
	{
		if (pPlayer->Get_Desc()->isUsingBlink)
			bIsLookAt = false;
	}

	if(bIsLookAt)
		m_pOwner->GetTransform()->LookAt(LerpRotation(fTimeDelta, 5.f));

	if (m_StaticMonsterData->fAttackRange >= m_fDistance)
		return;
	
	fSpeed = fSpeed * (m_fDistance / m_StaticMonsterData->fAttackRange);
	//fSpeed = Clamp<_float>(fSpeed, 0.f, m_StaticMonsterData->fMoveSpeed);
	if(DIRECTION::FRONT == eType)
		m_pOwner->GetTransform()->Move_Direction(fTimeDelta, m_pOwner->GetTransform()->Get_State(STATE::LOOK), fSpeed);

	if (DIRECTION::BACK == eType)
		m_pOwner->GetTransform()->Move_Direction(fTimeDelta, -1.f * m_pOwner->GetTransform()->Get_State(STATE::LOOK), fSpeed);
}

_vector CMonsterAttackState::LerpRotation(_float fRatio, _float fSpeed)
{
	_vector vOwnerPos{}, vTempPos{}, vTargetPos{};
	vOwnerPos = vTempPos = m_pOwner->GetTransform()->Get_State(STATE::POSITION);
	vTargetPos = m_pTarget->GetTransform()->Get_State(STATE::POSITION);
	vTempPos.m128_f32[1] = vTargetPos.m128_f32[1] = 0.f;

	_vector vLook = m_pOwner->GetTransform()->Get_State(STATE::LOOK);
	vLook.m128_f32[1] = 0.f;

	_vector vDir = XMVector3Normalize(vTargetPos - vTempPos);
	return vOwnerPos + XMVectorLerp(vLook, vDir, fRatio * fSpeed);
}

CMonsterAttackState* CMonsterAttackState::Create(void* pArg)
{
	CMonsterAttackState* pMonsterAttackState = new CMonsterAttackState();
	if (FAILED(pMonsterAttackState->Initialize(pArg)))
	{
		Safe_Release(pMonsterAttackState);
		MSG_BOX("Create Fail : Monster Attack State");
	}
	return pMonsterAttackState;
}

void CMonsterAttackState::Free()
{
	__super::Free();
}
