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
	auto Desc = m_pOwner->Get_UIBase_Desc().Get_UI_Anim_Desc(szAnimTag);

	if (Desc == nullptr)
		return;

	//auto pTrackDesc = Desc->Get_UI_Track_Desc(TEXT("Alpha"));

	auto AnimDescs = Desc->Get_UI_Track_Descs();
	
	for (auto& pAnimDesc : AnimDescs)
	{
		//if (pAnimDesc.first == TEXT("Position"))
			Play_Anim(Desc, pAnimDesc.second);
		//if (pAnimDesc.first == TEXT("Alpha"))
			//Play_AlphaAnim(Desc, pAnimDesc.second);
		
	}

	//CUIBase::UI_ANIM_DESC AnimDesc = *static_cast<CUIBase::UI_ANIM_DESC*>(Desc);
	//CUIBase::UI_ANIM_TRACK_DESC TrackDesc = *static_cast<CUIBase::UI_ANIM_TRACK_DESC*>(pTrackDesc);
	//
	//m_fDeltaTime = m_pGameInstance->Get_TimeDelta(TEXT("Timer_60"));
	//
	////_vector vCurPos = XMVectorSet(m_pOwner->Get_UIBase_Desc().fOffsetX, m_pOwner->Get_UIBase_Desc().fOffsetY, 0.f, 1.f);
	//_vector vCurPos = XMVectorSet(m_pOwner->Get_UIBase_Desc().fAlpha, 0.f, 0.f, 1.f);
	//
	////_vector vCurPos = XMVectorSet(m_pOwner->Get_UIBase_Desc().fOffsetX, m_pOwner->Get_UIBase_Desc().fOffsetY, 0.f, 1.f);
	//_vector vEndPos = XMVectorSet(TrackDesc.vEndParam.x, 0.f, 0.f, 1.f);
	//
	//float fSpeed = 10.f;
	//
	//float t = 1.f - powf(1.f - 0.15f, m_fDeltaTime * fSpeed); // 부드러운 lerp 감쇠
	//
	//m_fTimeStack += m_fDeltaTime;
	//if (AnimDesc.fDuration >= m_fTimeStack)
	//{
	//	_float4 vChangedPos{};
	//	XMStoreFloat4(&vChangedPos, XMVectorLerp(vCurPos, vEndPos, t));
	//
	//	//m_pOwner->Set_Position(vChangedPos.x, vChangedPos.y);
	//	m_pOwner->Set_Alpha(vChangedPos.x);
	//	//pTransform->Set_State(STATE::POSITION, XMVectorLerp(vCurPos, vEndPos, t));
	//}
	//else
	//{
	//	if (AnimDesc.isLoop)
	//	{
	//		m_pOwner->Set_Position(m_pOwner->Get_UIBase_OriginDesc().fOffsetX, m_pOwner->Get_UIBase_OriginDesc().fOffsetY);
	//		m_fTimeStack = 0.f;
	//	}
	//}

	/*CUIBase::UI_ANIM_DESC AnimDesc = *static_cast<CUIBase::UI_ANIM_DESC*>(Desc);
	auto PosTrack = AnimDesc.Get_UI_Track_Desc(TEXT("Position"));

	if (PosTrack == nullptr)
		return;

	CUIBase::UI_ANIM_TRACK_DESC TrackDesc = *static_cast<CUIBase::UI_ANIM_TRACK_DESC*>(PosTrack);

	m_fDeltaTime = m_pGameInstance->Get_TimeDelta(TEXT("Timer_60"));*/

	//CUIBase::UI_ANIM_DESC Desc = *static_cast<CUIBase::UI_ANIM_DESC*>(pDesc);
	//CUIBase::UI_ANIM_DESC Desc{};

	//CTransform* pTransform = m_pOwner->GetTransform();

	//_vector vCurPos = pTransform->Get_State(STATE::POSITION);
	/*_vector vCurPos = XMVectorSet(m_pOwner->Get_UIBase_Desc().fOffsetX, m_pOwner->Get_UIBase_Desc().fOffsetY, 0.f, 1.f);
	_vector vEndPos = XMVectorSet(1000.f, m_pOwner->Get_UIBase_OriginDesc().fOffsetY, 0.f, 1.f);*/
}

void CUIAnimationCom::Pause()
{
	m_fDeltaTime = 0.f;
}

void CUIAnimationCom::Stop()
{
	m_fDeltaTime = m_pGameInstance->Get_TimeDelta(TEXT("GameLoopTime"));
	m_pOwner->Set_Position(m_pOwner->Get_UIBase_OriginDesc().fOffsetX, m_pOwner->Get_UIBase_OriginDesc().fOffsetY);
	m_pOwner->Set_Alpha(m_pOwner->Get_UIBase_OriginDesc().fAlpha);
	//m_pOwner->Set_UIBase_Desc(m_pOwner->Get_UIBase_OriginDesc());
	m_fTimeStack = 0.f;
}

void CUIAnimationCom::Tick(_float fTimeDelta)
{
}

void CUIAnimationCom::Play_Anim(void* pAnimDesc, void* pTrackDesc)
{
	CUIBase::UI_ANIM_DESC AnimDesc = *static_cast<CUIBase::UI_ANIM_DESC*>(pAnimDesc);
	CUIBase::UI_ANIM_TRACK_DESC TrackDesc = *static_cast<CUIBase::UI_ANIM_TRACK_DESC*>(pTrackDesc);

	m_fDeltaTime = m_pGameInstance->Get_TimeDelta(TEXT("GameLoopTime"));
	m_fTimeStack += m_fDeltaTime;
	
	_vector vStartParam{};
	if (TrackDesc.szTrackTag == TEXT("Position"))
		vStartParam = XMVectorSet(m_pOwner->Get_UIBase_Desc().fOffsetX, m_pOwner->Get_UIBase_Desc().fOffsetY, 0.f, 0.f);
	if (TrackDesc.szTrackTag == TEXT("Alpha"))
		vStartParam = XMVectorSet(m_pOwner->Get_UIBase_Desc().fAlpha, 0.f, 0.f, 0.f);

	_vector vEndParam{ XMLoadFloat4(&TrackDesc.vEndParam) };

	// 부드러운 lerp 감쇠
	//_float fSpeed = TrackDesc.fSpeed;
	//_float fSpeed = 10.f;
	//_float t = 1.f - powf(1.f - 0.15f, m_fDeltaTime * fSpeed);
	
	_float t = m_fTimeStack / AnimDesc.fDuration;

	_vector vLerped = XMVectorLerp(vStartParam, vEndParam, t);

	//if (AnimDesc.fDuration >= m_fTimeStack)
	if (t <= 1.f)
	{
		if (TrackDesc.szTrackTag == TEXT("Position"))
		{
			m_pOwner->Set_Position(XMVectorGetX(vLerped), XMVectorGetY(vLerped));
		}
		if (TrackDesc.szTrackTag == TEXT("Alpha"))
		{
			m_pOwner->Set_Alpha(XMVectorGetX(vLerped));
		}
	}
	else
	{
		t = 1.f;

		if (AnimDesc.isLoop)
		{
			if (TrackDesc.szTrackTag == TEXT("Position"))
				m_pOwner->Set_Position(m_pOwner->Get_UIBase_OriginDesc().fOffsetX, m_pOwner->Get_UIBase_OriginDesc().fOffsetY);
			if (TrackDesc.szTrackTag == TEXT("Alpha"))
				m_pOwner->Set_Alpha(m_pOwner->Get_UIBase_OriginDesc().fAlpha);
			m_fTimeStack = 0.f;
		}
		else
		{
			if (TrackDesc.szTrackTag == TEXT("Position"))
				m_pOwner->Set_Position(XMVectorGetX(vEndParam), XMVectorGetY(vEndParam));
			if (TrackDesc.szTrackTag == TEXT("Alpha"))
				m_pOwner->Set_Alpha(XMVectorGetX(vEndParam));
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

	//Safe_Release(m_pOwner);

	Safe_Release(m_pGameInstance);

	//for (auto& iter : m_TextureDescs)
	//	Safe_Release(iter.second.pTexture);
	//m_TextureDescs.clear();
}
