#include "pch.h"
#include "UIBase.h"

#include "GameInstance.h"
#include "GameManager.h"

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
	m_tUIDesc = *static_cast<UIBASE_DESC*>(pArg);

	if (FAILED(__super::Initialize(&m_tUIDesc)))
		return E_FAIL;
	
	m_iZOrder = m_tUIDesc.iDepth;

	m_pGameManager = CGameManager::GetInstance();

	if (!m_pGameManager)
		return E_FAIL;

	if (FAILED(Ready_Texture()))
		return E_FAIL;

	return S_OK;
}

void CUIBase::Priority_Update(_float fTimeDelta)
{
}

void CUIBase::Update(_float fTimeDelta)
{
	if (m_pParent)
	{
		m_tUIDesc.fX = dynamic_cast<CUIBase*>(m_pParent)->Get_UIBase_Desc().fX + dynamic_cast<CUIBase*>(m_pParent)->Get_UIBase_Desc().fOffsetX;
		m_tUIDesc.fY = dynamic_cast<CUIBase*>(m_pParent)->Get_UIBase_Desc().fY + dynamic_cast<CUIBase*>(m_pParent)->Get_UIBase_Desc().fOffsetY;
	}
	ComputeTransform(XMVectorSet(m_tUIDesc.fX + m_tUIDesc.fOffsetX, m_tUIDesc.fY + m_tUIDesc.fOffsetY, 0.f, 1.f));
}

void CUIBase::Late_Update(_float fTimeDelta)
{
	if (m_tUIDesc.Get_UI_Texture_Desc() || m_tUIDesc.Get_UI_Text_Desc())
		m_pGameInstance->Add_RenderGroup((RENDER)m_tUIDesc.iRenderGroup, this);
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
	m_tUIDesc.fOffsetX = fX;
	m_tUIDesc.fOffsetY = fY;
}

void CUIBase::Set_Size(_float fSizeX, _float fSizeY) {
	m_tUIDesc.fSizeX = fSizeX;
	m_tUIDesc.fSizeY = fSizeY;

	m_pTransformCom->Set_Scale(m_tUIDesc.fSizeX, m_tUIDesc.fSizeY, 1.f);
}

HRESULT CUIBase::Set_TextureCom(_wstring szTextureTag, _uint iTextureIndex)
{
	CUIBase::UI_TEXTURE_DESC Desc{};
	Desc.iTextureIndex = iTextureIndex;
	Desc.szTextureComTag = szTextureTag;

	m_tUIDesc.Set_UI_Texture_Desc(Desc);

	if (m_tUIDesc.Get_UI_Texture_Desc() == nullptr)
		return S_OK;

	/*m_tUIDesc.Get_UI_Texture_Desc()->iTextureIndex = iTextureIndex;
	m_tUIDesc.Get_UI_Texture_Desc()->szTextureComTag = szTextureTag;*/

	m_pTextureCom = m_pGameManager->Get_UI_Texture_Desc(m_tUIDesc.Get_UI_Texture_Desc()->szTextureComTag.c_str()).pTexture;

	if (m_pTextureCom == nullptr)
		return E_FAIL;

	return S_OK;
}

HRESULT CUIBase::Ready_Texture()
{
	if (m_tUIDesc.Get_UI_Texture_Desc() == nullptr)
		return S_OK;

	m_pTextureCom = m_pGameManager->Get_UI_Texture_Desc(m_tUIDesc.Get_UI_Texture_Desc()->szTextureComTag.c_str()).pTexture;

	if (m_pTextureCom == nullptr)
		return E_FAIL;

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

	Safe_Delete(m_tUIDesc.m_pUITextDesc);
	
	Safe_Delete(m_tUIDesc.m_pUITextureDesc);

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
