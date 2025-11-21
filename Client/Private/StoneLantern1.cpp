#include "pch.h"
#include "StoneLantern1.h"
#include "GameInstance.h"

CStoneLantern1::CStoneLantern1(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CStoneLantern1::CStoneLantern1(const CStoneLantern1& Prototype)
	: CGameObject{ Prototype }
{
}

HRESULT CStoneLantern1::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CStoneLantern1::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CStoneLantern1::Priority_Update(_float fTimeDelta)
{
}

void CStoneLantern1::Update(_float fTimeDelta)
{

}

void CStoneLantern1::Late_Update(_float fTimeDelta)
{

	m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
}

HRESULT CStoneLantern1::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;


	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{

		if (FAILED(m_pModelCom->Bind_Material(i, m_pShaderCom, "g_DiffuseTexture", aiTextureType_DIFFUSE, 0)))
			return E_FAIL;
		if (FAILED(m_pModelCom->Bind_Material(i, m_pShaderCom, "g_NormalTexture", aiTextureType_NORMALS, 0)))
			return E_FAIL;


		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;


		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CStoneLantern1::Ready_Components()
{
	/* Com_Model */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_StoneLantern1"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* Com_Shader */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CStoneLantern1::Bind_ShaderResources()
{
	/*m_pShaderCom->Bind_Matrix("g_WorldMatrix", );*/
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
		return E_FAIL;

	return S_OK;
}

CStoneLantern1* CStoneLantern1::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CStoneLantern1* pInstance = new CStoneLantern1(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CStoneLantern1::Clone(void* pArg)
{
	CStoneLantern1* pInstance = new CStoneLantern1(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CStoneLantern1");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStoneLantern1::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}
