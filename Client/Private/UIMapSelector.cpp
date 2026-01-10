#include "pch.h"
#include "UIMapSelector.h"

#include "GameInstance.h"
#include "GameManager.h"
#include "Player.h"
#include "UIHUD.h"

#include "UIInstanceBuffer.h"
#include "UIPopup.h"

CUIMapSelector::CUIMapSelector(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIBase{ pDevice, pContext }
{
}

CUIMapSelector::CUIMapSelector(const CUIMapSelector& Prototype) 
	: CUIBase{ Prototype }
{
}

HRESULT CUIMapSelector::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUIMapSelector::Initialize(void* pArg)
{	
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CUIMapSelector::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CUIMapSelector::Update(_float fTimeDelta)
{
 	__super::Update(fTimeDelta);
}

void CUIMapSelector::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	if (FAILED(SetUp_Buttons()))
		return;
}

HRESULT CUIMapSelector::Render()
{
	__super::Render();

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(UI_SHADER_PASS::MAP_SELECTOR))))
		return E_FAIL;

	if (FAILED(m_pUIButtonsBufferCom->Bind_Resources()))
		return E_FAIL;

	if (FAILED(m_pUIButtonsBufferCom->Render()))
		return E_FAIL;

	if(FAILED(Render_Text()))
		return E_FAIL;

#ifdef _DEBUG
	__super::Render_Debug_Rect();
#endif

	return S_OK;
}

HRESULT CUIMapSelector::Ready_Components()
{
	__super::Ready_Components();

	///* Com_VIBuffer */
	//if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
	//	TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
	//	return E_FAIL;

	/* Com_VIBuffer */
	CUIInstanceBuffer::UI_INSTANCE_DESC InstanceDesc{};
	InstanceDesc.iNumInstance = 7;
	InstanceDesc.vAtlasIndex = _float2(1.f, 1.f);
	InstanceDesc.vUVAtlasSize = _float4(1.f, 1.f, 1.f, 1.f);
	InstanceDesc.vUVAtlasOffset = _float2(0.f, 0.f);

	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_UI_Instance_Buffer"),
		TEXT("Com_UI_InstanceBuffer_Buttons"), reinterpret_cast<CComponent**>(&m_pUIButtonsBufferCom), &InstanceDesc)))
		return E_FAIL;

	/* Com_Texture_UI_Button_Frame */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Popup_Inner_Frame"),
		TEXT("Com_Texture_UI_Button_Frame"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUIMapSelector::Bind_ShaderResources()
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

	_float2 atlasCount = { 1.f, 1.f };
	if (FAILED(m_pShaderCom->Bind_RawValue("g_AtlasCount", &atlasCount, sizeof(_float2))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUIMapSelector::Execute(const UI_EVENT_DESC& EventDesc)
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

void CUIMapSelector::CallbackEvent(void* pArg)
{
	
}

HRESULT CUIMapSelector::SetUp_Buttons()
{
	m_ButtonDescs.clear();

	for (size_t i = 0; i < m_pGameManager->Find_AllTransportDatas()->size(); ++i)
	{
		MAP_SELECTOR_DESC Desc{};
		if (m_pGameManager->Find_TransportData(i)->szAreaName == TEXT(""))
			continue;
		Desc.szAreaName = m_pGameManager->Find_TransportData(i)->szAreaName;
		Desc.bIsEnableMove = m_pGameManager->Find_TransportData(i)->bIsEnableMove;
		Desc.eTargetLevel = m_pGameManager->Find_TransportData(i)->eTargetLevel;
		Desc.vTransportpoint = m_pGameManager->Find_TransportData(i)->vTransportpoint;
		m_ButtonDescs.push_back(Desc);
	}

	m_ButtonInstances.clear();
	m_ButtonInstances.reserve(7);

	_int ATLAS_COL = 1;
	_int ATLAS_ROW = 7;

	_float2 vUISize = _float2{ 270.f, 60.f };
	_float fUIWidth = vUISize.x;
	_float fUIHeight = vUISize.y * (ATLAS_ROW - 1);

	_float2 vScale = _float2{ vUISize.x / m_tUIDesc.fSizeX, vUISize.y / m_tUIDesc.fSizeY };
	_float fWidth = vScale.x;
	_float fHeight = vScale.y * (ATLAS_ROW - 1);

	size_t i = 0;

	for (i; i < m_ButtonDescs.size(); ++i)
	{
		_float2 vPos = _float2{ 0.f, -(vScale.y * i) + (fHeight * 0.5f) + (10.f / vUISize.y * 0.5f) - (i * (_float(ATLAS_ROW * 0.5f) / vUISize.y * 0.5f)) };
		
		VTX_INSTANCE_DESC inst{};
		inst.vUVAtlasSize = _float4{ 1.f, 1.f, vScale.x, vScale.y };
		inst.vUVAtlasOffset = _float4{ 0.f, 0.f, vPos.x, vPos.y };

		_float2 vUIPos = _float2{
			m_tUIDesc.fX + m_tUIDesc.fOffsetX + (vUISize.x * 0.5f) - (fUIWidth * 0.5f),
			m_tUIDesc.fY + (m_tUIDesc.fOffsetY + (vUISize.y * 0.5f)) + (vUISize.y * (i)) - (m_tUIDesc.fSizeY * 0.5f) + (i * (15.f))
		};

		BTN_STATE eBtnState = BTN_STATE::DEFAULT;

		if (MouseEnter(vUIPos, vUISize))
		{
			eBtnState = BTN_STATE::HOVER;

			UI_EVENT_ARG_DESC Arg{};
			Arg.szActionTag = TEXT("Set_Texture_Index");
			Arg.Type = UI_EVENT_ARG_DESC::INT;
			Arg.pData = &i;
			__super::Trigger_Event(TEXT("Set_Texture_Index"), &Arg);

			if (m_ButtonDescs[i].bIsEnableMove && m_pGameInstance->KeyDown(KEY_INPUT::MOUSE, 0))
			{
				eBtnState = BTN_STATE::CLICK;

				if (m_ButtonDescs[i].eTargetLevel != m_pGameInstance->GetCurrentLevelID())
				{
					static_cast<CUIPopup*>(m_pParent)->Set_TargetLevel(LEVEL(m_ButtonDescs[i].eTargetLevel));
					static_cast<CUIPopup*>(m_pParent)->Close_Popup(true, false);
				}
				else
				{
					auto pPlayer = m_pGameManager->GetGameCharacter();

					if (!pPlayer)
					{
						Safe_Release(pPlayer);
						continue;
					}
					_vector vPos = XMVectorSetW(XMLoadFloat3(&m_ButtonDescs[i].vTransportpoint), 1.f);

					pPlayer->GetTransform()->Set_State(STATE::POSITION, vPos);

					auto PlayerDesc = dynamic_cast<CPlayer*>(pPlayer)->Get_Desc();

					PlayerDesc->pPlayerController->Set_Active(false);
					PlayerDesc->pPlayerController->Set_Position(pPlayer->GetTransform()->Get_State(STATE::POSITION));
					Safe_Release(pPlayer);

					static_cast<CUIPopup*>(m_pParent)->Close_Popup(false, true);
				}
			}
		}

		inst.vAtlasIndex = _float4{ 0, (_float)i, 0.f, (_float)ENUM_CLASS(eBtnState) };

		m_ButtonDescs[i].vPos = vUIPos;
		m_ButtonDescs[i].vSize = vUISize;
		m_ButtonDescs[i].eState = eBtnState;

		m_ButtonInstances.push_back(inst);
	}

	for(i; i < ATLAS_ROW; ++i)
	{
		VTX_INSTANCE_DESC inst{};
		inst.vUVAtlasSize = _float4{ 1.f, 1.f, vScale.x, vScale.y };
		inst.vUVAtlasOffset = _float4{ 0.f, 0.f, 0.f, 0.f };
		inst.vAtlasIndex = _float4{ 0.f, 0.f, 0.f, 99.f };

		m_ButtonInstances.push_back(inst);
	}

	m_pUIButtonsBufferCom->Update_Instance(m_ButtonInstances);

	return S_OK;
}

HRESULT CUIMapSelector::Render_Text()
{
	for (size_t i = 0; i < m_ButtonDescs.size(); ++i)
	{
		_float2 fTextSize = m_pGameInstance->Get_Text_Size(
			TEXT("KoPub"), m_ButtonDescs[i].szAreaName.c_str(), true, 0.8f);

		_float2 vPivot{
			m_ButtonDescs[i].vPos.x - (fTextSize.x * 0.5f),
			m_ButtonDescs[i].vPos.y + (fTextSize.y * 0.5f) - (m_ButtonDescs[i].vSize.y * 0.5f) - 7.f
		};

		_vector vColor{ 0.4f, 0.4f, 0.4f, 1.f };

		if (m_ButtonDescs[i].eState == BTN_STATE::HOVER && m_ButtonDescs[i].bIsEnableMove)
			vColor = _vector{ 1.f, 1.f, 1.f, 1.f };
		if(m_ButtonDescs[i].eState == BTN_STATE::CLICK && m_ButtonDescs[i].bIsEnableMove)
			vColor = _vector{ 1.f, 0.f, 0.f, 1.f };
		if(m_ButtonDescs[i].eState == BTN_STATE::SELECT)
			vColor = _vector{ 0.f, 0.f, 1.f, 1.f };

		m_pGameInstance->Render_Text(TEXT("KoPub"),
			m_ButtonDescs[i].szAreaName.c_str(),
			vPivot,
			vColor, 0.8f);
	}

	return S_OK;
}

_bool CUIMapSelector::MouseEnter(_float2 vPos, _float2 vSize)
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

CUIMapSelector* CUIMapSelector::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUIMapSelector* pInstance = new CUIMapSelector(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUIMapSelector::Clone(void* pArg)
{
	CUIMapSelector* pInstance = new CUIMapSelector(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUIMapSelector");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUIMapSelector::Free()
{
	__super::Free();

	Safe_Release(m_pUIButtonsBufferCom);
}