#include "pch.h"
#include "UITriggerKey.h"

#include "GameInstance.h"

#include "UIHUD.h"
#include "GameObject.h"

CUITriggerKey::CUITriggerKey(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIBase{ pDevice, pContext }
{
}

CUITriggerKey::CUITriggerKey(const CUITriggerKey& Prototype)
	: CUIBase{ Prototype }
{
}

HRESULT CUITriggerKey::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUITriggerKey::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CUITriggerKey::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CUITriggerKey::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

	if (m_isActive)
	{
		UI_EVENT_ARG_DESC Arg{};
		Arg.Type = UI_EVENT_ARG_DESC::BOOL;
		Arg.szActionTag = TEXT("Key_Active");
		Arg.pData = &m_isActive;
		__super::Trigger_Event(TEXT("Key_Active"), &Arg);

		m_isActive = false;
		m_fInteractionTime = 0.f;
	}
}

void CUITriggerKey::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
}

HRESULT CUITriggerKey::Render()
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

HRESULT CUITriggerKey::Ready_Components()
{
	__super::Ready_Components();

	/* Com_VIBuffer */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBaseBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;
	
	///* Com_Texture_UI_Interaction_Key */
	//if (FAILED(__super::Add_Component(m_tUIDesc.iLevel, TEXT("Prototype_Component_UI_Texture_Interaction_Key"),
	//	TEXT("Com_Texture_UI_Interaction_Key"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
	//	return E_FAIL;

	/* Com_Texture_UI_Interaction_Hold_Gauge */
	if (FAILED(__super::Add_Component(m_tUIDesc.iLevel, TEXT("Prototype_Component_UI_Texture_Interaction_Hold_Gauge"),
		TEXT("Com_Texture_UI_Interaction_Hold_Gauge"), reinterpret_cast<CComponent**>(&m_pHoldGaugeTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUITriggerKey::Bind_ShaderResources()
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

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture0", m_tUIDesc.m_tUITextureDesc.iTextureIndex)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_Alpha", &m_tUIDesc.fAlpha, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_bUseScale", &m_tUIDesc.m_tUIShaderDesc.bUseScale, sizeof(_bool))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fScale", &m_tUIDesc.m_tUIShaderDesc.fScale, sizeof(_float))))
		return E_FAIL;

	if (m_tUIDesc.m_tUITextureDesc.iTextureIndex != 0
		&& m_tUIDesc.m_tUITextureDesc.iTextureIndex != 2)
	{
		if (FAILED(m_pHoldGaugeTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture1", 0)))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Bind_RawValue("g_bUseCoolTime", &bUseCoolTime, sizeof(_bool))))
			return E_FAIL;
	}

	_float fRatio = m_fInteractionTime / 1.f;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fCoolAmount", &fRatio, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUITriggerKey::Execute(const UI_EVENT_DESC& EventDesc)
{
	return S_OK;
}

void CUITriggerKey::CallbackEvent(void* pArg)
{
	UI_EVENT_ARG_DESC* pEventArg = reinterpret_cast<UI_EVENT_ARG_DESC*>(pArg);

	szActionTag = pEventArg->szActionTag;

	switch (pEventArg->Type)
	{
	case UI_EVENT_ARG_DESC::FLOAT:
	{
		_float fTime = *reinterpret_cast<_float*>(pEventArg->pData);
		m_fInteractionTime = fTime;
		break;
	}
	case UI_EVENT_ARG_DESC::BOOL:
	{
		_bool bActive = *reinterpret_cast<_bool*>(pEventArg->pData);
		m_isActive = bActive;
		break;
	}
	}
}


CUITriggerKey* CUITriggerKey::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUITriggerKey* pInstance = new CUITriggerKey(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUITriggerKey::Clone(void* pArg)
{
	CUITriggerKey* pInstance = new CUITriggerKey(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUITriggerKey");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUITriggerKey::Free()
{
	__super::Free();

	Safe_Release(m_pHoldGaugeTextureCom);
}
