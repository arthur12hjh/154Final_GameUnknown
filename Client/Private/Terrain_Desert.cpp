#include "pch.h"
#include "Terrain_Desert.h"
#include "GameInstance.h"

CTerrain_Desert::CTerrain_Desert(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CTerrain_Desert::CTerrain_Desert(const CTerrain_Desert& Prototype)
	: CGameObject{ Prototype }
{
}

HRESULT CTerrain_Desert::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTerrain_Desert::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pGameInstance->Add_Terrain_ToPhysx(m_pVIBufferCom);

	return S_OK;
}

void CTerrain_Desert::Priority_Update(_float fTimeDelta)
{
	m_pNavigationCom->Update(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));
}

void CTerrain_Desert::Update(_float fTimeDelta)
{
	//m_pVIBufferCom->Culling(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));
}

void CTerrain_Desert::Late_Update(_float fTimeDelta)
{

	m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);

#ifdef _DEBUG
	m_pGameInstance->Add_DebugComponent(m_pNavigationCom);
#endif
}

HRESULT CTerrain_Desert::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(1)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Resources()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTerrain_Desert::Ready_Components()
{
	/* Com_VIBuffer */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_VIBuffer_Terrain_Desert"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* Com_NormalTexture */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::LEVEL_PROB), TEXT("Prototype_Component_Texture_Terrain_Desert_Normal"),
		TEXT("Com_NormalTexture"), reinterpret_cast<CComponent**>(&m_pNormalTextureCom))))

		return E_FAIL;
	/* Com_Texture */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::LEVEL_PROB), TEXT("Prototype_Component_Texture_Terrain_Desert"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;
	/* Com_Texture_Red */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::LEVEL_PROB), TEXT("Prototype_Component_Texture_Terrain_Desert_Red"),
		TEXT("Com_Texture_Red"), reinterpret_cast<CComponent**>(&m_pTextureCom_Red))))
		return E_FAIL;
	/* Com_Texture_Green */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::LEVEL_PROB), TEXT("Prototype_Component_Texture_Terrain_Desert_Green"),
		TEXT("Com_Texture_Green"), reinterpret_cast<CComponent**>(&m_pTextureCom_Green))))
		return E_FAIL;
	/* Com_Texture_Blue */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::LEVEL_PROB), TEXT("Prototype_Component_Texture_Terrain_Desert_Blue"),
		TEXT("Com_Texture_Blue"), reinterpret_cast<CComponent**>(&m_pTextureCom_Blue))))
		return E_FAIL;

	/* Com_Mask */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::LEVEL_PROB), TEXT("Prototype_Component_Texture_Terrain_Mask"),
		TEXT("Com_Mask"), reinterpret_cast<CComponent**>(&m_pMaskCom))))
		return E_FAIL;

	/* Com_ORM */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::LEVEL_PROB), TEXT("Prototype_Component_ORM_Texture_Terrain_Desert"),
		TEXT("Com_ORM"), reinterpret_cast<CComponent**>(&m_pORMTextureCom))))
		return E_FAIL;

	/* Com_Shader */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Shader_VtxNorTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CTerrain_Desert::Bind_ShaderResources()
{
	/*m_pShaderCom->Bind_Matrix("g_WorldMatrix", );*/
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
		return E_FAIL;
	if (FAILED(m_pTextureCom->Bind_ShaderResources(m_pShaderCom, "g_DiffuseTexture_Base")))
		return E_FAIL;
	if (FAILED(m_pTextureCom_Red->Bind_ShaderResources(m_pShaderCom, "g_DiffuseTexture_Red")))
		return E_FAIL;
	if (FAILED(m_pTextureCom_Green->Bind_ShaderResources(m_pShaderCom, "g_DiffuseTexture_Green")))
		return E_FAIL;
	if (FAILED(m_pTextureCom_Blue->Bind_ShaderResources(m_pShaderCom, "g_DiffuseTexture_Blue")))
		return E_FAIL;
	if (FAILED(m_pNormalTextureCom->Bind_ShaderResources(m_pShaderCom, "g_NormalTexture")))
		return E_FAIL;

	if (FAILED(m_pORMTextureCom->Bind_ShaderResources(m_pShaderCom, "g_ORMTexture")))
		return E_FAIL;

	if (FAILED(m_pMaskCom->Bind_ShaderResources(m_pShaderCom, "g_MaskTexture")))
		return E_FAIL;

	return S_OK;
}

CTerrain_Desert* CTerrain_Desert::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTerrain_Desert* pInstance = new CTerrain_Desert(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CTerrain_Desert::Clone(void* pArg)
{
	CTerrain_Desert* pInstance = new CTerrain_Desert(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTerrain_Desert");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTerrain_Desert::Free()
{
	__super::Free();

	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTextureCom_Red);
	Safe_Release(m_pTextureCom_Green);
	Safe_Release(m_pTextureCom_Blue);
	Safe_Release(m_pMaskCom);
	Safe_Release(m_pORMTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pNormalTextureCom);
}
