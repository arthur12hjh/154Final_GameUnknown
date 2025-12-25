#include "pch.h"
#include "LoadingBG.h"

#include "GameInstance.h"

CLoadingBG::CLoadingBG(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIObject{ pDevice, pContext }
{
}

CLoadingBG::CLoadingBG(const CLoadingBG& Prototype)
	: CUIObject{ Prototype }
{
}

HRESULT CLoadingBG::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CLoadingBG::Initialize(void* pArg)
{
	CUIObject::UIOBJECT_DESC	Desc{};

	Desc.fX = g_iWinSizeX >> 1;
	Desc.fY = g_iWinSizeY >> 1;
	Desc.fSizeX = g_iWinSizeX;
	Desc.fSizeY = g_iWinSizeY;

	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;


	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_iTextureIndices.push_back(0);
	m_iTextureIndices.push_back(3);
	m_iTextureIndices.push_back(4);
	m_iTextureIndices.push_back(5);

	m_iTextureIndex = 0;
	m_fLoadingTime = 0.f;
	return S_OK;
}

void CLoadingBG::Priority_Update(_float fTimeDelta)
{
	m_fLoadingTime += fTimeDelta;

	if (m_fLoadingTime > 5.f)
	{
		m_fLoadingTime = 0.f;
		++m_iTextureIndex;
	}
}

void CLoadingBG::Update(_float fTimeDelta)
{

}

void CLoadingBG::Late_Update(_float fTimeDelta)
{

	m_pGameInstance->Add_RenderGroup(RENDER::UI, this);
}

HRESULT CLoadingBG::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Resources()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoadingBG::Ready_Components()
{

	/* Com_VIBuffer */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_LoadingBG"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* Com_Shader */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoadingBG::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->GetIdentityMatrixPtr())))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;
	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iTextureIndices[m_iTextureIndex % m_iTextureIndices.size()])))
		return E_FAIL;

	return S_OK;
}

CLoadingBG* CLoadingBG::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLoadingBG* pInstance = new CLoadingBG(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CLoadingBG::Clone(void* pArg)
{
	CLoadingBG* pInstance = new CLoadingBG(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CLoadingBG");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLoadingBG::Free()
{
	__super::Free();

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
