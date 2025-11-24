#include "pch.h"
#include "BossController.h"

#include "GameInstance.h"
#include "BehaviorTree.h"

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
