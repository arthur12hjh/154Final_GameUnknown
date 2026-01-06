#include "MorphAnimation.h"
#include "MorphChannel.h"

CMorphAnimation::CMorphAnimation()
{
	/*XMMatrixDecompose();*/
}

CMorphAnimation::CMorphAnimation(const CMorphAnimation& Prototype)
	: m_fCurrentTrackPosition{ Prototype.m_fCurrentTrackPosition }
	, m_fDuration{ Prototype.m_fDuration }
	, m_fTickPerSecond{ Prototype.m_fTickPerSecond }
	, m_iNumMorphChannels{ Prototype.m_iNumMorphChannels }
	, m_MorphChannels{ Prototype.m_MorphChannels }
{
	for (auto& pChannel : m_MorphChannels)
		Safe_AddRef(pChannel);

	strcpy_s(m_szName, Prototype.m_szName);
}

HRESULT CMorphAnimation::Initialize(class CModel* pModel, binMorphAnimation* pMorphAnimation)
{
	m_fDuration = pMorphAnimation->fDuration;
	m_fTickPerSecond = pMorphAnimation->fTicksPerSecond;

	m_iNumMorphChannels = pMorphAnimation->iNumMorphChannels;


	m_CurrentKeyFrameIndices.resize(m_iNumMorphChannels);

	strcpy_s(m_szName, pMorphAnimation->szName);

	for (size_t i = 0; i < m_iNumMorphChannels; i++)
	{
		CMorphChannel* pChannel = CMorphChannel::Create(pModel, &pMorphAnimation->vMorphChannels[i]);
		if (nullptr == pChannel)
			return E_FAIL;

		m_MorphChannels.push_back(pChannel);
	}

	return S_OK;
}

_int CMorphAnimation::Update_TrackPosition(const vector<class CShapeKey*>& ShapeKeys, _bool isLoop, _float fTimeDelta, _float fEndTrackPosition)
{
	/* 내 애니메이션의 현재 재생위치. */
	m_fCurrentTrackPosition += m_fTickPerSecond * fTimeDelta;
	// m_fCurrentTrackPosition
	// 
	if (m_fCurrentTrackPosition >= m_fDuration
		|| (fEndTrackPosition != -1.f && m_fCurrentTrackPosition >= fEndTrackPosition))
	{
		if (true == isLoop)
		{
			m_fCurrentTrackPosition = m_fCurrentTrackPosition - m_fDuration;
			for (auto& iKeyFrameIndex : m_CurrentKeyFrameIndices)
				iKeyFrameIndex = 0;

			return iFLAG_ANIMATION_RESET;
		}
		else
		{
			m_fCurrentTrackPosition -= m_fTickPerSecond * fTimeDelta; // 마지막 프레임 고정
			return iFLAG_ANIMATION_FINISH;
		}
	}

	return iFLAG_ANIMATION_PLAY;
}

_bool CMorphAnimation::Update_CurrentKeyFrameIndices()
{
	// 채널을 돌며, 각 채널별 키프레임 인덱스를 갱신해준다.
	for (size_t i = 0; i < m_MorphChannels.size(); i++)
	{
		CMorphChannel* pChannel = m_MorphChannels[i];
		_uint& iKeyFrameIndex = m_CurrentKeyFrameIndices[i];

		auto& Frames = pChannel->Get_KeyFrames();
		if (Frames.size() <= 1)
			continue;

		//// 현재 구간 찾기
		//while (iKeyFrameIndex + 1 < Frames.size() &&
		//	Frames[iKeyFrameIndex + 1].fTrackPosition <= m_fCurrentTrackPosition)
		//{
		//	iKeyFrameIndex++;
		//}
		//
		//// 애니메이션 끝 처리
		//if (iKeyFrameIndex >= (int)Frames.size() - 1)
		//	iKeyFrameIndex = (int)Frames.size() - 1;

		// 이분탐색
		_int iLow = iKeyFrameIndex;
		_int iHigh = Frames.size() - 1;

		/// v       v         v      
		/// □□□□■□□★□□
		/// 
		///           v   v   v
		/// □□□□□□□★□□
		while (iLow < iHigh)
		{
			_int iMid = (iLow + iHigh) / 2;
			if (Frames[iMid].fTrackPosition <= m_fCurrentTrackPosition)
			{
				iLow = iMid + 1;
			}
			else
				iHigh = iMid;
		}

		if (iLow == 0)
			iKeyFrameIndex = 0;
		else
			iKeyFrameIndex = iLow - 1;

		// 애니메이션 끝 처리
		if (iKeyFrameIndex >= (int)Frames.size() - 1)
			iKeyFrameIndex = (int)Frames.size() - 1;

	}

	return false;
}

CMorphAnimation* CMorphAnimation::Create(CModel* pModel, binMorphAnimation* pMorphAnimation)
{
	CMorphAnimation* pInstance = new CMorphAnimation();

	if (FAILED(pInstance->Initialize(pModel, pMorphAnimation)))
	{
		MSG_BOX("Failed to Created : CMorphAnimation");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CMorphAnimation* CMorphAnimation::Clone()
{
	return new CMorphAnimation(*this);
}

void CMorphAnimation::Free()
{
	__super::Free();

	for (auto& pChannel : m_MorphChannels)
		Safe_Release(pChannel);

	m_MorphChannels.clear();
}
