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

	if (bIsRandomAttack)
	{
		m_pSkillData = pEntity->GetSkillData(false);
		m_szAnimationName = m_pSkillData->szAnimationName;
	}

	_vector vOwnerPos = m_pOwner->GetTransform()->Get_State(STATE::POSITION);
	_vector vOwnerLook = m_pOwner->GetTransform()->Get_State(STATE::LOOK);

	CAttackHitBox::HIT_BOX_DESC HitBoxDesc = {};
	HitBoxDesc.bIsApplyTransform = true;

	XMStoreFloat3(&HitBoxDesc.vPosition, vOwnerPos + vOwnerLook * (pOwnerInfo->fAttackRange * 0.5f));
	HitBoxDesc.vScale = { 2.f , 2.f, 2.f};

	HitBoxDesc.pData = static_cast<const void*>(m_pSkillData);
	HitBoxDesc.pAttacker = m_pOwner;
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
