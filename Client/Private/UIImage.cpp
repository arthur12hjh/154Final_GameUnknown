#include "pch.h"
#include "UIImage.h"

#include "GameInstance.h"
#include "UIHUD.h"
#include "UIButton.h"
#include "UIPlayAnimEvent.h"

CUIImage::CUIImage(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIBase{ pDevice, pContext }
{
}

CUIImage::CUIImage(const CUIImage& Prototype) 
	: CUIBase{ Prototype }
{
}

HRESULT CUIImage::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUIImage::Initialize(void* pArg)
{	
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CUIImage::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CUIImage::Update(_float fTimeDelta)
{
 	__super::Update(fTimeDelta);
}

void CUIImage::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
}

HRESULT CUIImage::Render()
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

HRESULT CUIImage::Ready_Components()
{
	__super::Ready_Components();

	/* Com_VIBuffer */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUIImage::Bind_ShaderResources()
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

HRESULT CUIImage::Execute(const UI_EVENT_DESC& EventDesc)
{
	const _wstring& Type = EventDesc.szTypeTag;
	const _wstring& Arg = EventDesc.szArg;

	// 애니메이션
	if (Type == TEXT("PlayAnimEvent"))
	{
		CUIHUD* pHUD = dynamic_cast<CUIHUD*>(m_pGameInstance->GetCurrentLevelHUD());
		auto AnimTag = m_tUIDesc.m_AnimTags.find(Arg);

		if (AnimTag != m_tUIDesc.m_AnimTags.end())
			pHUD->Anim_Play(m_tUIDesc.szLayerTag, m_tUIDesc.szUITag, AnimTag->first);

		Safe_Release(pHUD);
	}

	return S_OK;
}

void CUIImage::CallbackEvent(void* pArg)
{
	auto* arg = static_cast<UI_EVENT_ARG_DESC*>(pArg);
	if (!arg) return;
	
	if (arg->szActionTag == TEXT("Set_Texture_Index"))
	{
		_uint iTextureIndex = *static_cast<_uint*>(arg->pData);
		Set_Texture_Index(iTextureIndex);
	}
	else
	{
		CUIHUD* pHUD = dynamic_cast<CUIHUD*>(m_pGameInstance->GetCurrentLevelHUD());
	
		auto AnimTag = m_tUIDesc.m_AnimTags.find(arg->szActionTag);
		if (AnimTag != m_tUIDesc.m_AnimTags.end())
			pHUD->Anim_Play(m_tUIDesc.szLayerTag, m_tUIDesc.szUITag, AnimTag->first);
	
		Safe_Release(pHUD);
	}
}

CUIImage* CUIImage::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUIImage* pInstance = new CUIImage(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUIImage::Clone(void* pArg)
{
	CUIImage* pInstance = new CUIImage(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUIImage");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUIImage::Free()
{
	__super::Free();
}
