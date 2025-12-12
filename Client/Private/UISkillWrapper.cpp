#include "pch.h"
#include "UISkillWrapper.h"

#include "GameInstance.h"
#include "GameManager.h"

#include "Player.h"
#ifdef _DEBUG
#include "../../UI_Editor/Public/UI_Camera.h"
#endif // DEBUG

CUISkillWrapper::CUISkillWrapper(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIBase{ pDevice, pContext }
{
}

CUISkillWrapper::CUISkillWrapper(const CUISkillWrapper& Prototype) 
	: CUIBase{ Prototype }
{
}

HRESULT CUISkillWrapper::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUISkillWrapper::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	auto pCharactor{ CGameManager::GetInstance()->GetGameCharacter() };

	if (pCharactor)
	{
		for (size_t i = 0; i < 4; ++i)
		{
			m_eSkillState[i] = const_cast<SKILL_STATE*>(&pCharactor->Get_Desc()->eBetaSkillState[i]);
			m_iSkillID[i] = const_cast<_uint*>(&pCharactor->Get_Desc()->iBetaSkillId[i]);
			m_ePrevSkillState[i] = *m_eSkillState[i];
		}

		m_iCurrentBeta = const_cast<LONGLONG*>(&pCharactor->Get_Desc()->iCurrentBetaEnergy);
		m_eRushState = const_cast<SKILL_STATE*>(&pCharactor->Get_Desc()->eRushState);
		m_ePrevRushState = *m_eRushState;
		m_fMaxRushCoolTime = const_cast<_float*>(&pCharactor->Get_Desc()->fMaxRushCoolTime);
		m_fCurrentRushCoolTime = const_cast<_float*>(&pCharactor->Get_Desc()->fCurrentRushCoolTime);
	}
#ifdef _DEBUG
	else
	{
		auto pGaraPlayer = dynamic_cast<CUI_Camera*>(m_pGameInstance->GetMainCamera());

		for (size_t i = 0; i < 4; ++i)
		{
			m_eSkillState[i] = const_cast<SKILL_STATE*>(&pGaraPlayer->Get_Desc()->eBetaSkillState[i]);
			m_ePrevSkillState[i] = *m_eSkillState[i];
		}

		m_eRushState = const_cast<SKILL_STATE*>(&pGaraPlayer->Get_Desc()->eRushState);
		m_ePrevRushState = *m_eRushState;
		m_fMaxRushCoolTime = const_cast<_float*>(&pGaraPlayer->Get_Desc()->fMaxRushCoolTime);
		m_fCurrentRushCoolTime = const_cast<_float*>(&pGaraPlayer->Get_Desc()->fCurrentRushCoolTime);
		
		Safe_Release(pGaraPlayer);
	}
#endif

	Safe_Release(pCharactor);

	return S_OK;
}

void CUISkillWrapper::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CUISkillWrapper::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void CUISkillWrapper::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	// 러쉬 활성화 변화 감지
	if (*m_eRushState != m_ePrevRushState)
	{
		UI_SKILL_INFO_DESC tDesc = { 0, 0, ENUM_CLASS(m_ePrevRushState), ENUM_CLASS(*m_eRushState) };

		UI_EVENT_ARG_DESC Arg{};
		Arg.Type = UI_EVENT_ARG_DESC::SKILL_INFO;
		Arg.pData = &tDesc;

		switch(*m_eRushState)
		{
			case SKILL_STATE::DEFAULT:
			{
				__super::Trigger_Event(TEXT("Rush_Default"), &Arg);
				break;
			}
			case SKILL_STATE::ACTIVE_ON:
			{
				__super::Trigger_Event(TEXT("Rush_ActiveOn"), &Arg);
				break;
			}
			case SKILL_STATE::ACTIVE:
			{
				__super::Trigger_Event(TEXT("Rush_Active"), &Arg);
				break;
			}
			case SKILL_STATE::USE:
			{
				__super::Trigger_Event(TEXT("Rush_Use"), &Arg);
				break;
			}
		}

		m_ePrevRushState = *m_eRushState;
	}

	for (size_t i = 0; i < 4; ++i)
	{
		if (*m_iSkillID[i] <= 0)
			continue;

		if (m_ePrevSkillState[i] != *m_eSkillState[i])
		{
			UI_SKILL_INFO_DESC tDesc = {i, *m_iSkillID[i], ENUM_CLASS(m_ePrevSkillState[i]), ENUM_CLASS(*m_eSkillState[i])};

			UI_EVENT_ARG_DESC Arg{};
			Arg.Type = UI_EVENT_ARG_DESC::SKILL_INFO;
			Arg.pData = &tDesc;

			switch (*m_eSkillState[i])
			{
			case SKILL_STATE::DEFAULT:
			{
				__super::Trigger_Event(TEXT("Skill_") + to_wstring(i) + TEXT("_Default"), &Arg);
				break;
			}
			case SKILL_STATE::ACTIVE_ON:
			{
				__super::Trigger_Event(TEXT("Skill_") + to_wstring(i) + TEXT("_ActiveOn"), &Arg);
				
				_bool bActive = true;

				UI_EVENT_ARG_DESC Arg{};
				Arg.Type = UI_EVENT_ARG_DESC::BOOL;
				Arg.pData = &bActive;
				__super::Trigger_Event(TEXT("SkillWrapperActive"), &Arg);

				break;
			}
			case SKILL_STATE::ACTIVE:
			{
				__super::Trigger_Event(TEXT("Skill_") + to_wstring(i) + TEXT("_Active"), &Arg);

				break;
			}
			case SKILL_STATE::USE:
			{
				__super::Trigger_Event(TEXT("Skill_") + to_wstring(i) + TEXT("_Use"), &Arg);

				break;
			}
			}
			m_ePrevSkillState[i] = *m_eSkillState[i];
		}
	}
}

HRESULT CUISkillWrapper::Render()
{
	__super::Render();

#ifdef _DEBUG
	__super::Render_Debug_Rect();
#endif

	return S_OK;
}

HRESULT CUISkillWrapper::Ready_Components()
{
	__super::Ready_Components();

	return S_OK;
}

HRESULT CUISkillWrapper::Bind_ShaderResources()
{
	return S_OK;
}

HRESULT CUISkillWrapper::Execute(const UI_EVENT_DESC& EventDesc)
{
	return S_OK;
}

void CUISkillWrapper::CallbackEvent(void* pArg)
{
}

CUISkillWrapper* CUISkillWrapper::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUISkillWrapper* pInstance = new CUISkillWrapper(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUISkillWrapper::Clone(void* pArg)
{
	CUISkillWrapper* pInstance = new CUISkillWrapper(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUISkillWrapper");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUISkillWrapper::Free()
{
	__super::Free();
}
