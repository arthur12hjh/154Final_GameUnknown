#include "pch.h"
#include "UIButton.h"

#include "GameInstance.h"
#include "UIHUD.h"
#include "UIPlayAnimEvent.h"
#include "Level_Loading.h"

CUIButton::CUIButton(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIBase{ pDevice, pContext }
{
}

CUIButton::CUIButton(const CUIButton& Prototype) 
	: CUIBase{ Prototype }
{
}

HRESULT CUIButton::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUIButton::Initialize(void* pArg)
{	
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CUIButton::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CUIButton::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

	if (MouseEnter())
	{
		m_eBtnState = BTN_STATE::HOVER;

		if (m_pGameInstance->KeyDown(KEY_INPUT::MOUSE, 0))
			m_eBtnState = BTN_STATE::CLICK;
	}
	else
		m_eBtnState = BTN_STATE::DEFAULT;
}

void CUIButton::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	if (m_ePrevBtnState != m_eBtnState)
	{
		UI_EVENT_ARG_DESC Arg{};
		Arg.Type = UI_EVENT_ARG_DESC::BTN_STATE;
		Arg.pData = &m_eBtnState;

		switch (m_eBtnState)
		{
		case CUIButton::BTN_STATE::CLICK:
		{
			_bool bTrue = true;
			Arg.Type = UI_EVENT_ARG_DESC::BOOL;
			Arg.pData = &bTrue;
			__super::Trigger_Event(TEXT("Click"), &Arg);

			m_pGameInstance->Manager_PlaySound(TEXT("LOGO_BTN_CLICK.wav"), CHANNELID::EFFECT, 1.f, 1.f);
			m_eBtnState = BTN_STATE::HOVER;
			break;
		}
		case CUIButton::BTN_STATE::DEFAULT:
		{
			__super::Trigger_Event(TEXT("Default"), &Arg);
			break;
		}
		case CUIButton::BTN_STATE::HOVER:
		{
			m_pGameInstance->Manager_PlaySound(TEXT("LOGO_BTN_HOVER.wav"), CHANNELID::EFFECT, 1.f, 1.f);
			__super::Trigger_Event(TEXT("Hover"), &Arg);
			break;
		}
		}

		m_ePrevBtnState = m_eBtnState;
	}
}

HRESULT CUIButton::Render()
{
	__super::Render();

	if (m_tUIDesc.Get_UI_Texture_Desc())
	{
		if (FAILED(Bind_ShaderResources()))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(m_tUIDesc.m_tUITextureDesc.iPass)))
			return E_FAIL;

		if (FAILED(m_pVIBufferCom->Bind_Resources()))
			return E_FAIL;

		if (FAILED(m_pVIBufferCom->Render()))
			return E_FAIL;
	}

#ifdef _DEBUG
	__super::Render_Debug_Rect();
#endif

	return S_OK;
}

HRESULT CUIButton::Ready_Components()
{
	__super::Ready_Components();

	/* Com_VIBuffer */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUIButton::Bind_ShaderResources()
{
	__super::Bind_ShaderResources();

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->GetIdentityMatrixPtr())))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;
	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_tUIDesc.m_tUITextureDesc.iTextureIndex)))
		return E_FAIL;

	return S_OK;
}

HRESULT CUIButton::Execute(const UI_EVENT_DESC& EventDesc)
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
	// 액션
	//if (Type == TEXT("ActionEvent"))
	//{
	//	if (Arg == TEXT("Open_Popup"))
	//	{
	//		//팝업 오픈...
	//	}
	//}
	//// 사운드
	//if (Type == TEXT("PlaySoundEvent"))
	//{
	//	SoundMgr->Play()
	//}
	
	return S_OK;
}

void CUIButton::CallbackEvent(void* pArg)
{
}

_bool CUIButton::MouseEnter()
{
	POINT MousePoint = m_pGameInstance->GetMousePoint();

	_float4 fRect = {
		m_tUIDesc.fX + m_tUIDesc.fOffsetX - m_tUIDesc.fSizeX * 0.5f,
		m_tUIDesc.fY + m_tUIDesc.fOffsetY - m_tUIDesc.fSizeY * 0.5f,
		m_tUIDesc.fX + m_tUIDesc.fOffsetX + m_tUIDesc.fSizeX * 0.5f,
		m_tUIDesc.fY + m_tUIDesc.fOffsetY + m_tUIDesc.fSizeY * 0.5f
	};

	return (
		MousePoint.x >= fRect.x &&
		MousePoint.y >= fRect.y &&
		MousePoint.x <= fRect.z &&
		MousePoint.y <= fRect.w);
}

CUIButton* CUIButton::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUIButton* pInstance = new CUIButton(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUIButton::Clone(void* pArg)
{
	CUIButton* pInstance = new CUIButton(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUIButton");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUIButton::Free()
{
	__super::Free();
}
