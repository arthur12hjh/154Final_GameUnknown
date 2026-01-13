#include "pch.h"
#include "UISongSelector.h"

#include "GameInstance.h"
#include "GameManager.h"
#include "UIHUD.h"

#include "UIVideoThumbnail.h"

CUISongSelector::CUISongSelector(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIBase{ pDevice, pContext }
{
}

CUISongSelector::CUISongSelector(const CUISongSelector& Prototype) 
	: CUIBase{ Prototype }
{
}

HRESULT CUISongSelector::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUISongSelector::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_SongDescs.reserve(3);

	/*for (size_t i = 0; i < 10; ++i)
	{
		_float fCorrectionedY = (_float)i - (10 * 0.5f);

		_wstring szText = TEXT("노래 제목") + to_wstring(i) + to_wstring(fCorrectionedY);
		m_Songs.push_back(szText);
	}*/

	SONG_DESC Desc{};
	Desc.szSongName = TEXT("[NIKKE 오리지널 송]\nNIKKE DORODORA ODORO");
	Desc.szSongKey = TEXT("DoroDoro");
	_stprintf_s(Desc.szVideoTag, TEXT("DoroDoro"));
	m_SongDescs.push_back(Desc);

	Desc.szSongName = TEXT("Be My Light");
	Desc.szSongKey = TEXT("BML");
	_stprintf_s(Desc.szVideoTag, TEXT("BML"));
	m_SongDescs.push_back(Desc);

	Desc.szSongName = TEXT("Heart Breaker");
	Desc.szSongKey = TEXT("HEARTBREAKER");
	_stprintf_s(Desc.szVideoTag, TEXT("HEARTBREAKER"));
	m_SongDescs.push_back(Desc);

	return S_OK;
}

void CUISongSelector::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CUISongSelector::Update(_float fTimeDelta)
{
	if (!m_isOpen)
		return;

	__super::Update(fTimeDelta);

	Update_SongIndex(fTimeDelta);

	// ENTER 입력
	if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_RETURN) && !m_bActiveEnter)
	{
		//if (m_fEnterKeyPressedTime <= 0.f)
		//	m_pGameInstance->Manager_PlaySound(TEXT("UI_GaugeFX.wav"), CHANNELID::EFFECT, 1.f, 1.f);
		
		m_fEnterKeyPressedTime += fTimeDelta;

		UI_EVENT_ARG_DESC Arg{};
		Arg.szActionTag = TEXT("EnterKey_Event");
		Arg.Type = UI_EVENT_ARG_DESC::FLOAT;
		Arg.pData = &m_fEnterKeyPressedTime;
		__super::Trigger_Event(TEXT("EnterKey_Event"), &Arg);

		m_fEscKeyPressedTime = 0.f;

		UI_EVENT_ARG_DESC Arg3{};
		Arg3.szActionTag = TEXT("EscKey_Event");
		Arg3.Type = UI_EVENT_ARG_DESC::FLOAT;
		Arg3.pData = &m_fEscKeyPressedTime;
		__super::Trigger_Event(TEXT("EscKey_Event"), &Arg3);
	}
	if (m_pGameInstance->KeyUp(KEY_INPUT::KEYBOARD, DIK_RETURN))
	{
		m_fEnterKeyPressedTime = 0.f;

		UI_EVENT_ARG_DESC Arg{};
		Arg.szActionTag = TEXT("EnterKey_Event");
		Arg.Type = UI_EVENT_ARG_DESC::FLOAT;
		Arg.pData = &m_fEnterKeyPressedTime;
		__super::Trigger_Event(TEXT("EnterKey_Event"), &Arg);
	}

	if (m_fEnterKeyPressedTime >= 2.f)
	{
		m_bActiveEnter = true;
		UI_EVENT_ARG_DESC Arg2{};
		Arg2.szActionTag = TEXT("EnterKey_Event");
		Arg2.Type = UI_EVENT_ARG_DESC::BOOL;
		Arg2.pData = &m_bActiveEnter;
		__super::Trigger_Event(TEXT("EnterKey_Event"), &Arg2);
		m_fEnterKeyPressedTime = 0.f;

		Close_Song_Selector();
	}
	
	// ESC 입력
	if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_ESCAPE) && !m_bActiveEsc)
	{
		//if (m_fEscKeyPressedTime <= 0.f)
		//	m_pGameInstance->Manager_PlaySound(TEXT("UI_GaugeFX.wav"), CHANNELID::EFFECT, 1.f, 1.f);
		
		m_fEscKeyPressedTime += fTimeDelta;

		UI_EVENT_ARG_DESC Arg{};
		Arg.szActionTag = TEXT("EscKey_Event");
		Arg.Type = UI_EVENT_ARG_DESC::FLOAT;
		Arg.pData = &m_fEscKeyPressedTime;
		__super::Trigger_Event(TEXT("EscKey_Event"), &Arg);

		m_fEnterKeyPressedTime = 0.f;

		UI_EVENT_ARG_DESC Arg3{};
		Arg3.szActionTag = TEXT("EnterKey_Event");
		Arg3.Type = UI_EVENT_ARG_DESC::FLOAT;
		Arg3.pData = &m_fEnterKeyPressedTime;
		__super::Trigger_Event(TEXT("EnterKey_Event"), &Arg3);
	}
	if (m_pGameInstance->KeyUp(KEY_INPUT::KEYBOARD, DIK_ESCAPE))
	{
		m_fEscKeyPressedTime = 0.f;

		UI_EVENT_ARG_DESC Arg{};
		Arg.szActionTag = TEXT("EscKey_Event");
		Arg.Type = UI_EVENT_ARG_DESC::FLOAT;
		Arg.pData = &m_fEscKeyPressedTime;
		__super::Trigger_Event(TEXT("EscKey_Event"), &Arg);
	}
	if (m_fEscKeyPressedTime >= 2.f)
	{
		m_bActiveEsc = true;
		UI_EVENT_ARG_DESC Arg2{};
		Arg2.szActionTag = TEXT("EscKey_Event");
		Arg2.Type = UI_EVENT_ARG_DESC::BOOL;
		Arg2.pData = &m_bActiveEsc;
		__super::Trigger_Event(TEXT("EscKey_Event"), &Arg2);
		m_fEscKeyPressedTime = 0.f;

		Close_Song_Selector();
	}
}

void CUISongSelector::Late_Update(_float fTimeDelta)
{
	if (m_isOpen && m_isOpening && IsAnimFinished(TEXT("Open_Song_Selector")))
	{
		m_pGameInstance->SetGamePause(true);

		m_eVisibility = VISIBILITY::VISIBLE;

		for (auto& pChild : m_Children)
			Update_Children(pChild);

		m_isOpening = false;

		m_bActiveEnter = false;
		m_bActiveEsc = false;

		Update_Thumbnail();

		UI_EVENT_ARG_DESC Arg{};
		Arg.szActionTag = TEXT("PreListen");
		Arg.Type = UI_EVENT_ARG_DESC::WSTRING;
		Arg.pData = &m_SongDescs[m_iCurrentIndex].szSongKey;
		__super::Trigger_Event(TEXT("PreListen"), &Arg);
	}

	if (m_isOpen && m_isClosing && IsAnimFinished(TEXT("Close_Song_Selector")))
	{
		m_pGameInstance->SetGamePause(false);

		m_eVisibility = VISIBILITY::HIDDEN;

		for (auto& pChild : m_Children)
			Update_Children(pChild);

		m_isOpen = false;
		m_isClosing = false;

		if (m_bActiveEnter)
		{
			UI_EVENT_ARG_DESC Arg{};
			Arg.szActionTag = TEXT("PlaySong_Event");
			Arg.Type = UI_EVENT_ARG_DESC::WSTRING;
			Arg.pData = &m_SongDescs[m_iCurrentIndex].szSongKey;
			__super::Trigger_Event(TEXT("PlaySong_Event"), &Arg);
		}
		if (m_bActiveEsc)
		{
			UI_EVENT_ARG_DESC Arg{};
			Arg.szActionTag = TEXT("Change_Level");
			Arg.Type = UI_EVENT_ARG_DESC::BOOL;
			Arg.pData = &m_bActiveEsc;
			__super::Trigger_Event(TEXT("Change_Level"), &Arg);
		}

		m_bActiveEnter = false;
		m_bActiveEsc = false;

		CUIHUD* pHUD = dynamic_cast<CUIHUD*>(m_pGameInstance->GetCurrentLevelHUD());

		auto pOverlay = pHUD->Get_UIObject(TEXT("Layer_BeatSaber_Overlay"), TEXT("BeatSaber_Overlay"));
		if (pOverlay)
			pHUD->Anim_Play(TEXT("Layer_BeatSaber_Overlay"), TEXT("BeatSaber_Overlay"), TEXT("Intro"));

		Safe_Release(pHUD);
	}

	if (!m_isOpen)
		return;

	__super::Late_Update(fTimeDelta);
}

HRESULT CUISongSelector::Render()
{
	__super::Render();

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;
	
	if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(UI_SHADER_PASS::DORORONG_SABER_SONG_SELECTOR))))
		return E_FAIL;
	
	if (FAILED(m_pVIBufferCom->Bind_Resources()))
		return E_FAIL;
	
	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	if (!m_bActiveEsc && !m_bActiveEnter)
	{
		if (FAILED(Render_Text()))
			return E_FAIL;
	}

#ifdef _DEBUG
	__super::Render_Debug_Rect();
#endif

	return S_OK;
}

void CUISongSelector::Open_Song_Selector()
{
	CUIHUD* pHUD = dynamic_cast<CUIHUD*>(m_pGameInstance->GetCurrentLevelHUD());

	auto AnimTag = m_tUIDesc.m_AnimTags.find(TEXT("Open_Song_Selector"));

	if (AnimTag != m_tUIDesc.m_AnimTags.end())
		pHUD->Anim_Play(m_tUIDesc.szLayerTag, m_tUIDesc.szUITag, AnimTag->first);

	auto pThumbnail = pHUD->Get_UIObject(m_tUIDesc.szLayerTag, TEXT("UI_VideoThumbnail"));
	if (pThumbnail)
		pHUD->Anim_Play(m_tUIDesc.szLayerTag, TEXT("UI_VideoThumbnail"), TEXT("Open_Song_Selector"), 0.25f);

	Safe_Release(pHUD);

	m_isOpen = true;
	m_isOpening = true;
}

void CUISongSelector::Close_Song_Selector()
{
	CUIHUD* pHUD = dynamic_cast<CUIHUD*>(m_pGameInstance->GetCurrentLevelHUD());

	CUIVideoThumbnail* pThumbnail = dynamic_cast<CUIVideoThumbnail*>(pHUD->Get_UIObject(m_tUIDesc.szLayerTag, TEXT("UI_VideoThumbnail")));
	pThumbnail->Stop();

	if (pThumbnail)
		pHUD->Anim_Play(m_tUIDesc.szLayerTag, TEXT("UI_VideoThumbnail"), TEXT("Close_Song_Selector"), 0.25f);

	auto AnimTag = m_tUIDesc.m_AnimTags.find(TEXT("Close_Song_Selector"));

	if (AnimTag != m_tUIDesc.m_AnimTags.end())
		pHUD->Anim_Play(m_tUIDesc.szLayerTag, m_tUIDesc.szUITag, AnimTag->first, 0.5f);

	Safe_Release(pHUD);

	m_isClosing = true;
}

HRESULT CUISongSelector::Ready_Components()
{
	__super::Ready_Components();

	/* Com_VIBuffer */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* Com_Texture_UI_Song_Selector_BG */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::BEATSABER_GAME), TEXT("Prototype_Component_UI_Song_Selector_BG"),
		TEXT("Com_Texture_UI_Song_Selector_BG"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* Com_Texture_UI_Song_Selector_Arrows */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::BEATSABER_GAME), TEXT("Prototype_Component_UI_Texture_Song_Selector_Arrows"),
		TEXT("Com_Texture_UI_Song_Selector_Arrows"), reinterpret_cast<CComponent**>(&m_pArrowTextureCom))))
		return E_FAIL;

	/* Com_Texture_UI_Song_Selected */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::BEATSABER_GAME), TEXT("Prototype_Component_UI_Texture_Song_Selected"),
		TEXT("Com_Texture_UI_Song_Selected"), reinterpret_cast<CComponent**>(&m_pSelectedBGTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUISongSelector::Bind_ShaderResources()
{
	if (FAILED(__super::Initialize_ShaderResources()))
		return E_FAIL;

	__super::Bind_ShaderResources();

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->GetIdentityMatrixPtr())))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture0", 0)))
		return E_FAIL;
	if (FAILED(m_pArrowTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture1", 0)))
		return E_FAIL;
	if (FAILED(m_pSelectedBGTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture2", 0)))
		return E_FAIL;

	_float2 vBGOffset = { m_tUIDesc.fOffsetX, m_tUIDesc.fOffsetY };

	if (FAILED(m_pShaderCom->Bind_RawValue("g_Result_BG_Offset", &vBGOffset, sizeof(_float2))))
		return E_FAIL;

	_float fAlpha = 1.f;

	if (m_bActiveEsc || m_bActiveEnter)
	{
		fAlpha = 0.f;
		if (FAILED(m_pShaderCom->Bind_RawValue("g_Alpha", &fAlpha, sizeof(_float))))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CUISongSelector::Execute(const UI_EVENT_DESC& EventDesc)
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

void CUISongSelector::CallbackEvent(void* pArg)
{
	//auto* arg = static_cast<UI_EVENT_ARG_DESC*>(pArg);
	//if (!arg) return;
}

HRESULT CUISongSelector::Render_Text()
{
	const _float centerY = 460.f;
	const _float clipTop = 270.f;
	const _float clipBottom = 650.f;

	_int half = m_iVisibleCount / 2;

	// 기준 인덱스 + 부드러운 오프셋
	_int baseIndex = (_int)floor(m_fScroll / m_fItemGap);
	_float localOffset = m_fScroll - (baseIndex * m_fItemGap);

	for (_int slot = -half; slot <= half; ++slot)
	{
		// 실제 곡 인덱스 (무한 루프)
		_int songIndex = (baseIndex + slot) % (_int)m_SongDescs.size();
		if (songIndex < 0)
			songIndex += (_int)m_SongDescs.size();

		const wstring& text = m_SongDescs[songIndex].szSongName;

		// Y 위치
		_float y = centerY + (slot * m_fItemGap) - localOffset;

		// 논리적 overflow hidden
		if (y < clipTop || y > clipBottom)
			continue;

		_float2 textSize =
			m_pGameInstance->Get_Text_Size(TEXT("KoPub"), text.c_str(), true, 0.8f);

		_float2 vPivot{
			335.f + m_tUIDesc.fOffsetX - (textSize.x * 0.5f),
			y - m_tUIDesc.fOffsetY - (textSize.y * 0.5f)
		};

		// 중앙 강조용 알파
		_float dist = fabs(y - centerY);
		_float alpha = max(0.015f, 1.f - (dist / 200.f));

		_vector vColor = XMVectorSet(1.f * alpha, 1.f * alpha, 1.f * alpha, 1.f * alpha);

		vector<_wstring> SplitedTexts{};
		Text_Split(text.c_str(), SplitedTexts);

		_uint iLines = 0;

		if (SplitedTexts.size() > 0)
		{
			for (auto& SplitedText : SplitedTexts)
			{
				_float2 vNewSize =
					m_pGameInstance->Get_Text_Size(TEXT("KoPub"), SplitedText.c_str(), true, 0.8f);

				_float2 vNewPivot{
					335.f + m_tUIDesc.fOffsetX - (vNewSize.x * 0.5f),
					vPivot.y + ((iLines * (SplitedTexts.size() / 2)) * (vNewSize.y))
				};

				m_pGameInstance->Render_Text(
					TEXT("KoPub"),
					SplitedText.c_str(),
					vNewPivot,
					vColor,
					0.8f
				);

				++iLines;
			}
		}
		else
		{
			m_pGameInstance->Render_Text(
				TEXT("KoPub"),
				text.c_str(),
				vPivot,
				vColor,
				0.8f
			);
		}
	}

	return S_OK;
}

void CUISongSelector::Update_SongIndex(_float fTimeDelta)
{
	if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_DOWN))
	{
		m_pGameInstance->Manager_PlaySound(TEXT("tamp.wav"), CHANNELID::EFFECT, 1.f, 1.f);
		m_iCurrentIndex = (m_iCurrentIndex + 1) % m_SongDescs.size();

		UI_EVENT_ARG_DESC Arg{};
		Arg.szActionTag = TEXT("PreListen");
		Arg.Type = UI_EVENT_ARG_DESC::WSTRING;
		Arg.pData = &m_SongDescs[m_iCurrentIndex].szSongKey;
		__super::Trigger_Event(TEXT("PreListen"), &Arg);
		
		m_fTargetScroll += m_fItemGap;
		Update_Thumbnail();
	}
	if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_UP))
	{
		m_pGameInstance->Manager_PlaySound(TEXT("tamp.wav"), CHANNELID::EFFECT, 1.f, 1.f);
		m_iCurrentIndex = (m_iCurrentIndex - 1 + m_SongDescs.size()) % m_SongDescs.size();
		
		UI_EVENT_ARG_DESC Arg{};
		Arg.szActionTag = TEXT("PreListen");
		Arg.Type = UI_EVENT_ARG_DESC::WSTRING;
		Arg.pData = &m_SongDescs[m_iCurrentIndex].szSongKey;
		__super::Trigger_Event(TEXT("PreListen"), &Arg);
		
		m_fTargetScroll -= m_fItemGap;
		Update_Thumbnail();
	}
	float delta = m_fTargetScroll - m_fScroll;

	float maxMove = 600.f * fTimeDelta;

	if (fabs(delta) <= maxMove)
		m_fScroll = m_fTargetScroll;
	else
		m_fScroll += (delta > 0 ? 1.f : -1.f) * maxMove;
}

void CUISongSelector::Update_Thumbnail()
{
	CUIHUD* pHUD = dynamic_cast<CUIHUD*>(m_pGameInstance->GetCurrentLevelHUD());
	CUIVideoThumbnail* pThumbnail = dynamic_cast<CUIVideoThumbnail*>(pHUD->Get_UIObject(m_tUIDesc.szLayerTag, TEXT("UI_VideoThumbnail")));

	if (pThumbnail)
	{
		pThumbnail->Set_Source(m_SongDescs[m_iCurrentIndex].szVideoTag);
		pThumbnail->Play();
	}

	Safe_Release(pHUD);
}

CUISongSelector* CUISongSelector::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUISongSelector* pInstance = new CUISongSelector(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUISongSelector::Clone(void* pArg)
{
	CUISongSelector* pInstance = new CUISongSelector(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUISongSelector");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUISongSelector::Free()
{
	__super::Free();

	Safe_Release(m_pArrowTextureCom);
	Safe_Release(m_pSelectedBGTextureCom);
	Safe_Release(m_pScoreTextureCom);
}
