#include "pch.h"
#include "UICostumePuzzleButtons.h"

#include "GameInstance.h"
#include "UIHUD.h"

#include "UIInstanceBuffer.h"

CUICostumePuzzleButtons::CUICostumePuzzleButtons(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIBase{ pDevice, pContext }
{
}

CUICostumePuzzleButtons::CUICostumePuzzleButtons(const CUICostumePuzzleButtons& Prototype) 
	: CUIBase{ Prototype }
{
}

HRESULT CUICostumePuzzleButtons::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUICostumePuzzleButtons::Initialize(void* pArg)
{	
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_Buttons = {
		0, 1, 2, 3,
		4, 5, 6, 7,
		8, 9, 10, 11
	};

	return S_OK;
}

void CUICostumePuzzleButtons::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CUICostumePuzzleButtons::Update(_float fTimeDelta)
{
 	__super::Update(fTimeDelta);
}

void CUICostumePuzzleButtons::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
	
	if (FAILED(SetUp_Buttons()))
		return;

	for(auto& pChild : m_Children)
		Update_Children(pChild);
}

HRESULT CUICostumePuzzleButtons::Render()
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

	if (FAILED(Render_Buttons()))
		return E_FAIL;

#ifdef _DEBUG
	__super::Render_Debug_Rect();
#endif

	return S_OK;
}

HRESULT CUICostumePuzzleButtons::Ready_Components()
{
	__super::Ready_Components();

	/* Com_VIBuffer */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* Com_VIBuffer */
	CUIInstanceBuffer::UI_INSTANCE_DESC UIButtonsDesc{};
	UIButtonsDesc.iNumInstance = 12;
	UIButtonsDesc.vAtlasIndex = _float2(6.f, 3.f);
	UIButtonsDesc.vUVAtlasSize = _float4(1.f, 1.f, 1.f, 1.f);
	UIButtonsDesc.vUVAtlasOffset = _float2(0.f, 0.f);

	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_UI_Instance_Buffer"),
		TEXT("Com_UI_InstanceBuffer_Buttons"), reinterpret_cast<CComponent**>(&m_pUIButtonsBufferCom), &UIButtonsDesc)))
		return E_FAIL;

	/*if (FAILED(SetUp_Buttons()))
		return E_FAIL;*/

	/* Com_Texture_UI_Popup_Inner_Frame */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Popup_Inner_Frame"),
		TEXT("Com_Texture_UI_Popup_Inner_Frame"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* Com_Texture_UI_Suit_Icons */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Suit_Icons"),
		TEXT("Com_Texture_UI_Suit_Icons"), reinterpret_cast<CComponent**>(&m_pSuitIconsTextureCom))))
		return E_FAIL;

	/* Com_Texture_UI_Costume_Buttons */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Costume_Puzzle_Button"),
		TEXT("Com_Texture_UI_Costume_Buttons"), reinterpret_cast<CComponent**>(&m_pButtonsTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUICostumePuzzleButtons::Bind_ShaderResources()
{
	__super::Bind_ShaderResources();

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->GetIdentityMatrixPtr())))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;
	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;
	//if (FAILED(m_pButtonsTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture1", 1)))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CUICostumePuzzleButtons::Execute(const UI_EVENT_DESC& EventDesc)
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

void CUICostumePuzzleButtons::CallbackEvent(void* pArg)
{
	auto* arg = static_cast<UI_EVENT_ARG_DESC*>(pArg);
	if (!arg) return;
	
	/*CUIHUD* pHUD = dynamic_cast<CUIHUD*>(m_pGameInstance->GetCurrentLevelHUD());
	
	auto AnimTag = m_tUIDesc.m_AnimTags.find(arg->szActionTag);
	if (AnimTag != m_tUIDesc.m_AnimTags.end())
		pHUD->Anim_Play(m_tUIDesc.szLayerTag, m_tUIDesc.szUITag, AnimTag->first);
	
	Safe_Release(pHUD);*/

	if (arg->szActionTag == TEXT("Get_Costume_Puzzle_Unlock"))
	{
		if (arg->Type == UI_EVENT_ARG_DESC::ARG_TYPE::BOOL)
			m_bCanInput = !*static_cast<_bool*>(arg->pData);
	}
}

HRESULT CUICostumePuzzleButtons::Render_Buttons()
{
	if (FAILED(Bind_ButtonsResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(UI_SHADER_PASS::COSTUME_BUTTONS))))
		return E_FAIL;

	if (FAILED(m_pUIButtonsBufferCom->Bind_Resources()))
		return E_FAIL;

	if (FAILED(m_pUIButtonsBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUICostumePuzzleButtons::SetUp_Buttons()
{
	m_ButtonInstances.clear();
	m_ButtonInstances.reserve(12);

	_int ATLAS_COL = 4;
	_int ATLAS_ROW = 3;
	
	_float2 vUISize = _float2{ 56.f, 56.f };
	_float fUIWidth = vUISize.x * 1.5f * (ATLAS_COL - 1);
	_float fUIHeight = vUISize.y * (ATLAS_ROW - 1);

	_float2 vScale = _float2{ vUISize.x / m_tUIDesc.fSizeX, vUISize.y / m_tUIDesc.fSizeY };
	_float fWidth = vScale.x * 1.5f * (ATLAS_COL - 1);
	_float fHeight = vScale.y * (ATLAS_ROW - 1);

	for (size_t i = 0; i < m_Buttons.size(); ++i)
	{
		_int col = m_Buttons[i] % 6;
		_int row = m_Buttons[i] / 6;

		_float2 vPos = _float2{ (vScale.x * 1.5f * (i % 4)) - (fWidth * 0.5f), -((vScale.y * (i / 4)) - (fHeight * 0.5f)) };

		VTX_INSTANCE_DESC inst{};
		inst.vUVAtlasSize = _float4{ 1.f, 1.f, vScale.x, vScale.y };
		inst.vUVAtlasOffset = _float4(0.f, 0.f, vPos.x, vPos.y);

		_float2 vUIPos = _float2{
			m_tUIDesc.fX + m_tUIDesc.fOffsetX + (vUISize.x * 1.5f * (i % 4)) - (fUIWidth * 0.5f),
			m_tUIDesc.fY + m_tUIDesc.fOffsetY + (vUISize.y * (i / 4)) - (fUIHeight * 0.5f)
		};

		if (MouseEnter(vUIPos, vUISize))
		{
			m_eBtnState = BTN_STATE::HOVER;

			if (m_pGameInstance->KeyDown(KEY_INPUT::MOUSE, 0))
			{
				UI_EVENT_ARG_DESC Arg{};
				Arg.szActionTag = TEXT("Input_Answer");
				Arg.Type = UI_EVENT_ARG_DESC::INT;
				Arg.pData = &m_Buttons[i];
				__super::Trigger_Event(TEXT("Input_Answer"), &Arg);

				for (auto& pChild : m_Children)
				{
					if (pChild->Get_UIBase_Desc().szUITag == TEXT("UI_Costume_Button_FX"))
					{
						pChild->Set_Position((vUISize.x * 1.5f * (i % 4)) - (fUIWidth * 0.5f), (vUISize.y * (i / 4)) - (fUIHeight * 0.5f));
						//pChild->SetVisibility(VISIBILITY::VISIBLE);

						_bool bActive = true;
						UI_EVENT_ARG_DESC Arg2{};
						Arg2.szActionTag = TEXT("Costume_Button_Click");
						Arg2.Type = UI_EVENT_ARG_DESC::BOOL;
						Arg2.pData = &bActive;
						__super::Trigger_Event(TEXT("Costume_Button_Click"), &Arg2);
					}
				}
			}
		}
		else
		{
			m_eBtnState = BTN_STATE::DEFAULT;
		}
		
		inst.vAtlasIndex = _float4{ (_float)col, (_float)row, 0.f, (_float)ENUM_CLASS(m_eBtnState) };

		m_ButtonInstances.push_back(inst);
	}

	m_pUIButtonsBufferCom->Update_Instance(m_ButtonInstances);

	return S_OK;
}

HRESULT CUICostumePuzzleButtons::Bind_ButtonsResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->GetIdentityMatrixPtr())))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;
	if (FAILED(m_pSuitIconsTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture0", 0)))
		return E_FAIL;

	_float2 atlasCount = { 6.f, 3.f };
	if (FAILED(m_pShaderCom->Bind_RawValue("g_AtlasCount", &atlasCount, sizeof(_float2))))
		return E_FAIL;

	/*_uint hoverMask = 0;

	if (m_iButtonIdx >= 0)
		hoverMask |= (1 << m_iButtonIdx);

	if (FAILED(m_pShaderCom->Bind_RawValue("g_HoverMask", &hoverMask, sizeof(_uint))))
		return E_FAIL;*/

	//_bool bUseTintColor = true;
	//if (FAILED(m_pShaderCom->Bind_RawValue("g_bUseTintColor", &bUseTintColor, sizeof(_bool))))
	//	return E_FAIL;
	_float4 TintColor = {0.5f, 0.5f, 0.5f, 1.f};
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vTintColor", &TintColor, sizeof(_float4))))
		return E_FAIL;

	return S_OK;
}

_bool CUICostumePuzzleButtons::MouseEnter(_float2 vPos, _float2 vSize)
{
	if (!m_bCanInput)
		return false;

	POINT MousePoint = m_pGameInstance->GetMousePoint();

	_float4 fRect = {
		vPos.x - vSize.x * 0.5f,
		vPos.y - vSize.y * 0.5f,
		vPos.x + vSize.x * 0.5f,
		vPos.y + vSize.y * 0.5f
	};

	return (
		MousePoint.x >= fRect.x &&
		MousePoint.y >= fRect.y &&
		MousePoint.x <= fRect.z &&
		MousePoint.y <= fRect.w);
}

CUICostumePuzzleButtons* CUICostumePuzzleButtons::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUICostumePuzzleButtons* pInstance = new CUICostumePuzzleButtons(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUICostumePuzzleButtons::Clone(void* pArg)
{
	CUICostumePuzzleButtons* pInstance = new CUICostumePuzzleButtons(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUICostumePuzzleButtons");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUICostumePuzzleButtons::Free()
{
	__super::Free();

	Safe_Release(m_pUIButtonsBufferCom);
	Safe_Release(m_pButtonsTextureCom);
	Safe_Release(m_pSuitIconsTextureCom);
}
