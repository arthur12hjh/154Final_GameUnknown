#include "Service.h"

CService::CService() : CBehaviorNode()
{
}

HRESULT CService::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CService::Initialize(void* pArg)
{
	return S_OK;
}

_bool CService::Update(_float fTimeDelta)
{

	return true;
}

void CService::Free()
{
	__super::Free();
}
