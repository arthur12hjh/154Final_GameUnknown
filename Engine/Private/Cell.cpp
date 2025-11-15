#include "Cell.h"

#include "VIBuffer_Cell.h"

CCell::CCell(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice { pDevice }
	, m_pContext { pContext }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CCell::Initialize(const _float3* vPoints, _uint iIndex)
{
	memcpy(m_vPoints, vPoints, sizeof(_float3) * ENUM_CLASS(NAVI_POINT::END));

	_float3			vLines[ENUM_CLASS(NAVI_LINE::END)] = {};

	XMStoreFloat3(&vLines[ENUM_CLASS(NAVI_LINE::AB)],
		XMVector3Normalize(XMLoadFloat3(&m_vPoints[ENUM_CLASS(NAVI_POINT::B)]) - XMLoadFloat3(&m_vPoints[ENUM_CLASS(NAVI_POINT::A)])));
	XMStoreFloat3(&vLines[ENUM_CLASS(NAVI_LINE::BC)],
		XMVector3Normalize(XMLoadFloat3(&m_vPoints[ENUM_CLASS(NAVI_POINT::C)]) - XMLoadFloat3(&m_vPoints[ENUM_CLASS(NAVI_POINT::B)])));
	XMStoreFloat3(&vLines[ENUM_CLASS(NAVI_LINE::CA)],
		XMVector3Normalize(XMLoadFloat3(&m_vPoints[ENUM_CLASS(NAVI_POINT::A)]) - XMLoadFloat3(&m_vPoints[ENUM_CLASS(NAVI_POINT::C)])));

	m_vNormals[ENUM_CLASS(NAVI_LINE::AB)] = _float3(
		vLines[ENUM_CLASS(NAVI_LINE::AB)].z * -1.f, 0.f, vLines[ENUM_CLASS(NAVI_LINE::AB)].x);
	m_vNormals[ENUM_CLASS(NAVI_LINE::BC)] = _float3(
		vLines[ENUM_CLASS(NAVI_LINE::BC)].z * -1.f, 0.f, vLines[ENUM_CLASS(NAVI_LINE::BC)].x);
	m_vNormals[ENUM_CLASS(NAVI_LINE::CA)] = _float3(
		vLines[ENUM_CLASS(NAVI_LINE::CA)].z * -1.f, 0.f, vLines[ENUM_CLASS(NAVI_LINE::CA)].x);

	m_iIndex = iIndex;

#ifdef _DEBUG
	m_pVIBuffer = CVIBuffer_Cell::Create(m_pDevice, m_pContext, vPoints);
	if (nullptr == m_pVIBuffer)
		return E_FAIL;

#endif // _DEBUG

	XMStoreFloat4(&m_vPlane,
		XMPlaneFromPoints(XMLoadFloat3(&m_vPoints[0]), XMLoadFloat3(&m_vPoints[1]), XMLoadFloat3(&m_vPoints[2])));


	return S_OK;
}

_bool CCell::isIn(_fvector vPosition, _int* pNeighborIndex)
{
	for (size_t i = 0; i < ENUM_CLASS(NAVI_LINE::END); i++)
	{
		_vector	vDir = XMVector3Normalize(vPosition - XMLoadFloat3(&m_vPoints[i]));
		_vector vNormal = XMLoadFloat3(&m_vNormals[i]);

		if (0.f < XMVectorGetX(XMVector3Dot(vDir, vNormal)))
		{
			*pNeighborIndex = m_NeighborIndices[i];
			return false;
		}

	}
	return true;
}

_bool CCell::Compare(_fvector vSourPoint, _fvector vDestPoint)
{	
	if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[ENUM_CLASS(NAVI_POINT::A)]), vSourPoint))
	{
		if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[ENUM_CLASS(NAVI_POINT::B)]), vDestPoint))
			return true;
		
		if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[ENUM_CLASS(NAVI_POINT::C)]), vDestPoint))
			return true;
	}
	if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[ENUM_CLASS(NAVI_POINT::B)]), vSourPoint))
	{
		if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[ENUM_CLASS(NAVI_POINT::C)]), vDestPoint))
			return true;

		if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[ENUM_CLASS(NAVI_POINT::A)]), vDestPoint))
			return true;
	}

	if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[ENUM_CLASS(NAVI_POINT::C)]), vSourPoint))
	{
		if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[ENUM_CLASS(NAVI_POINT::A)]), vDestPoint))
			return true;

		if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[ENUM_CLASS(NAVI_POINT::B)]), vDestPoint))
			return true;
	}
	return false;
}

_float CCell::Compute_Height(_fvector vPoint)
{
	// y = (-ax - cz - d) / b;


	return (-m_vPlane.x * XMVectorGetX(vPoint) - m_vPlane.z * XMVectorGetZ(vPoint) - m_vPlane.w) / m_vPlane.y;
}

#ifdef _DEBUG
HRESULT CCell::Render()
{
	m_pVIBuffer->Bind_Resources();

	m_pVIBuffer->Render();

	return S_OK;
}
#endif // _DEBUG

CCell* CCell::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _float3* vPoints, _uint iIndex)
{
	CCell* pInstance = new CCell(pDevice, pContext);

	if (FAILED(pInstance->Initialize(vPoints, iIndex)))
	{
		MSG_BOX("Failed to Created : CCell");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCell::Free()
{
	__super::Free();



#ifdef _DEBUG
	Safe_Release(m_pVIBuffer);
#endif

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
