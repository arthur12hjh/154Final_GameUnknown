#include "pch.h"
#include "UIOwnGold.h"

#include "GameInstance.h"
#include "GameManager.h"
#include "UIHUD.h"

#include "Player.h"
#ifdef _DEBUG
#include "../../UI_Editor/Public/UI_Camera.h"
#endif // DEBUG

CUIOwnGold::CUIOwnGold(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIBase{ pDevice, pContext }
{
}

CUIOwnGold::CUIOwnGold(const CUIOwnGold& Prototype) 
	: CUIBase{ Prototype }
{
}

HRESULT CUIOwnGold::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUIOwnGold::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	auto pCharactor{ CGameManager::GetInstance()->GetGameCharacter() };

	if (pCharactor)
	{
		m_iOwnGold = const_cast<_int*>(&pCharactor->Get_Desc()->iOwnGold);
		m_iPrevOwnGold = *m_iOwnGold;
	}
#ifdef _DEBUG
	else
	{
		auto pGaraPlayer = dynamic_cast<CUI_Camera*>(m_pGameInstance->GetMainCamera());

		m_iOwnGold = const_cast<_int*>(&pGaraPlayer->Get_Desc()->iOwnGold);
		m_iPrevOwnGold = *m_iOwnGold;

		Safe_Release(pGaraPlayer);
	}
#endif

	Safe_Release(pCharactor);

	return S_OK;
}

void CUIOwnGold::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CUIOwnGold::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void CUIOwnGold::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	if (m_tOriginUIDesc.szLayerTag == TEXT("Layer_Shop"))
	{
		m_tUIDesc.fAlpha = 1.f;
		m_vTransOffset.y = 0.f;

		m_iPrevOwnGold = *m_iOwnGold;
	}
	else
	{
		if (m_iPrevOwnGold != *m_iOwnGold)
			m_bShow = true;

		if (m_bShow)
		{
			m_fTimeAcc += fTimeDelta;
			m_iPrevOwnGold = *m_iOwnGold;

			if (m_fTimeAcc >= 1.f)
			{
				m_tUIDesc.fAlpha -= fTimeDelta * 5.f;
				m_vTransOffset.y -= fTimeDelta * 5.f;
			}
			if (m_fTimeAcc <= 0.3f)
			{
				m_tUIDesc.fAlpha += fTimeDelta * 4.f;
				m_vTransOffset.y -= fTimeDelta * 7.f;
			}

			if (m_fTimeAcc >= 0.f && m_tUIDesc.fAlpha <= 0.f)
				m_bShow = false;
		}
		else
		{
			m_fTimeAcc = 0.f;
			m_iPrevOwnGold = *m_iOwnGold;
			m_vTransOffset = _float2(0.f, 0.f);
		}
	}
}

HRESULT CUIOwnGold::Render()
{
	__super::Render();

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(UI_SHADER_PASS::OWNGOLD))))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Resources()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	if (FAILED(RenderText()))
		return E_FAIL;

#ifdef _DEBUG
	__super::Render_Debug_Rect();
#endif

	return S_OK;
}

HRESULT CUIOwnGold::RenderText()
{
	_float2 fTextSize = m_pGameInstance->Get_Text_Size(
		TEXT("Iceberg"), to_wstring(*m_iOwnGold).c_str(), true, 1.f);
	_float fAlpha{ m_tUIDesc.fAlpha };

	_float2 vPivot{
		m_tUIDesc.fX + m_tUIDesc.fOffsetX + m_vTransOffset.x - (m_tUIDesc.fSizeX * 0.5f) + 40.f,
		m_tUIDesc.fY + m_tUIDesc.fOffsetY + m_vTransOffset.y - (m_tUIDesc.fSizeY * 0.5f) + (fTextSize.y * 0.5f) - 2.f
	};

	_float2 vShadowPivot{
		vPivot.x + 1.f,
		vPivot.y + 1.f
	};

	_vector vColor = XMVectorSet(
		1.f * fAlpha,
		1.f * fAlpha,
		1.f * fAlpha,
		1.f * fAlpha
	);
	
	_vector vShadowColor = XMVectorSet(
		0.f * fAlpha,
		0.f * fAlpha,
		0.f * fAlpha,
		1.f * fAlpha
	);

	m_pGameInstance->Render_Text(TEXT("Iceberg"),
		to_wstring(*m_iOwnGold).c_str(),
		vShadowPivot,
		vShadowColor, 1.f);

	m_pGameInstance->Render_Text(TEXT("Iceberg"),
		to_wstring(*m_iOwnGold).c_str(),
		vPivot,
		vColor, 1.f);

	return S_OK;
}

HRESULT CUIOwnGold::Ready_Components()
{
	__super::Ready_Components();

	/* Com_VIBuffer */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Icon_Gold"),
		TEXT("Com_Texture_UI_Icon_Gold"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Shadow_Block"),
		TEXT("Com_Texture_UI_Shadow_Block"), reinterpret_cast<CComponent**>(&m_pShadowTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUIOwnGold::Bind_ShaderResources()
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
	if (FAILED(m_pShadowTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture1", 0)))
		return E_FAIL;

	m_tUIDesc.m_tUIShaderDesc.fScale = m_tUIDesc.fSizeY / m_tUIDesc.fSizeX;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fScale", &m_tUIDesc.m_tUIShaderDesc.fScale, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_Alpha", &m_tUIDesc.fAlpha, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vTransOffset", &m_vTransOffset, sizeof(_float2))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUIOwnGold::Execute(const UI_EVENT_DESC& EventDesc)
{
	return S_OK;
}

void CUIOwnGold::CallbackEvent(void* pArg)
{
	auto* arg = static_cast<UI_EVENT_ARG_DESC*>(pArg);
	if (!arg) return;

	CUIHUD* pHUD = dynamic_cast<CUIHUD*>(m_pGameInstance->GetCurrentLevelHUD());

	auto AnimTag = m_tUIDesc.m_AnimTags.find(arg->szActionTag);
	if (AnimTag != m_tUIDesc.m_AnimTags.end())
		pHUD->Anim_Play(m_tUIDesc.szLayerTag, m_tUIDesc.szUITag, AnimTag->first);

	Safe_Release(pHUD);
}

CUIOwnGold* CUIOwnGold::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUIOwnGold* pInstance = new CUIOwnGold(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUIOwnGold::Clone(void* pArg)
{
	CUIOwnGold* pInstance = new CUIOwnGold(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUIOwnGold");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUIOwnGold::Free()
{
	__super::Free();

	Safe_Release(m_pShadowTextureCom);
}
