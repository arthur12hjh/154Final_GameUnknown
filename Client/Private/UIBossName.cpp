#include "pch.h"
#include "UIBossName.h"

#include "GameInstance.h"
#include "GameManager.h"

#include "UIBossVitalWrapper.h"
#ifdef _DEBUG
#include "../../UI_Editor/Public/UI_Camera.h"
#endif // DEBUG

CUIBossName::CUIBossName(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIBase{ pDevice, pContext }
{
}

CUIBossName::CUIBossName(const CUIBossName& Prototype) 
	: CUIBase{ Prototype }
{
}

HRESULT CUIBossName::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUIBossName::Initialize(void* pArg)
{	
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CUIBossName::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CUIBossName::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

	if (dynamic_cast<CUIBossVitalWrapper*>(m_pParent)->Get_NetworkDesc())
	{
		if (dynamic_cast<CUIBossVitalWrapper*>(m_pParent)->Get_NetworkDesc()->szMonsterName == "Gigas")
			m_iName = 0;
		if (dynamic_cast<CUIBossVitalWrapper*>(m_pParent)->Get_NetworkDesc()->szMonsterName == "Scarlet")
			m_iName = 1;
	}
}

void CUIBossName::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
}

HRESULT CUIBossName::Render()
{
	__super::Render();

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(UI_SHADER_PASS::UI))))
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

HRESULT CUIBossName::Ready_Components()
{
	__super::Ready_Components();

	/* Com_VIBaseBuffer */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* Com_Texture_HP */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Boss_Name"),
		TEXT("Com_Texture_Boss_Name"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUIBossName::Bind_ShaderResources()
{
	if (FAILED(__super::Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->GetIdentityMatrixPtr())))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;
	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iName)))
		return E_FAIL;

	return S_OK;
}

HRESULT CUIBossName::Execute(const UI_EVENT_DESC& EventDesc)
{
	return S_OK;
}

void CUIBossName::CallbackEvent(void* pArg)
{
}

CUIBossName* CUIBossName::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUIBossName* pInstance = new CUIBossName(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUIBossName::Clone(void* pArg)
{
	CUIBossName* pInstance = new CUIBossName(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUIBossName");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUIBossName::Free()
{
	__super::Free();
}
