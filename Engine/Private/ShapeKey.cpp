#include "ShapeKey.h"
#include "Model.h"

CShapeKey::CShapeKey()
{
}

HRESULT CShapeKey::Initialize(const CModel* pModel, binAnimMesh* pAnimMesh)
{
	strcpy_s(m_szName, pAnimMesh->szName);

	//m_iShapeIndex = pModel->Get_ShapeIndex(m_szName);
	//if (-1 == m_iShapeIndex)
	//	return E_FAIL;

	m_iNumVertices = pAnimMesh->iNumVertices;

	_float3				vDeltaNormal{};
	_float3				vDeltaPosition{};

	for (size_t i = 0; i < m_iNumVertices; i++)
	{
		memcpy(&vDeltaPosition, &pAnimMesh->vDeltaPositions[i], sizeof(_float3));
		memcpy(&vDeltaNormal, &pAnimMesh->vDeltaNormals[i], sizeof(_float3));

		m_vDeltaPositions.push_back(vDeltaPosition);
		m_vDeltaNormals.push_back(vDeltaNormal);
	}

	return S_OK;
}

CShapeKey* CShapeKey::Create(const CModel* pModel, binAnimMesh* pAnimMesh)
{
	CShapeKey* pInstance = new CShapeKey();

	if (FAILED(pInstance->Initialize(pModel, pAnimMesh)))
	{
		MSG_BOX("Failed to Created : CShapeKey");	
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CShapeKey::Free()
{
	__super::Free();

	m_vDeltaPositions.clear();
	m_vDeltaNormals.clear();

}
