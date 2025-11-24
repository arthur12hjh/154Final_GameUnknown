#include "pch.h"
#include "StaticMap.h"
#include "GameInstance.h"

CStaticMap::CStaticMap(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CStaticMap::CStaticMap(const CStaticMap& Prototype)
	: CGameObject{ Prototype }
{
}

HRESULT CStaticMap::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CStaticMap::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CStaticMap::Priority_Update(_float fTimeDelta)
{
}

void CStaticMap::Update(_float fTimeDelta)
{

}

void CStaticMap::Late_Update(_float fTimeDelta)
{

}

HRESULT CStaticMap::Render()
{

	return S_OK;
}

HRESULT CStaticMap::Ready_Components()
{

	return S_OK;
}

HRESULT CStaticMap::Bind_ShaderResources()
{

	return S_OK;
}

CStaticMap* CStaticMap::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CStaticMap* pInstance = new CStaticMap(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CStaticMap::Clone(void* pArg)
{
	CStaticMap* pInstance = new CStaticMap(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CStaticMap");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStaticMap::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}
