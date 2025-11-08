#include "pch.h"
#include "UIHUDLayer.h"

#include "GameInstance.h"

void CUIHUDLayer::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CUIHUDLayer::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void CUIHUDLayer::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
}

CUIHUDLayer* CUIHUDLayer::Create()
{
	CUIHUDLayer* pInstance = new CUIHUDLayer();

	if (FAILED(pInstance->Initialize_Layer()))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUIHUDLayer::Free()
{
	__super::Free();
}
