#include "pch.h"
#include "DesertObject.h"
#include "GameInstance.h"

CDesertObject::CDesertObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CDesertObject::CDesertObject(const CDesertObject& Prototype)
	: CGameObject{ Prototype }
{
}

HRESULT CDesertObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CDesertObject::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(nullptr)))
		return E_FAIL;

	return S_OK;
}

void CDesertObject::Priority_Update(_float fTimeDelta)
{
}

void CDesertObject::Update(_float fTimeDelta)
{
}

void CDesertObject::Late_Update(_float fTimeDelta)
{

}

HRESULT CDesertObject::Render()
{
	return S_OK;
}

HRESULT CDesertObject::Ready_Components(const _tchar* pComponentTag)
{

	return S_OK;
}

HRESULT CDesertObject::Bind_ShaderResources()
{

	return S_OK;
}

CDesertObject* CDesertObject::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDesertObject* pInstance = new CDesertObject(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CDesertObject::Clone(void* pArg)
{
	CDesertObject* pInstance = new CDesertObject(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CDesertObject");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDesertObject::Free()
{
	__super::Free();

}
