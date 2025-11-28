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
			m_pSkillData = pEntity->GetSkillData(ENUM_CLASS(SKILL_TYPE::MIMESIS_SKILL), false);
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
		m_pSkillData = pEntity->GetSkillData(ENUM_CLASS(SKILL_TYPE::DEFAULT_SKILL), false);
		m_szAnimationName = m_pSkillData->szAnimationName;
	}

	GaraSetting();
	//GaraHitBox();

	m_bIsEnableChange = false;
	_float fAttackSpeed = m_pGameInstance->Random(1.f, 1.7f);
	pEntity->Set_Animation(m_szAnimationName.c_str(), false, fAttackSpeed);
}

void CMonsterAttackState::Update(_float fTimeDelta)
{
	// 여기서 공격 분기
	auto pEntity = static_cast<CNayitba*>(m_pOwner);

	if (m_bIsGara)
	{
		switch (m_StaticMonsterData->iMonsetID)
		{
		case 2 :
			BeholderGara(fTimeDelta);
			break;
		case 4:
			StatueAGara(fTimeDelta);
			break;
		case 5:
			StatueBGara(fTimeDelta);
			break;
		}

		if (m_bIsRootGara)
			m_bIsFinished = pEntity->Play_Animation(fTimeDelta, m_pOwner->GetTransform(), 1.f);
		else
			m_bIsFinished = pEntity->Play_Animation(fTimeDelta);
	}
	else
	{
		m_bIsFinished = pEntity->Play_Animation(fTimeDelta, m_pOwner->GetTransform(), 1.f);
	}

	if (m_bIsFinished)
	{
		m_AttackCompletedFunc(0.f);
		m_bIsEnableChange = true;
	}
}

void CMonsterAttackState::End()
{

}

void CMonsterAttackState::GaraSetting()
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

		m_bIsGara = true;
	}
	break;
	}
}

//void CMonsterAttackState::GaraHitBox()
//{
//	auto pEntity = static_cast<CNayitba*>(m_pOwner);
//	auto pOwnerInfo = &pEntity->GetMonsterData();
//
//	_vector vOwnerPos = m_pOwner->GetTransform()->Get_State(STATE::POSITION);
//	_vector vOwnerLook = m_pOwner->GetTransform()->Get_State(STATE::LOOK);
//	// 충돌 충돌 충돌
//	CAttackHitBox::HIT_BOX_DESC HitBoxDesc = {};
//
//	// 이걸로 트랜스폼 바꿀지 말지 결정해서 True 면 초기세팅들어가고
//	// false 면 초기세팅 안들어갑니다.
//	HitBoxDesc.bIsApplyTransform = true;
//
//	// 위치 바꾸기
//	XMStoreFloat3(&HitBoxDesc.vPosition, vOwnerPos + vOwnerLook * (pOwnerInfo->fAttackRange * 0.5f));
//
//	// 크기 바꾸기
//	HitBoxDesc.vScale = { 2.f , 2.f, 2.f };
//
//	// 넘길 데이터 구조체
//	HitBoxDesc.pData = static_cast<const void*>(m_pSkillData);
//
//	// 공격자
//	HitBoxDesc.pAttacker = m_pOwner;
//
//	// 생성될 히트박스의 콜리전 모양
//	HitBoxDesc.eColType = COLLIDER::AABB;
//
//	HitBoxDesc.eHitBoxType = HIT_TYPE::MONSTER;
//	HitBoxDesc.eHitObjectType = HIT_TYPE::PLAYER;
//	m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_AttackHitBox"),
//		ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Hit_Box_Layer"), &HitBoxDesc);
//}

void CMonsterAttackState::BeholderGara(_float fTimeDelta)
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
		//m_fRootMotionRatio = 1.f;
		m_bIsRootGara = false;
		if (0.25f <= fAnimPlayRatio && 0.46f >= fAnimPlayRatio)
		{
			bMoveAction = true;
		}
		else if (0.46f <= fAnimPlayRatio && 0.6f >= fAnimPlayRatio)
		{
			fSpeed = m_fMoveSpeed - 1.f;
			bMoveAction = true;
		}
		else if(0.25f >= fAnimPlayRatio)
		{
			m_bIsRootGara = true;
			m_fRootMotionRatio = 1.f;
		}
	}

	if (bMoveAction)
		m_pOwner->GetTransform()->Move_Direction(fTimeDelta, XMLoadFloat3(&m_vMoveDir), fSpeed);
}

void CMonsterAttackState::StatueAGara(_float fTimeDelta)
{
	auto pEntity = static_cast<CNayitba*>(m_pOwner);
	_float fAnimPlayRatio = pEntity->Get_AnimationRatio();
	_float fSpeed = m_fMoveSpeed;
	_bool bMoveAction = false;

	if (nullptr == m_pSkillData)
		return;

	if (521 == m_pSkillData->iSkillID)
	{
		m_bIsRootGara = false;
		//잡기 공격
		if (0.1f <= fAnimPlayRatio && 0.3 >= fAnimPlayRatio)
			bMoveAction = true;
	}
	else if (522 == m_pSkillData->iSkillID)
	{
		m_bIsRootGara = false;
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
		m_bIsRootGara = false;
		// Rush Slash 공격
		// 30 ~ 80 프레임
		if (0.15f <= fAnimPlayRatio && 0.4f >= fAnimPlayRatio)
		{
			bMoveAction = true;
		}
	}
	else if (524 == m_pSkillData->iSkillID)
	{
		m_bIsRootGara = false;
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
			m_bIsRootGara = true;
			//m_fRootMotionRatio = 1.f;
		}
	}
	else if (525 == m_pSkillData->iSkillID)
	{
		m_bIsRootGara = false;
		// 기습 공격
		// 6 ~ 20 프레임 회전
		if (0.38f <= fAnimPlayRatio && 0.5f >= fAnimPlayRatio)
		{
			fSpeed = m_fMoveSpeed;
			bMoveAction = true;
		}
		// 100 ~ 130 프레임 이동
		else if(0.15f >= fAnimPlayRatio)
		{
			_vector vOwnerPos = m_pOwner->GetTransform()->Get_State(STATE::POSITION);
			m_pOwner->GetTransform()->LookAt(vOwnerPos + XMLoadFloat3(&m_vMoveDir));
		}
	}

	if (bMoveAction)
		m_pOwner->GetTransform()->Move_Direction(fTimeDelta, XMLoadFloat3(&m_vMoveDir), fSpeed);
}

void CMonsterAttackState::StatueBGara(_float fTimeDelta)
{
	auto pEntity = static_cast<CNayitba*>(m_pOwner);
	_float fAnimPlayRatio = pEntity->Get_AnimationRatio();
	_float fSpeed = m_fMoveSpeed;
	m_bIsRootGara = false;
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
		// 65 ~ 120
		if (0.13f >= fAnimPlayRatio)
		{
			_vector vOwnerPos = m_pOwner->GetTransform()->Get_State(STATE::POSITION);
			m_pOwner->GetTransform()->LookAt(vOwnerPos + XMLoadFloat3(&m_vMoveDir));
		}
		// 125 ~ 170
		else if (0.25f <= fAnimPlayRatio && 0.35f >= fAnimPlayRatio)
		{
			fSpeed = m_fMoveSpeed - 1.f;
			bMoveAction = true;
		}
	}

	if (bMoveAction)
		m_pOwner->GetTransform()->Move_Direction(fTimeDelta, XMLoadFloat3(&m_vMoveDir), fSpeed);
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
