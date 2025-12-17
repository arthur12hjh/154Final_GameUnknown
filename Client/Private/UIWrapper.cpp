#include "pch.h"
#include "UIWrapper.h"

#include "GameInstance.h"
#include "UIHUD.h"

CUIWrapper::CUIWrapper(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIBase{ pDevice, pContext }
{
}

CUIWrapper::CUIWrapper(const CUIWrapper& Prototype) 
	: CUIBase{ Prototype }
{
}

HRESULT CUIWrapper::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUIWrapper::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CUIWrapper::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);

	if (m_tUIDesc.fAlpha <= 0.f)
		m_eVisibility = VISIBILITY::HIDDEN;
	else
		m_eVisibility = VISIBILITY::VISIBLE;
}

void CUIWrapper::Update(_float fTimeDelta)
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

void CUIWrapper::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	if (m_ePrevBtnState != m_eBtnState)
	{
		UI_EVENT_ARG_DESC Arg{};
		Arg.Type = UI_EVENT_ARG_DESC::BTN_STATE;
		Arg.pData = &m_eBtnState;

		switch (m_eBtnState)
		{
		case CUIWrapper::BTN_STATE::CLICK:
		{
			_bool bTrue = true;
			Arg.Type = UI_EVENT_ARG_DESC::BOOL;
			Arg.pData = &bTrue;

			__super::Trigger_Event(TEXT("Rush_Active"), &Arg);
			break;
		}
		case CUIWrapper::BTN_STATE::HOVER:
		{
			_bool bTrue = true;
			Arg.Type = UI_EVENT_ARG_DESC::BOOL;
			Arg.pData = &bTrue;

			__super::Trigger_Event(TEXT("Skill_Active"), &Arg);
			break;
		}
		//case CUIWrapper::BTN_STATE::DEFAULT:
		//{
		//	break;
		//}
		}

		m_ePrevBtnState = m_eBtnState;
	}
}

HRESULT CUIWrapper::Render()
{
	__super::Render();

	if (m_tUIDesc.Get_UI_Texture_Desc())
	{
		if (FAILED(Bind_ShaderResources()))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(m_tUIDesc.Get_UI_Texture_Desc()->iPass)))
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

HRESULT CUIWrapper::Ready_Components()
{
	__super::Ready_Components();

	/* Com_VIBuffer */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIWrapper::Bind_ShaderResources()
{
	__super::Bind_ShaderResources();

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->GetIdentityMatrixPtr())))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;
	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_tUIDesc.Get_UI_Texture_Desc()->iTextureIndex)))
		return E_FAIL;

	return S_OK;
}

HRESULT CUIWrapper::Execute(const UI_EVENT_DESC& EventDesc)
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

void CUIWrapper::CallbackEvent(void* pArg)
{
	auto* arg = static_cast<UI_EVENT_ARG_DESC*>(pArg);
	if (!arg) return;

	CUIHUD* pHUD = dynamic_cast<CUIHUD*>(m_pGameInstance->GetCurrentLevelHUD());

	auto AnimTag = m_tUIDesc.m_AnimTags.find(arg->szActionTag);
	if (AnimTag != m_tUIDesc.m_AnimTags.end())
		pHUD->Anim_Play(m_tUIDesc.szLayerTag, m_tUIDesc.szUITag, AnimTag->first);

	Safe_Release(pHUD);
}

_bool CUIWrapper::MouseEnter()
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

CUIWrapper* CUIWrapper::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUIWrapper* pInstance = new CUIWrapper(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUIWrapper::Clone(void* pArg)
{
	CUIWrapper* pInstance = new CUIWrapper(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUIWrapper");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUIWrapper::Free()
{
	__super::Free();
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////