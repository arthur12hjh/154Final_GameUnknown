#include "pch.h"
#include "UIAnimInstance.h"

#include "GameInstance.h"
#include "UIBase.h"

CUIAnimInstance::CUIAnimInstance()
{
}

HRESULT CUIAnimInstance::Initialize(CUIBase* pUI, void* Desc)
{
	m_pGameInstance = CGameInstance::GetInstance();

	if (!m_pGameInstance)
		return E_FAIL;

	m_pTargetUI = pUI;
	m_tUIAnimDesc = *static_cast<CUIAnimInstance::UI_ANIM_DESC*>(Desc);

	m_fDeltaTime = m_pGameInstance->Get_TimeDelta(TEXT("GameLoopTime"));

	return S_OK;
}

_bool CUIAnimInstance::Update(_float fTimeDelta)
{
	auto TrackDescs = m_tUIAnimDesc.Get_UI_Track_Descs();

	for (auto& pTrackDesc : TrackDescs)
	{
		return Play_Anim(&m_tUIAnimDesc, pTrackDesc.second, fTimeDelta);
	}

	return true;
}

//void CUIAnimInstance::Play(_wstring szAnimTag)
//{
//	/*if (!m_pOwner)
//		return;*/
//
//	auto TrackDescs = m_tUIAnimDesc.Get_UI_Track_Descs();
//	
//	for (auto& pTrackDesc : TrackDescs)
//	{
//		//Play_Anim(&m_tUIAnimDesc, pTrackDesc.second);
//	}
//}
//
//void CUIAnimInstance::Pause()
//{
//	m_fDeltaTime = 0.f;
//}
//
//void CUIAnimInstance::Stop()
//{
//	m_fDeltaTime = m_pGameInstance->Get_TimeDelta(TEXT("GameLoopTime"));
//	m_pOwner->Set_Position(m_pOwner->Get_UIBase_OriginDesc().fOffsetX, m_pOwner->Get_UIBase_OriginDesc().fOffsetY);
//	m_pOwner->Set_Size(m_pOwner->Get_UIBase_OriginDesc().fSizeX, m_pOwner->Get_UIBase_OriginDesc().fSizeY);
//	m_pOwner->Set_Alpha(m_pOwner->Get_UIBase_OriginDesc().fAlpha);
//	m_fTimeStack = 0.f;
//}
//
//void CUIAnimInstance::Tick(_float fTimeDelta)
//{
//}
//
_bool CUIAnimInstance::Play_Anim(UI_ANIM_DESC* pAnimDesc, UI_ANIM_TRACK_DESC* pTrackDesc, _float fTimeDelta)
{
	//m_pTargetUI->Set_Anim_State(CUIBase::ANIM_STATE::PLAY);

	m_pTargetUI->Set_Follow_Parent(m_tUIAnimDesc.isInfluenceChildren);

	CUIAnimInstance::UI_ANIM_DESC AnimDesc = *pAnimDesc;
	CUIAnimInstance::UI_ANIM_TRACK_DESC TrackDesc = *pTrackDesc;

	m_fDeltaTime = m_pGameInstance->Get_TimeDelta(TEXT("GameLoopTime"));
	m_fTimeStack += fTimeDelta;
	
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
			m_pTargetUI->Set_Position(XMVectorGetX(vLerped), XMVectorGetY(vLerped));
		if (TrackDesc.szTrackTag == TEXT("Size"))
			m_pTargetUI->Set_Size(XMVectorGetX(vLerped), XMVectorGetY(vLerped));
		if (TrackDesc.szTrackTag == TEXT("Alpha"))
			m_pTargetUI->Set_Alpha(XMVectorGetX(vLerped));
	}
	else
	{
		t = 1.f;
		if (AnimDesc.isLoop)
		{
			if (TrackDesc.szTrackTag == TEXT("Position"))
				m_pTargetUI->Set_Position(m_pTargetUI->Get_UIBase_OriginDesc().fOffsetX, m_pTargetUI->Get_UIBase_OriginDesc().fOffsetY);
			if (TrackDesc.szTrackTag == TEXT("Size"))
				m_pTargetUI->Set_Size(m_pTargetUI->Get_UIBase_OriginDesc().fSizeX, m_pTargetUI->Get_UIBase_OriginDesc().fSizeY);
			if (TrackDesc.szTrackTag == TEXT("Alpha"))
				m_pTargetUI->Set_Alpha(m_pTargetUI->Get_UIBase_OriginDesc().fAlpha);
			m_fTimeStack = 0.f;
		}
		else
		{
			if (TrackDesc.szTrackTag == TEXT("Position"))
				m_pTargetUI->Set_Position(XMVectorGetX(vEndParam), XMVectorGetY(vEndParam));
			if (TrackDesc.szTrackTag == TEXT("Size"))
				m_pTargetUI->Set_Size(XMVectorGetX(vEndParam), XMVectorGetY(vEndParam));
			if (TrackDesc.szTrackTag == TEXT("Alpha"))
				m_pTargetUI->Set_Alpha(XMVectorGetX(vEndParam));

			//m_pTargetUI->Set_Anim_State(CUIBase::ANIM_STATE::IDLE);
			m_pTargetUI->Set_Follow_Parent(!m_tUIAnimDesc.isInfluenceChildren);
			return true;
		}
	}

	return false;
}

CUIAnimInstance* CUIAnimInstance::Create(CUIBase* pUI, void* Desc)
{
	CUIAnimInstance* pInstance = new CUIAnimInstance();

	if (FAILED(pInstance->Initialize(pUI, Desc)))
	{
		MSG_BOX("Failed to Created : CUIAnimInstance");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUIAnimInstance::Free()
{
	__super::Free();

	/*for (auto& TrackDesc : m_tUIAnimDesc.m_Tracks)
	{
		Safe_Delete(TrackDesc.second);
	}
	m_tUIAnimDesc.m_Tracks.clear();*/
}
