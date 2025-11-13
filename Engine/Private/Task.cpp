#include "Task.h"

CTask::CTask() : CBehaviorNode()
{
}

HRESULT CTask::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CTask::Initialize(void* pArg)
{
    return S_OK;
}

_bool CTask::Update(_float fTimeDelta)
{
    return true;
}

void CTask::Free()
{
    __super::Free();
}
