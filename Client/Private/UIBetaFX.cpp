#include "pch.h"
#include "UIBetaFX.h"

#include "GameInstance.h"
#include "UIHUD.h"

#include "UIBeta.h"

CUIBetaFX::CUIBetaFX(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIBase{ pDevice, pContext }
{
}

CUIBetaFX::CUIBetaFX(const CUIBetaFX& Prototype) 
	: CUIBase{ Prototype }
{
}

HRESULT CUIBetaFX::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUIBetaFX::Initialize(void* pArg)
{	
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;


	return S_OK;
}

void CUIBetaFX::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CUIBetaFX::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

	if (static_cast<CUIBeta*>(m_pParent))
		m_iCurrentGroup = static_cast<CUIBeta*>(m_pParent)->Get_CurrentGroupIdx();
	
	auto pBeta = static_cast<CUIBeta*>(m_pParent);
	if (!pBeta) return;

	_float fOffset = ((m_iCurrentGroup - 1) * 14.f) + (m_iCurrentGroup * 8.f);
	
	// 부모 기준 위치
	_vector vParentPos = pBeta->GetTransform()->Get_State(STATE::POSITION);
	
	_float posX = fOffset + (pBeta->Get_UIBase_Desc().fSizeX * -0.5f);

	// Glow 중심 위치 = Parent 중심 + posX
	_vector vGlowPos = XMVectorSet(
		XMVectorGetX(vParentPos) + posX,
		XMVectorGetY(vParentPos),
		XMVectorGetZ(vParentPos),
		1.f
	);

	m_pTransformCom->Set_State(STATE::POSITION, vGlowPos);

	if (m_iCurrentGroup != m_iPrevGroup && m_iCurrentGroup > m_iPrevGroup)
	{
		m_fTimeAcc += fTimeDelta;
		m_tUIDesc.fAlpha = 1.f;

		if (!m_isActive && m_tUIDesc.m_tUIShaderDesc.fScale > 0.5f)
		{
			m_tUIDesc.m_tUIShaderDesc.fScale -= m_fTimeAcc;
			m_tUIDesc.fRotation += XMConvertToRadians(360.f) * (fTimeDelta);

			if (m_tUIDesc.fRotation >= XM_2PI)
				m_tUIDesc.fRotation -= XM_2PI;
		}
		else
			m_isActive = true;

		if (m_isActive && m_tUIDesc.m_tUIShaderDesc.fGlowIntensity <= 1.f)
			m_tUIDesc.m_tUIShaderDesc.fGlowIntensity += (fTimeDelta);
		else if (m_isActive && m_tUIDesc.m_tUIShaderDesc.fGlowIntensity > 1.f)
		{
			m_tUIDesc.fAlpha = 0.f;
			m_iPrevGroup = m_iCurrentGroup;
		}
	}
	else
	{
		m_tUIDesc.m_tUIShaderDesc.fScale = 1.f;
		m_tUIDesc.fRotation = 0.f;
		m_tUIDesc.m_tUIShaderDesc.fGlowIntensity = 0.5f;
		m_tUIDesc.fAlpha = 0.f;
		m_fTimeAcc = 0.f;
		m_isActive = false;
		m_iPrevGroup = m_iCurrentGroup;
	}
	

	/*if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_B))
	{
		if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_7))
		{
			m_fTimeAcc += fTimeDelta;
			m_tUIDesc.fAlpha = 1.f;

			if (!m_isActive && m_tUIDesc.m_tUIShaderDesc.fScale > 0.5f)
			{
				m_tUIDesc.m_tUIShaderDesc.fScale -= m_fTimeAcc;
				m_tUIDesc.fRotation += XMConvertToRadians(360.f) * (fTimeDelta * 2.f);

				if (m_tUIDesc.fRotation >= XM_2PI)
					m_tUIDesc.fRotation -= XM_2PI;
			}
			else
				m_isActive = true;

			if (m_isActive && m_tUIDesc.m_tUIShaderDesc.fGlowIntensity <= 1.f)
				m_tUIDesc.m_tUIShaderDesc.fGlowIntensity += m_fTimeAcc;
			else if(m_isActive && m_tUIDesc.m_tUIShaderDesc.fGlowIntensity > 1.f)
				m_tUIDesc.fAlpha = 0.f;
		}
		if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_8))
		{
			m_tUIDesc.m_tUIShaderDesc.fScale = 1.f;
			m_tUIDesc.fRotation = 0.f;
			m_tUIDesc.m_tUIShaderDesc.fGlowIntensity = 0.f;
			m_tUIDesc.fAlpha = 0.f;
			m_fTimeAcc = 0.f;
			m_isActive = false;
		}
	}*/
}

void CUIBetaFX::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
}

HRESULT CUIBetaFX::Render()
{
	__super::Render();

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(UI_SHADER_PASS::BETA_FX))))
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

HRESULT CUIBetaFX::Ready_Components()
{
	__super::Ready_Components();

	/* Com_VIBuffer */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect_Instance"),
		TEXT("Com_VIBaseBuffer"), reinterpret_cast<CComponent**>(&m_pVIBaseBufferCom))))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Player_BetaFX"),
		TEXT("Com_Texture_UI_Beta_FX"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUIBetaFX::Bind_ShaderResources()
{
	if (FAILED(__super::Initialize_ShaderResources()))
		return E_FAIL;

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

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fScale", &m_tUIDesc.m_tUIShaderDesc.fScale, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fRotation", &m_tUIDesc.fRotation, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_isActive", &m_isActive, sizeof(_bool))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_Alpha", &m_tUIDesc.fAlpha, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_GlowIntensity", &m_tUIDesc.m_tUIShaderDesc.fGlowIntensity, sizeof(_float))))
		return E_FAIL;
	/*
	if (FAILED(m_pShaderCom->Bind_RawValue("g_bUseGlow", &m_tUIDesc.m_tUIShaderDesc.bUseGlow, sizeof(_bool))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_bUseScale", &m_tUIDesc.m_tUIShaderDesc.bUseScale, sizeof(_bool))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fScale", &m_tUIDesc.m_tUIShaderDesc.fScale, sizeof(_float))))
		return E_FAIL;*/

	return S_OK;
}

HRESULT CUIBetaFX::Execute(const UI_EVENT_DESC& EventDesc)
{
	return S_OK;
}

void CUIBetaFX::CallbackEvent(void* pArg)
{
	auto* arg = static_cast<UI_EVENT_ARG_DESC*>(pArg);
	if (!arg) return;

	CUIHUD* pHUD = dynamic_cast<CUIHUD*>(m_pGameInstance->GetCurrentLevelHUD());

	auto AnimTag = m_tUIDesc.m_AnimTags.find(arg->szActionTag);
	if (AnimTag != m_tUIDesc.m_AnimTags.end())
		pHUD->Anim_Play(m_tUIDesc.szLayerTag, m_tUIDesc.szUITag, AnimTag->first);

	Safe_Release(pHUD);
}

CUIBetaFX* CUIBetaFX::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUIBetaFX* pInstance = new CUIBetaFX(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUIBetaFX::Clone(void* pArg)
{
	CUIBetaFX* pInstance = new CUIBetaFX(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUIBetaFX");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUIBetaFX::Free()
{
	__super::Free();

	Safe_Release(m_pVIBaseBufferCom);
}
