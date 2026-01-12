#include "pch.h"
#include "UIUnlockFX.h"

#include "GameInstance.h"
#include "UIHUD.h"
#include "UIPopup.h"
#include "UICostumePuzzleAnswer.h"

CUIUnlockFX::CUIUnlockFX(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIBase{ pDevice, pContext }
{
}

CUIUnlockFX::CUIUnlockFX(const CUIUnlockFX& Prototype) 
	: CUIBase{ Prototype }
{
}

HRESULT CUIUnlockFX::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUIUnlockFX::Initialize(void* pArg)
{	
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CUIUnlockFX::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CUIUnlockFX::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

	if (m_bPlay)
	{
		m_fTimeAcc += fTimeDelta;

		if (m_fTimeAcc > 0.f && m_fTimeAcc <= 0.3f)
		{
			m_tUIDesc.fAlpha = 1.f;
			m_iTextureIdx = 0;
			m_eVisibility = VISIBILITY::VISIBLE;
		}
		else if (m_fTimeAcc > 0.3f && m_fTimeAcc <= 0.5f)
		{
			m_tUIDesc.fAlpha = 0.f;
		}
		else if (m_fTimeAcc > 0.5f && m_fTimeAcc <= 0.7f)
		{
			m_tUIDesc.fAlpha = 1.f;
		}
		else if (m_fTimeAcc > 0.7f && m_fTimeAcc <= 1.0f)
		{
			m_tUIDesc.fAlpha = 0.f;
		}
		else if (m_fTimeAcc > 1.0f && m_fTimeAcc <= 2.f)
		{
			m_tUIDesc.fAlpha = 1.f;
			m_iTextureIdx = 1;
		}
		else if (m_fTimeAcc > 2.0f)
		{
			m_tUIDesc.fAlpha = 0.f;
			m_fTimeAcc = 0.f;
			m_bPlay = false;

			CUICostumePuzzleAnswer* pAnswerUI = dynamic_cast<CUICostumePuzzleAnswer*>(m_pParent);

			if (pAnswerUI)
				pAnswerUI->Set_isSolved(true);

			m_eVisibility = VISIBILITY::HIDDEN;
		}
	}
}

void CUIUnlockFX::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
}

HRESULT CUIUnlockFX::Render()
{
	__super::Render();

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(UI_SHADER_PASS::UNLOCK_FX))))
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

HRESULT CUIUnlockFX::Ready_Components()
{
	__super::Ready_Components();

	/* Com_VIBuffer */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBaseBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Puzzle_Unlock"),
		TEXT("Com_Texture_UI_Puzzle_Unlock"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUIUnlockFX::Bind_ShaderResources()
{
	if(FAILED(__super::Initialize_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->GetIdentityMatrixPtr())))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;
	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture0", m_iTextureIdx)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_Alpha", &m_tUIDesc.fAlpha, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUIUnlockFX::Execute(const UI_EVENT_DESC& EventDesc)
{
	/*const _wstring& Type = EventDesc.szTypeTag;
	const _wstring& Arg = EventDesc.szArg;
	const _wstring& ActionTag = EventDesc.szActionTag;

	if (ActionTag == TEXT("Puzzle_Solve"))
	{
		_bool bActive = true;
		UI_EVENT_ARG_DESC Arg{};
		Arg.szActionTag = TEXT("Puzzle_Solve");
		Arg.Type = UI_EVENT_ARG_DESC::BOOL;
		Arg.pData = &bActive;
		__super::Trigger_Event(TEXT("Puzzle_Solve_Listener"), &Arg);
	}*/

	return S_OK;
}

void CUIUnlockFX::CallbackEvent(void* pArg)
{
	UI_EVENT_ARG_DESC* arg = static_cast<UI_EVENT_ARG_DESC*>(pArg);
	if (!arg) return;

	if (arg->szActionTag == TEXT("Costume_Puzzle_Unlock"))
	{
		if (arg->Type == UI_EVENT_ARG_DESC::ARG_TYPE::BOOL)
		{
			m_bPlay = *static_cast<_bool*>(arg->pData);
			m_pGameInstance->Manager_PlaySound(TEXT("PUZZLE_COMPLETE.wav"), CHANNELID::EFFECT, 0.7f, 1.f);
		}
	}
}

CUIUnlockFX* CUIUnlockFX::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUIUnlockFX* pInstance = new CUIUnlockFX(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUIUnlockFX::Clone(void* pArg)
{
	CUIUnlockFX* pInstance = new CUIUnlockFX(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUIUnlockFX");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUIUnlockFX::Free()
{
	__super::Free();
}
