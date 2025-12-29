#include "pch.h"
#include "MonsterStateMimesis.h"

#include "GameInstance.h"
#include "GameStruct.h"
#include "Nayitba.h"

CMonsterStateMimesis::CMonsterStateMimesis() :
	CState()
{
	m_iStateID = 1;
}

HRESULT CMonsterStateMimesis::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CMonsterStateMimesis::Start(void* pArg, CState* pPreState)
{
	auto pEntity = static_cast<CNaytiba*>(m_pOwner);
	auto pOwnerStaticInfo = pEntity->GetStaticMonsterData();
	m_szAnimationName = pOwnerStaticInfo->szAnimationName;
	m_szAnimationName += "_Stanby_01";

	//SettingMimesisPos(pEntity->GetStaticMonsterData()->iMonsetID);
	pEntity->Set_Animation(m_szAnimationName.c_str(), true);
}

void CMonsterStateMimesis::Update(_float fTimeDelta)
{
	auto pEntity = static_cast<CNaytiba*>(m_pOwner);
	pEntity->Play_Animation(fTimeDelta);
}

void CMonsterStateMimesis::End()
{

}

void CMonsterStateMimesis::SettingMimesisPos(_uint iMonsterID)
{
	switch (iMonsterID)
	{
	/*case 4 : case 5 :
	
		break;*/
	case 9 :
		m_szAnimationName = "M_Tentacle_Summoned";
		break;

	default :
		m_szAnimationName += "_Stanby_01";
		break;
	}
}

CMonsterStateMimesis* CMonsterStateMimesis::Create(void* pArg)
{
	CMonsterStateMimesis* pMonsterStateMimesis = new CMonsterStateMimesis();
	if (FAILED(pMonsterStateMimesis->Initialize(pArg)))
	{
		Safe_Release(pMonsterStateMimesis);
		MSG_BOX("Create Fail : Monster State Mimesis");
	}
	return pMonsterStateMimesis;
}

void CMonsterStateMimesis::Free()
{
	__super::Free();
}
