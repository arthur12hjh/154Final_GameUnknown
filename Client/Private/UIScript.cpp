#include "pch.h"
#include "UIScript.h"

#include "GameInstance.h"
#include "GameManager.h"
#include "UIHUD.h"

#include "Player.h"
#ifdef _DEBUG
#include "../../UI_Editor/Public/UI_Camera.h"
#endif // DEBUG

CUIScript::CUIScript(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIBase{ pDevice, pContext }
{
}

CUIScript::CUIScript(const CUIScript& Prototype) 
	: CUIBase{ Prototype }
{
}

HRESULT CUIScript::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUIScript::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CUIScript::Priority_Update(_float fTimeDelta)
{
	if (m_eVisibility == VISIBILITY::HIDDEN)
		return;

	__super::Priority_Update(fTimeDelta);
}

void CUIScript::Update(_float fTimeDelta)
{
	if (m_eVisibility == VISIBILITY::HIDDEN)
		return;

	__super::Update(fTimeDelta);
}

void CUIScript::Late_Update(_float fTimeDelta)
{
	if (m_eVisibility == VISIBILITY::HIDDEN)
		return;

	__super::Late_Update(fTimeDelta);

	if (m_pScriptDesc->bCanControl)
		ScriptControl();

	if (m_pScriptDesc->szAnimTag == TEXT("Loading"))
		LoadingAnim(fTimeDelta);
	else if (m_pScriptDesc->szAnimTag == TEXT("Default"))
		DefaultAnim(fTimeDelta);
}

HRESULT CUIScript::Render()
{
	if (m_eVisibility == VISIBILITY::HIDDEN)
		return S_OK;

	__super::Render();

	if (FAILED(RenderText()))
		return E_FAIL;

#ifdef _DEBUG
	__super::Render_Debug_Rect();
#endif

	return S_OK;
}

HRESULT CUIScript::RenderText()
{
	_float2 fTextSize = m_pGameInstance->Get_Text_Size(
		m_tUIDesc.m_tUITextDesc.szFont.c_str(), m_pScriptDesc->Scripts[m_iScriptIdx].szScriptText.c_str(), true, m_tUIDesc.m_tUITextDesc.fScale);
	_float fAlpha{ m_tScriptAnimDesc.fAlpha };

	_float2 vPivot{
		m_tUIDesc.fX + m_tUIDesc.fOffsetX - (fTextSize.x * 0.5f),
		m_tUIDesc.fY + m_tUIDesc.fOffsetY + m_tScriptAnimDesc.vOffset.y
	};

	_float2 vShadowPivot{
		vPivot.x + 1.f,
		vPivot.y + 1.f
	};

	_vector vColor = XMVectorSet(
		m_pScriptDesc->Scripts[m_iScriptIdx].vColor.x * fAlpha,
		m_pScriptDesc->Scripts[m_iScriptIdx].vColor.y * fAlpha,
		m_pScriptDesc->Scripts[m_iScriptIdx].vColor.z * fAlpha,
		m_pScriptDesc->Scripts[m_iScriptIdx].vColor.w * fAlpha
	);
	
	_vector vShadowColor = XMVectorSet(
		0.f * fAlpha,
		0.f * fAlpha,
		0.f * fAlpha,
		1.f * fAlpha
	);

	m_pGameInstance->Render_Text(m_tUIDesc.m_tUITextDesc.szFont.c_str(),
		m_pScriptDesc->Scripts[m_iScriptIdx].szScriptText.c_str(),
		vShadowPivot,
		vShadowColor, m_tUIDesc.m_tUITextDesc.fScale);

	m_pGameInstance->Render_Text(m_tUIDesc.m_tUITextDesc.szFont.c_str(),
		m_pScriptDesc->Scripts[m_iScriptIdx].szScriptText.c_str(),
		vPivot,
		vColor, m_tUIDesc.m_tUITextDesc.fScale);

	return S_OK;
}

HRESULT CUIScript::Ready_Components()
{
	__super::Ready_Components();

	return S_OK;
}

HRESULT CUIScript::Bind_ShaderResources()
{
	return S_OK;
}

HRESULT CUIScript::Execute(const UI_EVENT_DESC& EventDesc)
{
	return S_OK;
}

void CUIScript::CallbackEvent(void* pArg)
{
}

void CUIScript::DefaultAnim(_float fTimeDelta)
{
	m_fTimeAcc += fTimeDelta;

	m_tScriptAnimDesc.vOffset.y = (m_tUIDesc.fOffsetY + m_pScriptDesc->vInitOffset.y);

	if (m_fTimeAcc >= 3.f)
	{
		++m_iScriptIdx;
		m_fTimeAcc = 0.f;

		if (m_iScriptIdx >= m_pScriptDesc->Scripts.size())
		{
			//m_eVisibility = VISIBILITY::HIDDEN;
			End_Script();
			return;
		}
	}
}

void CUIScript::LoadingAnim(_float fTimeDelta)
{
	if (m_fTimeAcc <= 0)
	{
		m_tScriptAnimDesc.fAlpha = 0.f;
		m_tScriptAnimDesc.vOffset.y = (m_tUIDesc.fOffsetY + m_pScriptDesc->vInitOffset.y) + 10.f;
	}

	m_fTimeAcc += fTimeDelta;

	if (m_fTimeAcc >= 5.f)
	{
		++m_iScriptIdx;
		m_fTimeAcc = 0.f;
		m_tScriptAnimDesc.fAlpha = 0.f;
		m_tScriptAnimDesc.vOffset.y = (m_tUIDesc.fOffsetY + m_pScriptDesc->vInitOffset.y) + 10.f;

		if (m_iScriptIdx >= m_pScriptDesc->Scripts.size())
			m_iScriptIdx = 0;
	}
	
	if (m_tScriptAnimDesc.fAlpha <= 1.f)
		m_tScriptAnimDesc.fAlpha += fTimeDelta;
		
	if (m_tScriptAnimDesc.vOffset.y >= (m_tUIDesc.fOffsetY + m_pScriptDesc->vInitOffset.y))
		m_tScriptAnimDesc.vOffset.y -= fTimeDelta * 5.f;
}

void CUIScript::ScriptControl()
{
	if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_LCONTROL))
	{
		++m_iScriptIdx;
		m_fTimeAcc = 0.f;
	}

	if (m_iScriptIdx >= m_pScriptDesc->Scripts.size())
		m_iScriptIdx = 0;
}

CUIScript* CUIScript::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUIScript* pInstance = new CUIScript(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUIScript::Clone(void* pArg)
{
	CUIScript* pInstance = new CUIScript(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUIScript");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUIScript::Free()
{
	__super::Free();
}
