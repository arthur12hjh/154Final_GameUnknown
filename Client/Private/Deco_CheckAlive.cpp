#include "pch.h"
#include "Deco_CheckAlive.h"

#include "BossBlackBoard.h"
#include "BehaviorTree.h"

CDeco_CheckAlive::CDeco_CheckAlive()
{
}

HRESULT CDeco_CheckAlive::Initialize_Prototype(CBehaviorTree* pOwnerTree)
{
    if (FAILED(__super::Initialize_Prototype(pOwnerTree)))
        return E_FAIL;

    return S_OK;
}

CBehaviorNode::NODE_STATE CDeco_CheckAlive::Update(_float fTimeDelta)
{
    CBossBlackBoard* pBlackBoard = static_cast<CBossBlackBoard*>(m_pOwnerTree->GetBlackBoard());
    const NAYTIBA_DESC* CharacterInfo = pBlackBoard->GetBossInfo();
    Safe_Release(pBlackBoard);

    if (0 <= CharacterInfo->iCurrentHealth)
        return NODE_STATE::COMPLETE;

    return NODE_STATE::FAIL;
}

CDeco_CheckAlive* CDeco_CheckAlive::Create(CBehaviorTree* pOwnerTree)
{
    CDeco_CheckAlive* pAliveDescorator = new CDeco_CheckAlive();
    if (FAILED(pAliveDescorator->Initialize_Prototype(pOwnerTree)))
    {
        Safe_Release(pAliveDescorator);
        MSG_BOX("Create Fail : Alive Descorator");
    }
    return pAliveDescorator;
}

void CDeco_CheckAlive::Free()
{
    __super::Free();
}
