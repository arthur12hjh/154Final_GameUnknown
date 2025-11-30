#include "pch.h"
#include "MonsterAttackState.h"

#include "GameInstance.h"
#include "MonsterStateMimesis.h"
#include "Nayitba.h"

CMonsterAttackState::CMonsterAttackState() :
	CState()
{
}

HRESULT CMonsterAttackState::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CMonsterAttackState::Start(void* pArg, CState* pPreState)
{
	auto pEntity = static_cast<CNayitba*>(m_pOwner);
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
	if (bIsRandomAttack)
	{
		m_pSkillData = pEntity->GetSkillData(false);
		m_szAnimationName = m_pSkillData->szAnimationName;
	}

	ReadySetting();

	m_bIsEnableChange = false;
	_float fAttackSpeed = m_pGameInstance->Random(1.f, 1.7f);
	pEntity->Set_Animation(m_szAnimationName.c_str(), false, fAttackSpeed);
}

void CMonsterAttackState::Update(_float fTimeDelta)
{
	// 여기서 공격 분기
	auto pEntity = static_cast<CNayitba*>(m_pOwner);
	SearchTargetDistance();

	if (m_bIsPattern)
	{
		switch (m_StaticMonsterData->iMonsetID)
		{
		case 2 :
			BeholderPattern(fTimeDelta);
			break;
		case 4:
			StatueAPattern(fTimeDelta);
			break;
		case 5:
			StatueBPattern(fTimeDelta);
			break;
		}	
	}

	m_bIsFinished = pEntity->Play_Animation(fTimeDelta);

	if (m_bIsFinished)
	{
		m_AttackCompletedFunc(0.f);
		m_bIsEnableChange = true;
	}
}

void CMonsterAttackState::End()
{

}

void CMonsterAttackState::ReadySetting()
{
	auto pEntity = static_cast<CNayitba*>(m_pOwner);
	auto pOwnerInfo = &pEntity->GetMonsterData();

	switch (m_StaticMonsterData->iMonsetID)
	{
	case 2:
	case 4:
	case 5:
	{
		_vector vOwnerPos = m_pOwner->GetTransform()->Get_State(STATE::POSITION);
		_vector vTargetPos = m_pTarget->GetTransform()->Get_State(STATE::POSITION);

		vOwnerPos.m128_f32[1] = vTargetPos.m128_f32[1] = 0.f;
		_vector vDir = XMVector3Normalize(vTargetPos - vOwnerPos);
		XMStoreFloat3(&m_vMoveDir, vDir);

		_float fLength = XMVectorGetX(XMVector3Length(vTargetPos - vOwnerPos));
		_float fRangeRatio = fLength / pOwnerInfo->fAttackRange;
		if (0.4f >= fRangeRatio)
			m_fMoveSpeed *= 0.3f;
		else if (0.6f >= fRangeRatio)
			m_fMoveSpeed *= 0.5f;
		else
			m_fMoveSpeed *= 0.8f;

		m_bIsPattern = true;
	}
	break;
	}
}

void CMonsterAttackState::BeholderPattern(_float fTimeDelta)
{
	auto pEntity = static_cast<CNayitba*>(m_pOwner);
	_float fAnimPlayRatio = pEntity->Get_AnimationRatio();
	_float fSpeed = m_fMoveSpeed;
	_bool bMoveAction = false;

	if (512 == m_pSkillData->iSkillID)
	{
		if (0.1f <= fAnimPlayRatio && 0.3 >= fAnimPlayRatio)
			bMoveAction = true;
	}
	else if (513 == m_pSkillData->iSkillID)
	{
		if (0.05f <= fAnimPlayRatio && 0.45f >= fAnimPlayRatio)
			bMoveAction = true;
	}
	else if(511 == m_pSkillData->iSkillID)
	{
		if (0.25f <= fAnimPlayRatio && 0.46f >= fAnimPlayRatio)
		{
			bMoveAction = true;
		}
		else if (0.46f <= fAnimPlayRatio && 0.6f >= fAnimPlayRatio)
		{
			fSpeed = m_fMoveSpeed - 1.f;
			bMoveAction = true;
		}
	}

	if (bMoveAction)
		LerpMoveAction(fTimeDelta, fSpeed);
}

void CMonsterAttackState::StatueAPattern(_float fTimeDelta)
{
	auto pEntity = static_cast<CNayitba*>(m_pOwner);
	_float fAnimPlayRatio = pEntity->Get_AnimationRatio();
	_vector vOwnerPos = m_pOwner->GetTransform()->Get_State(STATE::POSITION);

	_float fSpeed = m_fMoveSpeed;
	_bool bMoveAction = false;

	if (nullptr == m_pSkillData)
		return;

	if (521 == m_pSkillData->iSkillID)
	{
		//잡기 공격
		if (0.1f <= fAnimPlayRatio && 0.3 >= fAnimPlayRatio)
			bMoveAction = true;
	}
	else if (522 == m_pSkillData->iSkillID)
	{
		// Slash 공격
		// 0 ~ 40 프레임
		if (0.0f <= fAnimPlayRatio && 0.24f >= fAnimPlayRatio)
			bMoveAction = true;

		// 40 ~ 60 프레임
		if (0.24f <= fAnimPlayRatio && 0.36f >= fAnimPlayRatio)
			bMoveAction = true;
	}
	else if (523 == m_pSkillData->iSkillID)
	{
		// Rush Slash 공격
		// 30 ~ 80 프레임
		if (0.15f <= fAnimPlayRatio && 0.4f >= fAnimPlayRatio)
		{
			_vector vLerpLook = LerpRotation(fTimeDelta);
			m_pOwner->GetTransform()->LookAt(vOwnerPos + vLerpLook);
			bMoveAction = true;
		}
	}
	else if (524 == m_pSkillData->iSkillID)
	{
		// Double Slash
		// 20 ~ 42 프레임
		if (0.07f <= fAnimPlayRatio && 0.15f >= fAnimPlayRatio)
		{
			fSpeed = m_fMoveSpeed - 1.f;
			bMoveAction = true;
		}
		// 44 ~ 90 프레임
		else if (0.16f <= fAnimPlayRatio && 0.32f >= fAnimPlayRatio)
		{
			fSpeed = m_fMoveSpeed - 1.f;
			bMoveAction = true;
		}
		// 109 ~ 140 프레임
		else if (0.4f <= fAnimPlayRatio && 0.51f >= fAnimPlayRatio)
		{
			//m_fRootMotionRatio = 1.f;
		}
	}
	else if (525 == m_pSkillData->iSkillID)
	{
		// 기습 공격
		// 6 ~ 20 프레임 회전
		if (0.38f <= fAnimPlayRatio && 0.5f >= fAnimPlayRatio)
		{
			fSpeed = m_fMoveSpeed;
			bMoveAction = true;
		}
		// 100 ~ 130 프레임 이동
		else if(0.25f >= fAnimPlayRatio)
		{
			_vector vOwnerPos = m_pOwner->GetTransform()->Get_State(STATE::POSITION);
			_vector vLerpLook = LerpRotation(fTimeDelta);

			m_pOwner->GetTransform()->LookAt(vOwnerPos + vLerpLook);
		}
	}

	if (bMoveAction)
		LerpMoveAction(fTimeDelta, fSpeed);
}

void CMonsterAttackState::StatueBPattern(_float fTimeDelta)
{
	auto pEntity = static_cast<CNayitba*>(m_pOwner);
	_float fAnimPlayRatio = pEntity->Get_AnimationRatio();
	_vector vOwnerPos = m_pOwner->GetTransform()->Get_State(STATE::POSITION);

	_float fSpeed = m_fMoveSpeed;
	_bool bMoveAction = false;

	if (531 == m_pSkillData->iSkillID)
	{
		// Explosion
		// 10 ~ 30 프레임
		if (0.05f <= fAnimPlayRatio && 0.12 >= fAnimPlayRatio)
			bMoveAction = true;

		// 40 ~ 70 프레임
		if (0.15f <= fAnimPlayRatio && 0.27 >= fAnimPlayRatio)
			bMoveAction = true;
	}
	else if (532 == m_pSkillData->iSkillID)
	{
		// Rush Slash
		// 60 ~ 110 프레임
		if (0.26f <= fAnimPlayRatio && 0.48f >= fAnimPlayRatio)
			bMoveAction = true;
	}
	else if (533 == m_pSkillData->iSkillID)
	{
		// Slash
		// 20 ~ 40
		if (0.13f <= fAnimPlayRatio && 0.24f >= fAnimPlayRatio)
		{
			bMoveAction = true;
		}
		// 40 ~ 55
		else if (0.24f <= fAnimPlayRatio && 0.34f >= fAnimPlayRatio)
		{
			fSpeed = m_fMoveSpeed - 1.f;
			bMoveAction = true;
		}
	}
	else if (534 == m_pSkillData->iSkillID)
	{
		// Rush Slash
		// 30 ~ 80
		if (0.15f <= fAnimPlayRatio && 0.4f >= fAnimPlayRatio)
		{
			_vector vLerpLook = LerpRotation(fTimeDelta);
			m_pOwner->GetTransform()->LookAt(vOwnerPos + vLerpLook);
			bMoveAction = true;
		}
	}
	else if (535 == m_pSkillData->iSkillID)
	{
		// Rush Double Slash
		// 50 ~ 80
		if (0.18f <= fAnimPlayRatio && 0.3f >= fAnimPlayRatio)
		{
			bMoveAction = true;
		}
		else if (0.43f <= fAnimPlayRatio && 0.55f >= fAnimPlayRatio)
		{
			fSpeed = m_fMoveSpeed - 1.f;
			bMoveAction = true;
		}
	}
	else if (536 == m_pSkillData->iSkillID)
	{
		// Slash Triple
		// 65 ~ 120
		if (0.22f <= fAnimPlayRatio && 0.44f >= fAnimPlayRatio)
		{
			bMoveAction = true;
		}
		// 125 ~ 170
		else if (0.46f <= fAnimPlayRatio && 0.62f >= fAnimPlayRatio)
		{
			fSpeed = m_fMoveSpeed - 1.f;
			bMoveAction = true;
		}
	}
	else if (537 == m_pSkillData->iSkillID)
	{
		// 기습 공격
		// 55 ~ 65 회전
		if (0.25f < fAnimPlayRatio && 0.3f >= fAnimPlayRatio )
		{
			
			// 지수 제곱
			// 지수 상수 값 * Time을해서 제곱을 구함 
			_vector vLerpLook = LerpRotation(fTimeDelta);
			m_pOwner->GetTransform()->LookAt(vOwnerPos + vLerpLook);
		}
		// 125 ~ 170
		else if (0.3f <= fAnimPlayRatio && 0.4f >= fAnimPlayRatio)
		{
			fSpeed = m_fMoveSpeed - 1.f;
			bMoveAction = true;
		}
	}

	if (bMoveAction)
	{
		LerpMoveAction(fTimeDelta, fSpeed);
	}
		
}

void CMonsterAttackState::SearchTargetDistance()
{
	_vector vOwnerPos = m_pOwner->GetTransform()->Get_State(STATE::POSITION);
	_vector vTargetPos = m_pTarget->GetTransform()->Get_State(STATE::POSITION);

	vOwnerPos.m128_f32[1] = vTargetPos.m128_f32[1] = 0.f;
	m_fDistance =	XMVectorGetX(XMVector3Length(vTargetPos - vOwnerPos));
}

void CMonsterAttackState::LerpMoveAction(_float fTimeDelta, _float fSpeed)
{
	if (m_StaticMonsterData->fAttackRange - m_pSkillData->fRange >= m_fDistance)
		return;

	m_pOwner->GetTransform()->Move_Direction(fTimeDelta, XMLoadFloat3(&m_vMoveDir), fSpeed);
}

_vector CMonsterAttackState::LerpRotation(_float fRatio, _float fSpeed)
{
	_vector vOwnerPos = m_pOwner->GetTransform()->Get_State(STATE::POSITION);
	_vector vTargetPos = m_pTarget->GetTransform()->Get_State(STATE::POSITION);

	_vector vLook = m_pOwner->GetTransform()->Get_State(STATE::LOOK);
	_vector vDir = XMVector3Normalize(vTargetPos - vOwnerPos);

	vLook.m128_f32[1] = vDir.m128_f32[1] = 0.f;
	return XMVectorLerp(vLook, vDir, fRatio);
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
