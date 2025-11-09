#include "pch.h"
#include "Village_Mou.h"
#include "GameInstance.h"

CVillage_Mou::CVillage_Mou(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CVillage_Mou::CVillage_Mou(const CVillage_Mou& Prototype)
	: CGameObject{ Prototype }
{
}

HRESULT CVillage_Mou::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CVillage_Mou::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(50.f, 4.f, 50.f, 1.f));

	return S_OK;
}

void CVillage_Mou::Priority_Update(_float fTimeDelta)
{
}

void CVillage_Mou::Update(_float fTimeDelta)
{
	m_pNavigationCom->Update(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));

}

void CVillage_Mou::Late_Update(_float fTimeDelta)
{

	m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
}

HRESULT CVillage_Mou::Render()
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

#ifdef _DEBUG
	m_pNavigationCom->Render();
#endif


	return S_OK;
}

HRESULT CVillage_Mou::Ready_Components()
{
	/* Com_Model */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_Village_Mou"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* Com_Shader */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* Com_Navigation */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Navigation"),
		TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CVillage_Mou::Bind_ShaderResources()
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

CVillage_Mou* CVillage_Mou::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CVillage_Mou* pInstance = new CVillage_Mou(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CVillage_Mou::Clone(void* pArg)
{
	CVillage_Mou* pInstance = new CVillage_Mou(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CVillage_Mou");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVillage_Mou::Free()
{
	__super::Free();

	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}
