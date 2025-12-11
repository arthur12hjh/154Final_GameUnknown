#include "pch.h"
#include "UILockOn.h"

#include "GameInstance.h"
#include "UIHUD.h"

CUILockOn::CUILockOn(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIBase{ pDevice, pContext }
{
}

CUILockOn::CUILockOn(const CUILockOn& Prototype) 
	: CUIBase{ Prototype }
{
}

HRESULT CUILockOn::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUILockOn::Initialize(void* pArg)
{	
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CUILockOn::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CUILockOn::Update(_float fTimeDelta)
{
 	__super::Update(fTimeDelta);

	if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_L))
		m_isFinisher = true;
	else if (m_pGameInstance->KeyUp(KEY_INPUT::KEYBOARD, DIK_L))
		m_isFinisher = false;
}

void CUILockOn::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	if (m_isFinisher)
	{
		m_fTimeAcc += fTimeDelta;

		if(m_fTimeAcc <= 0.25f)
			m_tUIDesc.m_tUIShaderDesc.fScale = 1.f + (m_fTimeAcc);
		else if(m_fTimeAcc > 0.25f && m_fTimeAcc <= 0.5f)
			m_tUIDesc.m_tUIShaderDesc.fScale = 1.5f - (m_fTimeAcc);

		
		m_tUIDesc.fRotation += XMConvertToRadians(360.f) * fTimeDelta;
		
		if (m_tUIDesc.fRotation >= XM_2PI)
			m_tUIDesc.fRotation -= XM_2PI;
	}
	else
	{
		m_tUIDesc.m_tUIShaderDesc.fScale = 1.f;
		m_tUIDesc.fRotation = 0.f;
		m_fTimeAcc = 0.f;
	}
}

HRESULT CUILockOn::Render()
{
	__super::Render();

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(UI_SHADER_PASS::LOCKON))))
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

HRESULT CUILockOn::Ready_Components()
{
	__super::Ready_Components();

	/* Com_VIBuffer */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect_Instance"),
		TEXT("Com_VIBaseBuffer"), reinterpret_cast<CComponent**>(&m_pVIBaseBufferCom))))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_LockOn"),
		TEXT("Com_Texture_LockOn"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_KeyIcon_Mouse"),
		TEXT("Com_Texture_MouseKey"), reinterpret_cast<CComponent**>(&m_pKeyTextureCom))))
		return E_FAIL;
	
	/* Com_Texture */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Shadow_Ball"),
		TEXT("Com_Texture_Shadow"), reinterpret_cast<CComponent**>(&m_pShadowTextureCom))))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Finish_Ring"),
		TEXT("Com_Texture_Ring"), reinterpret_cast<CComponent**>(&m_pRingTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUILockOn::Bind_ShaderResources()
{
	__super::Bind_ShaderResources();

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->GetIdentityMatrixPtr())))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;



	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture0", 0)))
		return E_FAIL;

	if (m_isFinisher)
	{
		if (FAILED(m_pShadowTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture1", 0)))
			return E_FAIL;
		if (FAILED(m_pKeyTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture2", 0)))
			return E_FAIL;
		if (FAILED(m_pRingTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture3", 0)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_fScale", &m_tUIDesc.m_tUIShaderDesc.fScale, sizeof(_float))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_fRotation", &m_tUIDesc.fRotation, sizeof(_float))))
			return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_isFinisher", &m_isFinisher, sizeof(_bool))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUILockOn::Execute(const UI_EVENT_DESC& EventDesc)
{
	return S_OK;
}

void CUILockOn::CallbackEvent(void* pArg)
{
}

CUILockOn* CUILockOn::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUILockOn* pInstance = new CUILockOn(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUILockOn::Clone(void* pArg)
{
	CUILockOn* pInstance = new CUILockOn(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUILockOn");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUILockOn::Free()
{
	__super::Free();

	Safe_Release(m_pVIBaseBufferCom);
	Safe_Release(m_pShadowTextureCom);
	Safe_Release(m_pKeyTextureCom);
	Safe_Release(m_pRingTextureCom);
}
