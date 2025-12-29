#include "pch.h"
#include "UIMonsterShield.h"

#include "GameInstance.h"
#include "GameManager.h"

#include "Nayitba.h"


CUIMonsterShield::CUIMonsterShield(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIBase{ pDevice, pContext }
{
}

CUIMonsterShield::CUIMonsterShield(const CUIMonsterShield& Prototype) 
	: CUIBase{ Prototype }
{
}

HRESULT CUIMonsterShield::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUIMonsterShield::Initialize(void* pArg)
{	
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;
	
	return S_OK;
}

void CUIMonsterShield::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);

	if (!m_isRent)
	{
		m_fCurrentFill = 0.f;
		m_fTargetFill = 1.f;
	}
}

void CUIMonsterShield::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

	auto pMonster = dynamic_cast<CNaytiba*>(m_pParent->GetParent());
	if (pMonster)
	{
		auto StaticDesc = pMonster->GetStaticMonsterData();
		auto CurDesc = pMonster->GetMonsterData();

		m_fTargetFill = (_float)CurDesc.iCurrentShield / (_float)StaticDesc->iMaxShield;
	}

	if (m_iCurrentShield)
		m_fTargetFill = static_cast<_float>(*m_iCurrentShield) / static_cast<_float>(*m_iMaxShield);

	m_fCurrentFill = Lerp(m_fCurrentFill, m_fTargetFill, fTimeDelta * m_fSpeed);
}

void CUIMonsterShield::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
}

HRESULT CUIMonsterShield::Render()
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

HRESULT CUIMonsterShield::Ready_Components()
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

HRESULT CUIMonsterShield::Bind_ShaderResources()
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
	vUV.y = 1.f;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_UVScale", &vUV, sizeof(_float2))))
		return E_FAIL;
	
	_float fGroupCount{ 1.f };

	if (FAILED(m_pShaderCom->Bind_RawValue("g_GroupCount", &fGroupCount, sizeof(_float))))
		return E_FAIL;

	_float fFillAmount = m_fCurrentFill;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fFillAmount", &fFillAmount, sizeof(_float))))
		return E_FAIL;

	_float2 vGap{ };
	vGap.x = 0.0f;
	vGap.y = 0.0f;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_UVGap", &vGap, sizeof(_float2))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_bUseTintColor", &m_tUIDesc.m_tUIShaderDesc.bUseTintColor, sizeof(_bool))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vTintColor", &m_tUIDesc.m_tUIShaderDesc.vTintColor, sizeof(_float4))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUIMonsterShield::Execute(const UI_EVENT_DESC& EventDesc)
{
	return S_OK;
}

//HRESULT CUIMonsterShield::Broadcast_Event(const _wstring& szEventTag, const _wstring& szActionTag, void* pArg)
//{
//	return S_OK;
//}

void CUIMonsterShield::CallbackEvent(void* pArg)
{
}

CUIMonsterShield* CUIMonsterShield::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUIMonsterShield* pInstance = new CUIMonsterShield(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUIMonsterShield::Clone(void* pArg)
{
	CUIMonsterShield* pInstance = new CUIMonsterShield(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUIMonsterShield");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUIMonsterShield::Free()
{
	__super::Free();

	Safe_Release(m_pVIBaseBufferCom);
}
