#include "Bone.h"

CBone::CBone()
{
}

HRESULT CBone::Initialize(binNode* pNode, _int iParentIndex)
{
	strcpy_s(m_szName, pNode->szName);

	m_iParentBoneIndex = iParentIndex;

	/* m_TransformationMatrix : 뼈만의 상태 */
	/* 갱신이 필요할거야! -> 어심프로부터 애니메이션이 이용하고 있는 특정 뼈대들만 정보를 받아와서 갱신 */
	memcpy(&m_TransformationMatrix, &pNode->matTransformation, sizeof(_float4x4));
	XMStoreFloat4x4(&m_TransformationMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_TransformationMatrix)));
		 
	/* 추후 최초 렌더링시에도 반드시 전체뼈를 갱신하여 생성하고 렌더링할 것이다. */
	XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMMatrixIdentity());

	return S_OK;
}

void CBone::Update_CombinedTransformationMatrix(const vector<CBone*>& Bones, _fmatrix PreTransformMatrix)
{
	if (-1 == m_iParentBoneIndex)
		XMStoreFloat4x4(&m_CombinedTransformationMatrix, 
			XMLoadFloat4x4(&m_TransformationMatrix) * PreTransformMatrix);

	else
		XMStoreFloat4x4(&m_CombinedTransformationMatrix,
			XMLoadFloat4x4(&m_TransformationMatrix) * Bones[m_iParentBoneIndex]->Get_CombinedTransformationMatrix());
}

CBone* CBone::Create(binNode* pNode, _int iParentIndex)
{
	CBone* pInstance = new CBone();

	if (FAILED(pInstance->Initialize(pNode, iParentIndex)))
	{
		MSG_BOX("Failed to Created : CBone");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CBone* CBone::Clone()
{
	return new CBone(*this);
}

void CBone::Free()
{
	__super::Free();


}
