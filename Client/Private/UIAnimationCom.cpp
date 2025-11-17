#include "pch.h"
#include "UIAnimationCom.h"

#include "GameInstance.h"
#include "UIBase.h"

CUIAnimationCom::CUIAnimationCom()
{
}

HRESULT CUIAnimationCom::Initialize()
{
	m_pGameInstance = CGameInstance::GetInstance();

	if (!m_pGameInstance)
		return E_FAIL;

	m_fDeltaTime = m_pGameInstance->Get_TimeDelta(TEXT("GameLoopTime"));

	return S_OK;
}

void CUIAnimationCom::Play(_wstring szAnimTag)
{
	if (!m_pOwner)
		return;

	auto TrackDescs = m_tUIAnimDesc.Get_UI_Track_Descs();
	
	for (auto& pTrackDesc : TrackDescs)
	{
		Play_Anim(&m_tUIAnimDesc, pTrackDesc.second);
	}
}

void CUIAnimationCom::Pause()
{
	m_fDeltaTime = 0.f;
}

void CUIAnimationCom::Stop()
{
	m_fDeltaTime = m_pGameInstance->Get_TimeDelta(TEXT("GameLoopTime"));
	m_pOwner->Set_Position(m_pOwner->Get_UIBase_OriginDesc().fOffsetX, m_pOwner->Get_UIBase_OriginDesc().fOffsetY);
	m_pOwner->Set_Size(m_pOwner->Get_UIBase_OriginDesc().fSizeX, m_pOwner->Get_UIBase_OriginDesc().fSizeY);
	m_pOwner->Set_Alpha(m_pOwner->Get_UIBase_OriginDesc().fAlpha);
	m_fTimeStack = 0.f;
}

void CUIAnimationCom::Tick(_float fTimeDelta)
{
}

void CUIAnimationCom::Play_Anim(void* pAnimDesc, void* pTrackDesc)
{
	CUIAnimationCom::UI_ANIM_DESC AnimDesc = *static_cast<CUIAnimationCom::UI_ANIM_DESC*>(pAnimDesc);
	CUIAnimationCom::UI_ANIM_TRACK_DESC TrackDesc = *static_cast<CUIAnimationCom::UI_ANIM_TRACK_DESC*>(pTrackDesc);

	m_fDeltaTime = m_pGameInstance->Get_TimeDelta(TEXT("GameLoopTime"));
	m_fTimeStack += m_fDeltaTime;
	
	_vector vStartParam{ XMLoadFloat4(&TrackDesc.vStartParam) };

	_vector vEndParam{ XMLoadFloat4(&TrackDesc.vEndParam) };

	// 부드러운 lerp 감쇠
	//_float fSpeed = TrackDesc.fSpeed;
	//_float fSpeed = 10.f;
	//_float t = 1.f - powf(1.f - 0.15f, m_fDeltaTime * fSpeed);
	//if (AnimDesc.fDuration >= m_fTimeStack)
	
	_float t = m_fTimeStack / AnimDesc.fDuration;

	if (t <= 1.f)
	{
		_vector vLerped = XMVectorLerp(vStartParam, vEndParam, t);

		if (TrackDesc.szTrackTag == TEXT("Position"))
			m_pOwner->Set_Position(XMVectorGetX(vLerped), XMVectorGetY(vLerped));
		if (TrackDesc.szTrackTag == TEXT("Size"))
			m_pOwner->Set_Size(XMVectorGetX(vLerped), XMVectorGetY(vLerped));
		if (TrackDesc.szTrackTag == TEXT("Alpha"))
			m_pOwner->Set_Alpha(XMVectorGetX(vLerped));
	}
	else
	{
		t = 1.f;
		if (AnimDesc.isLoop)
		{
			if (TrackDesc.szTrackTag == TEXT("Position"))
				m_pOwner->Set_Position(m_pOwner->Get_UIBase_OriginDesc().fOffsetX, m_pOwner->Get_UIBase_OriginDesc().fOffsetY);
			if (TrackDesc.szTrackTag == TEXT("Size"))
				m_pOwner->Set_Size(m_pOwner->Get_UIBase_OriginDesc().fSizeX, m_pOwner->Get_UIBase_OriginDesc().fSizeY);
			if (TrackDesc.szTrackTag == TEXT("Alpha"))
				m_pOwner->Set_Alpha(m_pOwner->Get_UIBase_OriginDesc().fAlpha);
			m_fTimeStack = 0.f;
		}
		else
		{
			if (TrackDesc.szTrackTag == TEXT("Position"))
				m_pOwner->Set_Position(XMVectorGetX(vEndParam), XMVectorGetY(vEndParam));
			if (TrackDesc.szTrackTag == TEXT("Size"))
				m_pOwner->Set_Size(XMVectorGetX(vEndParam), XMVectorGetY(vEndParam));
			if (TrackDesc.szTrackTag == TEXT("Alpha"))
				m_pOwner->Set_Alpha(XMVectorGetX(vEndParam));

			m_pOwner->Set_Anim_State(CUIBase::ANIM_STATE::IDLE);
		}
	}
}

CUIAnimationCom* CUIAnimationCom::Create()
{
	CUIAnimationCom* pInstance = new CUIAnimationCom();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CUIAnimationCom");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUIAnimationCom::Free()
{
	__super::Free();

	for (auto& TrackDesc : m_tUIAnimDesc.m_Tracks)
	{
		Safe_Delete(TrackDesc.second);
	}
	m_tUIAnimDesc.m_Tracks.clear();
}
