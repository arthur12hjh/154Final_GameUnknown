#include "pch.h"
#include "UISimpleKey.h"

#include "GameInstance.h"

#include "UIHUD.h"
#include "InteractionBinder.h"
#include "GameObject.h"
#include "UIWorldWrapper.h"
#include "StringHelper.h"
#include "SciFi_Door.h"
#include "GameManager.h"
#include "Lift_Controller.h"

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
	CInteractionBinder* pNearInterCom = dynamic_cast<CInteractionBinder*>(m_pGameInstance->GetNearInteraction());

	CGameObject* pOwner = nullptr;

	if (pNearInterCom)
		pOwner = pNearInterCom->GetOwner();

	if (pOwner)
		m_eInterState = pNearInterCom->Get_InterState();
	else
		m_eInterState = pNearInterCom ? pNearInterCom->Get_InterState() : INTERACTION_STATE::END;

	if (m_eInterState != INTERACTION_STATE::CONTACT)
		m_pTargetInteractionCom = pNearInterCom;

	CUIHUD* pHUD = dynamic_cast<CUIHUD*>(m_pGameInstance->GetCurrentLevelHUD());

	if (!pHUD)
		return;

	CUIBase* pUIText = pHUD->Get_UIObject(m_tUIDesc.szLayerTag, TEXT("Interaction_Text"));

	if (!pUIText)
		return;

	Safe_AddRef(pUIText);
	
	if (m_pTargetInteractionCom)
	{
		if (m_pTargetInteractionCom->GetOwner())
			m_pTargetOwner = m_pTargetInteractionCom->GetOwner();

		if (m_pTargetOwner != m_pParent)
			m_pInterDesc = *const_cast<INTERACTION_DATA*>(m_pTargetInteractionCom->Get_InterDesc());

		if (m_pTargetOwner && m_pTargetInteractionCom->Get_InterDesc())
		{
			_float3 vPivot{ 0.f, 0.f, 0.f };

			if (m_pTargetInteractionCom->Get_InterDesc())
				vPivot = m_pInterDesc.vUIPivot;

			if (dynamic_cast<CLift_Controller*>(m_pTargetOwner)
				&& dynamic_cast<CLift_Controller*>(m_pTargetOwner)->Get_CombinedMatrix()
				&& dynamic_cast<CLift_Controller*>(m_pTargetOwner)->Get_LiftPlatformPosition())
			{
				CLift_Controller* pLiftController = dynamic_cast<CLift_Controller*>(m_pTargetOwner);

				XMStoreFloat3(&m_vNewPivot,
					XMVectorSet(
						pLiftController->Get_CombinedMatrix()->m[3][0] + vPivot.x,
						pLiftController->Get_CombinedMatrix()->m[3][1] + vPivot.y,
						pLiftController->Get_CombinedMatrix()->m[3][2] + vPivot.z,
						1.f
					));
			}
			else
			{
				XMStoreFloat3(&m_vNewPivot,
					XMVectorSet(
						XMVectorGetX(m_pTargetOwner->GetTransform()->Get_State(STATE::POSITION)) + vPivot.x,
						XMVectorGetY(m_pTargetOwner->GetTransform()->Get_State(STATE::POSITION)) + vPivot.y,
						XMVectorGetZ(m_pTargetOwner->GetTransform()->Get_State(STATE::POSITION)) + vPivot.z,
						1.f
					));
			}

			m_pParent = m_pTargetOwner;
			m_pTargetPos = &m_vNewPivot;

			if (m_eInterState != INTERACTION_STATE::LOCK)
				m_eVisibility = VISIBILITY::VISIBLE;
			else
				m_eVisibility = VISIBILITY::HIDDEN;
			
			WCHAR szText[MAX_PATH] = {};

			pUIText->Get_UIBase_Desc().m_tUITextDesc.szText = m_pInterDesc.szInteractionText;
		}
	}
	else
	{
		m_eInterState = INTERACTION_STATE::END;
		m_ePrevInterState = INTERACTION_STATE::END;

		m_pParent = nullptr;
		m_pTargetPos = nullptr;
		m_pInterDesc = {};
		m_eVisibility = VISIBILITY::HIDDEN;
		m_vNewPivot = {0.f, 0.f, 0.f};

		m_pTargetOwner = nullptr;

		pUIText->Get_UIBase_Desc().m_tUITextDesc.szText = TEXT("");

		for (auto& pChild : m_Children)
			Update_Children(pChild);

		Safe_Release(pUIText);
		Safe_Release(pHUD);

		return;
	}

	__super::Update(fTimeDelta);

	if (m_pTargetOwner)
	{
		m_fInteractionRatio = m_pTargetInteractionCom->Get_Ratio();

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

				if (dynamic_cast<CSciFi_Door*>(m_pTargetOwner)
					&& dynamic_cast<CSciFi_Door*>(m_pTargetOwner)->Get_DoorState() == CSciFi_Door::SCIFI_DOOR_STATE::OPEN)
					m_pInterDesc.szInteractionText = TEXT("닫기");
				else
					m_pInterDesc.szInteractionText = m_pTargetInteractionCom->Get_InterDesc()->szInteractionText;

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
				__super::Trigger_Event(TEXT("Interaction_Active"), &Arg);

				break;
			}
			case INTERACTION_STATE::END:
			{
				m_pTargetInteractionCom = nullptr;
			}
			}

			m_ePrevInterState = m_eInterState;
		}
	}

	Safe_Release(pUIText);
	Safe_Release(pHUD);
	__super::Update(fTimeDelta);
}

void CUISimpleKey::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
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
	
	/* Com_Texture_UI_Interaction_Key */
	if (FAILED(__super::Add_Component(m_tUIDesc.iLevel, TEXT("Prototype_Component_UI_Texture_Interaction_Key"),
		TEXT("Com_Texture_UI_Interaction_Key"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* Com_Texture_UI_Interaction_Hold_Gauge */
	if (FAILED(__super::Add_Component(m_tUIDesc.iLevel, TEXT("Prototype_Component_UI_Texture_Interaction_Hold_Gauge"),
		TEXT("Com_Texture_UI_Interaction_Hold_Gauge"), reinterpret_cast<CComponent**>(&m_pHoldGaugeTextureCom))))
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
	
	_bool bUseCoolTime = false;

	if (m_pInterDesc.fInteractionTime > 0.f)
	{
		bUseCoolTime = true;
		if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture0", 1)))
			return E_FAIL;
		if (FAILED(m_pHoldGaugeTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture1", 0)))
			return E_FAIL;
	}
	else
	{
		if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture0", 0)))
			return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_Alpha", &m_tUIDesc.fAlpha, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_bUseScale", &m_tUIDesc.m_tUIShaderDesc.bUseScale, sizeof(_bool))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fScale", &m_tUIDesc.m_tUIShaderDesc.fScale, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_bUseCoolTime", &bUseCoolTime, sizeof(_bool))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fCoolAmount", &m_fInteractionRatio, sizeof(_float))))
		return E_FAIL;

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
	Safe_Release(m_pHoldGaugeTextureCom);
}
