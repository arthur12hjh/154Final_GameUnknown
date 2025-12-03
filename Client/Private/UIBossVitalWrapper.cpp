#include "pch.h"
#include "UIBossVitalWrapper.h"

#include "GameInstance.h"
#include "GameManager.h"

#include "Player.h"
#ifdef _DEBUG
#include "../../UI_Editor/Public/UI_Camera.h"
#endif // DEBUG

CUIBossVitalWrapper::CUIBossVitalWrapper(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIBase{ pDevice, pContext }
{
}

CUIBossVitalWrapper::CUIBossVitalWrapper(const CUIBossVitalWrapper& Prototype) 
	: CUIBase{ Prototype }
{
}

HRESULT CUIBossVitalWrapper::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUIBossVitalWrapper::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CUIBossVitalWrapper::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CUIBossVitalWrapper::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void CUIBossVitalWrapper::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	if (m_pNaytibaDesc && m_eVisibility == VISIBILITY::VISIBLE)
	{
		if (m_pNaytibaDesc->iCurrentHealth <= 0)
			m_eVisibility = VISIBILITY::HIDDEN;
	}
}

HRESULT CUIBossVitalWrapper::Render()
{
	__super::Render();

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(UI_SHADER_PASS::GLOW))))
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

HRESULT CUIBossVitalWrapper::Ready_Components()
{
	__super::Ready_Components();

	/* Com_VIBaseBuffer */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBaseBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* Com_Texture_HP */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Boss_Vital_Shadow"),
		TEXT("Com_Texture_UI_Boss_Vital_Shadow"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUIBossVitalWrapper::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->GetIdentityMatrixPtr())))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;
	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	return S_OK;
}

HRESULT CUIBossVitalWrapper::Execute(const UI_EVENT_DESC& EventDesc)
{
	return S_OK;
}

void CUIBossVitalWrapper::CallbackEvent(void* pArg)
{
}

CUIBossVitalWrapper* CUIBossVitalWrapper::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUIBossVitalWrapper* pInstance = new CUIBossVitalWrapper(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUIBossVitalWrapper::Clone(void* pArg)
{
	CUIBossVitalWrapper* pInstance = new CUIBossVitalWrapper(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUIBossVitalWrapper");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUIBossVitalWrapper::Free()
{
	__super::Free();
}
