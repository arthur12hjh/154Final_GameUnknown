#include "pch.h"
#include "Deco_CheckAlive.h"

#include "BossBlackBoard.h"
#include "BehaviorTree.h"

CDeco_CheckAlive::CDeco_CheckAlive()
{
}

HRESULT CDeco_CheckAlive::Initialize_Prototype(CBehaviorTree* pOwnerTree, _float fDeadPercent)
{
    if (FAILED(__super::Initialize_Prototype(pOwnerTree)))
        return E_FAIL;

    m_fDeadPercent = fDeadPercent / 100.f;
    return S_OK;
}

CBehaviorNode::NODE_STATE CDeco_CheckAlive::Update(_float fTimeDelta)
{
    CBossBlackBoard* pBlackBoard = static_cast<CBossBlackBoard*>(m_pOwnerTree->GetBlackBoard());
    const NAYTIBA_DESC* CharacterInfo = pBlackBoard->GetBossInfo();
    const NAYTIBA_NETWORK_DESC* CharacterInitInfo = pBlackBoard->GetBossDefaultInfo();
    Safe_Release(pBlackBoard);

    _float fPercent = (_float)CharacterInfo->iCurrentHealth / (_float)CharacterInitInfo->iMaxHealth;
    if (m_fDeadPercent < fPercent )
        return NODE_STATE::COMPLETE;

    return NODE_STATE::FAIL;
}

CDeco_CheckAlive* CDeco_CheckAlive::Create(CBehaviorTree* pOwnerTree, _float fDeadPercent)
{
    CDeco_CheckAlive* pAliveDescorator = new CDeco_CheckAlive();
    if (FAILED(pAliveDescorator->Initialize_Prototype(pOwnerTree, fDeadPercent)))
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
