#include "AIController.h"

CAIController::CAIController(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) :
    CGameObject(pDevice, pContext)
{
}

CAIController::CAIController(const CAIController& Prototype) :
    CGameObject(Prototype)
{
}

HRESULT CAIController::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CAIController::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    AI_CONTROLLER_DESC* pDesc = static_cast<AI_CONTROLLER_DESC*>(pArg);
    m_pOwner = pDesc->pOwner;

    return S_OK;
}

void CAIController::Priority_Update(_float fTimeDelta)
{
}

void CAIController::Update(_float fTimeDelta)
{
}

void CAIController::Late_Update(_float fTimeDelta)
{
}

HRESULT CAIController::Render()
{
    return S_OK;
}

CGameObject* CAIController::Clone(void* pArg)
{
    return nullptr;
}

void CAIController::Free()
{
    __super::Free();
}
