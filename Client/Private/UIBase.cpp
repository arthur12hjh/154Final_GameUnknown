#include "pch.h"
#include "UIBase.h"

#include "GameInstance.h"

#include "UIHUD.h"
#include "UIAnimationCom.h"

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
	m_iZOrder = m_tUIDesc.iDepth;


#ifdef _DEBUG
	if (FAILED(Ready_Components_For_Debug()))
		return E_FAIL;
#endif

	if (FAILED(Ready_Texture()))
		return E_FAIL;

	if (FAILED(Ready_UIAnimation()))
		return E_FAIL;

	return S_OK;
}

void CUIBase::Priority_Update(_float fTimeDelta)
{
	/*if (!m_pUIHUD)
	{
		m_pUIHUD = dynamic_cast<CUIHUD*>(m_pGameInstance->GetCurrentLevelHUD());
	}*/
}

void CUIBase::Update(_float fTimeDelta)
{
	////부모 따라가기
	if (m_pParent)
	{
		m_tUIDesc.fX = dynamic_cast<CUIBase*>(m_pParent)->Get_UIBase_Desc().fX + dynamic_cast<CUIBase*>(m_pParent)->Get_UIBase_Desc().fOffsetX;
		m_tUIDesc.fY = dynamic_cast<CUIBase*>(m_pParent)->Get_UIBase_Desc().fY + dynamic_cast<CUIBase*>(m_pParent)->Get_UIBase_Desc().fOffsetY;
		m_tUIDesc.fAlpha = dynamic_cast<CUIBase*>(m_pParent)->Get_UIBase_Desc().fAlpha;
		//m_eAnimState = dynamic_cast<CUIBase*>(m_pParent)->Get_Anim_State();
	}
	
	ComputeTransform(XMVectorSet(m_tUIDesc.fX + m_tUIDesc.fOffsetX, m_tUIDesc.fY + m_tUIDesc.fOffsetY, 0.f, 1.f));
}

void CUIBase::Late_Update(_float fTimeDelta)
{
	switch (m_eAnimState)
	{
	case ANIM_STATE::PLAY :
		m_pUIAnimCom->Play(m_szCurrentAnimTag);
		break;
	case ANIM_STATE::PAUSE :
		m_pUIAnimCom->Pause();
		break;
	case ANIM_STATE::STOP :
	{
		m_pUIAnimCom->Stop();
		m_eAnimState = ANIM_STATE::IDLE;
	}
		break;
	case ANIM_STATE::IDLE:
	{
		m_pUIAnimCom->Stop();
		//m_tUIDesc = m_tOriginUIDesc;
	}
		break;
	}

	if ((m_tUIDesc.Get_UI_Texture_Desc() || m_tUIDesc.Get_UI_Text_Desc()) && m_eVisibility == VISIBILITY::VISIBLE )
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
	
	Safe_AddRef(pUIObject);

	m_Children.push_back(pUIObject);
	return S_OK;
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

void CUIBase::Set_Alpha(_float fAlpha)
{
	m_tUIDesc.fAlpha = fAlpha;
}

void CUIBase::Set_Pass(_uint iPass)
{
	m_tUIDesc.Get_UI_Texture_Desc()->iPass = iPass;
}

// 툴에서 텍스쳐 변경할 때 사용
HRESULT CUIBase::Set_TextureCom(_wstring szTextureTag, _wstring szProtoTag, _uint iTextureIndex)
{
	CUIBase::UI_TEXTURE_DESC Desc{};
	Desc.iTextureIndex = iTextureIndex;
	Desc.szTextureComTag = szTextureTag;
	Desc.szProtoTag = szProtoTag;

	m_tUIDesc.Set_UI_Texture_Desc(Desc);

	if (m_tUIDesc.Get_UI_Texture_Desc() == nullptr)
		return S_OK;

	/* Com_Texture */
	if (FAILED(__super::Add_Component(m_tUIDesc.iLevel, m_tUIDesc.Get_UI_Texture_Desc()->szProtoTag,
		m_tUIDesc.Get_UI_Texture_Desc()->szTextureComTag, reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	if (m_pTextureCom == nullptr)
		return E_FAIL;

	return S_OK;
}

HRESULT CUIBase::Ready_Texture()
{
	if (m_tUIDesc.Get_UI_Texture_Desc() == nullptr)
		return S_OK;

	/* Com_Texture */
	if (FAILED(__super::Add_Component(m_tUIDesc.iLevel, m_tUIDesc.Get_UI_Texture_Desc()->szProtoTag,
		m_tUIDesc.Get_UI_Texture_Desc()->szTextureComTag, reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	if (m_pTextureCom == nullptr)
		return E_FAIL;

	return S_OK;
}

HRESULT CUIBase::Ready_UIAnimation()
{
	m_pUIAnimCom = CUIAnimationCom::Create();
	
	if (!m_pUIAnimCom)
		return E_FAIL;

	m_pUIAnimCom->Initialize();
	m_pUIAnimCom->Set_Owner(this);

	return S_OK;
}

#ifdef _DEBUG
HRESULT CUIBase::Ready_Components_For_Debug()
{
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Point"),
		TEXT("Com_VIBuffer_Debug"), reinterpret_cast<CComponent**>(&m_pVIDebugBufferCom))))
		return E_FAIL;

	return S_OK;
}

void CUIBase::Render_Debug_Rect()
{
#ifdef _DEBUG
	CUIHUD* pHUD = dynamic_cast<CUIHUD*>(m_pGameInstance->GetCurrentLevelHUD());

	if (pHUD && pHUD->Get_Show_Debug_Rect())
	{
		if (FAILED(Bind_Debug_ShaderResources()))
			return;

		if (FAILED(m_pShaderCom->Begin(1)))
			return;

		if (FAILED(m_pVIDebugBufferCom->Bind_Resources()))
			return;

		if (FAILED(m_pVIDebugBufferCom->Render()))
			return;

		Safe_Release(pHUD);
		return;
	}
#endif
}

HRESULT CUIBase::Bind_Debug_ShaderResources()
{
	_float2 vPos{};
	vPos.x = m_tUIDesc.fX + m_tUIDesc.fOffsetX;
	vPos.y = m_tUIDesc.fY + m_tUIDesc.fOffsetY;

	_float2 vSize{};
	vSize.x = m_tUIDesc.fSizeX;
	vSize.y = m_tUIDesc.fSizeY;

	_vector vColor{ 0.f, 1.f, 0.f, 1.f };

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_UIPosition", &vPos, sizeof(_float2))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_UISize", &vSize, sizeof(_float2))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_UIDebugLineColor", &vColor, sizeof(_vector))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->GetIdentityMatrixPtr())))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	return S_OK;
}
#endif

HRESULT CUIBase::Ready_Components()
{
	/* Com_Shader */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_UI"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUIBase::Bind_ShaderResources()
{
	if (m_pShaderCom)
	{
		if (FAILED(m_pShaderCom->Bind_RawValue("g_Alpha", &m_tUIDesc.fAlpha, sizeof(_float))))
			return E_FAIL;
	}

	return S_OK;
}

void CUIBase::Free()
{
	__super::Free();

	Safe_Delete(m_tOriginUIDesc.m_pUITextDesc);
	Safe_Delete(m_tOriginUIDesc.m_pUITextureDesc);

	for (auto iter : m_Children)
		Safe_Release(iter);

#ifdef _DEBUG
	Safe_Release(m_pVIDebugBufferCom);
#endif

	Safe_Release(m_pUIHUD);
	Safe_Release(m_pUIAnimCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
