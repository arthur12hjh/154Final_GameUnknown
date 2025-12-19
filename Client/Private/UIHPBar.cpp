#include "pch.h"
#include "UIHPBar.h"

#include "GameInstance.h"
#include "GameManager.h"

#include "Player.h"
#ifdef _DEBUG
#include "../../UI_Editor/Public/UI_Camera.h"
#endif // DEBUG

CUIHPBar::CUIHPBar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIBase{ pDevice, pContext }
{
}

CUIHPBar::CUIHPBar(const CUIHPBar& Prototype) 
	: CUIBase{ Prototype }
{
}

HRESULT CUIHPBar::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUIHPBar::Initialize(void* pArg)
{	
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	auto pCharactor{ CGameManager::GetInstance()->GetGameCharacter() };

	if (pCharactor)
	{
		m_iMaxHp = const_cast<LONGLONG*>(&CGameManager::GetInstance()->Get_PlayerDesc()->iMaxHealth);
		m_iCurrentHp = const_cast<LONGLONG*>(&CGameManager::GetInstance()->Get_PlayerDesc()->iCurrentHealth);
	}
#ifdef _DEBUG
	else
	{
		auto pGaraPlayer = dynamic_cast<CUI_Camera*>(m_pGameInstance->GetMainCamera());

		m_iMaxHp = const_cast<LONGLONG*>(&pGaraPlayer->Get_Desc()->iMaxHealth);
		m_iCurrentHp = const_cast<LONGLONG*>(&pGaraPlayer->Get_Desc()->iCurrentHealth);

		Safe_Release(pGaraPlayer);
	}
#endif // DEBUG

	Safe_Release(pCharactor);

	return S_OK;
}

void CUIHPBar::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CUIHPBar::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);


#ifdef _DEBUG
	// 테스트 용
	/*if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_U))
	{
		if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_9) && *m_iCurrentHp > 0)
			*m_iCurrentHp -= 10;
		if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_0) && *m_iCurrentHp < *m_iMaxHp)
			*m_iCurrentHp += 10;
	}*/
#endif
	
	m_fTargetFill = static_cast<_float>(*m_iCurrentHp) / static_cast<_float>(*m_iMaxHp);

	m_fCurrentFill = Lerp(m_fCurrentFill, m_fTargetFill, fTimeDelta * m_fSpeed);

}

void CUIHPBar::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
}

HRESULT CUIHPBar::Render()
{
	__super::Render();

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(UI_SHADER_PASS::HP_GAUGE))))
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

HRESULT CUIHPBar::Ready_Components()
{
	__super::Ready_Components();

	/* Com_VIBuffer */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* Com_Texture_HP */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Player_Hp"),
		TEXT("Com_Texture_HP"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUIHPBar::Bind_ShaderResources()
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
	vUV.x = 25.f;
	vUV.y = 3.f;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_UVScale", &vUV, sizeof(_float2))))
		return E_FAIL;
	
	_float fFillAmount = m_fCurrentFill;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fFillAmount", &fFillAmount, sizeof(_float))))
		return E_FAIL;

	_bool bUseTint = false;
	_float4 vTintColor = { 0.f, 0.f, 0.f, 0.f };

	if (m_fCurrentFill <= 0.5f)
	{
		bUseTint = true;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_bUseTintColor", &bUseTint, sizeof(_bool))))
			return E_FAIL;

		vTintColor = _float4{ 0.996f, 0.980f, 0.831f, 1.f };
		
		if (m_fCurrentFill <= 0.3f)
			vTintColor = _float4{ 0.988f, 0.675f, 0.702f, 1.f };

		if (FAILED(m_pShaderCom->Bind_RawValue("g_vTintColor", &vTintColor, sizeof(_float4))))
			return E_FAIL;
	}


	return S_OK;
}

HRESULT CUIHPBar::Execute(const UI_EVENT_DESC& EventDesc)
{
	return S_OK;
}

//HRESULT CUIHPBar::Broadcast_Event(const _wstring& szEventTag, const _wstring& szActionTag, void* pArg)
//{
//	return S_OK;
//}

void CUIHPBar::CallbackEvent(void* pArg)
{
}

CUIHPBar* CUIHPBar::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUIHPBar* pInstance = new CUIHPBar(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUIHPBar::Clone(void* pArg)
{
	CUIHPBar* pInstance = new CUIHPBar(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUIHPBar");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUIHPBar::Free()
{
	__super::Free();
}
