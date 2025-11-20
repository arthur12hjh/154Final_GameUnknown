#include "pch.h"
#include "UIText.h"

#include "GameInstance.h"

CUIText::CUIText(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIBase{ pDevice, pContext }
{
}

CUIText::CUIText(const CUIText& Prototype) 
	: CUIBase{ Prototype }
{
}

HRESULT CUIText::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUIText::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CUIText::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CUIText::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void CUIText::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
}

HRESULT CUIText::Render()
{
	__super::Render();

	if (m_tUIDesc.Get_UI_Text_Desc())
	{
		_float2 fTextSize = m_pGameInstance->Get_Text_Size(TEXT("KoPub"), m_tUIDesc.Get_UI_Text_Desc()->szText.c_str());

		_float fAlpha{ m_tUIDesc.fAlpha };
		if (m_pParent)
			fAlpha = m_tUIDesc.fAlpha * dynamic_cast<CUIBase*>(m_pParent)->Get_UIBase_Desc().fAlpha;

		_vector vColor = XMVectorSet(
			m_tUIDesc.Get_UI_Text_Desc()->vColor.x * fAlpha,
			m_tUIDesc.Get_UI_Text_Desc()->vColor.y * fAlpha,
			m_tUIDesc.Get_UI_Text_Desc()->vColor.z * fAlpha,
			m_tUIDesc.Get_UI_Text_Desc()->vColor.w * fAlpha
			);

		m_pGameInstance->Render_Text(TEXT("KoPub"),
			m_tUIDesc.Get_UI_Text_Desc()->szText.c_str(),
			_float2(m_tUIDesc.fX + m_tUIDesc.fOffsetX - fTextSize.x * 0.5f, m_tUIDesc.fY + m_tUIDesc.fOffsetY - fTextSize.y * 0.5f),
			vColor);
	}

#ifdef _DEBUG
	__super::Render_Debug_Rect();
#endif

	return S_OK;
}

HRESULT CUIText::Ready_Components()
{
	__super::Ready_Components();

	return S_OK;
}

HRESULT CUIText::Bind_ShaderResources()
{
	__super::Bind_ShaderResources();

	return S_OK;
}

CUIText* CUIText::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUIText* pInstance = new CUIText(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUIText::Clone(void* pArg)
{
	CUIText* pInstance = new CUIText(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUIText");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUIText::Free()
{
	__super::Free();
}
