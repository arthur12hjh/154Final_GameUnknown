#pragma once

#include "Base.h"

/* 이 뼈는 시간대별 취해야할 뼈의 상태들 .*/
/* Update함수에서 내가 취해야할 시간을 넣어주면 시간에 맞느 ㅅ아태행렬을 보간해서 만들꺼라고. */
/* 만든행렬을 뼈(CBone)에게 전달해줘야지. */
NS_BEGIN(Engine)

class CMorphChannel final : public CBase
{
private:
	CMorphChannel();
	virtual ~CMorphChannel() = default;

public:
	HRESULT Initialize(const class CModel* pModel, binMeshMorphChannel* pMorphChannel);
	void Update_ShapeMorphing(const vector<class CShapeKey*>& ShapeKeys, _float fCurrentTrackPosition, _uint* pCurrentKeyFrameIndex);

	_char* Get_Name() { return m_szName; }
	void Set_ShapeIndex(_uint iIndex) { m_iShapeIndex = iIndex; }

	_uint Get_NumKeyFrames() { return m_iNumKeyFrames; }
	const MORPH_KEYFRAME& Get_KeyFrame(_uint iIndex = 0) const { return m_KeyFrames[iIndex]; }
	_int Get_ShapeIndex() { return m_iShapeIndex; }

	const vector<MORPH_KEYFRAME>& Get_KeyFrames() { return m_KeyFrames; }

private:
	_char						m_szName[MAX_PATH] = {};
	_int						m_iShapeIndex = { -1 };

	_uint						m_iNumKeyFrames = {};
	vector<MORPH_KEYFRAME>		m_KeyFrames;

public:
	static CMorphChannel* Create(const class CModel* pModel, binMeshMorphChannel* pMorphChannel);
	virtual void Free() override;
};

NS_END