#include "pch.h"
#include "UIBase.h"

#include "GameInstance.h"

#include "UIHUD.h"
#include "UIPlayAnimEvent.h"
#include "UIActionEvent.h"

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

	m_eVisibility = (VISIBILITY)m_tUIDesc.iVisiblity;
	m_eDrawType = (DRAW_TYPE)m_tUIDesc.iDrawType;

#ifdef _DEBUG
	if (FAILED(Ready_Components_For_Debug()))
		return E_FAIL;
#endif

	if (FAILED(Ready_Texture()))
		return E_FAIL;

	if (FAILED(Ready_Events()))
		return E_FAIL;

	return S_OK;
}

void CUIBase::Priority_Update(_float fTimeDelta)
{
}

void CUIBase::Update(_float fTimeDelta)
{
	////부모 따라가기
	if (dynamic_cast<CUIBase*>(m_pParent))
	{
		m_eVisibility = m_pParent->GetVisibility();
		
		m_tUIDesc.fX = dynamic_cast<CUIBase*>(m_pParent)->Get_UIBase_Desc().fX + dynamic_cast<CUIBase*>(m_pParent)->Get_UIBase_Desc().fOffsetX;
		m_tUIDesc.fY = dynamic_cast<CUIBase*>(m_pParent)->Get_UIBase_Desc().fY + dynamic_cast<CUIBase*>(m_pParent)->Get_UIBase_Desc().fOffsetY;
		
		m_tUIDesc.iVisiblity = dynamic_cast<CUIBase*>(m_pParent)->Get_UIBase_Desc().iVisiblity;
	}
	
	m_pTransformCom->Rotation(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(m_tUIDesc.fRotation));

	ComputeTransform(XMVectorSet(m_tUIDesc.fX + m_tUIDesc.fOffsetX, m_tUIDesc.fY + m_tUIDesc.fOffsetY, 0.f, 1.f));
}

void CUIBase::Late_Update(_float fTimeDelta)
{
#ifdef _DEBUG
	if (m_eVisibility == VISIBILITY::VISIBLE)
		m_pGameInstance->Add_RenderGroup((RENDER)m_tUIDesc.iRenderGroup, this);
#elif
	if ((m_tUIDesc.Get_UI_Texture_Desc() || m_tUIDesc.Get_UI_Text_Desc()) && m_eVisibility == VISIBILITY::VISIBLE )
		m_pGameInstance->Add_RenderGroup((RENDER)m_tUIDesc.iRenderGroup, this);
#endif
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

void CUIBase::Set_Rotation(_float fRotation)
{
	m_tUIDesc.fRotation = fRotation;
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

void CUIBase::Set_Text_Color(_float4 vColor)
{
	if (m_tUIDesc.Get_UI_Text_Desc())
		m_tUIDesc.m_tUITextDesc.vColor = vColor;
}

void CUIBase::Set_TextureUV(_float4 vUV)
{
	if (m_tUIDesc.m_tUIShaderDesc.bUseUV)
	{
		m_tUIDesc.m_tUIShaderDesc.fUVScaleX = vUV.x;
		m_tUIDesc.m_tUIShaderDesc.fUVScaleY = vUV.y;
		m_tUIDesc.m_tUIShaderDesc.fUVOffsetX = vUV.z;
		m_tUIDesc.m_tUIShaderDesc.fUVOffsetY = vUV.w;
	}
}

void CUIBase::Set_FillAmount(_float fFillAmount)
{
	if (m_tUIDesc.m_tUIShaderDesc.bUseFillClip)
		m_tUIDesc.m_tUIShaderDesc.fFillAmount = fFillAmount;
}

void CUIBase::Set_TintColor(_float4 vColor)
{
	if (m_tUIDesc.Get_ShaderDesc())
		m_tUIDesc.m_tUIShaderDesc.vTintColor = vColor;
}

void CUIBase::Set_Texture_Index(_uint iTextureIndex)
{
	if (m_tUIDesc.Get_UI_Texture_Desc())
		m_tUIDesc.m_tUITextureDesc.iTextureIndex = iTextureIndex;
}

void CUIBase::Set_GlowIntensity(_float fIntensity)
{
	if (m_tUIDesc.Get_ShaderDesc())
		m_tUIDesc.m_tUIShaderDesc.fGlowIntensity = fIntensity;
}

// 툴에서 텍스쳐 변경할 때 사용
HRESULT CUIBase::Set_TextureCom(_wstring szTextureTag, _wstring szProtoTag, _uint iTextureIndex)
{
	UI_TEXTURE_DESC Desc{};
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

void CUIBase::Bind_Event(vector<_wstring> SubEvents, vector<CEventHandle*> Events)
{
	for (auto& SubEvent : SubEvents)
	{
		for(auto Event : Events)
			m_pGameInstance->Bind_Observer(SubEvent.c_str(), Event);
	}
}

void CUIBase::UnBind_Event()
{
	for (auto& SubEvent : m_SubscribeEvents)
	{
		for (auto& EventHandle : m_pEventHandles)
		{
			for (auto& pEvent : EventHandle.second)
			{
				m_pGameInstance->UnBind_Observer(SubEvent.c_str(), pEvent);
			}
		}
	}
}

HRESULT CUIBase::Ready_Texture()
{
	if (!m_tUIDesc.Get_UI_Texture_Desc())
		return S_OK;

	/* Com_Texture */
	if (FAILED(__super::Add_Component(m_tUIDesc.iLevel, m_tUIDesc.Get_UI_Texture_Desc()->szProtoTag,
		m_tUIDesc.Get_UI_Texture_Desc()->szTextureComTag, reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	if (!m_pTextureCom)
		return E_FAIL;

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
	CUIHUD* pUIHUD = dynamic_cast<CUIHUD*>(m_pGameInstance->GetCurrentLevelHUD());

	if (pUIHUD && pUIHUD->Get_Show_Debug_Rect())
	{
		if (FAILED(Bind_Debug_ShaderResources()))
		{
			Safe_Release(pUIHUD);
			return;
		}

		if (FAILED(m_pShaderCom->Begin(1)))
		{
			Safe_Release(pUIHUD);
			return;
		}

		if (FAILED(m_pVIDebugBufferCom->Bind_Resources()))
		{
			Safe_Release(pUIHUD);
			return;
		}

		if (FAILED(m_pVIDebugBufferCom->Render()))
		{
			Safe_Release(pUIHUD);
			return;
		}

		Safe_Release(pUIHUD);
		return;
	}
	Safe_Release(pUIHUD);
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

HRESULT CUIBase::Ready_Events()
{
	for (auto& EventDesc : m_tUIDesc.m_Events)
	{
		vector<UI_EVENT_DESC> EventDescs = EventDesc.second;

		_wstring szEventTag = EventDesc.first;
		vector<CEventHandle*> Events{};

		for (auto& Desc : EventDescs)
		{
			CEventHandle* pEvent{ nullptr };

			if (Desc.szTypeTag == TEXT("PlayAnimEvent"))
				pEvent = CUIPlayAnimEvent::Create([&](void* pArg) {this->CallbackEvent(pArg);});

			if (Desc.szTypeTag == TEXT("ActionEvent"))
				pEvent = CUIActionEvent::Create([&](void* pArg) {this->CallbackEvent(pArg);});

			if (!pEvent)
				return E_FAIL;

			m_pGameInstance->Add_Event(szEventTag.c_str(), pEvent);

			Events.push_back(pEvent);
			Bind_Event(Desc.szSubscribeEventTags, Events);
			m_SubscribeEvents = Desc.szSubscribeEventTags;
		}

		m_pEventHandles.emplace(szEventTag, Events);
	}

	return S_OK;
}

HRESULT CUIBase::Initialize_ShaderResources()
{
	_float fAlpha{ 1.f };

	if (FAILED(m_pShaderCom->Bind_RawValue("g_Alpha", &fAlpha, sizeof(_float))))
		return E_FAIL;

	_float2 vUV{};
	vUV.x = 1.f;
	vUV.y = 1.f;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_UVScale", &vUV, sizeof(_float2))))
		return E_FAIL;

	vUV.x = 0.f;
	vUV.y = 0.f;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_UVOffset", &vUV, sizeof(_float2))))
		return E_FAIL;

	_bool bUseFilClip{ false };
	_float fFillAmount{ 1.f };

	if (FAILED(m_pShaderCom->Bind_RawValue("g_bUseFillClip", &bUseFilClip, sizeof(_bool))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fFillAmount", &fFillAmount, sizeof(_float))))
		return E_FAIL;

	_bool bUseTintColor{ false };
	_float4 vTintColor{ 1.f, 1.f, 1.f, 1.f };

	if (FAILED(m_pShaderCom->Bind_RawValue("g_bUseTintColor", &bUseTintColor, sizeof(_bool))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vTintColor", &vTintColor, sizeof(_float4))))
		return E_FAIL;

	_bool bDiscardBlack{ false };

	if (FAILED(m_pShaderCom->Bind_RawValue("g_bDiscardBlack", &bDiscardBlack, sizeof(_bool))))
		return E_FAIL;

	_bool bUseGlow{ false };

	if (FAILED(m_pShaderCom->Bind_RawValue("g_bUseGlow", &bUseGlow, sizeof(_bool))))
		return E_FAIL;

	_bool bUsePulse{ false };

	if (FAILED(m_pShaderCom->Bind_RawValue("g_bUsePulse", &bUsePulse, sizeof(_bool))))
		return E_FAIL;

	_bool bUseScroll{ false };
	_float fScrollSpeed{ 0.f };

	if (FAILED(m_pShaderCom->Bind_RawValue("g_bScroll", &bUseScroll, sizeof(_bool))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_ScrollSpeed", &fScrollSpeed, sizeof(_float))))
		return E_FAIL;

	_float fTime{ 0.f };
	_float g_PulseSpeed{ 0.f };
	_float g_GlowIntensity{ 0.f };
	_float g_GlowSpread{ 0.f };
	if (FAILED(m_pShaderCom->Bind_RawValue("g_PulseTime", &fTime, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_PulseSpeed", &g_PulseSpeed, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_GlowIntensity", &g_GlowIntensity, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_GlowSpread", &g_GlowSpread, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

void CUIBase::Trigger_Event(const _wstring& szActionTag, void* pArg)
{
 	for (auto& Events : m_tUIDesc.m_Events)
	{
		for (auto& EventDesc : Events.second)
		{
			if (EventDesc.szActionTag == szActionTag)
			{
				Execute(EventDesc);
				Broadcast_Event(Events.first, szActionTag, pArg);
			}
		}
	}
}

HRESULT CUIBase::Bind_ShaderResources()
{
	if (m_pShaderCom)
	{
		if (FAILED(Initialize_ShaderResources()))
			return E_FAIL;

		_float fAlpha{ m_tUIDesc.fAlpha };

		/*if(m_pParent && dynamic_cast<CUIBase*>(m_pParent))
			fAlpha = m_tUIDesc.fAlpha * dynamic_cast<CUIBase*>(m_pParent)->Get_UIBase_Desc().fAlpha;*/

		if (FAILED(m_pShaderCom->Bind_RawValue("g_Alpha", &fAlpha, sizeof(_float))))
			return E_FAIL;

		if (m_tUIDesc.m_tUIShaderDesc.bUseUV)
		{
			_float2 vUV{};
			vUV.x = m_tUIDesc.m_tUIShaderDesc.fUVScaleX;
			vUV.y = m_tUIDesc.m_tUIShaderDesc.fUVScaleY;

			if (FAILED(m_pShaderCom->Bind_RawValue("g_UVScale", &vUV, sizeof(_float2))))
				return E_FAIL;

			vUV.x = m_tUIDesc.m_tUIShaderDesc.fUVOffsetX;
			vUV.y = m_tUIDesc.m_tUIShaderDesc.fUVOffsetY;

			if (FAILED(m_pShaderCom->Bind_RawValue("g_UVOffset", &vUV, sizeof(_float2))))
				return E_FAIL;
		}

		if (m_tUIDesc.m_tUIShaderDesc.bUseFillClip)
		{
			if (FAILED(m_pShaderCom->Bind_RawValue("g_bUseFillClip", &m_tUIDesc.m_tUIShaderDesc.bUseFillClip, sizeof(_bool))))
				return E_FAIL;

			if (FAILED(m_pShaderCom->Bind_RawValue("g_fFillAmount", &m_tUIDesc.m_tUIShaderDesc.fFillAmount, sizeof(_float))))
				return E_FAIL;
		}

		if (m_tUIDesc.m_tUIShaderDesc.bUseTintColor)
		{
			if (FAILED(m_pShaderCom->Bind_RawValue("g_bUseTintColor", &m_tUIDesc.m_tUIShaderDesc.bUseTintColor, sizeof(_bool))))
				return E_FAIL;
			if (FAILED(m_pShaderCom->Bind_RawValue("g_vTintColor", &m_tUIDesc.m_tUIShaderDesc.vTintColor, sizeof(_float4))))
				return E_FAIL;
		}

		if (m_tUIDesc.m_tUIShaderDesc.bDiscardBlack)
		{
			if (FAILED(m_pShaderCom->Bind_RawValue("g_bDiscardBlack", &m_tUIDesc.m_tUIShaderDesc.bDiscardBlack, sizeof(_bool))))
				return E_FAIL;
		}

		if (m_tUIDesc.m_tUIShaderDesc.bUseGlow)
		{
			if (FAILED(m_pShaderCom->Bind_RawValue("g_bUseGlow", &m_tUIDesc.m_tUIShaderDesc.bUseGlow, sizeof(_bool))))
				return E_FAIL;

			if (FAILED(m_pShaderCom->Bind_RawValue("g_GlowIntensity", &m_tUIDesc.m_tUIShaderDesc.fGlowIntensity, sizeof(_float))))
				return E_FAIL;

			if (FAILED(m_pShaderCom->Bind_RawValue("g_GlowSpread", &m_tUIDesc.m_tUIShaderDesc.fGlowSpread, sizeof(_float))))
				return E_FAIL;
		}

		if (m_tUIDesc.m_tUIShaderDesc.bUsePulseEffect)
		{
			if (FAILED(m_pShaderCom->Bind_RawValue("g_bUsePulse", &m_tUIDesc.m_tUIShaderDesc.bUsePulseEffect, sizeof(_bool))))
				return E_FAIL;

			if (FAILED(m_pShaderCom->Bind_RawValue("g_bScroll", &m_tUIDesc.m_tUIShaderDesc.bUseScroll, sizeof(_bool))))
				return E_FAIL;

			if (FAILED(m_pShaderCom->Bind_RawValue("g_ScrollSpeed", &m_tUIDesc.m_tUIShaderDesc.fScrollSpeed, sizeof(_float))))
				return E_FAIL;

			if (FAILED(m_pShaderCom->Bind_RawValue("g_PulseTime", &m_tUIDesc.m_tUIShaderDesc.fPulseTime, sizeof(_float))))
				return E_FAIL;

			if (FAILED(m_pShaderCom->Bind_RawValue("g_PulseSpeed", &m_tUIDesc.m_tUIShaderDesc.fPulseSpeed, sizeof(_float))))
				return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CUIBase::Broadcast_Event(const _wstring& szEventTag, const _wstring& szActionTag, void* pArg)
{
	// (1) 인자 유효성 검증: 반드시 UI_EVENT_ARG_DESC*
	if (!ValidateEventArg(pArg))
		return E_FAIL;

	auto it = m_pEventHandles.find(szEventTag);
	if (it == m_pEventHandles.end())
		return S_OK;

	UI_EVENT_ARG_DESC* pEventArg = static_cast<UI_EVENT_ARG_DESC*>(pArg);
	pEventArg->szActionTag = szActionTag;

	// (2) 모든 핸들에 안전하게 Notify
	for (auto* pHandle : it->second)
	{
		if (!pHandle) continue;
 		pHandle->Notify(pArg);
	}
	return S_OK;
}

bool CUIBase::ValidateEventArg(void* pArg) const
{
	if (pArg == nullptr)
		return false;

	auto* p = static_cast<UI_EVENT_ARG_DESC*>(pArg);
	// 최소한의 sanity check (NONE은 보통 사용 금지)
	if (p->Type == UI_EVENT_ARG_DESC::NONE)
		return false;

	return true;
}

void CUIBase::Free()
{
	__super::Free();

	for (auto iter : m_Children)
		Safe_Release(iter);

	UnBind_Event();

	for (auto& EventHandle : m_pEventHandles)
	{
		for (auto& Event : EventHandle.second)
			Safe_Release(Event);
		EventHandle.second.clear();
	}
	m_pEventHandles.clear();

#ifdef _DEBUG
	Safe_Release(m_pVIDebugBufferCom);
#endif

	//Safe_Release(m_pParent);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
