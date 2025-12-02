#include "pch.h"
#include "UIBossHPBar.h"

#include "GameInstance.h"
#include "GameManager.h"

#include "UIBossVitalWrapper.h"
#ifdef _DEBUG
#include "../../UI_Editor/Public/UI_Camera.h"
#endif // DEBUG

CUIBossHPBar::CUIBossHPBar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIBase{ pDevice, pContext }
{
}

CUIBossHPBar::CUIBossHPBar(const CUIBossHPBar& Prototype) 
	: CUIBase{ Prototype }
{
}

HRESULT CUIBossHPBar::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUIBossHPBar::Initialize(void* pArg)
{	
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CUIBossHPBar::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CUIBossHPBar::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
	
	if (dynamic_cast<CUIBossVitalWrapper*>(m_pParent) && m_pParent->GetVisibility() == VISIBILITY::VISIBLE)
	{
		CUIBossVitalWrapper* pVitalWrapper = dynamic_cast<CUIBossVitalWrapper*>(m_pParent);

		m_fTargetFill = static_cast<_float>(pVitalWrapper->Get_NaytibaDesc()->iCurrentHealth) / static_cast<_float>(pVitalWrapper->Get_NetworkDesc()->iMaxHealth);

		m_fCurrentFill = Lerp(m_fCurrentFill, m_fTargetFill, fTimeDelta * m_fSpeed);
	}
}

void CUIBossHPBar::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
}

HRESULT CUIBossHPBar::Render()
{
	__super::Render();

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(UI_SHADER_PASS::HP_GAUGE))))
		return E_FAIL;

	if (FAILED(m_pVIBaseBuffer->Bind_Resources()))
		return E_FAIL;

	if (FAILED(m_pVIBaseBuffer->Render()))
		return E_FAIL;

#ifdef _DEBUG
	__super::Render_Debug_Rect();
#endif

	return S_OK;
}

HRESULT CUIBossHPBar::Ready_Components()
{
	__super::Ready_Components();

	/* Com_VIBaseBuffer */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect_Instance"),
		TEXT("Com_VIBaseBuffer"), reinterpret_cast<CComponent**>(&m_pVIBaseBuffer))))
		return E_FAIL;

	/* Com_Texture_HP */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Player_Hp"),
		TEXT("Com_Texture_HP"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUIBossHPBar::Bind_ShaderResources()
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
	vUV.x = 88.f;
	vUV.y = 3.f;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_UVScale", &vUV, sizeof(_float2))))
		return E_FAIL;
	
	_float fFillAmount = m_fCurrentFill;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fFillAmount", &fFillAmount, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUIBossHPBar::Execute(const UI_EVENT_DESC& EventDesc)
{
	return S_OK;
}

//HRESULT CUIBossHPBar::Broadcast_Event(const _wstring& szEventTag, const _wstring& szActionTag, void* pArg)
//{
//	return S_OK;
//}

void CUIBossHPBar::CallbackEvent(void* pArg)
{
}

CUIBossHPBar* CUIBossHPBar::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUIBossHPBar* pInstance = new CUIBossHPBar(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUIBossHPBar::Clone(void* pArg)
{
	CUIBossHPBar* pInstance = new CUIBossHPBar(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUIBossHPBar");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUIBossHPBar::Free()
{
	__super::Free();

	Safe_Release(m_pVIBaseBuffer);
}
