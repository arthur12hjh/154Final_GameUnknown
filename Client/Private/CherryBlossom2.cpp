#include "pch.h"
#include "CherryBlossom2.h"
#include "GameInstance.h"

CCherryBlossom2::CCherryBlossom2(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CCherryBlossom2::CCherryBlossom2(const CCherryBlossom2& Prototype)
	: CGameObject{ Prototype }
{
}

HRESULT CCherryBlossom2::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCherryBlossom2::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CCherryBlossom2::Priority_Update(_float fTimeDelta)
{
}

void CCherryBlossom2::Update(_float fTimeDelta)
{

}

void CCherryBlossom2::Late_Update(_float fTimeDelta)
{

	m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
}

HRESULT CCherryBlossom2::Render()
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

HRESULT CCherryBlossom2::Ready_Components()
{
	/* Com_Model */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_CherryBlossom2"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* Com_Shader */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CCherryBlossom2::Bind_ShaderResources()
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

CCherryBlossom2* CCherryBlossom2::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCherryBlossom2* pInstance = new CCherryBlossom2(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCherryBlossom2::Clone(void* pArg)
{
	CCherryBlossom2* pInstance = new CCherryBlossom2(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCherryBlossom2");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCherryBlossom2::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}
