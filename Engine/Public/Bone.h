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

public:
	void Set_TransformationMatrix(_fmatrix TransformationMatrix);

public:
	HRESULT Initialize(const aiNode* pAINode, _int iParentIndex);
	void Update_CombinedTransformationMatrix(const vector<CBone*>& Bones, _fmatrix PreTransformMatrix);

private:
	_char				m_szName[MAX_PATH] = {};
	_float4x4			m_TransformationMatrix = {}; /* 이 뼈만의 상태변환행렬 */
	_float4x4			m_CombinedTransformationMatrix = {}; /* m_TransformatinoMatrix * Parent`s m_CombinedTransformationMatrix */	
	_int				m_iParentBoneIndex = { -1 };
	/*CBone*				m_pParent = { nullptr };*/

public:
	static CBone* Create(const aiNode* pAINode, _int iParentIndex);
	CBone* Clone();
	virtual void Free() override;
};

NS_END