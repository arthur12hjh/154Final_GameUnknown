#include "MorphChannel.h"
#include "Model.h"

CMorphChannel::CMorphChannel()
{
}                            

HRESULT CMorphChannel::Initialize(const CModel* pModel, binMeshMorphChannel* pMorphChannel)
{
	strcpy_s(m_szName, pMorphChannel->szName);

	//m_iShapeIndex = pModel->Get_ShapeIndex(m_szName);
	//if (-1 == m_iShapeIndex)
	//	return E_FAIL;

	m_iNumKeyFrames = pMorphChannel->iNumKeys;
	m_iCurrentKeyFrameIndex = 0;

	for (_uint i = 0; i < m_iNumKeyFrames; i++)
	{	
		MORPH_KEYFRAME MorphKeyFrame{};

		MorphKeyFrame.fTrackPosition = pMorphChannel->vKeys[i].fTime;

		for (_uint j = 0; j < pMorphChannel->vKeys[i].iNumValuesAndWeights; ++j) 
		{
			MorphKeyFrame.vValues.push_back(pMorphChannel->vKeys[i].vValues[j]);
			MorphKeyFrame.vWeights.push_back(pMorphChannel->vKeys[i].vWeights[j]);
		}

		m_KeyFrames.push_back(MorphKeyFrame);
	}

	return S_OK;
}

// fCurrentTrackPosition 기반으로 
// MORPH_KEYFRAME 순회.
// 
void CMorphChannel::Update_ShapeMorphing(_float fCurrentTrackPosition, const MORPH_KEYFRAME*& pOutCurrent)
{
	pOutCurrent = nullptr;
	
	if (m_KeyFrames.empty())
		return;

	if (fCurrentTrackPosition <= m_KeyFrames.front().fTrackPosition)
	{
		pOutCurrent = &m_KeyFrames.front();
		return;
	}

	for (size_t i = 0; i + 1 < m_KeyFrames.size(); ++i)
	{
		const auto& Cur = m_KeyFrames[i];
		const auto& Next = m_KeyFrames[i + 1];

		if (fCurrentTrackPosition >= Cur.fTrackPosition &&
			fCurrentTrackPosition < Next.fTrackPosition)
		{
			pOutCurrent = &Cur;
			return;
		}
	}

	// 재생범위 밖이면.
	pOutCurrent = &m_KeyFrames.back();


    return;
}

void CMorphChannel::Reset()
{
	m_iCurrentKeyFrameIndex = 0;
}

CMorphChannel* CMorphChannel::Create(const CModel* pModel, binMeshMorphChannel* pMorphChannel)
{
	CMorphChannel* pInstance = new CMorphChannel();

	if (FAILED(pInstance->Initialize(pModel, pMorphChannel)))
	{
		MSG_BOX("Failed to Created : CMorphChannel");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMorphChannel::Free()
{
	__super::Free();

	m_KeyFrames.clear();
	m_KeyFrames.shrink_to_fit();

}
