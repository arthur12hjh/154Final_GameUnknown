#include "pch.h"
#include "UIText.h"

#include "GameInstance.h"
#include "UIButton.h"
#include "UIPlayAnimEvent.h"
#include "UIHUD.h"

CUIText::CUIText(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIBase{ pDevice, pContext }
{
}

CUIText::CUIText(const CUIText& Prototype) 
	: CUIBase{ Prototype }
{
}

HRESULT CUIText::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUIText::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CUIText::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CUIText::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void CUIText::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
}

HRESULT CUIText::Render()
{
	__super::Render();

	if (m_tUIDesc.Get_UI_Text_Desc())
	{
		_float2 fTextSize = m_pGameInstance->Get_Text_Size(TEXT("KoPub"), m_tUIDesc.m_tUITextDesc.szText.c_str());
		_float fAlpha{ m_tUIDesc.fAlpha };
		/*if (m_pParent)
			fAlpha = m_tUIDesc.fAlpha * dynamic_cast<CUIBase*>(m_pParent)->Get_UIBase_Desc().fAlpha;*/

		_vector vColor = XMVectorSet(
			m_tUIDesc.m_tUITextDesc.vColor.x * fAlpha,
			m_tUIDesc.m_tUITextDesc.vColor.y * fAlpha,
			m_tUIDesc.m_tUITextDesc.vColor.z * fAlpha,
			m_tUIDesc.m_tUITextDesc.vColor.w * fAlpha
			);

		m_pGameInstance->Render_Text(TEXT("KoPub"),
			m_tUIDesc.Get_UI_Text_Desc()->szText.c_str(),
			_float2(m_tUIDesc.fX + m_tUIDesc.fOffsetX - fTextSize.x * 0.5f, m_tUIDesc.fY + m_tUIDesc.fOffsetY - fTextSize.y * 0.5f),
			vColor);
	}

#ifdef _DEBUG
	__super::Render_Debug_Rect();
#endif

	return S_OK;
}

HRESULT CUIText::Ready_Components()
{
	__super::Ready_Components();

	// m_Events 돌면서 분기쳐서 이벤트 핸들 세팅해주기??
	/*m_pEventHandle = CUIPlayAnimEvent::Create([&](void* pArg) {
		CUIButton::BTN_STATE eState = *static_cast<CUIButton::BTN_STATE*>(pArg);

		CUIHUD* pHUD = dynamic_cast<CUIHUD*>(m_pGameInstance->GetCurrentLevelHUD());
		if (eState == CUIButton::BTN_STATE::HOVER)
		{
			auto AnimTag = m_tUIDesc.m_AnimTags.find(TEXT("Hover"));

			if (AnimTag == m_tUIDesc.m_AnimTags.end())
				return;

			pHUD->Anim_Play(m_tUIDesc.szLayerTag, m_tUIDesc.szUITag, AnimTag->first);
		}
		else if (eState == CUIButton::BTN_STATE::DEFAULT)
		{
			auto AnimTag = m_tUIDesc.m_AnimTags.find(TEXT("Default"));

			if (AnimTag == m_tUIDesc.m_AnimTags.end())
				return;

			pHUD->Anim_Play(m_tUIDesc.szLayerTag, m_tUIDesc.szUITag, AnimTag->first);
		}

		Safe_Release(pHUD);
		});

	if (nullptr == m_pEventHandle)
		return E_FAIL;

	m_pGameInstance->Add_Event(TEXT("UI_Button_Action_Reaction"), m_pEventHandle);

	Bind_Event();*/

	return S_OK;
}

HRESULT CUIText::Bind_ShaderResources()
{
	__super::Bind_ShaderResources();

	return S_OK;
}

HRESULT CUIText::Execute(const UI_EVENT_DESC& EventDesc)
{
	return S_OK;
}

//HRESULT CUIText::Broadcast_Event(const _wstring& szEventTag, const _wstring& szActionTag, void* pArg)
//{
//	return S_OK;
//}

void CUIText::CallbackEvent(void* pArg)
{
	auto* arg = static_cast<UI_EVENT_ARG_DESC*>(pArg);
	if (!arg) return;

	switch (arg->Type)
	{
	case UI_EVENT_ARG_DESC::BTN_STATE:
	{
		auto* state = static_cast<CUIButton::BTN_STATE*>(arg->pData);
		if (!state) break;

		CUIHUD* pHUD = dynamic_cast<CUIHUD*>(m_pGameInstance->GetCurrentLevelHUD());

		if (*state == CUIButton::BTN_STATE::HOVER)
		{
			auto AnimTag = m_tUIDesc.m_AnimTags.find(arg->szActionTag);
			if (AnimTag != m_tUIDesc.m_AnimTags.end())
				pHUD->Anim_Play(m_tUIDesc.szLayerTag, m_tUIDesc.szUITag, AnimTag->first);
		}
		else // DEFAULT/CLICK/SELECT → 필요에 맞게 매핑
		{
			auto AnimTag = m_tUIDesc.m_AnimTags.find(arg->szActionTag);
			if (AnimTag != m_tUIDesc.m_AnimTags.end())
				pHUD->Anim_Play(m_tUIDesc.szLayerTag, m_tUIDesc.szUITag, AnimTag->first);
		}

		Safe_Release(pHUD);
		break;
	}

	case UI_EVENT_ARG_DESC::INT:
		// int value = *static_cast<int*>(arg->pData);
		break;

	case UI_EVENT_ARG_DESC::FLOAT:
		// float v = *static_cast<float*>(arg->pData);
		break;

	case UI_EVENT_ARG_DESC::WSTRING:
		// auto* ws = static_cast<_wstring*>(arg->pData);
		break;

	default:
		break;
	}

	/*CUIButton::BTN_STATE eState = *static_cast<CUIButton::BTN_STATE*>(pArg);

	CUIHUD* pHUD = dynamic_cast<CUIHUD*>(m_pGameInstance->GetCurrentLevelHUD());
	if (eState == CUIButton::BTN_STATE::HOVER)
	{
		auto AnimTag = m_tUIDesc.m_AnimTags.find(TEXT("Hover"));

		if (AnimTag == m_tUIDesc.m_AnimTags.end())
			return;

		pHUD->Anim_Play(m_tUIDesc.szLayerTag, m_tUIDesc.szUITag, AnimTag->first);
	}
	else if (eState == CUIButton::BTN_STATE::DEFAULT)
	{
		auto AnimTag = m_tUIDesc.m_AnimTags.find(TEXT("Default"));

		if (AnimTag == m_tUIDesc.m_AnimTags.end())
			return;

		pHUD->Anim_Play(m_tUIDesc.szLayerTag, m_tUIDesc.szUITag, AnimTag->first);
	}

	Safe_Release(pHUD);*/
}

CUIText* CUIText::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUIText* pInstance = new CUIText(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUIText::Clone(void* pArg)
{
	CUIText* pInstance = new CUIText(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUIText");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUIText::Free()
{
	__super::Free();
}
