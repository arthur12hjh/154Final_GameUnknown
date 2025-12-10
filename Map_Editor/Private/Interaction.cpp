#include "pch.h"
#include "Interaction.h"
#include "GameInstance.h"

CInteraction::CInteraction(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CDesertObject{ pDevice, pContext }
{
}

CInteraction::CInteraction(const CInteraction& Prototype)
	: CDesertObject{ Prototype }
{
}

HRESULT CInteraction::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CInteraction::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CInteraction::Priority_Update(_float fTimeDelta)
{
}

void CInteraction::Update(_float fTimeDelta)
{
}

void CInteraction::Late_Update(_float fTimeDelta)
{

}

HRESULT CInteraction::Render()
{
	return S_OK;
}

HRESULT CInteraction::Ready_Components(const _tchar* pComponentTag)
{

	return S_OK;
}

HRESULT CInteraction::Bind_ShaderResources()
{

	return S_OK;
}

CInteraction* CInteraction::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CInteraction* pInstance = new CInteraction(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CInteraction::Clone(void* pArg)
{
	CInteraction* pInstance = new CInteraction(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CInteraction");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CInteraction::Free()
{
	__super::Free();

}
