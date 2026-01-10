#include "pch.h"
#include "UIPopup.h"

#include "GameInstance.h"
#include "GameManager.h"
#include "UIHUD.h"

#include "Player.h"

CUIPopup::CUIPopup(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIBase{ pDevice, pContext }
{
}

CUIPopup::CUIPopup(const CUIPopup& Prototype) 
	: CUIBase{ Prototype }
{
}

HRESULT CUIPopup::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUIPopup::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CUIPopup::Priority_Update(_float fTimeDelta)
{
	if (!m_isOpen)
		return;

	__super::Priority_Update(fTimeDelta);

	if (m_tUIDesc.fAlpha <= 0.f)
		m_eVisibility = VISIBILITY::HIDDEN;
	else
		m_eVisibility = VISIBILITY::VISIBLE;
}

void CUIPopup::Update(_float fTimeDelta)
{
	if (!m_isOpen)
		return;

	__super::Update(fTimeDelta);

	if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_ESCAPE) && m_eVisibility == VISIBILITY::VISIBLE)
		Close_Popup();
}

void CUIPopup::Late_Update(_float fTimeDelta)
{
	if (m_isOpen && m_isClosing && IsAnimFinished(TEXT("Popup_Close")))
	{
		m_pGameInstance->SetGamePause(false);
		g_bIsMouseLock = true;

		auto pPlayer = m_pGameManager->GetGameCharacter();

		if (!pPlayer)
		{
			Safe_Release(pPlayer);
			return;
		}

		auto PlayerDesc = dynamic_cast<CPlayer*>(pPlayer)->Get_Desc();

		PlayerDesc->pPlayerController->Set_Active(true);

		m_eVisibility = VISIBILITY::HIDDEN;

		for (auto& pChild : m_Children)
			Update_Children(pChild);

		m_isOpen = false;
		m_isClosing = false;

		LEVEL_CHANGER LevelChanger{ m_isLevelChange, ENUM_CLASS(m_eTargetLevel) };

		UI_EVENT_ARG_DESC Arg{};
		Arg.Type = UI_EVENT_ARG_DESC::LEVEL_CHANGER;
		Arg.pData = &LevelChanger;
		__super::Trigger_Event(TEXT("Change_Map"), &Arg);

		UI_EVENT_ARG_DESC Arg2{};
		Arg2.Type = UI_EVENT_ARG_DESC::BOOL;
		Arg2.pData = &m_isTeleport;
		__super::Trigger_Event(TEXT("TelePort"), &Arg2);

		m_isTeleport = false;
		m_isLevelChange = false;

		Safe_Release(pPlayer);
	}

	if (!m_isOpen)
		return;

	__super::Late_Update(fTimeDelta);
}

HRESULT CUIPopup::Render()
{
	if (!m_isOpen)
		return S_OK;

	__super::Render();

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(UI_SHADER_PASS::POPUP))))
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

HRESULT CUIPopup::Ready_Components()
{
	__super::Ready_Components();

	/* Com_VIBuffer */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* Com_Texture_Dim */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Overlay"),
		TEXT("Com_Texture_UI_Dim"), reinterpret_cast<CComponent**>(&m_pDimTextureCom))))
		return E_FAIL;

	/* Com_Texture_PopupBG */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_PopupBG"),
		TEXT("Com_Texture_UI_PopupBG"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUIPopup::Bind_ShaderResources()
{
	__super::Bind_ShaderResources();

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->GetIdentityMatrixPtr())))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_isActive", &m_tUIDesc.m_tUIPopupDesc.isDimed, sizeof(_bool))))
		return E_FAIL;

	/*if (m_tUIDesc.m_tUIPopupDesc.isDimed)
	{
		if (FAILED(m_pDimTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture0", 0)))
			return E_FAIL;
	}
	else
	{
		if (FAILED(m_pShaderCom->Bind_SRV("g_Texture0", nullptr)))
			return E_FAIL;
	}*/

	_int iTextureIndex = 0;

	if (m_tUIDesc.fSizeX >= 1000.f)
		iTextureIndex = 1;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture1", iTextureIndex)))
		return E_FAIL;

	/*_float2 vPos = { m_tUIDesc.fX, m_tUIDesc.fY };
	if (FAILED(m_pShaderCom->Bind_RawValue("g_UIPosition", &vPos, sizeof(_float2))))
		return E_FAIL;

	_float2 vSize = { m_tUIDesc.fSizeX, m_tUIDesc.fSizeY };
	if (FAILED(m_pShaderCom->Bind_RawValue("g_UISize", &vSize, sizeof(_float2))))
		return E_FAIL;*/

	return S_OK;
}

HRESULT CUIPopup::Execute(const UI_EVENT_DESC& EventDesc)
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

void CUIPopup::CallbackEvent(void* pArg)
{
	auto* arg = static_cast<UI_EVENT_ARG_DESC*>(pArg);
	if (!arg) return;

	CUIHUD* pHUD = dynamic_cast<CUIHUD*>(m_pGameInstance->GetCurrentLevelHUD());

	auto AnimTag = m_tUIDesc.m_AnimTags.find(arg->szActionTag);
	if (AnimTag != m_tUIDesc.m_AnimTags.end())
		pHUD->Anim_Play(m_tUIDesc.szLayerTag, m_tUIDesc.szUITag, AnimTag->first);

	Safe_Release(pHUD);
}

void CUIPopup::Open_Popup()
{
	m_pGameInstance->SetGamePause(true);
	g_bIsMouseLock = false;

	CUIHUD* pHUD = dynamic_cast<CUIHUD*>(m_pGameInstance->GetCurrentLevelHUD());
	auto AnimTag = m_tUIDesc.m_AnimTags.find(TEXT("Popup_Open"));

	if (AnimTag != m_tUIDesc.m_AnimTags.end())
		pHUD->Anim_Play(m_tUIDesc.szLayerTag, m_tUIDesc.szUITag, AnimTag->first);

	Safe_Release(pHUD);

	m_eVisibility = VISIBILITY::VISIBLE;

	for (auto& pChild : m_Children)
		Update_Children(pChild);
	
	m_isOpen = true;
	m_isClosing = false;
}

void CUIPopup::Close_Popup(_bool isLevelChange, _bool isTeleport)
{
	CUIHUD* pHUD = dynamic_cast<CUIHUD*>(m_pGameInstance->GetCurrentLevelHUD());
	auto AnimTag = m_tUIDesc.m_AnimTags.find(TEXT("Popup_Close"));

	if (AnimTag != m_tUIDesc.m_AnimTags.end())
		pHUD->Anim_Play(m_tUIDesc.szLayerTag, m_tUIDesc.szUITag, AnimTag->first);

	Safe_Release(pHUD);

	m_isClosing = true;

	if (isLevelChange)
		m_isLevelChange = isLevelChange;
	else
		m_isTeleport = isTeleport;
}

CUIPopup* CUIPopup::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUIPopup* pInstance = new CUIPopup(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUIPopup::Clone(void* pArg)
{
	CUIPopup* pInstance = new CUIPopup(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUIPopup");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUIPopup::Free()
{
	__super::Free();

	Safe_Release(m_pDimTextureCom);
}
