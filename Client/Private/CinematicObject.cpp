#include "pch.h"
#include "CinematicObject.h"


CCinematicObject::CCinematicObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCharacter(pDevice, pContext)
{
}

CCinematicObject::CCinematicObject(const CCinematicObject& Prototype)
	: CCharacter(Prototype)
{
}

HRESULT CCinematicObject::Initialize_Prototype()
{


	return S_OK;
}

HRESULT CCinematicObject::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_bIsActive = FALSE;

	return S_OK;
}

HRESULT CCinematicObject::Set_Cinematic_Object(const ANIM_NOTIFY& NotifyReference)
{
	return S_OK;
}

CCinematicObject* CCinematicObject::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCinematicObject* pInstance = new CCinematicObject(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCinematicObject");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCinematicObject::Clone(void* pArg)
{
	CCinematicObject* pInstance = new CCinematicObject(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCinematicObject");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCinematicObject::Free()
{
	__super::Free();
}
