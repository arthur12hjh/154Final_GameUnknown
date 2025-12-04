#pragma once

#include "Base.h"

/* 뼈 */
/* aiBone : 어떤 정점에게 얼마나? */
/* aiNode : 계층구조를 표현 + 상태행렬을 표현 */
/* aiAnimNode(Channel) : 이 뼈가 특정 애니메이션 안에서 어떤 시간에 어떤 상태를 표현하는지에 대한 정보를 저장. */

NS_BEGIN(Engine)

class CBone final : public CBase
{
private:
	CBone();
	virtual ~CBone() = default;

public:
	_matrix Get_CombinedTransformationMatrix() const {
		return XMLoadFloat4x4(&m_CombinedTransformationMatrix);
	}
	const _float4x4* Get_CombinedTransformationMatrixPtr() const {
		return &m_CombinedTransformationMatrix;
	}
	_bool Compare_Name(const _char* pBoneName) const {
		return !strcmp(pBoneName, m_szName);
	}

	_char* Get_Name() { return m_szName; }

	void Set_Name(const _char* szName) { strcpy_s(m_szName, szName); }

	_int Get_ParentBoneIndex() { return m_iParentBoneIndex; }

public:
	_matrix Get_TransformationMatrix() const {
		return XMLoadFloat4x4(&m_TransformationMatrix);
	}

	void Set_TransformationMatrix(_fmatrix TransformationMatrix) {
		XMStoreFloat4x4(&m_TransformationMatrix, TransformationMatrix);
	}
	
	void Set_CombinedTransformationMatrix(_fmatrix CombinedMatrix)
	{
		XMStoreFloat4x4(&m_CombinedTransformationMatrix, CombinedMatrix);
	}

public:
	HRESULT Initialize(binNode* pNode, _int iParentIndex);
	void Update_CombinedTransformationMatrix(const vector<CBone*>& Bones, _fmatrix PreTransformMatrix);

private:
	_char				m_szName[MAX_PATH] = {};
	_float4x4			m_TransformationMatrix = {}; /* 이 뼈만의 상태변환행렬 */
	_float4x4			m_CombinedTransformationMatrix = {}; /* m_TransformatinoMatrix * Parent`s m_CombinedTransformationMatrix */	
	_int				m_iParentBoneIndex = { -1 };
	/*CBone*				m_pParent = { nullptr };*/

public:
	static CBone* Create(binNode* pNode, _int iParentIndex);
	CBone* Clone();
	virtual void Free() override;
};

NS_END