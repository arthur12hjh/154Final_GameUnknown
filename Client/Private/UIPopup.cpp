#include "pch.h"
#include "UIPopup.h"

#include "GameInstance.h"
#include "UIHUD.h"

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
	__super::Priority_Update(fTimeDelta);

	if (m_tUIDesc.fAlpha <= 0.f)
		m_eVisibility = VISIBILITY::HIDDEN;
	else
		m_eVisibility = VISIBILITY::VISIBLE;
}

void CUIPopup::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void CUIPopup::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	//m_fAmount = (4 / (_float)6) / 1.f;
}

HRESULT CUIPopup::Render()
{
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

	if (m_tUIDesc.m_tUIPopupDesc.isDimed)
	{
		if (FAILED(m_pDimTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture0", 0)))
			return E_FAIL;
	}
	else
	{
		if (FAILED(m_pShaderCom->Bind_SRV("g_Texture0", nullptr)))
			return E_FAIL;
	}

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture1", 0)))
		return E_FAIL;

	_float2 vPos = { m_tUIDesc.m_tUIPopupDesc.fPosX, m_tUIDesc.m_tUIPopupDesc.fPosY };
	if (FAILED(m_pShaderCom->Bind_RawValue("g_UIPosition", &vPos, sizeof(_float2))))
		return E_FAIL;

	_float2 vSize = { m_tUIDesc.m_tUIPopupDesc.fSizeX, m_tUIDesc.m_tUIPopupDesc.fSizeY };
	if (FAILED(m_pShaderCom->Bind_RawValue("g_UISize", &vSize, sizeof(_float2))))
		return E_FAIL;

	//if (FAILED(m_pInnerFrameTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture2", 0)))
	//	return E_FAIL;
	//if (FAILED(m_pSuitIconsTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture3", 0)))
	//	return E_FAIL;

	///*int col = iconIndex % ATLAS_COL;
	//int row = iconIndex / ATLAS_COL;*/

	//_float2 fAtlasCount = { 6.f, 2.f };
	//if(FAILED(m_pShaderCom->Bind_RawValue("g_AtlasCount", &fAtlasCount, sizeof(_float2))))
	//	return E_FAIL;

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
	if (Type == TEXT("ActionEvent"))
	{
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
	Safe_Release(m_pInnerFrameTextureCom);
	Safe_Release(m_pAnswerTextureCom);
	Safe_Release(m_pSuitIconsTextureCom);
}
