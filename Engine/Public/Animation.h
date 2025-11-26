#pragma once

#include "Base.h"

/* 이 애니메이션을 구동하기위해 갱신해야할 뼈(Channel)의 갯수, 뼈의 시간대별 상태값, 애니메이션의 전체 길이, 재생 속도  */

NS_BEGIN(Engine)

class CAnimation final : public CBase
{
private:
	CAnimation();
	CAnimation(const CAnimation& Prototype);
	virtual ~CAnimation() = default;

public:
	HRESULT Initialize(class CModel* pModel, binAnimation* pAnimation);
	_bool Update_TransformationMatrices(const vector<class CBone*>& Bones, _bool isLoop, _float fTimeDelta);
	_bool Update_TrackPosition(const vector<class CBone*>& Bones, _bool isLoop, _float fTimeDelta);
	_bool Update_CurrentKeyFrameIndices();

	_uint Get_AnimationKeyFrameIndex(_uint iIndex = 0) { return m_CurrentKeyFrameIndices[iIndex]; }
	_float Get_Duration() { return m_fDuration; }

	_uint Get_TrackPosition() { return (_uint)m_fCurrentTrackPosition; }
	_float	Get_fTrackPosition() { return m_fCurrentTrackPosition; }
	_float Get_SaturatedTrackPosition() {
		return max(m_fCurrentTrackPosition / m_fDuration, 0.f);
	}
	_float Get_TickPerSecond() { return m_fTickPerSecond; }

	_bool CompareAnimationTag(const _char* szAnimationTag)
	{
		if (0 == strcmp(szAnimationTag, m_szName))
			return TRUE;
		else
			return FALSE;
	}

	vector<class CChannel*>*	Get_vChannels() { return &m_Channels; }

	HRESULT Swap_AnimationChannel(_uint iSrc, _uint iDst);

	_char* Get_Name() { return m_szName; }

	void Set_CurrentTrackPosition(_float fPosition) {

		m_fCurrentTrackPosition = fPosition;

		for (auto& iKeyFrameIndex : m_CurrentKeyFrameIndices)
			iKeyFrameIndex = 0;
	}

	void Set_BoneToChannelMappingLists(const vector<_int>& vBoneMappinglists) { m_BoneToChannelMappingLists = vBoneMappinglists; }
	const vector<_int>& Get_BoneToChannelMappingLists() const { return m_BoneToChannelMappingLists; }

	// Mapping을 위해 사용하는 함수. 아 진짜 어쩔 수 없다 이건. 어차피 레카 해줘서 상관은 없을듯
	void Set_Channels(vector<CChannel*> Channels) { m_Channels = Channels; }

	void Reset();
private:
	_char						m_szName[MAX_PATH] = {};
	_float						m_fCurrentTrackPosition = {};
	/* 애니메이션 재생을 위한 전체 길이 */
	_float						m_fDuration = {};
	/* 초당 얼마나 재생되어야하는지 : 재생 속도 */
	_float						m_fTickPerSecond = {};

	_uint						m_iNumChannels = {};
	vector<class CChannel*>		m_Channels;
	vector<_uint>				m_CurrentKeyFrameIndices;
	vector<_int>				m_BoneToChannelMappingLists;

public:
	static CAnimation* Create(class CModel* pModel, binAnimation* pAnimation);
	CAnimation* Clone();
	virtual void Free() override;
};

NS_END

