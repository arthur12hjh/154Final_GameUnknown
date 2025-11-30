#include "pch.h"
#include "UISkillSlot.h"

#include "GameInstance.h"
#include "GameManager.h"
#include "Player.h"

#include "UIHUD.h"
#include "UISkillWrapper.h"

CUISkillSlot::CUISkillSlot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIBase{ pDevice, pContext }
{
}

CUISkillSlot::CUISkillSlot(const CUISkillSlot& Prototype) 
	: CUIBase{ Prototype }
{
}

HRESULT CUISkillSlot::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUISkillSlot::Initialize(void* pArg)
{	
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	/*auto pCharactor{ CGameManager::GetInstance()->GetGameCharacter() };

	if (pCharactor)
	{
		m_iMaxPotions = const_cast<_int*>(&CGameManager::GetInstance()->Get_PlayerDesc()->iMaxPotions);
		m_iPotions = const_cast<_int*>(&CGameManager::GetInstance()->Get_PlayerDesc()->iCurrentPotions);
	}

#ifdef _DEBUG
	else
	{
		m_iMaxPotions = &m_MaxGara;
		m_iPotions = &m_Gara;
	}
#endif
	
	Safe_Release(pCharactor);*/

	return S_OK;
}

void CUISkillSlot::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CUISkillSlot::Update(_float fTimeDelta)
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

void CUISkillSlot::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
}

HRESULT CUISkillSlot::Render()
{
	__super::Render();

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(8)))
		return E_FAIL;

	if (FAILED(m_pVIBaseBufferCom->Bind_Resources()))
		return E_FAIL;

	if (FAILED(m_pVIBaseBufferCom->Render()))
		return E_FAIL;

	if (m_eSkillState != SKILL_STATE::DEFAULT)
	{
		if (FAILED(Render_Glow()))
			return E_FAIL;
	}

#ifdef _DEBUG
	__super::Render_Debug_Rect();
#endif

	return S_OK;
}

HRESULT CUISkillSlot::Ready_Components()
{
	__super::Ready_Components();

	/* Com_VIBuffer */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect_Instance"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBaseBufferCom))))
		return E_FAIL;

	/* Com_VIBuffer */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect_Instance"),
		TEXT("Com_VIBuffer_Glow"), reinterpret_cast<CComponent**>(&m_pVIGlowBufferCom))))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_SkillFrame_Shadow"),
		TEXT("Com_ShadowTexture"), reinterpret_cast<CComponent**>(&m_pShadowTextureCom))))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_SkillFrame"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_On_Ring"),
		TEXT("Com_GlowTexture"), reinterpret_cast<CComponent**>(&m_pGlowTextureCom))))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Skill_Focus_Glow"),
		TEXT("Com_GlowTexture2"), reinterpret_cast<CComponent**>(&m_pGlowTextureCom2))))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Skills"),
		TEXT("Com_Texture2"), reinterpret_cast<CComponent**>(&m_pTextureCom2))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUISkillSlot::Bind_ShaderResources()
{
	__super::Bind_ShaderResources();

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->GetIdentityMatrixPtr())))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;
	if (FAILED(m_pShadowTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture0", 0))) 
		return E_FAIL;
	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture1", 0)))
		return E_FAIL;
	if (FAILED(m_pTextureCom2->Bind_ShaderResource(m_pShaderCom, "g_Texture2", 0))) // 스킬 텍스쳐 : 플레이어한테서 받아야함
		return E_FAIL;

	_bool bUseTintColor = false;

	if (m_eSkillState == SKILL_STATE::DEFAULT)
		bUseTintColor = true;
	
	if (FAILED(m_pShaderCom->Bind_RawValue("g_bUseTintColor", &bUseTintColor, sizeof(_bool))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vTintColor", &m_tUIDesc.m_tUIShaderDesc.vTintColor, sizeof(_float4))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUISkillSlot::Execute(const UI_EVENT_DESC& EventDesc)
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

void CUISkillSlot::CallbackEvent(void* pArg)
{
	auto* arg = static_cast<UI_EVENT_ARG_DESC*>(pArg);
	if (!arg) return;

	auto* state = static_cast<SKILL_STATE*>(arg->pData);
	
	m_eSkillState = *state;

	CUIHUD* pHUD = dynamic_cast<CUIHUD*>(m_pGameInstance->GetCurrentLevelHUD());
	
	// 이거 애니메이션 매니저로 돌리면 안될지도..
	if (*state == SKILL_STATE::DEFAULT)
		pHUD->Anim_Stop(m_tUIDesc.szLayerTag, m_tUIDesc.szUITag);
	else
	{
		auto AnimTag = m_tUIDesc.m_AnimTags.find(arg->szActionTag);
		if (AnimTag != m_tUIDesc.m_AnimTags.end())
			pHUD->Anim_Play(m_tUIDesc.szLayerTag, m_tUIDesc.szUITag, AnimTag->first);
	}

	Safe_Release(pHUD);
}

HRESULT CUISkillSlot::Render_Glow()
{
	if (FAILED(Bind_GlowShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(9)))
		return E_FAIL;

	if (FAILED(m_pVIGlowBufferCom->Bind_Resources()))
		return E_FAIL;

	if (FAILED(m_pVIGlowBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUISkillSlot::Bind_GlowShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->GetIdentityMatrixPtr())))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;
	if (FAILED(m_pGlowTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture0", 0)))
		return E_FAIL;
	if (FAILED(m_pShadowTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture1", 1)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_bUseGlow", &m_tUIDesc.m_tUIShaderDesc.bUseGlow, sizeof(_bool))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_GlowIntensity", &m_tUIDesc.m_tUIShaderDesc.fGlowIntensity, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

CUISkillSlot* CUISkillSlot::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUISkillSlot* pInstance = new CUISkillSlot(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUISkillSlot::Clone(void* pArg)
{
	CUISkillSlot* pInstance = new CUISkillSlot(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUISkillSlot");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUISkillSlot::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom2);
	Safe_Release(m_pShadowTextureCom);
	Safe_Release(m_pGlowTextureCom);
	Safe_Release(m_pGlowTextureCom2);
	Safe_Release(m_pVIBaseBufferCom);
	Safe_Release(m_pVIGlowBufferCom);
}
