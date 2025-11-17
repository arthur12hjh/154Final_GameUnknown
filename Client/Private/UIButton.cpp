#include "pch.h"
#include "UIButton.h"

#include "GameInstance.h"
#include "MousePointer.h"
#include "UIHUD.h"
#include "UIAnimationCom.h"

CUIButton::CUIButton(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIBase{ pDevice, pContext }
{
}

CUIButton::CUIButton(const CUIButton& Prototype) 
	: CUIBase{ Prototype }
{
}

HRESULT CUIButton::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUIButton::Initialize(void* pArg)
{	
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pMousePointer = dynamic_cast<CMousePointer*>(
		m_pGameInstance->GetAllObejctToLayer(ENUM_CLASS(LEVEL::STATIC), TEXT("Static_Level_Layer_Mouse"))->front());

	return S_OK;
}

void CUIButton::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CUIButton::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

	if (MouseEnter())
	{
		//m_pUIHUD->Anim_Play(m_tUIDesc.szLayerTag, m_tUIDesc.szUITag, TEXT("Test_Anim"));
		m_eBtnState = BTN_STATE::HOVER;
	}
	else
	{
		//m_pUIHUD->Anim_Stop(m_tUIDesc.szLayerTag, m_tUIDesc.szUITag);
		m_eBtnState = BTN_STATE::DEFAULT;
		/*CUIHUD* pHUD = dynamic_cast<CUIHUD*>(m_pGameInstance->GetCurrentLevelHUD());
		pHUD->Anim_Stop(m_tUIDesc.szLayerTag, m_tUIDesc.szUITag, TEXT("Test_Anim"));
		Safe_Release(pHUD);*/
	}
}

void CUIButton::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	if (m_ePrevBtnState != m_eBtnState)
	{
		switch (m_eBtnState)
		{
		case Client::CUIButton::BTN_STATE::DEFAULT:
		{
			CUIHUD* pHUD = dynamic_cast<CUIHUD*>(m_pGameInstance->GetCurrentLevelHUD());
			pHUD->Anim_Stop(m_tUIDesc.szLayerTag, m_tUIDesc.szUITag);
			Safe_Release(pHUD);
			break;
		}
		case Client::CUIButton::BTN_STATE::HOVER:
		{
			CUIHUD* pHUD = dynamic_cast<CUIHUD*>(m_pGameInstance->GetCurrentLevelHUD());
			pHUD->Anim_Play(m_tUIDesc.szLayerTag, m_tUIDesc.szUITag, TEXT("Test_Anim"));
			Safe_Release(pHUD);
			//m_pUIAnimCom->Play(TEXT("Test_Anim"));
			break;
		}
		case Client::CUIButton::BTN_STATE::CLICK:
			break;
		case Client::CUIButton::BTN_STATE::SELECT:
			break;
		default:
			break;
		}

		m_ePrevBtnState = m_eBtnState;
	}
}

HRESULT CUIButton::Render()
{
	__super::Render();

	if (m_tUIDesc.Get_UI_Texture_Desc())
	{
		if (FAILED(Bind_ShaderResources()))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(m_tUIDesc.Get_UI_Texture_Desc()->iPass)))
			return E_FAIL;

		if (FAILED(m_pVIBufferCom->Bind_Resources()))
			return E_FAIL;

		if (FAILED(m_pVIBufferCom->Render()))
			return E_FAIL;
	}

#ifdef _DEBUG
	__super::Render_Debug_Rect();
#endif

	return S_OK;
}

HRESULT CUIButton::Ready_Components()
{
	__super::Ready_Components();

	/* Com_VIBuffer */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUIButton::Bind_ShaderResources()
{
	__super::Bind_ShaderResources();

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->GetIdentityMatrixPtr())))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;
	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_tUIDesc.Get_UI_Texture_Desc()->iTextureIndex)))
		return E_FAIL;

	return S_OK;
}

_bool CUIButton::MouseEnter()
{
	/*_float4 fMousePos{};
	XMStoreFloat4(&fMousePos,
		dynamic_cast<CTransform*>(m_pMousePointer->Find_Component(TEXT("Com_Transform")))->Get_State(STATE::POSITION));*/

	POINT MousePoint = m_pGameInstance->GetMousePoint();

	_float4 fRect = {
		m_tUIDesc.fX + m_tUIDesc.fOffsetX - m_tUIDesc.fSizeX * 0.5f,
		m_tUIDesc.fY + m_tUIDesc.fOffsetY - m_tUIDesc.fSizeY * 0.5f,
		m_tUIDesc.fX + m_tUIDesc.fOffsetX + m_tUIDesc.fSizeX * 0.5f,
		m_tUIDesc.fY + m_tUIDesc.fOffsetY + m_tUIDesc.fSizeY * 0.5f
	};

	return (
		MousePoint.x >= fRect.x &&
		MousePoint.y >= fRect.y &&
		MousePoint.x <= fRect.z &&
		MousePoint.y <= fRect.w);
}

CUIButton* CUIButton::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUIButton* pInstance = new CUIButton(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUIButton::Clone(void* pArg)
{
	CUIButton* pInstance = new CUIButton(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUIButton");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUIButton::Free()
{
	__super::Free();

	Safe_Release(m_pMousePointer);
}
