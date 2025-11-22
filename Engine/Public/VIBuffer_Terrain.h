#pragma once

#include "VIBuffer.h"

NS_BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Terrain final : public CVIBuffer
{
private:
	CVIBuffer_Terrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Terrain(const CVIBuffer_Terrain& Prototype);
	virtual ~CVIBuffer_Terrain() = default;

public:
	_float* Get_HeightData();
	_uint Get_NumVerticesX() { return m_iNumVerticesX; }
	_uint Get_NumVerticesZ() { return m_iNumVerticesZ; }

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pHeightMapFilePath);
	virtual HRESULT Initialize(void* pArg) override;
	void Culling(_fmatrix WorldMatrix);

	void Change_Height_Rect(_vector vPickingPos, _float fHeight, _float fRadius);
	void Change_Height_Flat(_vector vPickingPos, _float fHeight, _float fRadius);
	_float Get_Interpolated_Height_Local(_float fLocalX, _float fLocalZ) const;

private:
	_uint			m_iNumVerticesX = {};
	_uint			m_iNumVerticesZ = {};

	class CQuadTree* m_pQuadTree = { nullptr };

public:
	static CVIBuffer_Terrain* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pHeightMapFilePath);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END