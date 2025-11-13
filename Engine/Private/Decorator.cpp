#include "Decorator.h"

CDecorator::CDecorator() : CBehaviorNode()
{
}

HRESULT CDecorator::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CDecorator::Initialize(void* pArg)
{
    return S_OK;
}

_bool CDecorator::Update(_float fTimeDelta)
{
    return true;
}

void CDecorator::Free()
{
    __super::Free();
}
