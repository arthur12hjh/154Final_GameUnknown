#include "pch.h"
#include "UIItemSlot.h"

#include "GameInstance.h"
#include "GameManager.h"
#include "Player.h"
#include "UIHUD.h"
#include "MousePointer.h"

#include "UIInstanceBuffer.h"
#include "UIPopup.h"

#include "InteractionBinder.h"

CUIItemSlot::CUIItemSlot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIBase{ pDevice, pContext }
{
}

CUIItemSlot::CUIItemSlot(const CUIItemSlot& Prototype) 
	: CUIBase{ Prototype }
{
}

HRESULT CUIItemSlot::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUIItemSlot::Initialize(void* pArg)
{	
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pMousePointer = dynamic_cast<CMousePointer*>(m_pGameInstance->GetAllObejctToLayer(ENUM_CLASS(LEVEL::STATIC), TEXT("Static_Level_Layer_Mouse"))->front());

	m_ItemSlotDescs.clear();
	m_ItemSlotDescs.reserve(m_pGameManager->Get_ShopDatas()->size());

	for (size_t i = 0; i < m_pGameManager->Get_ShopDatas()->size(); ++i)
	{
		ITEM_SLOT_DESC Desc{};
		Desc.iID = m_pGameManager->Get_ShopDatas()->at(i).iID;
		Desc.iPrice = m_pGameManager->Get_ShopDatas()->at(i).iPrice;
		Desc.szItemTag = m_pGameManager->Get_ShopDatas()->at(i).szItemTag;
		Desc.szScriptTag = m_pGameManager->Get_ShopDatas()->at(i).szScriptTag;
		m_ItemSlotDescs.push_back(Desc);
	}

	return S_OK;
}

void CUIItemSlot::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CUIItemSlot::Update(_float fTimeDelta)
{
 	__super::Update(fTimeDelta);

	if (m_eState == INTERACTION_STATE::ACTIVE)
	{
		auto pPlayer = CGameManager::GetInstance()->GetGameCharacter();

		if (pPlayer)
		{
			_int iOwnGold = static_cast<CPlayer*>(pPlayer)->Get_Desc()->iOwnGold - m_ItemSlotDescs[m_iCurrentItemIdx].iPrice;

			if (iOwnGold >= 0)
			{
				static_cast<CPlayer*>(pPlayer)->Get_Desc()->iOwnGold -= m_ItemSlotDescs[m_iCurrentItemIdx].iPrice;
				Apply_Item(pPlayer);
			}
		}

		Safe_Release(pPlayer);
	}
}

void CUIItemSlot::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	if (FAILED(SetUp_ItemSlot(fTimeDelta)))
		return;
	if (FAILED(SetUp_ItemIcon()))
		return;

	MouseAction(fTimeDelta);
}

HRESULT CUIItemSlot::Render()
{
	__super::Render();

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(UI_SHADER_PASS::SHOP_SLOT))))
		return E_FAIL;

	if (FAILED(m_pUIItemSlotBufferCom->Bind_Resources()))
		return E_FAIL;

	if (FAILED(m_pUIItemSlotBufferCom->Render()))
		return E_FAIL;

	if (FAILED(ItemIcon_Render()))
		return E_FAIL;

#ifdef _DEBUG
	__super::Render_Debug_Rect();
#endif

	return S_OK;
}

HRESULT CUIItemSlot::Ready_Components()
{
	__super::Ready_Components();

	/* Com_VIBuffer */
	CUIInstanceBuffer::UI_INSTANCE_DESC InstanceDesc{};
	InstanceDesc.iNumInstance = 21;
	InstanceDesc.vAtlasIndex = _float2(1.f, 1.f);
	InstanceDesc.vUVAtlasSize = _float4(1.f, 1.f, 1.f, 1.f);
	InstanceDesc.vUVAtlasOffset = _float2(0.f, 0.f);

	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_UI_Instance_Buffer"),
		TEXT("Com_UI_InstanceBuffer_ItemSlots"), reinterpret_cast<CComponent**>(&m_pUIItemSlotBufferCom), &InstanceDesc)))
		return E_FAIL;

	/* Com_VIBuffer */
	CUIInstanceBuffer::UI_INSTANCE_DESC IconInstanceDesc{};
	IconInstanceDesc.iNumInstance = 3;
	IconInstanceDesc.vAtlasIndex = _float2(3.f, 1.f);
	IconInstanceDesc.vUVAtlasSize = _float4(1.f, 1.f, 1.f, 1.f);
	IconInstanceDesc.vUVAtlasOffset = _float2(0.f, 0.f);

	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_UI_Instance_Buffer"),
		TEXT("Com_UI_InstanceBuffer_Items"), reinterpret_cast<CComponent**>(&m_pUIItemIconBufferCom), &IconInstanceDesc)))
		return E_FAIL;

	/* Com_Texture_UI_Shop_Slot */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Shop_Slot"),
		TEXT("Com_Texture_UI_Shop_Slot"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* Com_Texture_UI_ItemIcons */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Item_Icons"),
		TEXT("Com_Texture_UI_ItemIcons"), reinterpret_cast<CComponent**>(&m_pItemIconsTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUIItemSlot::Bind_ShaderResources()
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
	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture1", 1)))
		return E_FAIL;

	_float2 atlasCount = { 1.f, 1.f };
	if (FAILED(m_pShaderCom->Bind_RawValue("g_AtlasCount", &atlasCount, sizeof(_float2))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUIItemSlot::Execute(const UI_EVENT_DESC& EventDesc)
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

void CUIItemSlot::CallbackEvent(void* pArg)
{
	
}

HRESULT CUIItemSlot::SetUp_ItemSlot(_float fTimeDelta)
{
	m_ItemSlotInstances.clear();
	m_ItemSlotInstances.reserve(21);

	_int ATLAS_COL = 7;
	_int ATLAS_ROW = 3;

	_float fRatio = 158.f / 256.f;

	//_float2 vUISize = _float2{ 158.f, 158.f };
	_float2 vUISize = _float2{ 256.f, 256.f };
	_float fUIWidth = vUISize.x * fRatio * (ATLAS_COL - 1);
	_float fUIHeight = vUISize.y * fRatio * (ATLAS_ROW - 1);

	_float2 vScale = _float2{ vUISize.x / m_tUIDesc.fSizeX, vUISize.y / m_tUIDesc.fSizeY };
	_float fWidth = vScale.x * fRatio * (ATLAS_COL - 1);
	_float fHeight = vScale.y * fRatio * (ATLAS_ROW - 1);

	_bool isHover = false;

	for (size_t i = 0; i < (size_t)(ATLAS_COL * ATLAS_ROW); ++i)
	{
		_float2 vPos = _float2{ (vScale.x * fRatio * (i % ATLAS_COL)) - (fWidth * 0.5f), -((vScale.y * fRatio * (i / ATLAS_COL)) - (fHeight * 0.5f)) };

		VTX_INSTANCE_DESC inst{};
		inst.vUVAtlasSize = _float4{ 1.f, 1.f, vScale.x, vScale.y };
		inst.vUVAtlasOffset = _float4{ 0.f, 0.f, vPos.x, vPos.y };

		_float2 vUIPos = _float2{
			m_tUIDesc.fX + m_tUIDesc.fOffsetX + (vUISize.x * fRatio * (i % ATLAS_COL)) - (fUIWidth * 0.5f),
			m_tUIDesc.fY + m_tUIDesc.fOffsetY + (vUISize.y * fRatio * (i / ATLAS_COL)) - (fUIHeight * 0.5f)
		};

		BTN_STATE eBtnState = BTN_STATE::DEFAULT;

		_float2 newUISize = { vUISize.x * fRatio, vUISize.y * fRatio };

		if (MouseEnter(vUIPos, newUISize) && i < m_ItemSlotDescs.size())
		{
			eBtnState = BTN_STATE::HOVER;
			isHover = true;
			m_iCurrentItemIdx = i;
		}

		inst.vAtlasIndex = _float4{ 0.f, 0.f, 0.f, (_float)eBtnState };

		m_ItemSlotInstances.push_back(inst);
	}

	if (isHover)
	{
		m_pMousePointer->SetVisibility(VISIBILITY::HIDDEN);
 		m_eState = INTERACTION_STATE::DEFAULT;
		UI_EVENT_ARG_DESC Arg{};
		Arg.szActionTag = TEXT("ItemSlot_Event");
		Arg.Type = UI_EVENT_ARG_DESC::INTERACTION_STATE;
		Arg.pData = &m_eState;
		__super::Trigger_Event(TEXT("ItemSlot_Event"), &Arg);
	}
	else
	{
		m_iCurrentItemIdx = -1;
		m_iPrevItemIdx = -1;
		m_pMousePointer->SetVisibility(VISIBILITY::VISIBLE);
		m_eState = INTERACTION_STATE::END;
		UI_EVENT_ARG_DESC Arg{};
		Arg.szActionTag = TEXT("ItemSlot_Event");
		Arg.Type = UI_EVENT_ARG_DESC::INTERACTION_STATE;
		Arg.pData = &m_eState;
		__super::Trigger_Event(TEXT("ItemSlot_Event"), &Arg);
	}

	m_pUIItemSlotBufferCom->Update_Instance(m_ItemSlotInstances);

	return S_OK;
}

HRESULT CUIItemSlot::SetUp_ItemIcon()
{
	m_ItemIconInstances.clear();
	m_ItemIconInstances.reserve(3);

	_int ATLAS_COL = 7;
	_int ATLAS_ROW = 3;

	_float fRatio = 158.f / 128.f;

	_float2 vUISize = _float2{ 128.f, 128.f };
	_float fUIWidth = vUISize.x * fRatio * (ATLAS_COL - 1);
	_float fUIHeight = vUISize.y * fRatio * (ATLAS_ROW - 1);

	_float2 vScale = _float2{ vUISize.x / m_tUIDesc.fSizeX, vUISize.y / m_tUIDesc.fSizeY };
	_float fWidth = vScale.x * fRatio * (ATLAS_COL - 1);
	_float fHeight = vScale.y * fRatio * (ATLAS_ROW - 1);

	for (size_t i = 0; i < 3; ++i)
	{
		_float2 vPos = _float2{ (vScale.x * fRatio * (i % ATLAS_COL)) - (fWidth * 0.5f), -((vScale.y * fRatio * (i / ATLAS_COL)) - (fHeight * 0.5f)) };

		_uint col = i % 3;
		_uint row = i / 3;

		VTX_INSTANCE_DESC inst{};
		inst.vUVAtlasSize = _float4{ 1.f, 1.f, vScale.x, vScale.y };
		inst.vUVAtlasOffset = _float4{ 0.f, 0.f, vPos.x, vPos.y };

		_float2 vUIPos = _float2{
			m_tUIDesc.fX + m_tUIDesc.fOffsetX + (vUISize.x * fRatio * (i % ATLAS_COL)) - (fUIWidth * 0.5f),
			m_tUIDesc.fY + m_tUIDesc.fOffsetY + (vUISize.y * fRatio * (i / ATLAS_COL)) - (fUIHeight * 0.5f)
		};

		BTN_STATE eBtnState = BTN_STATE::DEFAULT;

		if (MouseEnter(vUIPos, vUISize))
		{
			eBtnState = BTN_STATE::HOVER;

			/*UI_EVENT_ARG_DESC Arg{};
			Arg.szActionTag = TEXT("Set_Texture_Index");
			Arg.Type = UI_EVENT_ARG_DESC::INT;
			Arg.pData = &i;
			__super::Trigger_Event(TEXT("Set_Texture_Index"), &Arg);

			if (m_pGameInstance->KeyDown(KEY_INPUT::MOUSE, 0))
			{
				eBtnState = BTN_STATE::CLICK;
			}*/
		}

		inst.vAtlasIndex = _float4{ (_float)col, (_float)row, 0.f, 0.f };

		m_ItemIconInstances.push_back(inst);
	}

	m_pUIItemIconBufferCom->Update_Instance(m_ItemIconInstances);

	return S_OK;
}

HRESULT CUIItemSlot::ItemIcon_Render()
{
	__super::Render();

	if (FAILED(Bind_ItemIcon_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(UI_SHADER_PASS::ITEM_ICON))))
		return E_FAIL;

	if (FAILED(m_pUIItemIconBufferCom->Bind_Resources()))
		return E_FAIL;

	if (FAILED(m_pUIItemIconBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUIItemSlot::Bind_ItemIcon_ShaderResources()
{
	__super::Bind_ShaderResources();

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->GetIdentityMatrixPtr())))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;
	if (FAILED(m_pItemIconsTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture0", 0)))
		return E_FAIL;

	_float2 atlasCount = { 3.f, 1.f };
	if (FAILED(m_pShaderCom->Bind_RawValue("g_AtlasCount", &atlasCount, sizeof(_float2))))
		return E_FAIL;

	return S_OK;
}

_bool CUIItemSlot::MouseEnter(_float2 vPos, _float2 vSize)
{ 
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

void CUIItemSlot::MouseAction(_float fTimeDelta)
{
	/*if(m_eState == INTERACTION_STATE::END)
		return;*/

	if (m_pGameInstance->KeyUp(KEY_INPUT::MOUSE, 1)
		|| m_iPrevItemIdx != m_iCurrentItemIdx)
	{
		m_fInteractionTime = 0.f;
		m_eState = INTERACTION_STATE::DEFAULT;
		UI_EVENT_ARG_DESC Arg{};
		Arg.szActionTag = TEXT("ItemSlot_Event");
		Arg.Type = UI_EVENT_ARG_DESC::INTERACTION_STATE;
		Arg.pData = &m_eState;	
		__super::Trigger_Event(TEXT("ItemSlot_Event"), &Arg);

		UI_EVENT_ARG_DESC Arg2{};
		Arg2.szActionTag = TEXT("ItemSlot_Event");
		Arg2.Type = UI_EVENT_ARG_DESC::FLOAT;
		Arg2.pData = &m_fInteractionTime;
		__super::Trigger_Event(TEXT("ItemSlot_Event"), &Arg2);
	}
	else if ((m_eState == INTERACTION_STATE::DEFAULT || m_eState == INTERACTION_STATE::LOCK)
		&& m_pGameInstance->KeyPressed(KEY_INPUT::MOUSE, 1))
	{
		m_eState = INTERACTION_STATE::LOCK;
		m_fInteractionTime += fTimeDelta;
		
		if (m_fInteractionTime >= 1.f)
			m_eState = INTERACTION_STATE::CONTACT;

		UI_EVENT_ARG_DESC Arg{};
		Arg.szActionTag = TEXT("ItemSlot_Event");
		Arg.Type = UI_EVENT_ARG_DESC::INTERACTION_STATE;
		Arg.pData = &m_eState;
		__super::Trigger_Event(TEXT("ItemSlot_Event"), &Arg);

		UI_EVENT_ARG_DESC Arg2{};
		Arg2.szActionTag = TEXT("ItemSlot_Event");
		Arg2.Type = UI_EVENT_ARG_DESC::FLOAT;
		Arg2.pData = &m_fInteractionTime;
		__super::Trigger_Event(TEXT("ItemSlot_Event"), &Arg2);
	}
	if (m_eState == INTERACTION_STATE::CONTACT)
	{
		m_fInteractionTime = 0.f;
		m_eState = INTERACTION_STATE::ACTIVE;
	}

	m_iPrevItemIdx = m_iCurrentItemIdx;
}

void CUIItemSlot::Apply_Item(CPlayer* pPlayer)
{
	if (m_ItemSlotDescs[m_iCurrentItemIdx].iID == 0) // 포션 갯수
	{
		pPlayer->Get_Desc()->iMaxPotions += 1;
		pPlayer->Get_Desc()->iCurrentPotions = pPlayer->Get_Desc()->iMaxPotions;
	}
	if (m_ItemSlotDescs[m_iCurrentItemIdx].iID == 1) // 최대 체력
	{
		pPlayer->Get_Desc()->iMaxHealth += 10;
		pPlayer->Get_Desc()->iCurrentHealth = pPlayer->Get_Desc()->iMaxHealth;
	}
	if (m_ItemSlotDescs[m_iCurrentItemIdx].iID == 2) // 베타 포인트
	{
		pPlayer->Get_Desc()->iMaxBetaEnergy += 4;
		pPlayer->Get_Desc()->iCurrentBetaEnergy = pPlayer->Get_Desc()->iMaxBetaEnergy;
	}
}

CUIItemSlot* CUIItemSlot::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUIItemSlot* pInstance = new CUIItemSlot(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUIItemSlot::Clone(void* pArg)
{
	CUIItemSlot* pInstance = new CUIItemSlot(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUIItemSlot");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUIItemSlot::Free()
{
	__super::Free();

	Safe_Release(m_pMousePointer);
	
	Safe_Release(m_pUIItemSlotBufferCom);
	Safe_Release(m_pUIItemIconBufferCom);
	
	Safe_Release(m_pItemIconsTextureCom);
}