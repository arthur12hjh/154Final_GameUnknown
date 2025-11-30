#include "pch.h"
#include "UIPotion.h"

#include "GameInstance.h"
#include "GameManager.h"

#include "Player.h"
#ifdef _DEBUG
#include "../../UI_Editor/Public/UI_Camera.h"
#endif // DEBUG

CUIPotion::CUIPotion(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIBase{ pDevice, pContext }
{
}

CUIPotion::CUIPotion(const CUIPotion& Prototype) 
	: CUIBase{ Prototype }
{
}

HRESULT CUIPotion::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUIPotion::Initialize(void* pArg)
{	
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	auto pCharactor{ CGameManager::GetInstance()->GetGameCharacter() };

	if (pCharactor)
	{
		m_iMaxPotions = const_cast<_int*>(&CGameManager::GetInstance()->Get_PlayerDesc()->iMaxPotions);
		m_iPotions = const_cast<_int*>(&CGameManager::GetInstance()->Get_PlayerDesc()->iCurrentPotions);
	}

#ifdef _DEBUG
	else
	{
		m_iMaxPotions = const_cast<_int*>(&dynamic_cast<CUI_Camera*>(m_pGameInstance->GetMainCamera())->Get_Desc()->iMaxPotions);
		m_iPotions = const_cast<_int*>(&dynamic_cast<CUI_Camera*>(m_pGameInstance->GetMainCamera())->Get_Desc()->iCurrentPotions);
	}
#endif
	
	Safe_Release(pCharactor);

	return S_OK;
}

void CUIPotion::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CUIPotion::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

#ifdef _DEBUG
	/*if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_P))
	{
		if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_9) && *m_iPotions > 0)
			*m_iPotions -= 1;
		if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_0) && *m_iPotions < *m_iMaxPotions)
			*m_iPotions += 1;
	}*/
#endif
}

void CUIPotion::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
}

HRESULT CUIPotion::Render()
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

HRESULT CUIPotion::Ready_Components()
{
	__super::Ready_Components();

	/* Com_VIBuffer */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* Com_Texture_UI_Potion */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Potion"),
		TEXT("Com_Texture_UI_Potion"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUIPotion::Bind_ShaderResources()
{
	__super::Bind_ShaderResources();

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->GetIdentityMatrixPtr())))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;
	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	return S_OK;
}

HRESULT CUIPotion::Execute(const UI_EVENT_DESC& EventDesc)
{
	return S_OK;
}

void CUIPotion::CallbackEvent(void* pArg)
{
}

CUIPotion* CUIPotion::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUIPotion* pInstance = new CUIPotion(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUIPotion::Clone(void* pArg)
{
	CUIPotion* pInstance = new CUIPotion(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUIPotion");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUIPotion::Free()
{
	__super::Free();
}
