#include "pch.h"
#include "UIAnimInstance.h"

#include "UIBase.h"

CUIAnimInstance::CUIAnimInstance()
{
}

HRESULT CUIAnimInstance::Initialize(CUIBase* pUI, void* Desc, _float fDelay)
{
	m_pTargetUI = pUI;
	m_pTargetUI->Set_Anim_Playing(true);
	m_tUIAnimDesc = *static_cast<UI_ANIM_DESC*>(Desc);
	m_pTargetUI->SetAnimFinish(m_tUIAnimDesc.szAnimTag, false);
	m_fDelayTime = fDelay;

	return S_OK;
}

_bool CUIAnimInstance::Update(_float fTimeDelta)
{
	m_AnimFinishs.clear();

	for (auto& pTrackDesc : m_tUIAnimDesc.m_Tracks)
	{
		m_AnimFinishs.push_back(Play_Anim(&m_tUIAnimDesc, &pTrackDesc.second, fTimeDelta));
	}

	for (auto pAnimFinish : m_AnimFinishs)
	{
		if (pAnimFinish == false)
		{
			m_pTargetUI->SetAnimFinish(m_tUIAnimDesc.szAnimTag, false);
			return false;
		}
	}

	m_pTargetUI->SetAnimFinish(m_tUIAnimDesc.szAnimTag, true);

	return true;
}

_bool CUIAnimInstance::Play_Anim(UI_ANIM_DESC* pAnimDesc, UI_ANIM_TRACK_DESC* pTrackDesc, _float fTimeDelta)
{
	UI_ANIM_DESC AnimDesc = *pAnimDesc;
	UI_ANIM_TRACK_DESC TrackDesc = *pTrackDesc;

	if (m_fDelayTime > 0.f)
		m_fDelayTime -= fTimeDelta;
	else
		m_fTimeStack += fTimeDelta;
	
	_vector vStartParam{ XMLoadFloat4((m_isReverse ? &TrackDesc.vEndParam : &TrackDesc.vStartParam)) };

	_vector vEndParam{ XMLoadFloat4((m_isReverse ? &TrackDesc.vStartParam : &TrackDesc.vEndParam)) };

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
		if (TrackDesc.szTrackTag == TEXT("Text_Color"))
		{
			_float4 vColor{};
			XMStoreFloat4(&vColor, vLerped);

			m_pTargetUI->Set_Text_Color(vColor);
		}
		if (TrackDesc.szTrackTag == TEXT("Texture_UV"))
		{
			_float4 vUV{};
			XMStoreFloat4(&vUV, vLerped);

			m_pTargetUI->Set_TextureUV(vUV);
		}
		if (TrackDesc.szTrackTag == TEXT("FillClip"))
			m_pTargetUI->Set_FillAmount(XMVectorGetX(vLerped));
		if (TrackDesc.szTrackTag == TEXT("TintColor"))
		{
			_float4 vColor{};
			XMStoreFloat4(&vColor, vLerped);

			m_pTargetUI->Set_TintColor(vColor);
		}
		if (TrackDesc.szTrackTag == TEXT("SpriteAction"))
			m_pTargetUI->Set_Texture_Index(static_cast<_uint>(XMVectorGetX(vLerped)));
		if (TrackDesc.szTrackTag == TEXT("GlowIntensity"))
			m_pTargetUI->Set_GlowIntensity(XMVectorGetX(vLerped));
		if (TrackDesc.szTrackTag == TEXT("Rotation"))
			m_pTargetUI->Set_Rotation(XMVectorGetX(vLerped));
		if (TrackDesc.szTrackTag == TEXT("Scale"))
			m_pTargetUI->Set_Texture_Scale(XMVectorGetX(vLerped));
	}
	else
	{
		t = 1.f;

		if (AnimDesc.isBeapBeap)
		{
			++m_iRepeatCount;
			m_isReverse = !m_isReverse;
			m_fTimeStack = 0.f;

			if (AnimDesc.isLoop)
				m_iRepeatCount = 0;

			if (m_iRepeatCount >= 2 && !AnimDesc.isLoop)
			{
				Stop_Anim();
				m_pTargetUI->Set_Anim_Playing(false);

				return true;
			}

			return false;
		}

		if (AnimDesc.isLoop)
		{
			if (TrackDesc.szTrackTag == TEXT("Position"))
				m_pTargetUI->Set_Position(m_pTargetUI->Get_UIBase_OriginDesc().fOffsetX, m_pTargetUI->Get_UIBase_OriginDesc().fOffsetY);
			if (TrackDesc.szTrackTag == TEXT("Size"))
				m_pTargetUI->Set_Size(m_pTargetUI->Get_UIBase_OriginDesc().fSizeX, m_pTargetUI->Get_UIBase_OriginDesc().fSizeY);
			if (TrackDesc.szTrackTag == TEXT("Alpha"))
				m_pTargetUI->Set_Alpha(m_pTargetUI->Get_UIBase_OriginDesc().fAlpha);
			if (TrackDesc.szTrackTag == TEXT("Text_Color"))
				m_pTargetUI->Set_Text_Color(m_pTargetUI->Get_UIBase_OriginDesc().m_tUITextDesc.vColor);
			if (TrackDesc.szTrackTag == TEXT("Texture_UV"))
			{
				_float4 vOriginUV{
					m_pTargetUI->Get_UIBase_OriginDesc().m_tUIShaderDesc.fUVScaleX,
					m_pTargetUI->Get_UIBase_OriginDesc().m_tUIShaderDesc.fUVScaleY,
					m_pTargetUI->Get_UIBase_OriginDesc().m_tUIShaderDesc.fUVOffsetX,
					m_pTargetUI->Get_UIBase_OriginDesc().m_tUIShaderDesc.fUVOffsetY
				};

				m_pTargetUI->Set_TextureUV(vOriginUV);
			}
			if (TrackDesc.szTrackTag == TEXT("FillClip"))
				m_pTargetUI->Set_FillAmount(m_pTargetUI->Get_UIBase_OriginDesc().m_tUIShaderDesc.fFillAmount);
			if (TrackDesc.szTrackTag == TEXT("TintColor"))
				m_pTargetUI->Set_TintColor(m_pTargetUI->Get_UIBase_OriginDesc().m_tUIShaderDesc.vTintColor);
			if (TrackDesc.szTrackTag == TEXT("SpriteAction"))
				m_pTargetUI->Set_Texture_Index(static_cast<_uint>(m_pTargetUI->Get_UIBase_OriginDesc().m_tUITextureDesc.iTextureIndex));
			if (TrackDesc.szTrackTag == TEXT("GlowIntensity"))
				m_pTargetUI->Set_GlowIntensity(m_pTargetUI->Get_UIBase_OriginDesc().m_tUIShaderDesc.fGlowIntensity);
			if (TrackDesc.szTrackTag == TEXT("Rotation"))
				m_pTargetUI->Set_Rotation(m_pTargetUI->Get_UIBase_OriginDesc().fRotation);
			if (TrackDesc.szTrackTag == TEXT("Scale"))
				m_pTargetUI->Set_Texture_Scale(m_pTargetUI->Get_UIBase_OriginDesc().m_tUIShaderDesc.fScale);

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
			if (TrackDesc.szTrackTag == TEXT("Text_Color"))
			{
				_float4 vColor{};
				XMStoreFloat4(&vColor, vEndParam);

				m_pTargetUI->Set_Text_Color(vColor);
			}
			if (TrackDesc.szTrackTag == TEXT("Texture_UV"))
			{
				_float4 vUV{};
				XMStoreFloat4(&vUV, vEndParam);

				m_pTargetUI->Set_TextureUV(vUV);
			}
			if (TrackDesc.szTrackTag == TEXT("FillClip"))
				m_pTargetUI->Set_FillAmount(XMVectorGetX(vEndParam));
			if (TrackDesc.szTrackTag == TEXT("TintColor"))
			{
				_float4 vColor{};
				XMStoreFloat4(&vColor, vEndParam);

				m_pTargetUI->Set_TintColor(vColor);
			}
			if (TrackDesc.szTrackTag == TEXT("GlowIntensity"))
				m_pTargetUI->Set_GlowIntensity(XMVectorGetX(vEndParam));
			if (TrackDesc.szTrackTag == TEXT("SpriteAction"))
				m_pTargetUI->Set_Texture_Index(static_cast<_uint>(XMVectorGetX(vEndParam)));
			if (TrackDesc.szTrackTag == TEXT("Rotation"))
				m_pTargetUI->Set_Rotation(XMVectorGetX(vEndParam));
			if (TrackDesc.szTrackTag == TEXT("Scale"))
				m_pTargetUI->Set_Texture_Scale(XMVectorGetX(vEndParam));

			m_pTargetUI->Set_Anim_Playing(false);

			return true;
		}
	}

	return false;
}

void CUIAnimInstance::Stop_Anim()
{
	/*m_pTargetUI->Set_Position(m_pTargetUI->Get_UIBase_OriginDesc().fOffsetX, m_pTargetUI->Get_UIBase_OriginDesc().fOffsetY);
	m_pTargetUI->Set_Size(m_pTargetUI->Get_UIBase_OriginDesc().fSizeX, m_pTargetUI->Get_UIBase_OriginDesc().fSizeY);
	m_pTargetUI->Set_Alpha(m_pTargetUI->Get_UIBase_OriginDesc().fAlpha);
	if (m_pTargetUI->Get_UIBase_Desc().Get_UI_Text_Desc())
		m_pTargetUI->Set_Text_Color(m_pTargetUI->Get_UIBase_OriginDesc().m_tUITextDesc.vColor);
	if (m_pTargetUI->Get_UIBase_Desc().Get_ShaderDesc())
		m_pTargetUI->Get_UIBase_Desc().m_tUIShaderDesc = m_pTargetUI->Get_UIBase_OriginDesc().m_tUIShaderDesc;*/

	m_pTargetUI->Get_UIBase_Desc() = m_pTargetUI->Get_UIBase_OriginDesc();

	m_fTimeStack = 0.f;
}

CUIAnimInstance* CUIAnimInstance::Create(CUIBase* pUI, void* Desc, _float fDelay)
{
	CUIAnimInstance* pInstance = new CUIAnimInstance();

	if (FAILED(pInstance->Initialize(pUI, Desc, fDelay)))
	{
		MSG_BOX("Failed to Created : CUIAnimInstance");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUIAnimInstance::Free()
{
	__super::Free();

	//Safe_Release(m_pTargetUI);

	/*for (auto& TrackDesc : m_tUIAnimDesc.m_Tracks)
		Safe_Delete(TrackDesc.second);
	m_tUIAnimDesc.m_Tracks.clear();*/
}
