#include "pch.h"
#include "UIGetterQueue.h"

#include "GameInstance.h"
#include "UIHUD.h"

CUIGetterQueue::CUIGetterQueue(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIBase{ pDevice, pContext }
{
}

CUIGetterQueue::CUIGetterQueue(const CUIGetterQueue& Prototype) 
	: CUIBase{ Prototype }
{
}

HRESULT CUIGetterQueue::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUIGetterQueue::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;
	
	m_QueueItems.reserve(7);

	for (_int i = 0; i < 7; ++i)
	{
		QUEUE_DESC Desc;
		Desc.iIdx = i;

		m_QueueItems.push_back(Desc);
	}

	return S_OK;
}

void CUIGetterQueue::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CUIGetterQueue::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

	m_fTimeAcc += fTimeDelta;

	_int iCnt = 0;
	for (auto& iter : m_QueueItems)
		if (!iter.bShow)
			++iCnt;
		
	if (iCnt >= 7)
	{
		m_fTimeAcc = 0.f;
		m_iCount = 0;
	}

	/*if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_Q))
	{
		if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_8))
			Insert_Queue(TEXT("+ ") + to_wstring(m_fTimeAcc));
		if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_9))
			Insert_Queue(TEXT("+ ") + to_wstring(m_fTimeAcc));
		if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_0))
			Insert_Queue(TEXT("+ ") + to_wstring(m_fTimeAcc));
	}*/
	
	Pop_Queue(fTimeDelta);
}

void CUIGetterQueue::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
}

HRESULT CUIGetterQueue::Render()
{
	__super::Render();

	for (size_t i = 0; i < m_QueueItems.size(); ++i)
		Render_Queue_Item(m_QueueItems[i]);

#ifdef _DEBUG
	__super::Render_Debug_Rect();
#endif

	return S_OK;
}

void CUIGetterQueue::Insert_Queue(_wstring szText)
{
	QUEUE_DESC Desc;
	Desc.fStart = m_fTimeAcc;
	Desc.szText = TEXT("+ ") + szText;
	Desc.fAlpha = 0.f;
	Desc.fTime = 0.f;
	Desc.fOffsetY = 0.f;
	Desc.iIdx = m_iCount;
	Desc.bShow = true;

	m_QueueItems[m_iCount] = Desc;

	if (m_iCount + 1 < 7)
		++m_iCount;
	else
		m_iCount = 0;
}

void CUIGetterQueue::Pop_Queue(_float fTimeDelta)
{
	_uint iSize = 0;

	for (auto& item : m_QueueItems)
	{
		item.fTime += fTimeDelta;
		if (item.fTime >= 1.f)
		{
			item.fAlpha -= fTimeDelta * 5.f;
			item.fOffsetY -= fTimeDelta * 5.f;
		}
		if(item.fTime <= 0.3f)
		{
			item.fAlpha += fTimeDelta * 4.f;
			item.fOffsetY -= fTimeDelta * 7.f;
		}

		if (item.fTime >= 0.f && item.fAlpha <= 0.f)
			item.bShow = false;
	}
}

HRESULT CUIGetterQueue::Ready_Components()
{
	__super::Ready_Components();

	return S_OK;
}

HRESULT CUIGetterQueue::Bind_ShaderResources()
{
	return S_OK;
}

HRESULT CUIGetterQueue::Execute(const UI_EVENT_DESC& EventDesc)
{
	return S_OK;
}

void CUIGetterQueue::CallbackEvent(void* pArg)
{
}

void CUIGetterQueue::Render_Queue_Item(QUEUE_DESC Desc)
{
	_vector vColor = XMVectorSet(
		1.f * Desc.fAlpha,
		1.f * Desc.fAlpha,
		1.f * Desc.fAlpha,
		1.f * Desc.fAlpha
	);
	_vector vShadowColor = XMVectorSet(
		0.f * Desc.fAlpha,
		0.f * Desc.fAlpha,
		0.f * Desc.fAlpha,
		1.f * Desc.fAlpha
	);

	_float2 vPivot{
		m_tUIDesc.fX + m_tUIDesc.fOffsetX - (m_tUIDesc.fSizeX * 0.5f),
		m_tUIDesc.fY + m_tUIDesc.fOffsetY - (m_tUIDesc.fSizeY * 0.6f) - (Desc.iIdx * 20.f) - Desc.fOffsetY
	};

	Render_MixedText(Desc.szText.c_str(),
		_float2(vPivot.x + 1.f, vPivot.y + 1.f),
		vShadowColor, 0.8f);
	Render_MixedText(Desc.szText.c_str(),
		_float2(vPivot.x, vPivot.y),
		vColor, 0.8f);
}

_int CUIGetterQueue::GetCharType(WCHAR c)
{
	if (c >= 0xAC00 && c <= 0xD7A3) return 0; // 한글
	if ((c >= L'A' && c <= L'Z') || (c >= L'a' && c <= L'z')) return 1; // 영어
	if (c >= L'0' && c <= L'9') return 2; // 숫자
	return 3; // 특수문자
}

void CUIGetterQueue::Render_MixedText(const wstring& text, const _float2& pos, const _vector& color, _float fScale)
{
	_float2 cursor = pos;
	_wstring runText;
	_int prevType = -1;

	for (size_t i = 0; i <= text.length(); ++i)
	{
		int type = (i < text.length()) ? GetCharType(text[i]) : -1;

		// 타입이 변경되면 run 끊기
		if (type != prevType && !runText.empty())
		{
			const wchar_t* fontName = nullptr;
			_float2 adjustPos = cursor;
			_float adjustScale = fScale;

			if (prevType == 0)
			{
				fontName = L"KoPub";      // 한글
				adjustPos.y -= 1.75f;
				adjustScale *= 0.6f;
			}
			else               fontName = L"Iceberg";   // 영어+숫자+특수문자

			// 렌더링
			m_pGameInstance->Render_Text(fontName, runText.c_str(), adjustPos, color, adjustScale);

			// 커서 이동
			//cursor.x += Measure_TextWidth(fontName, runText.c_str());
			cursor.x += m_pGameInstance->Get_Text_Size(fontName, runText.c_str(), true, adjustScale).x;

			runText.clear();
		}

		if (i < text.length())
			runText += text[i];

		prevType = type;
	}
}

CUIGetterQueue* CUIGetterQueue::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUIGetterQueue* pInstance = new CUIGetterQueue(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUIGetterQueue::Clone(void* pArg)
{
	CUIGetterQueue* pInstance = new CUIGetterQueue(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUIGetterQueue");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUIGetterQueue::Free()
{
	__super::Free();
}
