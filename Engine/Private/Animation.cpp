#include "Animation.h"
#include "Channel.h"

CAnimation::CAnimation()
{
	/*XMMatrixDecompose();*/
}

CAnimation::CAnimation(const CAnimation& Prototype)
	: m_fCurrentTrackPosition { Prototype.m_fCurrentTrackPosition }
	, m_fDuration{ Prototype.m_fDuration }
	, m_fTickPerSecond{ Prototype.m_fTickPerSecond }
	, m_iNumChannels{ Prototype.m_iNumChannels }
	, m_Channels{ Prototype.m_Channels }
	, m_CurrentKeyFrameIndices{ Prototype.m_CurrentKeyFrameIndices }
	, m_BoneToChannelMappingLists{ Prototype.m_BoneToChannelMappingLists }
{
	for (auto& pChannel : m_Channels)
		Safe_AddRef(pChannel);

	strcpy_s(m_szName, Prototype.m_szName);
}

HRESULT CAnimation::Initialize(class CModel* pModel, binAnimation* pAnimation)
{
	m_fDuration = pAnimation->fDuration;
	m_fTickPerSecond = pAnimation->fTicksPerSecond;

	m_iNumChannels = pAnimation->iNumChannels;


	m_CurrentKeyFrameIndices.resize(m_iNumChannels);

	strcpy_s(m_szName, pAnimation->szName);

	for (size_t i = 0; i < m_iNumChannels; i++)
	{
		CChannel* pChannel = CChannel::Create(pModel, &pAnimation->vChannels[i]);
		if (nullptr == pChannel)
			return E_FAIL;

		m_Channels.push_back(pChannel);
	}

	return S_OK;
}

_bool CAnimation::Update_TransformationMatrices(const vector<class CBone*>& Bones, _bool isLoop, _float fTimeDelta)
{
	/* 내 애니메이션의 현재 재생위치. */
	m_fCurrentTrackPosition += m_fTickPerSecond * fTimeDelta;

	if (m_fCurrentTrackPosition >= m_fDuration)
	{
		if (true == isLoop)
			m_fCurrentTrackPosition = 0.f;
		else
			return true;
	}

	_uint		iIndex = {};

	for (auto& pChannel : m_Channels)
	{
		pChannel->Update_TransformationMatrix(Bones, m_fCurrentTrackPosition, &m_CurrentKeyFrameIndices[iIndex++]);
	}

	return false;
}

_int CAnimation::Update_TrackPosition(const vector<class CBone*>& Bones, _bool isLoop, _float fTimeDelta, _float fEndTrackPosition)
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

_bool CAnimation::Update_CurrentKeyFrameIndices()
{
	// 채널을 돌며, 각 채널별 키프레임 인덱스를 갱신해준다.
	for (size_t i = 0; i < m_Channels.size(); i++)
	{
		CChannel* pChannel = m_Channels[i];
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


HRESULT CAnimation::Swap_AnimationChannel(_uint iSrc, _uint iDst)
{
	if (m_Channels.size() <= iSrc
		|| m_Channels.size() <= iDst)
		return E_FAIL;
	
	swap(m_Channels[iSrc], m_Channels[iDst]);

	return S_OK;
}

void CAnimation::Reset()
{
	m_fCurrentTrackPosition = 0.f;

	for (auto& iKeyFrameIndex : m_CurrentKeyFrameIndices)
		iKeyFrameIndex = 0;
}

CAnimation* CAnimation::Create(CModel* pModel, binAnimation* pAnimation)
{
	CAnimation* pInstance = new CAnimation();

	if (FAILED(pInstance->Initialize(pModel, pAnimation)))
	{
		MSG_BOX("Failed to Created : CAnimation");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CAnimation* CAnimation::Clone()
{
	return new CAnimation(*this);
}

void CAnimation::Free()
{
	__super::Free();

	for (auto& pChannel : m_Channels)	
		Safe_Release(pChannel);

	m_Channels.clear();

	m_CurrentKeyFrameIndices.clear();

}
