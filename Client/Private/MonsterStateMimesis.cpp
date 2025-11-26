#include "pch.h"
#include "MonsterStateMimesis.h"

#include "GameInstance.h"
#include "GameStruct.h"
#include "Nayitba.h"

CMonsterStateMimesis::CMonsterStateMimesis() :
	CState()
{
}

HRESULT CMonsterStateMimesis::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CMonsterStateMimesis::Start(void* pArg, CState* pPreState)
{
	auto pEntity = static_cast<CNayitba*>(m_pOwner);
	auto pOwnerStaticInfo = pEntity->GetStaticMonsterData();

	string szAnimationName = pOwnerStaticInfo->szAnimationName;

	szAnimationName += "_Stanby_01";
	m_iMimesisIndex = 1;
	
	pEntity->Set_Animation(szAnimationName.c_str());
}

void CMonsterStateMimesis::Update(_float fTimeDelta)
{
	auto pEntity = static_cast<CNayitba*>(m_pOwner);
	pEntity->Play_Animation(fTimeDelta);
}

void CMonsterStateMimesis::End()
{

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
