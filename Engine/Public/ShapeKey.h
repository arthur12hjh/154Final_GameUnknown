#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class CShapeKey final : public CBase
{
private:
	CShapeKey();
	virtual ~CShapeKey() = default;

public:
	const _char* Get_Name() const {
		return m_szName;
	}
	
	const vector<_float3>* Get_DeltaPosition() { return &m_vDeltaPositions; }
	const vector<_float3>* Get_DeltaNormals() { return &m_vDeltaNormals; }

public:
	HRESULT Initialize(const class CModel* pModel, const binAnimMesh* pAnimMesh);


private:
	_char					m_szName[MAX_PATH] = {};
	_int					m_iShapeIndex = { -1 };

	_uint					m_iNumVertices = {};

	/* 절차값당 Position 변화량 */
	vector<_float3>			m_vDeltaPositions;
	/* 절차값당 Normal 변화량 */
	vector<_float3>			m_vDeltaNormals;

public:
	static CShapeKey* Create(const class CModel* pModel,const binAnimMesh* pAnimMesh);
	virtual void Free() override;
};

NS_END