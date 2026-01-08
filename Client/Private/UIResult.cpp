#include "pch.h"
#include "UIResult.h"

#include "GameInstance.h"
#include "GameManager.h"
#include "UIHUD.h"

#include "UIInstanceBuffer.h"
#include "UIVideoThumbnail.h"

CUIResult::CUIResult(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIBase{ pDevice, pContext }
{
}

CUIResult::CUIResult(const CUIResult& Prototype) 
	: CUIBase{ Prototype }
{
}

HRESULT CUIResult::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUIResult::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CUIResult::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CUIResult::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

	if (m_isOpen)
	{
		if (!(m_fTimeAcc >= 4.5f && m_isShowCombo && m_isShowScore && m_isShowRank))
			m_fTimeAcc += fTimeDelta;

		if (m_fTimeAcc >= 0.5f)
		{
			m_isShowCombo = true;

			_float comboT = (m_fTimeAcc - 0.5f) / 1.0f; // 1초
			comboT = min(comboT, 1.0f);

			m_iCurCombo = static_cast<_uint>(m_iCombo * comboT);
		}

		if (m_fTimeAcc >= 2.f)
		{
			m_isShowScore = true;
			float scoreT = (m_fTimeAcc - 2.f) / 1.0f; // 1초
			scoreT = min(scoreT, 1.0f);

			m_iCurScore = static_cast<int>(m_iScore * scoreT);
		}

		if (m_fTimeAcc >= 3.5f)
		{
			m_isShowRank = true;

			float scaleT = (m_fTimeAcc - 3.5f) / 1.f; // 2초
			scaleT = min(max(scaleT, 0.f), 1.f);

			float ease = sinf(scaleT * XM_PIDIV2); // 0 → 1

			m_fGlowPower = 15.f + (1.f - 15.f) * ease;
		}
	}
	else
		return;

	//// ESC 입력
	//if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_ESCAPE) && !m_bActiveEsc)
	//{
	//	m_fEscKeyPressedTime += fTimeDelta;

	//	UI_EVENT_ARG_DESC Arg{};
	//	Arg.szActionTag = TEXT("EscKey_Event");
	//	Arg.Type = UI_EVENT_ARG_DESC::FLOAT;
	//	Arg.pData = &m_fEscKeyPressedTime;
	//	__super::Trigger_Event(TEXT("EscKey_Event"), &Arg);
	//}
	//if (m_pGameInstance->KeyUp(KEY_INPUT::KEYBOARD, DIK_ESCAPE))
	//{
	//	m_fEscKeyPressedTime = 0.f;

	//	UI_EVENT_ARG_DESC Arg{};
	//	Arg.szActionTag = TEXT("EscKey_Event");
	//	Arg.Type = UI_EVENT_ARG_DESC::FLOAT;
	//	Arg.pData = &m_fEscKeyPressedTime;
	//	__super::Trigger_Event(TEXT("EscKey_Event"), &Arg);
	//}
	//if (m_fEscKeyPressedTime >= 2.f)
	//{
	//	m_bActiveEsc = true;
	//	UI_EVENT_ARG_DESC Arg2{};
	//	Arg2.szActionTag = TEXT("EscKey_Event");
	//	Arg2.Type = UI_EVENT_ARG_DESC::BOOL;
	//	Arg2.pData = &m_bActiveEsc;
	//	__super::Trigger_Event(TEXT("EscKey_Event"), &Arg2);
	//	m_fEscKeyPressedTime = 0.f;

	//	Close_Result();
	//}

	if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_ESCAPE) && m_eVisibility == VISIBILITY::VISIBLE)
		Close_Result();
		
}

void CUIResult::Late_Update(_float fTimeDelta)
{
	CUIHUD* pHUD = dynamic_cast<CUIHUD*>(m_pGameInstance->GetCurrentLevelHUD());

	if (!pHUD)
	{
		Safe_Release(pHUD);
		return;
	}

	auto pOverlay = pHUD->Get_UIObject(TEXT("Layer_BeatSaber_Overlay"), TEXT("BeatSaber_Overlay"));

	if (m_isOpen && m_isOpening && pOverlay->IsAnimFinished(TEXT("Outro")))
	{
		m_pGameInstance->SetGamePause(true);

		auto AnimTag = m_tUIDesc.m_AnimTags.find(TEXT("Open_Result"));

		if (AnimTag != m_tUIDesc.m_AnimTags.end())
			pHUD->Anim_Play(m_tUIDesc.szLayerTag, m_tUIDesc.szUITag, AnimTag->first);

		CUIVideoThumbnail* pThumbnail = dynamic_cast<CUIVideoThumbnail*>(pHUD->Get_UIObject(m_tUIDesc.szLayerTag, TEXT("UI_VideoThumbnail")));
		if (pThumbnail)
		{
			pThumbnail->Set_Source(TEXT("DoroDoro"));
			pThumbnail->Play();
		}

		if (pThumbnail)
			pHUD->Anim_Play(m_tUIDesc.szLayerTag, TEXT("UI_VideoThumbnail"), TEXT("Open_Result"), 0.125f);

		m_eVisibility = VISIBILITY::VISIBLE;

		for (auto& pChild : m_Children)
			Update_Children(pChild);

		m_isOpening = false;
	}

	Safe_Release(pHUD);

	if (!m_isOpen)
		return;

	__super::Late_Update(fTimeDelta);

	if (FAILED(SetUp_Combo()))
		return;
	if (FAILED(SetUp_Score()))
		return;
}

HRESULT CUIResult::Render()
{
	if (!m_isOpen)
		return S_OK;

	__super::Render();

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(UI_SHADER_PASS::DORORONG_SABER_RESULT))))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Resources()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	if (FAILED(Render_Text()))
		return E_FAIL;

	if (FAILED(Render_Combo()))
		return E_FAIL;

	if (FAILED(Render_Score()))
		return E_FAIL;

	if (FAILED(Render_Glows()))
		return E_FAIL;

#ifdef _DEBUG
	__super::Render_Debug_Rect();
#endif

	return S_OK;
}

void CUIResult::Open_Result()
{
	CUIHUD* pHUD = dynamic_cast<CUIHUD*>(m_pGameInstance->GetCurrentLevelHUD());

	auto pOverlay = pHUD->Get_UIObject(TEXT("Layer_BeatSaber_Overlay"), TEXT("BeatSaber_Overlay"));
	if (pOverlay)
		pHUD->Anim_Play(TEXT("Layer_BeatSaber_Overlay"), TEXT("BeatSaber_Overlay"), TEXT("Outro"));

	Safe_Release(pHUD);

	m_isOpen = true;
	m_isOpening = true;
}

void CUIResult::Close_Result()
{
	CUIHUD* pHUD = dynamic_cast<CUIHUD*>(m_pGameInstance->GetCurrentLevelHUD());

	CUIVideoThumbnail* pThumbnail = dynamic_cast<CUIVideoThumbnail*>(pHUD->Get_UIObject(m_tUIDesc.szLayerTag, TEXT("UI_VideoThumbnail")));
	if (pThumbnail)
		pThumbnail->Stop();

	if (pThumbnail)
		pHUD->Anim_Play(m_tUIDesc.szLayerTag, TEXT("UI_VideoThumbnail"), TEXT("Close_Result"));

	auto AnimTag = m_tUIDesc.m_AnimTags.find(TEXT("Close_Result"));

	if (AnimTag != m_tUIDesc.m_AnimTags.end())
		pHUD->Anim_Play(m_tUIDesc.szLayerTag, m_tUIDesc.szUITag, AnimTag->first, 0.25f);

	pHUD->Open_Song_Selector();

	m_isShowCombo = false;
	m_isShowScore = false;
	m_isShowRank = false;

	m_isOpen = false;
	m_fTimeAcc = 0.f;
	m_fScale = 3.f;
	m_bActiveEsc = false;
	m_fEscKeyPressedTime = 0.f;

	Safe_Release(pHUD);
}

HRESULT CUIResult::Ready_Components()
{
	__super::Ready_Components();

	/* Com_VIBuffer */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* Com_Texture_UI_Result_BG */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::BEATSABER_GAME), TEXT("Prototype_Component_UI_Result_BG"),
		TEXT("Com_Texture_UI_Result_BG"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::BEATSABER_GAME), TEXT("Prototype_Component_UI_Texture_Rank"),
		TEXT("Com_Texture_UI_Rank"), reinterpret_cast<CComponent**>(&m_pRankTextureCom))))
		return E_FAIL;

	/* Com_VIBuffer */
	CUIInstanceBuffer::UI_INSTANCE_DESC IconInstanceDesc{};
	IconInstanceDesc.iNumInstance = 5;
	IconInstanceDesc.vAtlasIndex = _float2(1.f, 10.f);
	IconInstanceDesc.vUVAtlasSize = _float4(1.f, 1.f, 1.f, 1.f);
	IconInstanceDesc.vUVAtlasOffset = _float2(0.f, 0.f);

	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_UI_Instance_Buffer"),
		TEXT("Com_UI_InstanceBuffer_Score"), reinterpret_cast<CComponent**>(&m_pUIScoreBufferCom), &IconInstanceDesc)))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::BEATSABER_GAME), TEXT("Prototype_Component_UI_Texture_Numbers"),
		TEXT("Com_Texture_UI_Numbers"), reinterpret_cast<CComponent**>(&m_pNumberTextureCom))))
		return E_FAIL;

	/* Com_VIBuffer */
	CUIInstanceBuffer::UI_INSTANCE_DESC ComboInstanceDesc{};
	ComboInstanceDesc.iNumInstance = 3;
	ComboInstanceDesc.vAtlasIndex = _float2(5.f, 2.f);
	ComboInstanceDesc.vUVAtlasSize = _float4(1.f, 1.f, 1.f, 1.f);
	ComboInstanceDesc.vUVAtlasOffset = _float2(0.f, 0.f);

	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_UI_Instance_Buffer"),
		TEXT("Com_UI_InstanceBuffer_NeonNumber"), reinterpret_cast<CComponent**>(&m_pUIComboBufferCom), &ComboInstanceDesc)))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::BEATSABER_GAME), TEXT("Prototype_Component_UI_Texture_NeonNumber"),
		TEXT("Com_Texture_UI_NeonNumber"), reinterpret_cast<CComponent**>(&m_pNeonNumberTextureCom))))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::BEATSABER_GAME), TEXT("Prototype_Component_UI_Texture_Combo"),
		TEXT("Com_Texture_UI_Combo"), reinterpret_cast<CComponent**>(&m_pComboTextureCom))))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::BEATSABER_GAME), TEXT("Prototype_Component_UI_Texture_Score"),
		TEXT("Com_Texture_UI_Score"), reinterpret_cast<CComponent**>(&m_pScoreTextureCom))))
		return E_FAIL;
	
	/* Com_Texture */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::BEATSABER_GAME), TEXT("Prototype_Component_UI_Texture_Song_Thumbnail"),
		TEXT("Com_Texture_UI_Song_Thumbnail"), reinterpret_cast<CComponent**>(&m_pThumbnailTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUIResult::Bind_ShaderResources()
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

	_float2 vBGOffset = { m_tUIDesc.fOffsetX, m_tUIDesc.fOffsetY };

	if (FAILED(m_pShaderCom->Bind_RawValue("g_Result_BG_Offset", &vBGOffset, sizeof(_float2))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUIResult::Execute(const UI_EVENT_DESC& EventDesc)
{
	const _wstring& Type = EventDesc.szTypeTag;
	const _wstring& Arg = EventDesc.szArg;
	const _wstring& ActionTag = EventDesc.szActionTag;

	// 애니메이션
	if (Type == TEXT("PlayAnimEvent"))
	{
		CUIHUD* pHUD = dynamic_cast<CUIHUD*>(m_pGameInstance->GetCurrentLevelHUD());
		auto AnimTag = m_tUIDesc.m_AnimTags.find(ActionTag);

		if (AnimTag != m_tUIDesc.m_AnimTags.end())
			pHUD->Anim_Play(m_tUIDesc.szLayerTag, m_tUIDesc.szUITag, AnimTag->first);

		Safe_Release(pHUD);
	}

	return S_OK;
}

void CUIResult::CallbackEvent(void* pArg)
{
	auto* arg = static_cast<UI_EVENT_ARG_DESC*>(pArg);
	if (!arg) return;

	if(arg->szActionTag == TEXT("Combo_Result"))
		m_iCombo = *static_cast<_uint*>(arg->pData);
	else if (arg->szActionTag == TEXT("Score_Result"))
		m_iScore = *static_cast<_uint*>(arg->pData);
	else if (arg->szActionTag == TEXT("Rank_Result"))
		m_iRank = *static_cast<_uint*>(arg->pData);
	else
	{
		CUIHUD* pHUD = dynamic_cast<CUIHUD*>(m_pGameInstance->GetCurrentLevelHUD());

		auto AnimTag = m_tUIDesc.m_AnimTags.find(arg->szActionTag);
		if (AnimTag != m_tUIDesc.m_AnimTags.end())
			pHUD->Anim_Play(m_tUIDesc.szLayerTag, m_tUIDesc.szUITag, AnimTag->first);

		Safe_Release(pHUD);
	}
}

HRESULT CUIResult::Render_Text()
{
	_float2 fTextSize = m_pGameInstance->Get_Text_Size(TEXT("KoPub"), TEXT("[NIKKE 오리지널 송] NIKKE DORODORA ODORO"), true, 1.f);

	_float2 vPivot{
		m_tUIDesc.fX + m_tUIDesc.fOffsetX + (m_tUIDesc.fSizeX * 0.5f) - 25.f - fTextSize.x,
		m_tUIDesc.fY + m_tUIDesc.fOffsetY - (m_tUIDesc.fSizeY * 0.5f) + 75.f,
	};
	_float2 vShadowPivot{
		vPivot.x + 2.f,
		vPivot.y + 1.f
	};

	_vector vColor = XMVectorSet(
		1.f,
		1.f,
		1.f,
		1.f
	);

	_vector vShadowColor = XMVectorSet(
		0.f,
		0.f,
		0.f,
		1.f
	);

	m_pGameInstance->Render_Text(TEXT("KoPub"), TEXT("[NIKKE 오리지널 송] NIKKE DORODORA ODORO"),
		vShadowPivot, vShadowColor, 1.f);
	m_pGameInstance->Render_Text(TEXT("KoPub"), TEXT("[NIKKE 오리지널 송] NIKKE DORODORA ODORO"),
		vPivot, vColor, 1.f);

	return S_OK;
}

HRESULT CUIResult::Render_Glows()
{
	if (FAILED(Bind_GlowShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(UI_SHADER_PASS::DORORONG_SABER_RESULT_GLOW))))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Resources()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUIResult::Bind_GlowShaderResources()
{
	__super::Bind_ShaderResources();

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->GetIdentityMatrixPtr())))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;
	if (FAILED(m_pRankTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture0", m_iRank)))
		return E_FAIL;
	if (FAILED(m_pComboTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture1", 0)))
		return E_FAIL;
	if (FAILED(m_pScoreTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture2", 0)))
		return E_FAIL;

	_float fAlpha = 0.f;

	if(m_isShowRank)
		fAlpha = 1.f;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_Alpha", &fAlpha, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_GlowIntensity", &m_fGlowPower, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUIResult::Render_Score()
{
	if (FAILED(Bind_ScoreShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(UI_SHADER_PASS::SCORE))))
		return E_FAIL;

	if (FAILED(m_pUIScoreBufferCom->Bind_Resources()))
		return E_FAIL;

	if (FAILED(m_pUIScoreBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUIResult::SetUp_Score()
{
	m_ScoreInstances.clear();
	m_ScoreInstances.reserve(5);

	_int ATLAS_COL = 5;
	_int ATLAS_ROW = 1;

	_float fRatio = 54.f / 32.f;

	_float2 vUISize = _float2{ 54.f, 54.f };
	_float fUIWidth = vUISize.x * fRatio * (ATLAS_COL - 1);
	_float fUIHeight = vUISize.y * fRatio * (ATLAS_ROW - 1);

	_float2 vScale = _float2{ (vUISize.x / m_tUIDesc.fSizeX), (vUISize.y / (m_tUIDesc.fSizeY)) };
	_float fWidth = vScale.x * fRatio * (ATLAS_COL - 1);
	_float fHeight = vScale.y * fRatio * (ATLAS_ROW - 1);

	for (size_t i = 0; i < ATLAS_COL; ++i)
	{
		_float2 vPos = _float2{ (vScale.x * fRatio * (i % ATLAS_COL)) + (850.f / 1600.f) - (fWidth), -((vScale.y * fRatio * (i / ATLAS_COL)) - (fHeight * 0.5f) + (50.f / 900.f))};

		_uint iScore = 0;

		if (i == 0)
		{
			iScore = ((m_iCurScore / 10000) % 10);
			vPos.x += (108.f/1600.f);
		}
		else if (i == 1)
		{
			iScore = ((m_iCurScore / 1000) % 10);
			vPos.x += (54.f / 1600.f);
		}
		else if (i == 2)
			iScore = ((m_iCurScore / 100) % 10);
		else if (i == 3)
		{
			iScore = ((m_iCurScore / 10) % 10);
			vPos.x -= (54.f / 1600.f);
		}
		else if (i == 4)
		{
			iScore = (m_iCurScore % 10);
			vPos.x -= (108.f / 1600.f);
		}

		_uint col = 0;
		_uint row = iScore;

		VTX_INSTANCE_DESC inst{};
		inst.vUVAtlasSize = _float4{ 1.f, 1.f, vScale.x, vScale.y };
		inst.vUVAtlasOffset = _float4{ 0.f, 0.f, vPos.x, vPos.y };

		inst.vAtlasIndex = _float4{ (_float)col, (_float)row, 0.f, 0.f };

		m_ScoreInstances.push_back(inst);
	}

	m_pUIScoreBufferCom->Update_Instance(m_ScoreInstances);

	return S_OK;
}

HRESULT CUIResult::Bind_ScoreShaderResources()
{
	__super::Bind_ShaderResources();

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->GetIdentityMatrixPtr())))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;
	if (FAILED(m_pNumberTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture0", 0)))
		return E_FAIL;

	_float2 atlasCount = { 1.f, 10.f };
	if (FAILED(m_pShaderCom->Bind_RawValue("g_AtlasCount", &atlasCount, sizeof(_float2))))
		return E_FAIL;

	_float fAlpha = 0.f;

	if (m_isShowScore)
		fAlpha = 1.f;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_Alpha", &fAlpha, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUIResult::Render_Combo()
{
	if (FAILED(Bind_ComboShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(UI_SHADER_PASS::NEON_NUMBER))))
		return E_FAIL;

	if (FAILED(m_pUIComboBufferCom->Bind_Resources()))
		return E_FAIL;

	if (FAILED(m_pUIComboBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUIResult::SetUp_Combo()
{
	m_ComboInstances.clear();
	m_ComboInstances.reserve(3);

	_int ATLAS_COL = 3;
	_int ATLAS_ROW = 1;

	_float fRatio = 128.f / 256.f;

	_float2 vUISize = _float2{ 128.f, 128.f };
	_float fUIWidth = vUISize.x * fRatio * (ATLAS_COL - 1);
	_float fUIHeight = vUISize.y * fRatio * (ATLAS_ROW - 1);

	_float2 vScale = _float2{ (vUISize.x / m_tUIDesc.fSizeX), (vUISize.y / (m_tUIDesc.fSizeY)) };
	_float fWidth = vScale.x * fRatio * (ATLAS_COL - 1);
	_float fHeight = vScale.y * fRatio * (ATLAS_ROW - 1);

	for (size_t i = 0; i < ATLAS_COL; ++i)
	{
		_float2 vPos = _float2{ (vScale.x * fRatio * (i % ATLAS_COL)) + (700.f / 1600.f) - fWidth, -((vScale.y * fRatio * (i / ATLAS_COL)) - (172.f / 900.f))};

		_uint iCombo = 0;

		if (i == 0)
		{
			iCombo = ((m_iCurCombo / 100) % 10);
			vPos.x -= (16.f / 1600.f);
		}
		else if (i == 1)
			iCombo = ((m_iCurCombo / 10) % 10);
		else if (i == 2)
		{
			iCombo = (m_iCurCombo % 10);
			vPos.x += (16.f / 1600.f);
		}

		_uint col = iCombo % 5;
		_uint row = iCombo / 5;

		VTX_INSTANCE_DESC inst{};
		inst.vUVAtlasSize = _float4{ 1.f, 1.f, vScale.x, vScale.y };
		inst.vUVAtlasOffset = _float4{ 0.f, 0.f, vPos.x, vPos.y };

		inst.vAtlasIndex = _float4{ (_float)col, (_float)row, 0.f, 0.f };

		m_ComboInstances.push_back(inst);
	}

	m_pUIComboBufferCom->Update_Instance(m_ComboInstances);

	return S_OK;
}

HRESULT CUIResult::Bind_ComboShaderResources()
{
	__super::Bind_ShaderResources();

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->GetIdentityMatrixPtr())))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;
	if (FAILED(m_pNeonNumberTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture0", 0)))
		return E_FAIL;

	_float2 atlasCount = { 5.f, 2.f };
	if (FAILED(m_pShaderCom->Bind_RawValue("g_AtlasCount", &atlasCount, sizeof(_float2))))
		return E_FAIL;

	_float fAlpha = 0.f;

	if (m_isShowCombo)
		fAlpha = 1.f;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_Alpha", &fAlpha, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

CUIResult* CUIResult::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUIResult* pInstance = new CUIResult(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUIResult::Clone(void* pArg)
{
	CUIResult* pInstance = new CUIResult(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUIResult");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUIResult::Free()
{
	__super::Free();

	Safe_Release(m_pUIScoreBufferCom);
	Safe_Release(m_pUIComboBufferCom);

	Safe_Release(m_pThumbnailTextureCom);
	Safe_Release(m_pRankTextureCom);
	Safe_Release(m_pScoreTextureCom);
	Safe_Release(m_pNumberTextureCom);
	Safe_Release(m_pComboTextureCom);
	Safe_Release(m_pNeonNumberTextureCom);
}
