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
	else if (m_pScriptDesc->szAnimTag == TEXT("CutScene"))
		CutSceneAnim(fTimeDelta);
}

HRESULT CUIScript::Render()
{
	if (m_eVisibility == VISIBILITY::HIDDEN)
		return S_OK;

	__super::Render();

	if (m_bShow)
	{
		if (m_pScriptDesc->szSpeaker != TEXT(""))
		{
			if (FAILED(RenderSpeaker()))
				return E_FAIL;
		}

		if (FAILED(RenderScript()))
			return E_FAIL;
	}

#ifdef _DEBUG
	__super::Render_Debug_Rect();
#endif

	return S_OK;
}

void CUIScript::Play_Next_Script()
{
	if (!m_pScriptDesc)
	{
		End_Script();
		return;
	}

	if (m_pScriptDesc->Scripts.size() > 0)
	{
		++m_iScriptIdx;
		m_iPrevScriptIdx = m_iScriptIdx;
		m_fTimeAcc = 0.f;
		m_tUIDesc.fAlpha = 1.f;
	}

	if (m_iScriptIdx >= m_pScriptDesc->Scripts.size())
		End_Script();
}

void CUIScript::Stop_Script()
{
	m_tUIDesc.fAlpha = 0.f;
}

HRESULT CUIScript::RenderSpeaker()
{
	_float2 fTextSize = m_pGameInstance->Get_Text_Size(
		m_tUIDesc.m_tUITextDesc.szFont.c_str(), m_pScriptDesc->szSpeaker.c_str(), true, m_tUIDesc.m_tUITextDesc.fScale);
	_float fAlpha{ m_tScriptAnimDesc.fAlpha * m_tUIDesc.fAlpha };

	_float2 vPivot{
		m_tUIDesc.fX + m_tUIDesc.fOffsetX - (fTextSize.x * 0.5f),
		m_tUIDesc.fY + m_tUIDesc.fOffsetY + m_tScriptAnimDesc.vOffset.y - 40.f
	};

	_float2 vShadowPivot{
		vPivot.x + 1.f,
		vPivot.y + 1.f
	};

	_vector vColor = XMVectorSet(1.f, 1.f, 1.f, 1.f);

	if (m_pScriptDesc->szSpeaker == TEXT("홍련"))
	{
		vColor = XMVectorSet(
			COLOR_PATTERN_SCARLET.x * fAlpha,
			COLOR_PATTERN_SCARLET.y * fAlpha,
			COLOR_PATTERN_SCARLET.z * fAlpha,
			COLOR_PATTERN_SCARLET.w * fAlpha
		);
	}
	if (m_pScriptDesc->szSpeaker == TEXT("도로롱"))
	{
		vColor = XMVectorSet(
			COLOR_PATTERN_DORORONG.x * fAlpha,
			COLOR_PATTERN_DORORONG.y * fAlpha,
			COLOR_PATTERN_DORORONG.z * fAlpha,
			COLOR_PATTERN_DORORONG.w * fAlpha
		);
	}

	_vector vShadowColor = XMVectorSet(
		0.f * fAlpha,
		0.f * fAlpha,
		0.f * fAlpha,
		1.f * fAlpha
	);

	m_pGameInstance->Render_Text(m_tUIDesc.m_tUITextDesc.szFont.c_str(),
		m_pScriptDesc->szSpeaker.c_str(),
		vShadowPivot,
		vShadowColor, m_tUIDesc.m_tUITextDesc.fScale);

	m_pGameInstance->Render_Text(m_tUIDesc.m_tUITextDesc.szFont.c_str(),
		m_pScriptDesc->szSpeaker.c_str(),
		vPivot,
		vColor, m_tUIDesc.m_tUITextDesc.fScale);

	return S_OK;
}

HRESULT CUIScript::RenderScript()
{
	_float2 fTextSize = m_pGameInstance->Get_Text_Size(
		m_tUIDesc.m_tUITextDesc.szFont.c_str(), m_pScriptDesc->Scripts[m_iScriptIdx].szScriptText.c_str(), true, m_tUIDesc.m_tUITextDesc.fScale);
	_float fAlpha{ m_tScriptAnimDesc.fAlpha * m_tUIDesc.fAlpha };

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

	vector<_wstring> SplitedTexts{};
	Text_Split(m_pScriptDesc->Scripts[m_iScriptIdx].szScriptText.c_str(), SplitedTexts);

	_uint iLines = 0;

	if (SplitedTexts.size() > 0)
	{
		for (auto& SplitedText : SplitedTexts)
		{
			_float2 vNewSize =
				m_pGameInstance->Get_Text_Size(TEXT("KoPub"), SplitedText.c_str(), true, 0.8f);

			_float2 vNewPivot{
				m_tUIDesc.fX + m_tUIDesc.fOffsetX - (vNewSize.x * 0.5f),
				vPivot.y + ((iLines * (SplitedTexts.size() / 2)) * (vNewSize.y))
			};

			_float2 vNewShadowPivot{
				vNewPivot.x + 1.f,
				vNewPivot.y + 1.f
			};

			m_pGameInstance->Render_Text(
				TEXT("KoPub"),
				SplitedText.c_str(),
				vNewPivot,
				vColor,
				0.8f
			);

			m_pGameInstance->Render_Text(m_tUIDesc.m_tUITextDesc.szFont.c_str(),
				SplitedText.c_str(),
				vNewShadowPivot,
				vShadowColor, m_tUIDesc.m_tUITextDesc.fScale);

			if (m_pScriptDesc->szSpeaker == TEXT("도로롱") && iLines > 0)
			{
				vColor = XMVectorSet(
					COLOR_PATTERN_GRAY.x * fAlpha,
					COLOR_PATTERN_GRAY.y * fAlpha,
					COLOR_PATTERN_GRAY.z * fAlpha,
					COLOR_PATTERN_GRAY.w * fAlpha
				);;
			}

			m_pGameInstance->Render_Text(m_tUIDesc.m_tUITextDesc.szFont.c_str(),
				SplitedText.c_str(),
				vNewPivot,
				vColor, m_tUIDesc.m_tUITextDesc.fScale);

			++iLines;
		}
	}
	else
	{
		m_pGameInstance->Render_Text(m_tUIDesc.m_tUITextDesc.szFont.c_str(),
			m_pScriptDesc->Scripts[m_iScriptIdx].szScriptText.c_str(),
			vShadowPivot,
			vShadowColor, m_tUIDesc.m_tUITextDesc.fScale);

		m_pGameInstance->Render_Text(m_tUIDesc.m_tUITextDesc.szFont.c_str(),
			m_pScriptDesc->Scripts[m_iScriptIdx].szScriptText.c_str(),
			vPivot,
			vColor, m_tUIDesc.m_tUITextDesc.fScale);
	}

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

	if (m_fTimeAcc >= 1.5f)
	{
		++m_iScriptIdx;
		m_iPrevScriptIdx = m_iScriptIdx;
		m_fTimeAcc = 0.f;

		if (m_iScriptIdx >= m_pScriptDesc->Scripts.size())
		{
			End_Script();
			return;
		}
	}
}

void CUIScript::CutSceneAnim(_float fTimeDelta)
{
	m_tScriptAnimDesc.vOffset.y = (m_tUIDesc.fOffsetY + m_pScriptDesc->vInitOffset.y);
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
		m_iPrevScriptIdx = m_iScriptIdx;
		m_fTimeAcc = 0.f;
	}

	if (m_iScriptIdx >= m_pScriptDesc->Scripts.size())
	{
		m_iScriptIdx = 0;
		m_iPrevScriptIdx = m_iScriptIdx;
	}
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
