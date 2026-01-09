#include "MorphAnimation.h"
#include "MorphChannel.h"

#include "Model.h"

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

// 여기서 뭐해야하는데?
// 일단 fCurrentTrackPosition을 계산하고, 맞으면 Update_ShapeMorphing을 실행해.

// Update_ShapeMorphing은 뭔데?
// m_MorphChannels(사실 구조 잘못짜서 하나밖에 없음ㅎ)를 순회하면서 웨이트를 받아 Set_ShapeWeight를 해줌
// m_CurrentKeyFrameIndices를 
_int CMorphAnimation::Update_TrackPosition(class CModel* pModel, _bool isLoop, _float fTimeDelta)
{
	/* 내 애니메이션의 현재 재생위치. */
	m_fCurrentTrackPosition += m_fTickPerSecond * fTimeDelta;
	// m_fCurrentTrackPosition
	// 
	if (m_fCurrentTrackPosition >= m_fDuration)
	{
		if (true == isLoop)
		{
			m_fCurrentTrackPosition = m_fCurrentTrackPosition - m_fDuration;
			for (auto& pMorphChannel : m_MorphChannels)
				pMorphChannel->Reset();

			return iFLAG_ANIMATION_RESET;
		}
		else
		{
			m_fCurrentTrackPosition -= m_fTickPerSecond * fTimeDelta; // 마지막 프레임 고정
			return iFLAG_ANIMATION_FINISH;
		}
	}

	_uint iIndex = {};

	for (auto& pMorphChannel : m_MorphChannels)
		pMorphChannel->Update_ShapeMorphing(pModel, m_fCurrentTrackPosition);


	pModel->Bind_ShapeWeight();


	return iFLAG_ANIMATION_PLAY;
}

_bool CMorphAnimation::Update_CurrentKeyFrameIndices()
{
	
	return false;
}

void CMorphAnimation::Reset()
{
	for (auto& pMorphChannel : m_MorphChannels)
		pMorphChannel->Reset();
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
