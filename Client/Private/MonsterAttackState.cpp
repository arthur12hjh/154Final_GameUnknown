#include "pch.h"
#include "MonsterAttackState.h"

#include "MonsterStateMimesis.h"
#include "GameInstance.h"

#include "AttackHitBox.h"
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
	auto pStaticOwnerInfo = pEntity->GetStaticMonsterData();
	auto pOwnerInfo = &pEntity->GetMonsterData();

	MONSTER_ATTACK_DESC* pDesc = static_cast<MONSTER_ATTACK_DESC*>(pArg);
	m_AttackCompletedFunc = pDesc->AttackCompletedFunc;
	m_pTarget = pDesc->pTarget;

	_bool bIsRandomAttack = true;
	if (AI_TYPE::PASSIVE == pStaticOwnerInfo->eAI_Type)
	{
		auto pMimesisState = dynamic_cast<CMonsterStateMimesis*>(pPreState);
		if (nullptr != pMimesisState)
		{
			m_szAnimationName = pStaticOwnerInfo->szAnimationName;
			switch (pMimesisState->GetMimesisIndex())
			{
			case 1:
				m_szAnimationName += "_StanbyToAttack_01";
				bIsRandomAttack = false;
				break;
			}
		}
	}

	_vector vOwnerPos = {};
	if (bIsRandomAttack)
	{
		m_pSkillData = pEntity->GetSkillData(false);
		m_szAnimationName = m_pSkillData->szAnimationName;
		
		if (510 <= m_pSkillData->iSkillID && 520 > m_pSkillData->iSkillID)
		{
			vOwnerPos = m_pOwner->GetTransform()->Get_State(STATE::POSITION);
			_vector vTargetPos = m_pTarget->GetTransform()->Get_State(STATE::POSITION);

			_vector vDir = XMVector3Normalize(vTargetPos - vOwnerPos);
			XMStoreFloat3(&m_vMoveDir, vDir);

			_float fLength = XMVectorGetX(XMVector3Length(vTargetPos - vOwnerPos));
			_float fRangeRatio = fLength / pOwnerInfo->fAttackRange;
			if (0.3f <= fRangeRatio)
				m_fMoveSpeed = 2.0f;
			else if (0.6f <= fRangeRatio)
				m_fMoveSpeed = 3.5f;
			else if (0.9f <= fRangeRatio)
				m_fMoveSpeed = 5.f;

			m_bIsGara = true;
		}
	}

	vOwnerPos = m_pOwner->GetTransform()->Get_State(STATE::POSITION);
	_vector vOwnerLook = m_pOwner->GetTransform()->Get_State(STATE::LOOK);

	// 충돌 충돌 충돌
	CAttackHitBox::HIT_BOX_DESC HitBoxDesc = {};

	// 이걸로 트랜스폼 바꿀지 말지 결정해서 True 면 초기세팅들어가고
	// false 면 초기세팅 안들어갑니다.
	HitBoxDesc.bIsApplyTransform = true;

	// 위치 바꾸기
	XMStoreFloat3(&HitBoxDesc.vPosition, vOwnerPos + vOwnerLook * (pOwnerInfo->fAttackRange * 0.5f));

	// 크기 바꾸기
	HitBoxDesc.vScale = { 2.f , 2.f, 2.f};

	// 넘길 데이터 구조체
	HitBoxDesc.pData = static_cast<const void*>(m_pSkillData);

	// 공격자
	HitBoxDesc.pAttacker = m_pOwner;

	// 생성될 히트박스의 콜리전 모양
	HitBoxDesc.eColType = COLLIDER::AABB;

	HitBoxDesc.eHitBoxType = HIT_TYPE::MONSTER;
	HitBoxDesc.eHitObjectType = HIT_TYPE::PLAYER;
	m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_AttackHitBox"),
		ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Hit_Box_Layer"), &HitBoxDesc);

	m_bIsEnableChange = false;
	pEntity->Set_Animation(m_szAnimationName.c_str(), false);
}

void CMonsterAttackState::Update(_float fTimeDelta)
{
	// 여기서 공격 분기
	auto pEntity = static_cast<CNayitba*>(m_pOwner);

	if (m_bIsGara)
	{
		BeholderGara(fTimeDelta);

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
