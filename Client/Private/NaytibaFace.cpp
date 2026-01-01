#include "pch.h"
#include "NaytibaFace.h"

#include "GameInstance.h"

CNaytibaFace::CNaytibaFace(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) :
    CPartObject(pDevice, pContext)
{
}

CNaytibaFace::CNaytibaFace(const CNaytibaFace& Prototype) :
    CPartObject(Prototype)
{
}

HRESULT CNaytibaFace::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CNaytibaFace::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    return S_OK;
}

void CNaytibaFace::Priority_Update(_float fTimeDelta)
{
}

void CNaytibaFace::Update(_float fTimeDelta)
{
}

void CNaytibaFace::Late_Update(_float fTimeDelta)
{
}

HRESULT CNaytibaFace::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_AllMaterials(i, m_pShaderCom, 0)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CNaytibaFace::Render_Shadow()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_Shadow_Resource_Cascade(m_pShaderCom, "g_ViewMatrix", D3DTS::VIEW)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_Shadow_Resource_Cascade(m_pShaderCom, "g_ProjMatrix", D3DTS::PROJ)))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pShaderCom->Begin(1)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}
	return S_OK;
}


HRESULT CNaytibaFace::Ready_Components()
{
    return S_OK;
}

HRESULT CNaytibaFace::Bind_ShaderResources()
{
	/*m_pShaderCom->Bind_Matrix("g_WorldMatrix", );*/
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
		return E_FAIL;

	return S_OK;
}

CNaytibaFace* CNaytibaFace::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CNaytibaFace* pInstance = new CNaytibaFace(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : NaytibaFace");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CNaytibaFace::Clone(void* pArg)
{
	CNaytibaFace* pInstance = new CNaytibaFace(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : NaytibaFace");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CNaytibaFace::Free()
{
	__super::Free();
}
