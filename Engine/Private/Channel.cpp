#include "Channel.h"
#include "Model.h"

#include "Bone.h"

CChannel::CChannel()
{
}

HRESULT CChannel::Initialize(const CModel* pModel, binChannel* pChannel)
{
	strcpy_s(m_szName, pChannel->szName);

	m_iBoneIndex = pModel->Get_BoneIndex(m_szName);
	if (-1 == m_iBoneIndex)
		return E_FAIL;

	m_iNumKeyFrames = max(pChannel->iNumScalingKeys, pChannel->iNumRotationKeys);
	m_iNumKeyFrames = max(m_iNumKeyFrames, pChannel->iNumPositionKeys);

	_float3				vScale{};
	_float4				vRotation{};
	_float3				vTranslation{};

	for (size_t i = 0; i < m_iNumKeyFrames; i++)
	{
		KEYFRAME		KeyFrame{};

		if (i < pChannel->iNumScalingKeys)
		{
			memcpy(&vScale, &pChannel->cScalingKeys[i].vValue, sizeof(_float3));
			KeyFrame.fTrackPosition = pChannel->cScalingKeys[i].fTime;
		}

		if (i < pChannel->iNumRotationKeys)
		{
			vRotation.x = pChannel->cRotationKeys[i].vValue.x;
			vRotation.y = pChannel->cRotationKeys[i].vValue.y;
			vRotation.z = pChannel->cRotationKeys[i].vValue.z;
			vRotation.w = pChannel->cRotationKeys[i].vValue.w;

			KeyFrame.fTrackPosition = pChannel->cRotationKeys[i].fTime;
		}

		if (i < pChannel->iNumPositionKeys)
		{
			memcpy(&vTranslation, &pChannel->cPositionKeys[i].vValue, sizeof(_float3));
			KeyFrame.fTrackPosition = pChannel->cPositionKeys[i].fTime;
		}

		KeyFrame.vScale = vScale;
		KeyFrame.vRotation = vRotation;
		KeyFrame.vTranslation = vTranslation;

		m_KeyFrames.push_back(KeyFrame);
	}

	return S_OK;
}

void CChannel::Update_TransformationMatrix(const vector<class CBone*>& Bones, _float fCurrentTrackPosition, _uint* pCurrentKeyFrameIndex)
{
	if (0.f == fCurrentTrackPosition)
		*pCurrentKeyFrameIndex= 0;

	KEYFRAME		LastKeyFrame = m_KeyFrames.back();

	_vector			vScale{};
	_vector			vRotation{};
	_vector			vTranslation{};

	if (fCurrentTrackPosition >= LastKeyFrame.fTrackPosition)
	{
		vScale = XMLoadFloat3(&LastKeyFrame.vScale);
		vRotation = XMLoadFloat4(&LastKeyFrame.vRotation);
		vTranslation = XMVectorSetW(XMLoadFloat3(&LastKeyFrame.vTranslation), 1.f);
	}

	else /* 선형보간을 해야겠다. */
	{
		while (fCurrentTrackPosition >= m_KeyFrames[*pCurrentKeyFrameIndex+ 1].fTrackPosition)
			++*pCurrentKeyFrameIndex;

		_float3		vSourScale{}, vDestScale{};
		_float4		vSourRotation{}, vDestRotation{};
		_float3		vSourTranslation{}, vDestTranslation{};

		vSourScale = m_KeyFrames[*pCurrentKeyFrameIndex].vScale;
		vDestScale = m_KeyFrames[*pCurrentKeyFrameIndex+ 1].vScale;

		vSourRotation = m_KeyFrames[*pCurrentKeyFrameIndex].vRotation;
		vDestRotation = m_KeyFrames[*pCurrentKeyFrameIndex+ 1].vRotation;

		vSourTranslation = m_KeyFrames[*pCurrentKeyFrameIndex].vTranslation;
		vDestTranslation = m_KeyFrames[*pCurrentKeyFrameIndex+ 1].vTranslation;

		_float		fRatio = (fCurrentTrackPosition - m_KeyFrames[*pCurrentKeyFrameIndex].fTrackPosition) / 
			(m_KeyFrames[*pCurrentKeyFrameIndex+ 1].fTrackPosition - m_KeyFrames[*pCurrentKeyFrameIndex].fTrackPosition);

		vScale = XMVectorLerp(XMLoadFloat3(&vSourScale), XMLoadFloat3(&vDestScale), fRatio);
		vRotation = XMQuaternionSlerp(XMLoadFloat4(&vSourRotation), XMLoadFloat4(&vDestRotation), fRatio);
		vTranslation = XMVectorSetW(XMVectorLerp(XMLoadFloat3(&vSourTranslation), XMLoadFloat3(&vDestTranslation), fRatio), 1.f);

	}

	// _matrix		BoneTransformationMatrix = XMMatrixScaling() * 자전행렬 * ;
	_matrix		BoneTransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vTranslation);

	Bones[m_iBoneIndex]->Set_TransformationMatrix(BoneTransformationMatrix);


}

CChannel* CChannel::Create(const CModel* pModel, binChannel* pChannel)
{
	CChannel* pInstance = new CChannel();

	if (FAILED(pInstance->Initialize(pModel, pChannel)))
	{
		MSG_BOX("Failed to Created : CChannel");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CChannel::Free()
{
	__super::Free();

	m_KeyFrames.clear();
	
}
