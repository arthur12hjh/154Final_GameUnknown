#include "pch.h"
#include "UIPotionCount.h"

#include "GameInstance.h"

#include "UIPotion.h"

CUIPotionCount::CUIPotionCount(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIBase{ pDevice, pContext }
{
}

CUIPotionCount::CUIPotionCount(const CUIPotionCount& Prototype) 
	: CUIBase{ Prototype }
{
}

HRESULT CUIPotionCount::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUIPotionCount::Initialize(void* pArg)
{	
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CUIPotionCount::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CUIPotionCount::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void CUIPotionCount::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
}

HRESULT CUIPotionCount::Render()
{
	__super::Render();

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(0)))
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

HRESULT CUIPotionCount::Ready_Components()
{
	__super::Ready_Components();

	/* Com_VIBuffer */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* Com_Texture_UI_Number */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Number"),
		TEXT("Com_Texture_UI_Number"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUIPotionCount::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->GetIdentityMatrixPtr())))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;
	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", dynamic_cast<CUIPotion*>(m_pParent)->Get_PotionCount())))
		return E_FAIL;

	return S_OK;
}

HRESULT CUIPotionCount::Execute(const UI_EVENT_DESC& EventDesc)
{
	return S_OK;
}

//HRESULT CUIPotionCount::Broadcast_Event(const _wstring& szEventTag, const _wstring& szActionTag, void* pArg)
//{
//	return S_OK;
//}

void CUIPotionCount::CallbackEvent(void* pArg)
{
}

CUIPotionCount* CUIPotionCount::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUIPotionCount* pInstance = new CUIPotionCount(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUIPotionCount::Clone(void* pArg)
{
	CUIPotionCount* pInstance = new CUIPotionCount(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUIPotionCount");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUIPotionCount::Free()
{
	__super::Free();
}
