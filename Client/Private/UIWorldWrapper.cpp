#include "pch.h"
#include "UIWorldWrapper.h"

#include "GameInstance.h"
#include "GameManager.h"

#include "Player.h"
#ifdef _DEBUG
#include "../../UI_Editor/Public/UI_Camera.h"
#endif // DEBUG

CUIWorldWrapper::CUIWorldWrapper(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIBase{ pDevice, pContext }
{
}

CUIWorldWrapper::CUIWorldWrapper(const CUIWorldWrapper& Prototype) 
	: CUIBase{ Prototype }
{
}

HRESULT CUIWorldWrapper::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUIWorldWrapper::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	/*auto pCharactor{ CGameManager::GetInstance()->GetGameCharacter() };

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

	Safe_Release(pCharactor);*/

	return S_OK;
}

void CUIWorldWrapper::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CUIWorldWrapper::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void CUIWorldWrapper::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
}

HRESULT CUIWorldWrapper::Render()
{
	__super::Render();

#ifdef _DEBUG
	__super::Render_Debug_Rect();
#endif

	return S_OK;
}

HRESULT CUIWorldWrapper::Ready_Components()
{
	__super::Ready_Components();

	return S_OK;
}

HRESULT CUIWorldWrapper::Bind_ShaderResources()
{
	return S_OK;
}

HRESULT CUIWorldWrapper::Execute(const UI_EVENT_DESC& EventDesc)
{
	return S_OK;
}

void CUIWorldWrapper::CallbackEvent(void* pArg)
{
}

CUIWorldWrapper* CUIWorldWrapper::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUIWorldWrapper* pInstance = new CUIWorldWrapper(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUIWorldWrapper::Clone(void* pArg)
{
	CUIWorldWrapper* pInstance = new CUIWorldWrapper(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUIWorldWrapper");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUIWorldWrapper::Free()
{
	__super::Free();
}
