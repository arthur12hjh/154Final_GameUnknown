#include "pch.h"
#include "BossController.h"

#include "GameInstance.h"
#include "BehaviorTree.h"
#include "GameManager.h"
#include "Player.h"
#include "BossBlackBoard.h"

CBossController::CBossController(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) :
    CAIController(pDevice, pContext)
{
}

CBossController::CBossController(const CBossController& Prototype) :
    CAIController(Prototype)
{
}

HRESULT CBossController::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CBossController::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    BOSS_CONTROLLER_DESC* pControllerDesc = static_cast<BOSS_CONTROLLER_DESC*>(pArg);
    if (FAILED(Ready_Behavior(*pControllerDesc)))
        return E_FAIL;

    auto pBossBlackBoard = static_cast<CBossBlackBoard*>(m_pBehaviorTree->GetBlackBoard());
    auto pPlayer = CGameManager::GetInstance()->GetGameCharacter();
    pBossBlackBoard->SetTarget(pPlayer);
    Safe_Release(pPlayer);
    Safe_Release(pBossBlackBoard);
    return S_OK;
}

void CBossController::Priority_Update(_float fTimeDelta)
{
}

void CBossController::Update(_float fTimeDelta)
{
    m_pBehaviorTree->Update(fTimeDelta);
}

void CBossController::Late_Update(_float fTimeDelta)
{
}

HRESULT CBossController::Render()
{
    return S_OK;
}

void CBossController::Damage(void* pArg)
{
    auto pBossBlackBoard = static_cast<CBossBlackBoard*>(m_pBehaviorTree->GetBlackBoard());
    pBossBlackBoard->SetHitData(static_cast<Default_Damage_Desc*>(pArg));
    Safe_Release(pBossBlackBoard);
}

void CBossController::ActionSuccess(void* pArg)
{
}

HRESULT CBossController::Ready_Behavior(const BOSS_CONTROLLER_DESC& pDesc)
{
    CBehaviorTree::BEHAVIORTREE_DESC Desc = {};
    Desc.pOwner = m_pParent;

    auto pClone = m_pGameInstance->Clone_Prototype(PROTOTYPE::COMPONENT, ENUM_CLASS(LEVEL::GAMEPLAY), pDesc.szBehaviorProtoType, &Desc);
    if (nullptr == pClone)
        return E_FAIL;

    m_pBehaviorTree = static_cast<CBehaviorTree*>(pClone);
    return S_OK;
}

CBossController* CBossController::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CBossController* pBossController = new CBossController(pDevice, pContext);
    if (FAILED(pBossController->Initialize_Prototype()))
    {
        Safe_Release(pBossController);
        MSG_BOX("Create Fail : Boss Controller");
    }
    return pBossController;
}

CGameObject* CBossController::Clone(void* pArg)
{
    CBossController* pBossController = new CBossController(*this);
    if (FAILED(pBossController->Initialize(pArg)))
    {
        Safe_Release(pBossController);
        MSG_BOX("Clone Fail : Boss Controller");
    }
    return pBossController;
}

void CBossController::Free()
{
    __super::Free();

    Safe_Release(m_pBehaviorTree);
}
