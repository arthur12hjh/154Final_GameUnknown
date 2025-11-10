#include "pch.h"
#include "UIBase.h"

#include "GameInstance.h"
#include "UIHUD.h"

CUIBase::CUIBase(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIObject{ pDevice, pContext }
{
}

CUIBase::CUIBase(const CUIBase& Prototype) 
	: CUIObject{ Prototype }
{
}

HRESULT CUIBase::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUIBase::Initialize(void* pArg)
{	
	m_tOriginUIDesc = *static_cast<UIBASE_DESC*>(pArg);

	if (FAILED(__super::Initialize(&m_tOriginUIDesc)))
		return E_FAIL;

	m_tUIDesc = m_tOriginUIDesc;

	if (FAILED(Ready_Texture()))
		return E_FAIL;

	return S_OK;
}

void CUIBase::Priority_Update(_float fTimeDelta)
{
}

void CUIBase::Update(_float fTimeDelta)
{
}

void CUIBase::Late_Update(_float fTimeDelta)
{
}

HRESULT CUIBase::Render()
{
	return S_OK;
}

HRESULT CUIBase::Add_Child(CGameObject* pObj)
{
	CUIBase* pUIObject = dynamic_cast<CUIBase*>(pObj);

	if (pUIObject == nullptr)
		return E_FAIL;

	m_Children.push_back(pUIObject);
}

void CUIBase::Set_Position(_float fX, _float fY)
{
	m_tUIDesc.fX = fX;
	m_tUIDesc.fY = fY;

	_uint2 ScreenSize = m_pGameInstance->GetScreenSize();
	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(m_tUIDesc.fX - ScreenSize.x * 0.5f, -m_tUIDesc.fY + ScreenSize.y * 0.5f, 0.f, 1.f));
}

void CUIBase::Set_Size(_float fSizeX, _float fSizeY) {
	m_tUIDesc.fSizeX = fSizeX;
	m_tUIDesc.fSizeY = fSizeY;

	m_pTransformCom->Set_Scale(m_tUIDesc.fSizeX, m_tUIDesc.fSizeY, 1.f);
}

HRESULT CUIBase::Set_TextureCom(_wstring szTextureTag, _uint iTextureIndex)
{
	m_tUIDesc.iTextureIndex = iTextureIndex;
	m_tUIDesc.szTextureComTag = szTextureTag;

	if (m_tUIDesc.szTextureComTag == TEXT(""))
	{
		m_bRender = false;
		return S_OK;
	}

	m_pTextureCom = dynamic_cast<CUIHUD*>(m_pGameInstance->GetCurrentLevelHUD())->Get_TextureCom(m_tUIDesc.szTextureComTag.c_str());

	if (m_pTextureCom == nullptr)
		return E_FAIL;

	m_bRender = true;

	return S_OK;
}

HRESULT CUIBase::Ready_Texture()
{
	if (m_tUIDesc.szTextureComTag == TEXT(""))
	{
		m_bRender = false;
		return S_OK;
	}

	m_pTextureCom = dynamic_cast<CUIHUD*>(m_pGameInstance->GetCurrentLevelHUD())->Get_TextureCom(m_tUIDesc.szTextureComTag.c_str());

	if (m_pTextureCom == nullptr)
		return E_FAIL;

	m_bRender = true;

	return S_OK;
}

HRESULT CUIBase::Ready_Components()
{
	return S_OK;
}

HRESULT CUIBase::Bind_ShaderResources()
{
	return S_OK;
}

void CUIBase::Free()
{
	__super::Free();

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
