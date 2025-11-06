#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class CCell final : public CBase
{
private:
	CCell(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CCell() = default;

public:
	_vector Get_Point(NAVI_POINT ePoint) {
		return XMLoadFloat3(&m_vPoints[ENUM_CLASS(ePoint)]);
	}

	void Set_Neighbor(NAVI_LINE eLine, CCell* pNeighbor) {
		m_NeighborIndices[ENUM_CLASS(eLine)] = pNeighbor->m_iIndex;		
	}

public:
	HRESULT Initialize(const _float3* vPoints, _uint iIndex);
	_bool isIn(_fvector vPosition, _int* pNeighborIndex);

	_bool Compare(_fvector vSourPoint, _fvector vDestPoint);
	_float Compute_Height(_fvector vPoint);

#ifdef _DEBUG
public:
	HRESULT Render();

#endif

private:
	_uint					m_iIndex = {};
	_float3					m_vPoints[ENUM_CLASS(NAVI_POINT::END)] = {};
	_float3					m_vNormals[ENUM_CLASS(NAVI_LINE::END)] = {};

	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };
	
	_int					m_NeighborIndices[ENUM_CLASS(NAVI_LINE::END)] = { -1, -1, -1 };
		
	_float4					m_vPlane = {};

#ifdef _DEBUG	
	class CVIBuffer_Cell*	m_pVIBuffer = { nullptr };
#endif

public:
	static CCell* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _float3* vPoints, _uint iIndex);
	virtual void Free();
};

NS_END