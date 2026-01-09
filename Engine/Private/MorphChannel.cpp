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

// 이거 뭔데 어떻게 하는건데
// - m_MorphChannels(사실 구조 잘못짜서 하나밖에 없음ㅎ)를 순회하면서 웨이트를 받아 Set_ShapeWeight를 해줌
// - m_CurrentKeyFrameIndices - m_KeyFrames는 1대1 매칭이 됨
// - index 별로 순회하면서 Cur - Next 사이를 보간해서 Weight값을 보간
// 정확한 순서를 한글 의사코드로 작성해봐
//  1. fCurrentTrackPosition == 0이면 모든 pCurrentKeyFrameIndices 초기화
//  2. LastKeyFrame에 마지막 키프레임 구조체 받기
//  3. fCurrentTrackPosition이 LaskKeyFrame의 fTrackPosition보다 클 경우, 마지막 프레임으로 고정하고 신호를 쏴준다.
//  4. 아닐 경우, 다음 인덱스의 fTrackPosition과 fTrackPosition을 비교해서 더하기를 해준다.
//    4-2. 지금 Weight랑 다음 Weight 사이를 보간해준다.
//	  4-3. Set_ShapeWeight()를 실행해준다.
//    4-4. 이를 반복한다.
void CMorphChannel::Update_ShapeMorphing(class CModel* pModel, _float fCurrentTrackPosition)
{
	if (0.f == fCurrentTrackPosition)
	{
		m_iCurrentKeyFrameIndex = 0;
	}

	MORPH_KEYFRAME	LastKeyFrame = m_KeyFrames.back();

	vector<_uint> vCurrentValues;
	vector<_float> vCurrentWeights;

	if (fCurrentTrackPosition >= LastKeyFrame.fTrackPosition)
	{
		for (_uint i = 0; i < LastKeyFrame.vValues.size(); ++i)
		{
			_uint iShapeKeyIndex = LastKeyFrame.vValues[i];
			_float fWeight = LastKeyFrame.vWeights[i];

			pModel->Set_ShapeWeightIndex(iShapeKeyIndex, fWeight);
		}

		return;
	}
	else /* 선형보간을 해야겠다. */
	{
		while (m_iCurrentKeyFrameIndex + 1 < m_KeyFrames.size() &&
			fCurrentTrackPosition >= m_KeyFrames[m_iCurrentKeyFrameIndex + 1].fTrackPosition)
			++m_iCurrentKeyFrameIndex;

		vector<_uint> vDestValues;
		vector<_float> vDestWeights;

		vCurrentValues = m_KeyFrames[m_iCurrentKeyFrameIndex].vValues;
		vDestValues = m_KeyFrames[m_iCurrentKeyFrameIndex + 1].vValues;

		vCurrentWeights = m_KeyFrames[m_iCurrentKeyFrameIndex].vWeights;
		vDestWeights = m_KeyFrames[m_iCurrentKeyFrameIndex + 1].vWeights;

		for (_uint iIndex = 0; iIndex < vCurrentValues.size(); ++iIndex)
		{
			pModel->Set_ShapeWeightIndex(vCurrentValues[iIndex], vCurrentWeights[iIndex]);
		}

	}

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
