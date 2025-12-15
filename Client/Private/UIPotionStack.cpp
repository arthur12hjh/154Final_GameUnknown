#include "pch.h"
#include "UIPotionStack.h"

#include "GameInstance.h"

#include "UIPotion.h"

CUIPotionStack::CUIPotionStack(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIBase{ pDevice, pContext }
{
}

CUIPotionStack::CUIPotionStack(const CUIPotionStack& Prototype) 
	: CUIBase{ Prototype }
{
}

HRESULT CUIPotionStack::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUIPotionStack::Initialize(void* pArg)
{	
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CUIPotionStack::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CUIPotionStack::Update(_float fTimeDelta)
{
	auto pPotionUI = static_cast<CUIPotion*>(m_pParent);
	m_fTargetFill = pPotionUI->Get_PotionPercent();
	m_vPotionUV.y = pPotionUI->Get_MaxPotionCount();

	m_fCurrentFill = Lerp(m_fCurrentFill, m_fTargetFill, fTimeDelta * m_fSpeed);
	__super::Update(fTimeDelta);
}

void CUIPotionStack::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
}

HRESULT CUIPotionStack::Render()
{
	__super::Render();

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(UI_SHADER_PASS::POTION))))
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

HRESULT CUIPotionStack::Ready_Components()
{
	__super::Ready_Components();

	/* Com_VIBuffer */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* Com_Texture_UI_PotionStack */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Potion_Stack"),
		TEXT("Com_Texture_UI_PotionStack"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUIPotionStack::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->GetIdentityMatrixPtr())))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;
	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_UVScale", &m_vPotionUV, sizeof(_float2))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fFillAmount", &m_fCurrentFill, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUIPotionStack::Execute(const UI_EVENT_DESC& EventDesc)
{
	return S_OK;
}

//HRESULT CUIPotionStack::Broadcast_Event(const _wstring& szEventTag, const _wstring& szActionTag, void* pArg)
//{
//	return S_OK;
//}

void CUIPotionStack::CallbackEvent(void* pArg)
{
}

CUIPotionStack* CUIPotionStack::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUIPotionStack* pInstance = new CUIPotionStack(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUIPotionStack::Clone(void* pArg)
{
	CUIPotionStack* pInstance = new CUIPotionStack(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUIPotionStack");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUIPotionStack::Free()
{
	__super::Free();
}
