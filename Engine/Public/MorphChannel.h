#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class CMorphChannel final : public CBase
{
private:
	CMorphChannel();
	virtual ~CMorphChannel() = default;

public:
	HRESULT Initialize(const class CModel* pModel, binMeshMorphChannel* pMorphChannel);
	void Update_ShapeMorphing(class CModel* pModel, _float fCurrentTrackPosition);

	_char* Get_Name() { return m_szName; }
	void Set_ShapeIndex(_uint iIndex) { m_iShapeIndex = iIndex; }

	_uint Get_NumKeyFrames() { return m_iNumKeyFrames; }
	const MORPH_KEYFRAME& Get_KeyFrame(_uint iIndex = 0) const { return m_KeyFrames[iIndex]; }
	_int Get_ShapeIndex() { return m_iShapeIndex; }

	const vector<MORPH_KEYFRAME>& Get_KeyFrames() { return m_KeyFrames; }

	void Reset();

private:
	_char						m_szName[MAX_PATH] = {};
	_int						m_iShapeIndex = { -1 };

	_uint						m_iNumKeyFrames = {};
	_uint						m_iCurrentKeyFrameIndex;
	vector<MORPH_KEYFRAME>		m_KeyFrames;

public:
	static CMorphChannel* Create(const class CModel* pModel, binMeshMorphChannel* pMorphChannel);
	virtual void Free() override;
};

NS_END