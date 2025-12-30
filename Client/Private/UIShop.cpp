#include "pch.h"
#include "UIShop.h"

#include "GameInstance.h"
#include "UIHUD.h"

CUIShop::CUIShop(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIBase{ pDevice, pContext }
{
}

CUIShop::CUIShop(const CUIShop& Prototype) 
	: CUIBase{ Prototype }
{
}

HRESULT CUIShop::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUIShop::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CUIShop::Priority_Update(_float fTimeDelta)
{
	if (!m_isOpen)
		return;

	__super::Priority_Update(fTimeDelta);

	if (m_tUIDesc.fAlpha <= 0.f)
		m_eVisibility = VISIBILITY::HIDDEN;
	else
		m_eVisibility = VISIBILITY::VISIBLE;
}

void CUIShop::Update(_float fTimeDelta)
{
	if (!m_isOpen)
		return;

	__super::Update(fTimeDelta);

	if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_ESCAPE) && m_eVisibility == VISIBILITY::VISIBLE)
		Close_Shop();
}

void CUIShop::Late_Update(_float fTimeDelta)
{
	if (m_isOpen && m_isClosing && IsAnimFinished(TEXT("Shop_Close")))
	{
		m_pGameInstance->SetGamePause(false);
		g_bIsMouseLock = true;

		m_eVisibility = VISIBILITY::HIDDEN;

		for (auto& pChild : m_Children)
			Update_Children(pChild);

		m_isOpen = false;
		m_isClosing = false;
	}

	if (!m_isOpen)
		return;

	__super::Late_Update(fTimeDelta);
}

HRESULT CUIShop::Render()
{
	if (!m_isOpen)
		return S_OK;

	__super::Render();

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(UI_SHADER_PASS::SHOP))))
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

HRESULT CUIShop::Ready_Components()
{
	__super::Ready_Components();

	/* Com_VIBuffer */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* Com_Texture_UI_Frame */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Shop_Frame"),
		TEXT("Com_Texture_UI_Shop_Frame"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* Com_Texture_UI_Shop_Npc */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Shop_Npc"),
		TEXT("Com_Texture_UI_Shop_Npc"), reinterpret_cast<CComponent**>(&m_pNpcTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUIShop::Bind_ShaderResources()
{
	__super::Bind_ShaderResources();

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->GetIdentityMatrixPtr())))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;
	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture0", 0)))
		return E_FAIL;
	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture1", 1)))
		return E_FAIL;
	if (FAILED(m_pNpcTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture2", 0)))
		return E_FAIL;

	return S_OK;
}

HRESULT CUIShop::Execute(const UI_EVENT_DESC& EventDesc)
{
	const _wstring& Type = EventDesc.szTypeTag;
	const _wstring& Arg = EventDesc.szArg;
	const _wstring& ActionTag = EventDesc.szActionTag;

	// 애니메이션
	if (Type == TEXT("PlayAnimEvent"))
	{
		CUIHUD* pHUD = dynamic_cast<CUIHUD*>(m_pGameInstance->GetCurrentLevelHUD());
		auto AnimTag = m_tUIDesc.m_AnimTags.find(ActionTag);

		if (AnimTag != m_tUIDesc.m_AnimTags.end())
			pHUD->Anim_Play(m_tUIDesc.szLayerTag, m_tUIDesc.szUITag, AnimTag->first);

		Safe_Release(pHUD);
	}

	return S_OK;
}

void CUIShop::CallbackEvent(void* pArg)
{
	auto* arg = static_cast<UI_EVENT_ARG_DESC*>(pArg);
	if (!arg) return;

	CUIHUD* pHUD = dynamic_cast<CUIHUD*>(m_pGameInstance->GetCurrentLevelHUD());

	auto AnimTag = m_tUIDesc.m_AnimTags.find(arg->szActionTag);
	if (AnimTag != m_tUIDesc.m_AnimTags.end())
		pHUD->Anim_Play(m_tUIDesc.szLayerTag, m_tUIDesc.szUITag, AnimTag->first);

	Safe_Release(pHUD);
}

void CUIShop::Open_Shop()
{
	m_pGameInstance->SetGamePause(true);
	g_bIsMouseLock = false;

	CUIHUD* pHUD = dynamic_cast<CUIHUD*>(m_pGameInstance->GetCurrentLevelHUD());
	auto AnimTag = m_tUIDesc.m_AnimTags.find(TEXT("Shop_Open"));

	if (AnimTag != m_tUIDesc.m_AnimTags.end())
		pHUD->Anim_Play(m_tUIDesc.szLayerTag, m_tUIDesc.szUITag, AnimTag->first);

	Safe_Release(pHUD);

	m_eVisibility = VISIBILITY::VISIBLE;

	for (auto& pChild : m_Children)
		Update_Children(pChild);
	
	m_isOpen = true;
	m_isClosing = false;
}

void CUIShop::Close_Shop()
{
	CUIHUD* pHUD = dynamic_cast<CUIHUD*>(m_pGameInstance->GetCurrentLevelHUD());
	auto AnimTag = m_tUIDesc.m_AnimTags.find(TEXT("Shop_Close"));

	if (AnimTag != m_tUIDesc.m_AnimTags.end())
		pHUD->Anim_Play(m_tUIDesc.szLayerTag, m_tUIDesc.szUITag, AnimTag->first);

	Safe_Release(pHUD);

	m_isClosing = true;
}

CUIShop* CUIShop::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUIShop* pInstance = new CUIShop(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUIShop::Clone(void* pArg)
{
	CUIShop* pInstance = new CUIShop(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUIShop");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUIShop::Free()
{
	__super::Free();

	Safe_Release(m_pNpcTextureCom);
}
