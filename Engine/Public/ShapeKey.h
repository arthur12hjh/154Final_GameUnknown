#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class CShapeKey final : public CBase
{
private:
	CShapeKey();
	virtual ~CShapeKey() = default;

public:
	HRESULT Initialize(const class CModel* pModel, binAnimMesh* pAnimMesh);


private:
	_char					m_szName[MAX_PATH] = {};
	_int					m_iShapeIndex = { -1 };

	_uint					m_iNumVertices = {};

	/* 절차값당 Position 변화량 */
	vector<_float3>			m_vDeltaPositions;
	/* 절차값당 Normal 변화량 */
	vector<_float3>			m_vDeltaNormals;

public:
	static CShapeKey* Create(const class CModel* pModel, binAnimMesh* pAnimMesh);
	virtual void Free() override;
};

NS_END