#include "pch.h"
#include "UILoadingBlock.h"

#include "GameInstance.h"
#include "GameManager.h"

#include "Player.h"
#ifdef _DEBUG
#include "../../UI_Editor/Public/UI_Camera.h"
#endif // DEBUG

CUILoadingBlock::CUILoadingBlock(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIBase{ pDevice, pContext }
{
}

CUILoadingBlock::CUILoadingBlock(const CUILoadingBlock& Prototype) 
	: CUIBase{ Prototype }
{
}

HRESULT CUILoadingBlock::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUILoadingBlock::Initialize(void* pArg)
{	
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CUILoadingBlock::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CUILoadingBlock::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void CUILoadingBlock::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	m_fTimeAcc += fTimeDelta;

	if (m_fTimeAcc >= 2.5f)
		m_fTimeAcc = 0.f;
}

HRESULT CUILoadingBlock::Render()
{
	__super::Render();

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(UI_SHADER_PASS::LOADING_BLOCK))))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Resources()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

#ifdef _DEBUG
	__super::Render_Debug_Rect();
#endif

	return S_OK;
}

HRESULT CUILoadingBlock::Ready_Components()
{
	__super::Ready_Components();

	/* Com_VIBuffer */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* Com_Texture_Beta */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::LOADING), TEXT("Prototype_Component_UI_Texture_LoadingBlock"),
		TEXT("Com_Texture_Loading_Block"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUILoadingBlock::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->GetIdentityMatrixPtr())))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture0", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fTimeDelta", &m_fTimeAcc, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUILoadingBlock::Execute(const UI_EVENT_DESC& EventDesc)
{
	return S_OK;
}

void CUILoadingBlock::CallbackEvent(void* pArg)
{
}

CUILoadingBlock* CUILoadingBlock::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUILoadingBlock* pInstance = new CUILoadingBlock(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUILoadingBlock::Clone(void* pArg)
{
	CUILoadingBlock* pInstance = new CUILoadingBlock(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUILoadingBlock");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUILoadingBlock::Free()
{
	__super::Free();
}
