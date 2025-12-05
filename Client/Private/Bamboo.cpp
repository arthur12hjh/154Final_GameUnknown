#include "pch.h"
#include "Bamboo.h"

#include "GameInstance.h"

CBamboo::CBamboo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) :
	CGameObject(pDevice, pContext)
{
}

CBamboo::CBamboo(const CBamboo& Prototype) :
	CGameObject(Prototype)
{
}

HRESULT CBamboo::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CBamboo::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CBamboo::Priority_Update(_float fTimeDelta)
{
}

void CBamboo::Update(_float fTimeDelta)
{
}

void CBamboo::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
}

HRESULT CBamboo::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->GetModelNumMeshes();
	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_MatrialTexture(m_pShaderCom, i, "g_DiffuseTexture", aiTextureType_DIFFUSE, 0)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_MatrialTexture(m_pShaderCom, i, "g_NormalTexture", aiTextureType_NORMALS, 0)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_MatrialTexture(m_pShaderCom, i, "g_ORMTexture", aiTextureType_METALNESS, 0)))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CBamboo::Ready_Components()
{
	/* Com_Model */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_Bamboo"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* Com_Shader */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Shader_Instance_Model"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CBamboo::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
		return E_FAIL;

	return S_OK;
}

CBamboo* CBamboo::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBamboo* pInstanceModel = new CBamboo(pDevice, pContext);
	if (FAILED(pInstanceModel->Initialize_Prototype()))
	{
		Safe_Release(pInstanceModel);
		MSG_BOX("Create Fail : Instance Model");
	}
	return pInstanceModel;
}

CGameObject* CBamboo::Clone(void* pArg)
{
	CBamboo* pInstanceModel = new CBamboo(*this);
	if (FAILED(pInstanceModel->Initialize(pArg)))
	{
		Safe_Release(pInstanceModel);
		MSG_BOX("Clone Fail : Instance Model");
	}
	return pInstanceModel;
}

void CBamboo::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}
