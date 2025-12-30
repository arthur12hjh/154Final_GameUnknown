#include "pch.h"
#include "UIMouseInteraction.h"

#include "GameInstance.h"

#include "UIHUD.h"
#include "GameObject.h"
#include "GameManager.h"
#include "MousePointer.h"

CUIMouseInteraction::CUIMouseInteraction(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIBase{ pDevice, pContext }
{
}

CUIMouseInteraction::CUIMouseInteraction(const CUIMouseInteraction& Prototype)
	: CUIBase{ Prototype }
{
}

HRESULT CUIMouseInteraction::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUIMouseInteraction::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pMousePointer = dynamic_cast<CMousePointer*>(m_pGameInstance->GetAllObejctToLayer(ENUM_CLASS(LEVEL::STATIC), TEXT("Static_Level_Layer_Mouse"))->front());

	return S_OK;
}

void CUIMouseInteraction::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CUIMouseInteraction::Update(_float fTimeDelta)
{
	/*if (m_pMousePointer->GetVisibility() == VISIBILITY::HIDDEN)
		m_eVisibility = VISIBILITY::VISIBLE;
	else
		m_eVisibility = VISIBILITY::HIDDEN;*/

	Set_Position((_float)m_pGameInstance->GetMousePoint().x - m_tUIDesc.fX, (_float)m_pGameInstance->GetMousePoint().y - m_tUIDesc.fY);

	for (auto& pChild : m_Children)
		Update_Children(pChild);

	if(m_eVisibility == VISIBILITY::HIDDEN)
		return;

	CUIHUD* pHUD = dynamic_cast<CUIHUD*>(m_pGameInstance->GetCurrentLevelHUD());

	if (!pHUD)
	{
		Safe_Release(pHUD);
		return;
	}

	if (m_eInterState != m_ePrevInterState)
	{
		UI_EVENT_ARG_DESC Arg{};
		Arg.Type = UI_EVENT_ARG_DESC::INTERACTION_STATE;
		Arg.pData = &m_eInterState;

		switch (m_eInterState)
		{
		case INTERACTION_STATE::DEFAULT:
		{
			auto AnimTag = m_tUIDesc.m_AnimTags.find(TEXT("Show_Key"));
			if (AnimTag != m_tUIDesc.m_AnimTags.end())
				pHUD->Anim_Play(m_tUIDesc.szLayerTag, m_tUIDesc.szUITag, AnimTag->first);
			__super::Trigger_Event(TEXT("Show_Key"), &Arg);

			break;
		}
		case INTERACTION_STATE::LOCK:
		{
			break;
		}
		case INTERACTION_STATE::CONTACT:
		{
			auto AnimTag = m_tUIDesc.m_AnimTags.find(TEXT("Hide_Key"));
			if (AnimTag != m_tUIDesc.m_AnimTags.end())
				pHUD->Anim_Play(m_tUIDesc.szLayerTag, m_tUIDesc.szUITag, AnimTag->first);
			__super::Trigger_Event(TEXT("Hide_Key"), &Arg);

			_bool bActive = true;
			UI_EVENT_ARG_DESC Arg{};
			Arg.Type = UI_EVENT_ARG_DESC::BOOL;
			Arg.pData = &bActive;
			__super::Trigger_Event(TEXT("MaouseInteraction_Active"), &Arg);

			break;
		}
		case INTERACTION_STATE::END:
		{
			auto AnimTag = m_tUIDesc.m_AnimTags.find(TEXT("Hide_Key"));
			if (AnimTag != m_tUIDesc.m_AnimTags.end())
				pHUD->Anim_Play(m_tUIDesc.szLayerTag, m_tUIDesc.szUITag, AnimTag->first);
			__super::Trigger_Event(TEXT("Hide_Key"), &Arg);
		}
		}

		m_ePrevInterState = m_eInterState;
	}

	Safe_Release(pHUD);

	__super::Update(fTimeDelta);
}

void CUIMouseInteraction::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
}

HRESULT CUIMouseInteraction::Render()
{
	__super::Render();

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(UI_SHADER_PASS::SIMPLE_KEY))))
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

HRESULT CUIMouseInteraction::Ready_Components()
{
	__super::Ready_Components();

	/* Com_VIBuffer */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBaseBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;
	
	/* Com_Texture_UI_Interaction_Key */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Texture_Mouse_Right_Hold"),
		TEXT("Com_Texture_UI_Mouse_Right_Hold"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* Com_Texture_UI_Interaction_Hold_Gauge */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Interaction_Hold_Gauge"),
		TEXT("Com_Texture_UI_Interaction_Hold_Gauge"), reinterpret_cast<CComponent**>(&m_pHoldGaugeTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUIMouseInteraction::Bind_ShaderResources()
{
	if(FAILED(__super::Initialize_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->GetIdentityMatrixPtr())))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;
	
	_bool bUseCoolTime = true;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture0", 0)))
		return E_FAIL;
	if (FAILED(m_pHoldGaugeTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture1", 0)))
		return E_FAIL;
	

	if (FAILED(m_pShaderCom->Bind_RawValue("g_Alpha", &m_tUIDesc.fAlpha, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_bUseScale", &m_tUIDesc.m_tUIShaderDesc.bUseScale, sizeof(_bool))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fScale", &m_tUIDesc.m_tUIShaderDesc.fScale, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_bUseCoolTime", &bUseCoolTime, sizeof(_bool))))
		return E_FAIL;

	_float fRatio = m_fInteractionTime / 1.f;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fCoolAmount", &fRatio, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUIMouseInteraction::Execute(const UI_EVENT_DESC& EventDesc)
{
	return S_OK;
}

void CUIMouseInteraction::CallbackEvent(void* pArg)
{
	UI_EVENT_ARG_DESC* pEventArg = reinterpret_cast<UI_EVENT_ARG_DESC*>(pArg);
	switch (pEventArg->Type)
	{
	case UI_EVENT_ARG_DESC::INTERACTION_STATE:
	{
		INTERACTION_STATE eState = *reinterpret_cast<INTERACTION_STATE*>(pEventArg->pData);
		m_eInterState = eState;
		break;
	}
	case UI_EVENT_ARG_DESC::FLOAT:
	{
		_float fTime = *reinterpret_cast<_float*>(pEventArg->pData);
		m_fInteractionTime = fTime;
		break;
	}
	}
}


CUIMouseInteraction* CUIMouseInteraction::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUIMouseInteraction* pInstance = new CUIMouseInteraction(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUIMouseInteraction::Clone(void* pArg)
{
	CUIMouseInteraction* pInstance = new CUIMouseInteraction(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUIMouseInteraction");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUIMouseInteraction::Free()
{
	__super::Free();

	Safe_Release(m_pMousePointer);
	Safe_Release(m_pHoldGaugeTextureCom);
}
