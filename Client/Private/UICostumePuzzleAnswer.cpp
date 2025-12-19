#include "pch.h"
#include "UICostumePuzzleAnswer.h"

#include "GameInstance.h"
#include "UIHUD.h"

#include "UIInstanceBuffer.h"

CUICostumePuzzleAnswer::CUICostumePuzzleAnswer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIBase{ pDevice, pContext }
{
}

CUICostumePuzzleAnswer::CUICostumePuzzleAnswer(const CUICostumePuzzleAnswer& Prototype) 
	: CUIBase{ Prototype }
{
}

HRESULT CUICostumePuzzleAnswer::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUICostumePuzzleAnswer::Initialize(void* pArg)
{	
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_SelectedIndices = { 12, 12, 12, 12, 12, 12 };

	return S_OK;
}

void CUICostumePuzzleAnswer::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CUICostumePuzzleAnswer::Update(_float fTimeDelta)
{
 	__super::Update(fTimeDelta);
}

void CUICostumePuzzleAnswer::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	if (FAILED(SetUp_Icons()))
		return;
}

HRESULT CUICostumePuzzleAnswer::Render()
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

	if (FAILED(Render_Icons()))
		return E_FAIL;

	if (FAILED(Render_Text()))
		return E_FAIL;

#ifdef _DEBUG
	__super::Render_Debug_Rect();
#endif

	return S_OK;
}

HRESULT CUICostumePuzzleAnswer::Ready_Components()
{
	__super::Ready_Components();

	/* Com_VIBuffer */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* Com_VIBuffer */
	CUIInstanceBuffer::UI_INSTANCE_DESC UIIconsDesc{};
	UIIconsDesc.iNumInstance = 6;
	UIIconsDesc.vAtlasIndex = _float2(6.f, 3.f);
	UIIconsDesc.vUVAtlasSize = _float2(1.f, 1.f);
	UIIconsDesc.vUVAtlasOffset = _float2(0.f, 0.f);

	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_UI_Instance_Buffer"),
		TEXT("Com_UI_InstanceBuffer_Icons"), reinterpret_cast<CComponent**>(&m_pUIIconsBufferCom), &UIIconsDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Icons()))
		return E_FAIL;

	/* Com_Texture_UI_Popup_Inner_Frame */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Popup_Inner_Frame"),
		TEXT("Com_Texture_UI_Popup_Inner_Frame"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* Com_Texture_UI_Suit_Icons */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_UI_Texture_Suit_Icons"),
		TEXT("Com_Texture_UI_Suit_Icons"), reinterpret_cast<CComponent**>(&m_pSuitIconsTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUICostumePuzzleAnswer::Bind_ShaderResources()
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

	return S_OK;
}

HRESULT CUICostumePuzzleAnswer::Execute(const UI_EVENT_DESC& EventDesc)
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

void CUICostumePuzzleAnswer::CallbackEvent(void* pArg)
{
	auto* arg = static_cast<UI_EVENT_ARG_DESC*>(pArg);
	if (!arg) return;
	
	CUIHUD* pHUD = dynamic_cast<CUIHUD*>(m_pGameInstance->GetCurrentLevelHUD());
	
	auto AnimTag = m_tUIDesc.m_AnimTags.find(arg->szActionTag);
	if (AnimTag != m_tUIDesc.m_AnimTags.end())
		pHUD->Anim_Play(m_tUIDesc.szLayerTag, m_tUIDesc.szUITag, AnimTag->first);
	
	Safe_Release(pHUD);
}

HRESULT CUICostumePuzzleAnswer::Render_Icons()
{
	if (FAILED(Bind_IconsResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(UI_SHADER_PASS::ATLAS))))
		return E_FAIL;

	if (FAILED(m_pUIIconsBufferCom->Bind_Resources()))
		return E_FAIL;

	if (FAILED(m_pUIIconsBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUICostumePuzzleAnswer::Render_Text()
{
	_float2 fTextSize = m_pGameInstance->Get_Text_Size(TEXT("Iceberg"), TEXT("Enter Passcode"), true, 1.2f);

	_float2 vPivot{
		m_tUIDesc.fX + m_tUIDesc.fOffsetX - (m_tUIDesc.fSizeX * 0.5f),
		m_tUIDesc.fY + m_tUIDesc.fOffsetY - m_tUIDesc.fSizeY - (fTextSize.y * 0.5f) - 10.f,
	};

	_vector vColor = XMVectorSet(
		1.f,
		1.f,
		1.f,
		1.f
	);

	m_pGameInstance->Render_Text(TEXT("Iceberg"), TEXT("Enter Passcode"),
		vPivot, vColor, 1.2f);

	return S_OK;
}

HRESULT CUICostumePuzzleAnswer::SetUp_Icons()
{
	m_IconInstances.clear();
	m_IconInstances.reserve(6);

	_int iconCount = m_SelectedIndices.size();
	_float baseWidth = m_tUIDesc.fSizeX; // 부모 UI 폭
	_float iconWidth = baseWidth / iconCount;
	_float scaleX = iconWidth / baseWidth;   // UI 기준 단위

	_float startX = -(scaleX * (iconCount - 1)) * 0.5f;

	for (_int i = 0; i < iconCount; ++i)
	{
		VTX_INSTANCE_DESC inst{};

		_int col = m_SelectedIndices[i] % 6;
		_int row = m_SelectedIndices[i] / 6;

		inst.vUVAtlasSize = { 1.f, 1.f, scaleX, 0.5f };
		inst.vUVAtlasOffset = { 0.f, 0.f, startX + scaleX * i, 0.f };

		inst.vAtlasIndex = {
			static_cast<_float>(col),
			static_cast<_float>(row),
			0.f, 0.f
		};

		m_IconInstances.push_back(inst);
	}

	m_pUIIconsBufferCom->Update_Instance(m_IconInstances);

	return S_OK;
}

HRESULT CUICostumePuzzleAnswer::Bind_IconsResources()
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

	return S_OK;
}

CUICostumePuzzleAnswer* CUICostumePuzzleAnswer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUICostumePuzzleAnswer* pInstance = new CUICostumePuzzleAnswer(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUICostumePuzzleAnswer::Clone(void* pArg)
{
	CUICostumePuzzleAnswer* pInstance = new CUICostumePuzzleAnswer(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUICostumePuzzleAnswer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUICostumePuzzleAnswer::Free()
{
	__super::Free();

	Safe_Release(m_pUIIconsBufferCom);
	Safe_Release(m_pSuitIconsTextureCom);
}
