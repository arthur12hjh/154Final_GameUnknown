#include "pch.h"
#include "UIBase.h"

#include "GameInstance.h"

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
