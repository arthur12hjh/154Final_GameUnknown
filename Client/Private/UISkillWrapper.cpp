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
			m_eSkillState[i] = const_cast<SKILL_STATE*>(&CGameManager::GetInstance()->Get_PlayerDesc()->eBetaSkillState[i]);
			m_ePrevSkillState[i] = *m_eSkillState[i];
		}

		m_eRushState = const_cast<SKILL_STATE*>(&CGameManager::GetInstance()->Get_PlayerDesc()->eRushState);
		m_ePrevRushState = *m_eRushState;
		m_fMaxRushCoolTime = const_cast<_float*>(&CGameManager::GetInstance()->Get_PlayerDesc()->fMaxRushCoolTime);
		m_fCurrentRushCoolTime = const_cast<_float*>(&CGameManager::GetInstance()->Get_PlayerDesc()->fCurrentRushCoolTime);
	}
#ifdef _DEBUG
	else
	{
		for (size_t i = 0; i < 4; ++i)
		{
			m_eSkillState[i] = const_cast<SKILL_STATE*>(&dynamic_cast<CUI_Camera*>(m_pGameInstance->GetMainCamera())->Get_Desc()->eBetaSkillState[i]);
			m_ePrevSkillState[i] = *m_eSkillState[i];
		}

		m_eRushState = const_cast<SKILL_STATE*>(&dynamic_cast<CUI_Camera*>(m_pGameInstance->GetMainCamera())->Get_Desc()->eRushState);
		m_ePrevRushState = *m_eRushState;
		m_fMaxRushCoolTime = const_cast<_float*>(&dynamic_cast<CUI_Camera*>(m_pGameInstance->GetMainCamera())->Get_Desc()->fMaxRushCoolTime);
		m_fCurrentRushCoolTime = const_cast<_float*>(&dynamic_cast<CUI_Camera*>(m_pGameInstance->GetMainCamera())->Get_Desc()->fCurrentRushCoolTime);
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

	//if (m_fRushCoolAmount < 1.f)
	//{
	//	m_bRushActive = false; // 플레이어한테 받아오기???
	//	m_fRushCoolAmount += fTimeDelta / 5.f; // 쿨타임 플레이어한테 받아오기???
	//}
	//else
	//{
	//	m_bRushActive = true;
	//	m_fRushCoolAmount = 1.f;
	//}
}


void CUISkillWrapper::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	// 러쉬 활성화 변화 감지
	if (*m_eRushState != m_ePrevRushState)
	{
		UI_EVENT_ARG_DESC Arg{};
		Arg.Type = UI_EVENT_ARG_DESC::SKILL_STATE;
		Arg.pData = m_eRushState;

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
			/*case SKILL_STATE::USE:
			{
				__super::Trigger_Event(TEXT("Rush_ActiveOff"), &Arg);
				break;
			}*/
		}

		m_ePrevRushState = *m_eRushState;
	}

	for (size_t i = 0; i < 4; ++i)
	{
		if(m_ePrevSkillState[i] != *m_eSkillState[i])
		{
			UI_EVENT_ARG_DESC Arg{};
			Arg.Type = UI_EVENT_ARG_DESC::SKILL_STATE;
			Arg.pData = m_eSkillState[i];

			switch (*m_eSkillState[i])
			{
			case SKILL_STATE::DEFAULT:
			{
				__super::Trigger_Event(TEXT("Skill_") + to_wstring(i) + TEXT("_Default"), &Arg);
				break;
			}
			case SKILL_STATE::ACTIVE:
			{
				__super::Trigger_Event(TEXT("Skill_") + to_wstring(i) + TEXT("_Active"), &Arg);
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

	/* Com_VIBuffer */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;
	return S_OK;
}

HRESULT CUISkillWrapper::Bind_ShaderResources()
{
	return S_OK;
}

HRESULT CUISkillWrapper::Execute(const UI_EVENT_DESC& EventDesc)
{
	//const _wstring& Type = EventDesc.szTypeTag;
	//const _wstring& Arg = EventDesc.szArg;
	//const _wstring& ActionTag = EventDesc.szActionTag;

	//// 애니메이션
	//if (Type == TEXT("PlayAnimEvent"))
	//{
	//	CUIHUD* pHUD = dynamic_cast<CUIHUD*>(m_pGameInstance->GetCurrentLevelHUD());
	//	auto AnimTag = m_tUIDesc.m_AnimTags.find(ActionTag);

	//	if (AnimTag != m_tUIDesc.m_AnimTags.end())
	//		pHUD->Anim_Play(m_tUIDesc.szLayerTag, m_tUIDesc.szUITag, AnimTag->first);

	//	Safe_Release(pHUD);
	//}
	//if (Type == TEXT("ActionEvent"))
	//{
	//}

	return S_OK;
}

void CUISkillWrapper::CallbackEvent(void* pArg)
{
	/*auto* arg = static_cast<UI_EVENT_ARG_DESC*>(pArg);
	if (!arg) return;

	CUIHUD* pHUD = dynamic_cast<CUIHUD*>(m_pGameInstance->GetCurrentLevelHUD());

	auto AnimTag = m_tUIDesc.m_AnimTags.find(arg->szActionTag);
	if (AnimTag != m_tUIDesc.m_AnimTags.end())
		pHUD->Anim_Play(m_tUIDesc.szLayerTag, m_tUIDesc.szUITag, AnimTag->first);

	Safe_Release(pHUD);*/
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
