#include "pch.h"
#include "MonsterDeadState.h"

#include "GameInstance.h"
#include "GameStruct.h"
#include "Nayitba.h"

CMonsterDeadState::CMonsterDeadState() :
	CState()
{
	m_iStateID = 6;
}

HRESULT CMonsterDeadState::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CMonsterDeadState::Start(void* pArg, CState* pPreState)
{
	auto pEntity = static_cast<CNaytiba*>(m_pOwner);
	auto pDesc = static_cast<DEFAULT_DAMAGE_DESC*>(pArg);

	m_pMonsterDefaultData = pEntity->GetStaticMonsterData();
	m_pLastHitSkill = static_cast<const CHARACTER_SKILL_DESC*>(pDesc->pSkillData);
	// 이거 공격한 대상이랑 외적으로 하든 내적으로하든 앞뒤 판단해서 
	// 이름 더해주자
	if (nullptr == pDesc->pAttacker)
	{
		m_bIsFinished = true;
		MSG_BOX("Not Bind Attacker");
		return;
	}

	switch (m_pMonsterDefaultData->iMonsetID)
	{
	case 9 :
		m_szAnimationName = m_pMonsterDefaultData->szAnimationName;
		m_szAnimationName += "_Dead_S";
		m_bIsNoneDeadAnim = false;
		break;
	default :
		SettingNoneDeadAnim(pDesc);
		m_bIsNoneDeadAnim = true;
		break;
	}
	
	pEntity->Set_Animation(m_szAnimationName.c_str(), false);
	
	m_fDeadEndTime.y = 3.f;
}

void CMonsterDeadState::Update(_float fTimeDelta)
{
	auto pEntity = static_cast<CNaytiba*>(m_pOwner);
	
	_bool bIsFinished = pEntity->Play_Animation(fTimeDelta);
	if (0 == m_iSectionIndex)
	{
		if (bIsFinished)
		{
			if (m_bIsNoneDeadAnim)
			{
				m_szAnimationName = "Result_State_KnockDown_L";
			}
			else
			{
				m_szAnimationName = m_pMonsterDefaultData->szAnimationName;
				m_szAnimationName += "_Dead_L";
			}

			pEntity->Set_Animation(m_szAnimationName.c_str(), true, 1.f, 0.4f);
			m_iSectionIndex++;
		}
		else
		{
			_float fAnimRatio = pEntity->Get_AnimationRatio();
			_float LimitRatio = {};
			switch (m_eDeadState)
			{
			case DEAD_STATE::LINK_FRONT :
				LimitRatio = 0.25f;
				break;
			case DEAD_STATE::LINK_BACK :
				LimitRatio = 0.6f;
				break;
			}
		
			if (LimitRatio > fAnimRatio)
				m_pOwner->GetTransform()->Move_Direction(fTimeDelta, XMLoadFloat3(&m_vImpactDir), m_fImpactForce);
		}
	}
	else
	{
		m_fDeadEndTime.x += fTimeDelta;
		if (!m_bIsDeadEffect && m_fDeadEndTime.x >= m_fDeadEndTime.y)
		{
			pEntity->PlayDeadEffect();
			m_bIsDeadEffect = true;
		}
	}
}

void CMonsterDeadState::End()
{
	m_iSectionIndex = 0;
	m_pLastHitSkill = nullptr;
	m_bIsFinished = false;
}

void CMonsterDeadState::SettingDefaultDeadAnim()
{
	
}

void CMonsterDeadState::SettingNoneDeadAnim(DEFAULT_DAMAGE_DESC* pDesc)
{
	_vector vOwnerPos = m_pOwner->GetTransform()->Get_State(STATE::POSITION);
	_vector vAttackerPos = pDesc->pAttacker->GetTransform()->Get_State(STATE::POSITION);
	vOwnerPos.m128_f32[1] = vAttackerPos.m128_f32[1] = 0.f;

	if (SKILL_TYPE::BETA_SKILL == m_pLastHitSkill->eSkillType)
	{
		m_szAnimationName = "Result_State_KnockDown_S";
		_vector vDir = XMVector3Normalize(vAttackerPos - vOwnerPos);
		_float fScalar = XMVectorGetX(XMVector3Dot(m_pOwner->GetTransform()->Get_State(STATE::LOOK), vDir));
		if (0 <= fScalar)
		{
			m_szAnimationName += "_Bw";
			m_eDeadState = DEAD_STATE::LINK_BACK;
		}
		else
		{
			m_szAnimationName += "_Fw";
			m_eDeadState = DEAD_STATE::LINK_FRONT;
		}

		if (XMVector3Equal(XMLoadFloat3(&pDesc->vImpactDir), XMVectorZero()))
		{
			XMStoreFloat3(&m_vImpactDir, -1.f * vDir);
			m_vImpactDir.y += 0.1f;
		}
		else
		{
			m_vImpactDir = pDesc->vImpactDir;
		}

		m_fImpactForce = pDesc->fImpactForce;
	}
	else
	{
		m_szAnimationName = "Result_State_Groggy_S";
		m_eDeadState = DEAD_STATE::END;
	}
}

CMonsterDeadState* CMonsterDeadState::Create(void* pArg)
{
	CMonsterDeadState* pMonsterDeadState = new CMonsterDeadState();
	if (FAILED(pMonsterDeadState->Initialize(pArg)))
	{
		Safe_Release(pMonsterDeadState);
		MSG_BOX("Create Fail : Monster Dead State");
	}

	return pMonsterDeadState;
}

void CMonsterDeadState::Free()
{
	__super::Free();
}
