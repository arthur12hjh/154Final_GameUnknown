#include "pch.h"
#include "UISimpleKey.h"

#include "GameInstance.h"

#include "UIHUD.h"
#include "Interaction_Component.h"
#include "Prob_Interaction.h"
#include "UIWorldWrapper.h"

/*
테스트(Lift_Controller) 위치
X=425.43, Y=55.00, Z=281.58
*/

CUISimpleKey::CUISimpleKey(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIBase{ pDevice, pContext }
{
}

CUISimpleKey::CUISimpleKey(const CUISimpleKey& Prototype) 
	: CUIBase{ Prototype }
{
}

HRESULT CUISimpleKey::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUISimpleKey::Initialize(void* pArg)
{	
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CUISimpleKey::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CUISimpleKey::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

	
	/*if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_Y))
	{
		if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_7))
			m_eInterState = INTERACTION_STATE::DEFAULT;
		if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_8))
			m_eInterState = INTERACTION_STATE::CONTACT;
		if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_9))
			m_eInterState = INTERACTION_STATE::ACTIVE;
	}*/

	if (dynamic_cast<CUIWorldWrapper*>(m_pParent)->Get_InteractionCom())
	{
		m_eInterState = dynamic_cast<CProb_Interaction*>(dynamic_cast<CUIWorldWrapper*>(m_pParent)->Get_InteractionCom()->GetOwner())->Get_InterState();
	}
}

void CUISimpleKey::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	auto pInteraction = m_pGameInstance->GetNearInteraction();
	if (pInteraction)
	{

		if (pInteraction == static_cast<CUIWorldWrapper*>(m_pParent)->Get_InteractionCom())
		{
			/*	if (m_eInterState != m_ePrevInterState)
				{*/
			CUIHUD* pHUD = dynamic_cast<CUIHUD*>(m_pGameInstance->GetCurrentLevelHUD());

			UI_EVENT_ARG_DESC Arg{};
			Arg.Type = UI_EVENT_ARG_DESC::INTERACTION_STATE;
			Arg.pData = &m_eInterState;

			switch (m_eInterState)
			{
			case INTERACTION_STATE::DEFAULT:
			{
				auto AnimTag = m_tUIDesc.m_AnimTags.find(TEXT("Show_Key_") + to_wstring(m_iCloneIdx));
				if (AnimTag != m_tUIDesc.m_AnimTags.end())
					pHUD->Anim_Play(m_tUIDesc.szLayerTag, m_tUIDesc.szUITag, AnimTag->first);
				__super::Trigger_Event(TEXT("Show_Key_") + to_wstring(m_iCloneIdx), &Arg);
				break;
			}
			case INTERACTION_STATE::CONTACT:
			{

				break;
			}
			case INTERACTION_STATE::LOCK:
			{
				break;
			}
			case INTERACTION_STATE::ACTIVE:
			{
				auto AnimTag = m_tUIDesc.m_AnimTags.find(TEXT("Hide_Key_") + to_wstring(m_iCloneIdx));
				if (AnimTag != m_tUIDesc.m_AnimTags.end())
					pHUD->Anim_Play(m_tUIDesc.szLayerTag, m_tUIDesc.szUITag, AnimTag->first);
				__super::Trigger_Event(TEXT("Hide_Key_") + to_wstring(m_iCloneIdx), &Arg);

				_bool bActive = true;
				UI_EVENT_ARG_DESC Arg{};
				Arg.Type = UI_EVENT_ARG_DESC::BOOL;
				Arg.pData = &bActive;
				__super::Trigger_Event(TEXT("Interaction_Active_") + to_wstring(m_iCloneIdx), &Arg);

				break;
			}
			}

			m_ePrevInterState = m_eInterState;
			Safe_Release(pHUD);
			/*	}*/
		}
	}
}

HRESULT CUISimpleKey::Render()
{
	__super::Render();

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(UI_SHADER_PASS::SIMPLE_KEY))))
		return E_FAIL;

	if (FAILED(m_pVIBaseBufferCom->Bind_Resources()))
		return E_FAIL;

	if (FAILED(m_pVIBaseBufferCom->Render()))
		return E_FAIL;

#ifdef _DEBUG
	__super::Render_Debug_Rect();
#endif

	return S_OK;
}

HRESULT CUISimpleKey::Ready_Components()
{
	__super::Ready_Components();

	/* Com_VIBuffer */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect_Instance"),
		TEXT("Com_VIBaseBuffer"), reinterpret_cast<CComponent**>(&m_pVIBaseBufferCom))))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_LockOn"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;
	
	/* Com_Texture_UI_Interaction_Key */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Interaction_Key"),
		TEXT("Com_Texture_UI_Interaction_Key"), reinterpret_cast<CComponent**>(&m_pKeyTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUISimpleKey::Bind_ShaderResources()
{
	if(FAILED(__super::Initialize_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->GetIdentityMatrixPtr())))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture0", 0)))
		return E_FAIL;
	if (FAILED(m_pKeyTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture1", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_Alpha", &m_tUIDesc.fAlpha, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_bUseScale", &m_tUIDesc.m_tUIShaderDesc.bUseScale, sizeof(_bool))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fScale", &m_tUIDesc.m_tUIShaderDesc.fScale, sizeof(_float))))
		return E_FAIL;

	/*if (FAILED(m_pShaderCom->Bind_RawValue("g_bUseGlow", &m_tUIDesc.m_tUIShaderDesc.bUseGlow, sizeof(_bool))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_GlowIntensity", &m_tUIDesc.m_tUIShaderDesc.fGlowIntensity, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_GlowSpread", &m_tUIDesc.m_tUIShaderDesc.fGlowSpread, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_bUseTintColor", &m_tUIDesc.m_tUIShaderDesc.bUseTintColor, sizeof(_bool))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vTintColor", &m_tUIDesc.m_tUIShaderDesc.vTintColor, sizeof(_float4))))
		return E_FAIL;*/

	return S_OK;
}

HRESULT CUISimpleKey::Execute(const UI_EVENT_DESC& EventDesc)
{
	return S_OK;
}

void CUISimpleKey::CallbackEvent(void* pArg)
{
}


CUISimpleKey* CUISimpleKey::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUISimpleKey* pInstance = new CUISimpleKey(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUISimpleKey::Clone(void* pArg)
{
	CUISimpleKey* pInstance = new CUISimpleKey(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUISimpleKey");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUISimpleKey::Free()
{
	__super::Free();

	Safe_Release(m_pVIBaseBufferCom);
	Safe_Release(m_pKeyTextureCom);
}
