#include "pch.h"
#include "UIMonsterHPBar.h"

#include "GameInstance.h"
#include "GameManager.h"

#include "UIWorldWrapper.h"
#include "Nayitba.h"

CUIMonsterHPBar::CUIMonsterHPBar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIBase{ pDevice, pContext }
{
}

CUIMonsterHPBar::CUIMonsterHPBar(const CUIMonsterHPBar& Prototype) 
	: CUIBase{ Prototype }
{
}

HRESULT CUIMonsterHPBar::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUIMonsterHPBar::Initialize(void* pArg)
{	
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CUIMonsterHPBar::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);

	if (!m_isRent)
	{
		m_fCurrentFill = 0.f;
		m_fTargetFill = 1.f;
	}
}

void CUIMonsterHPBar::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

	auto pMonster = dynamic_cast<CNaytiba*>(m_pParent->GetParent());
	if (pMonster)
	{
		auto StaticDesc = pMonster->GetStaticMonsterData();
		auto CurDesc = pMonster->GetMonsterData();

		m_fTargetFill = (_float)CurDesc.iCurrentHealth / (_float)StaticDesc->iMaxHealth;
	}
		
	m_fCurrentFill = Lerp(m_fCurrentFill, m_fTargetFill, fTimeDelta * m_fSpeed);
}

void CUIMonsterHPBar::Late_Update(_float fTimeDelta)
{
	if (m_isRent)
		__super::Late_Update(fTimeDelta);
}

HRESULT CUIMonsterHPBar::Render()
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

HRESULT CUIMonsterHPBar::Ready_Components()
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

HRESULT CUIMonsterHPBar::Bind_ShaderResources()
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

	_bool bUseTintColor = false;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_bUseTintColor", &bUseTintColor, sizeof(_bool))))
		return E_FAIL;

	_float2 vUV{};
	vUV.x = 22.f;
	vUV.y = 2.f;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_UVScale", &vUV, sizeof(_float2))))
		return E_FAIL;
	
	_float fFillAmount = m_fCurrentFill;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fFillAmount", &fFillAmount, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUIMonsterHPBar::Execute(const UI_EVENT_DESC& EventDesc)
{
	return S_OK;
}

//HRESULT CUIMonsterHPBar::Broadcast_Event(const _wstring& szEventTag, const _wstring& szActionTag, void* pArg)
//{
//	return S_OK;
//}

void CUIMonsterHPBar::CallbackEvent(void* pArg)
{
}

CUIMonsterHPBar* CUIMonsterHPBar::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUIMonsterHPBar* pInstance = new CUIMonsterHPBar(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUIMonsterHPBar::Clone(void* pArg)
{
	CUIMonsterHPBar* pInstance = new CUIMonsterHPBar(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUIMonsterHPBar");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUIMonsterHPBar::Free()
{
	__super::Free();

	Safe_Release(m_pVIBaseBuffer);
}
