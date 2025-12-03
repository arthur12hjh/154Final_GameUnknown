#include "pch.h"
#include "UIShield.h"

#include "GameInstance.h"
#include "GameManager.h"

#include "Player.h"
#ifdef _DEBUG
#include "../../UI_Editor/Public/UI_Camera.h"
#endif // DEBUG

CUIShield::CUIShield(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIBase{ pDevice, pContext }
{
}

CUIShield::CUIShield(const CUIShield& Prototype) 
	: CUIBase{ Prototype }
{
}

HRESULT CUIShield::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUIShield::Initialize(void* pArg)
{	
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	auto pCharactor{ CGameManager::GetInstance()->GetGameCharacter()};

	if (pCharactor)
	{
		m_iMaxShield = const_cast<LONGLONG*>(&CGameManager::GetInstance()->Get_PlayerDesc()->iMaxShield);
		m_iCurrentShield = const_cast<LONGLONG*>(&CGameManager::GetInstance()->Get_PlayerDesc()->iCurrentShield);
	}
#ifdef _DEBUG
	else
	{
		auto pGaraPlayer = dynamic_cast<CUI_Camera*>(m_pGameInstance->GetMainCamera());

		m_iMaxShield = const_cast<LONGLONG*>(&pGaraPlayer->Get_Desc()->iMaxShield);
		m_iCurrentShield = const_cast<LONGLONG*>(&pGaraPlayer->Get_Desc()->iCurrentShield);

		Safe_Release(pGaraPlayer);
	}
#endif // DEBUG

	Safe_Release(pCharactor);
	
	return S_OK;
}

void CUIShield::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CUIShield::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

#ifdef _DEBUG
	// 테스트 용
	/*if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_I))
	{
		if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_9))
			*m_iCurrentShield -= 10;
		if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_0))
			*m_iCurrentShield += 10;
	}*/
#endif

	m_fTargetFill = static_cast<_float>(*m_iCurrentShield) / static_cast<_float>(*m_iMaxShield);

	m_fCurrentFill = Lerp(m_fCurrentFill, m_fTargetFill, fTimeDelta * m_fSpeed);

}

void CUIShield::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
}

HRESULT CUIShield::Render()
{
	__super::Render();

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(UI_SHADER_PASS::SHIELD))))
		return E_FAIL;

	if (FAILED(m_pVIBaseBufferCom->Bind_Resources()))
		return E_FAIL;

	if (FAILED(m_pVIBaseBufferCom->Render()))
		return E_FAIL;

#ifdef _DEBUG
	__super::Render_Debug_Rect();
#endif

	return S_OK;
}

HRESULT CUIShield::Ready_Components()
{
	__super::Ready_Components();

	/* Com_VIBaseBuffer */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect_Instance"),
		TEXT("Com_VIBaseBuffer"), reinterpret_cast<CComponent**>(&m_pVIBaseBufferCom))))
		return E_FAIL;

	/* Com_Texture_Shield */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Player_Hp"),
		TEXT("Com_Texture_Shield"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUIShield::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->GetIdentityMatrixPtr())))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture0", 0)))
		return E_FAIL;
	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture1", 1)))
		return E_FAIL;

	_float2 vUV{};
	vUV.x = 12.f;
	vUV.y = 2.f;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_UVScale", &vUV, sizeof(_float2))))
		return E_FAIL;
	
	_float fGroupCount{ 2.f };

	if (FAILED(m_pShaderCom->Bind_RawValue("g_GroupCount", &fGroupCount, sizeof(_float))))
		return E_FAIL;

	_float fFillAmount = m_fCurrentFill;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fFillAmount", &fFillAmount, sizeof(_float))))
		return E_FAIL;

	_float2 vGap{ };
	vGap.x = 10.0f / m_tUIDesc.fSizeX;
	vGap.y = 0.0f;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_UVGap", &vGap, sizeof(_float2))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_bUseTintColor", &m_tUIDesc.m_tUIShaderDesc.bUseTintColor, sizeof(_bool))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vTintColor", &m_tUIDesc.m_tUIShaderDesc.vTintColor, sizeof(_float4))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUIShield::Execute(const UI_EVENT_DESC& EventDesc)
{
	return S_OK;
}

void CUIShield::CallbackEvent(void* pArg)
{
}

CUIShield* CUIShield::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUIShield* pInstance = new CUIShield(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUIShield::Clone(void* pArg)
{
	CUIShield* pInstance = new CUIShield(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUIShield");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUIShield::Free()
{
	__super::Free();

	Safe_Release(m_pVIBaseBufferCom);
}
