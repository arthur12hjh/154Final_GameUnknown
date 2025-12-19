#include "pch.h"
#include "UIBossStamina.h"

#include "GameInstance.h"
#include "GameManager.h"

#include "UIBossVitalWrapper.h"

CUIBossStamina::CUIBossStamina(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIBase{ pDevice, pContext }
{
}

CUIBossStamina::CUIBossStamina(const CUIBossStamina& Prototype) 
	: CUIBase{ Prototype }
{
}

HRESULT CUIBossStamina::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUIBossStamina::Initialize(void* pArg)
{	
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CUIBossStamina::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CUIBossStamina::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

#ifdef _DEBUG
	// 테스트 용
	if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_B))
	{
		if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_9) && m_iCurrentStamina > 0)
			m_iCurrentStamina -= 1;
		if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_0) && m_iCurrentStamina < m_iMaxStamina)
			m_iCurrentStamina += 1;
	}
#endif

	if (static_cast<CUIBossVitalWrapper*>(m_pParent)->Get_NaytibaDesc() && m_pParent->GetVisibility() == VISIBILITY::VISIBLE)
	{
		CUIBossVitalWrapper* pVitalWrapper = static_cast<CUIBossVitalWrapper*>(m_pParent);
		
		auto StaticDesc = static_cast<CUIBossVitalWrapper*>(m_pParent)->Get_NetworkDesc();
		auto Desc = static_cast<CUIBossVitalWrapper*>(m_pParent)->Get_NaytibaDesc();

		m_iCurrentStamina = Desc->iCurrentStamina;
		m_iMaxStamina = StaticDesc->iMaxStamina;

		m_tUIDesc.fSizeX = (m_iMaxStamina * 12.f) + ((m_iMaxStamina - 1) * 3.f);

		_float4 vScale{ m_tUIDesc.fSizeX, m_pTransformCom->Get_Scale().y, m_pTransformCom->Get_Scale().z, 1.f };

		m_pTransformCom->Set_Scale(XMLoadFloat4(&vScale));
		m_tUIDesc.fOffsetX = -(static_cast<CUIBase*>(m_pParent)->Get_UIBase_Desc().fSizeX * 0.5f) + (m_tUIDesc.fSizeX * 0.5f);
	}
	
	m_fCurrentFill = static_cast<_float>(m_iCurrentStamina) / m_iMaxStamina;
}

void CUIBossStamina::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
}

HRESULT CUIBossStamina::Render()
{
	__super::Render();

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(UI_SHADER_PASS::STAMINA))))
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

HRESULT CUIBossStamina::Ready_Components()
{
	__super::Ready_Components();

	/* Com_VIBuffer */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* Com_Texture_HP */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Boss_Stamina"),
		TEXT("Com_Texture_UI_Boss_Stamina"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUIBossStamina::Bind_ShaderResources()
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
	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture2", 2)))
		return E_FAIL;

	_float2 vTile{};
	vTile.x = 1.f;
	vTile.y = 1.f;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_TileCount", &vTile, sizeof(_float2))))
		return E_FAIL;

	_float fGroupCount{ (_float)m_iMaxStamina };

	if (FAILED(m_pShaderCom->Bind_RawValue("g_GroupCount", &fGroupCount, sizeof(_float))))
		return E_FAIL;

	/*_float fScale{ 0.5f };

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fScale", &fScale, sizeof(_float))))
		return E_FAIL;*/
	
	//_float fFillAmount = m_fCurrentFill;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fFillAmount", &m_fCurrentFill, sizeof(_float))))
		return E_FAIL;

	_float2 vGap{ };
	vGap.x = 3.0f / m_tUIDesc.fSizeX;
	vGap.y = 0.f;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_UVGap", &vGap, sizeof(_float2))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUIBossStamina::Execute(const UI_EVENT_DESC& EventDesc)
{
	return S_OK;
}

void CUIBossStamina::CallbackEvent(void* pArg)
{
}

CUIBossStamina* CUIBossStamina::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUIBossStamina* pInstance = new CUIBossStamina(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUIBossStamina::Clone(void* pArg)
{
	CUIBossStamina* pInstance = new CUIBossStamina(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUIBossStamina");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUIBossStamina::Free()
{
	__super::Free();
}
