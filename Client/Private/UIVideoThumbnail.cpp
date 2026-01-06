#include "pch.h"
#include "UIVideoThumbnail.h"

#include "GameInstance.h"
#include "UIHUD.h"
#include "VideoPlayer.h"

CUIVideoThumbnail::CUIVideoThumbnail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIBase{ pDevice, pContext }
{
}

CUIVideoThumbnail::CUIVideoThumbnail(const CUIVideoThumbnail& Prototype) 
	: CUIBase{ Prototype }
{
}

HRESULT CUIVideoThumbnail::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUIVideoThumbnail::Initialize(void* pArg)
{	
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CUIVideoThumbnail::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CUIVideoThumbnail::Update(_float fTimeDelta)
{
 	__super::Update(fTimeDelta);

	if(m_pVideoPlayerCom->isPlaying())
		m_pVideoPlayerCom->Update(fTimeDelta);
}

void CUIVideoThumbnail::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
}

HRESULT CUIVideoThumbnail::Render()
{
	__super::Render();

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(UI_SHADER_PASS::VIDEO))))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Resources()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

#ifdef _DEBUG
	__super::Render_Debug_Rect();
#endif

	return S_OK;
}

void CUIVideoThumbnail::Play() { m_pVideoPlayerCom->Play(); }
void CUIVideoThumbnail::Pause() { m_pVideoPlayerCom->Pause(); }
void CUIVideoThumbnail::Stop() { m_pVideoPlayerCom->Stop(); }
void CUIVideoThumbnail::Set_Source(const _tchar* szPath)
{
	_tchar szFullPath[256]{};
	_stprintf_s(szFullPath, TEXT("../../Client/Bin/Resources/Video/%s.mp4"), szPath);

	m_pVideoPlayerCom->Set_Source(szFullPath);
}

HRESULT CUIVideoThumbnail::Ready_Components()
{
	__super::Ready_Components();

	/* Com_VIBuffer */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* Com_VideoPlayer */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::BEATSABER_GAME), TEXT("Prototype_Component_VideoPlayer"),
		TEXT("Com_VideoPlayer"), reinterpret_cast<CComponent**>(&m_pVideoPlayerCom))))
		return E_FAIL;

	//m_pVideoPlayerCom->Set_Source(L"../../Client/Bin/Resources/Video/DoroDoro.mp4");
	//m_pVideoPlayerCom->Play();

	return S_OK;
}

HRESULT CUIVideoThumbnail::Bind_ShaderResources()
{
	__super::Bind_ShaderResources();

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->GetIdentityMatrixPtr())))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_SRV("g_YTex", m_pVideoPlayerCom->Get_SRV_Y())))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_SRV("g_UVTex", m_pVideoPlayerCom->Get_SRV_UV())))
		return E_FAIL;

	return S_OK;
}

HRESULT CUIVideoThumbnail::Execute(const UI_EVENT_DESC& EventDesc)
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

void CUIVideoThumbnail::CallbackEvent(void* pArg)
{
	auto* arg = static_cast<UI_EVENT_ARG_DESC*>(pArg);
	if (!arg) return;
	
	if (arg->szActionTag == TEXT("Set_Texture_Index"))
	{
		_uint iTextureIndex = *static_cast<_uint*>(arg->pData);
		Set_Texture_Index(iTextureIndex);
	}
	else
	{
		CUIHUD* pHUD = dynamic_cast<CUIHUD*>(m_pGameInstance->GetCurrentLevelHUD());
	
		auto AnimTag = m_tUIDesc.m_AnimTags.find(arg->szActionTag);
		if (AnimTag != m_tUIDesc.m_AnimTags.end())
			pHUD->Anim_Play(m_tUIDesc.szLayerTag, m_tUIDesc.szUITag, AnimTag->first);
	
		Safe_Release(pHUD);
	}
}

CUIVideoThumbnail* CUIVideoThumbnail::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUIVideoThumbnail* pInstance = new CUIVideoThumbnail(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUIVideoThumbnail::Clone(void* pArg)
{
	CUIVideoThumbnail* pInstance = new CUIVideoThumbnail(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUIVideoThumbnail");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUIVideoThumbnail::Free()
{
	__super::Free();

	Safe_Release(m_pVideoPlayerCom);

	/*for (auto& Event : m_tUIDesc.m_Events)
	{
		for (auto& EventDesc : Event.second)
		{
			for (auto& SubEvent : EventDesc.szSubscribeEventTags)
			{
				m_pGameInstance->UnBind_Observer(SubEvent.c_str(), m_pEventHandle);
			}
		}
	}*/

	//Safe_Release(m_pEventHandle);
}
