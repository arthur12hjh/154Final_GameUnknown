#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class CMorphAnimation final : public CBase
{
private:
	CMorphAnimation();
	CMorphAnimation(const CMorphAnimation& Prototype);
	virtual ~CMorphAnimation() = default;

public:
	HRESULT Initialize(class CModel* pModel, binMorphAnimation* pMorphAnimation);
	_int Update_TrackPosition(class CModel* pModel, _bool isLoop, _float fTimeDelta);
	_bool Update_CurrentKeyFrameIndices();

	void Reset();

	_float Get_Duration() { return m_fDuration; }

	_uint Get_TrackPosition() { return (_uint)m_fCurrentTrackPosition; }
	_float	Get_fTrackPosition() { return m_fCurrentTrackPosition; }
	_float Get_SaturatedTrackPosition() {
		return max(m_fCurrentTrackPosition / m_fDuration, 0.f);
	}
	_float Get_TickPerSecond() { return m_fTickPerSecond; }

	_char* Get_Name() { return m_szName; }

	vector<class CMorphChannel*>* Get_MorphChannels() { return &m_MorphChannels; }

	void Set_Name(_char* szName) { strcpy_s(m_szName, szName); }
	void Set_CurrentTrackPosition(_float fCurrentTrackPosition) { m_fCurrentTrackPosition = fCurrentTrackPosition; }

private:
	_char								m_szName[MAX_PATH] = {};
	_float								m_fCurrentTrackPosition = {};
	/* 애니메이션 재생을 위한 전체 길이 */
	_float								m_fDuration = {};
	/* 초당 얼마나 재생되어야하는지 : 재생 속도 */
	_float								m_fTickPerSecond = {};

	_uint								m_iNumMorphChannels = {};
	vector<class CMorphChannel*>		m_MorphChannels;

public:
	static CMorphAnimation* Create(class CModel* pModel, binMorphAnimation* pMorphAnimation);
	CMorphAnimation* Clone();
	virtual void Free() override;
};

NS_END

