#pragma once

#include "VIBuffer.h"

NS_BEGIN(Engine)

class ENGINE_DLL CMesh final : public CVIBuffer
{
private:
	CMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMesh(const CMesh& Prototype);
	virtual ~CMesh() = default;

public:
	const _char* Get_Name() const {
		return m_szName;
	}

	_uint Get_MaterialIndex() const {
		return m_iMaterialIndex;
	}

	const vector<_float4x4>& Get_OffsetMatrices() const { return m_OffsetMatrices; }
	const vector<_int>& Get_BoneIndices() const { return m_BoneIndices; }
	const _float4x4* Get_BoneMatrices() { return m_pBoneMatrices; }

	const _float3* Get_VertexPositionList() { return m_pVertexPositions; }

	const _float3* Get_VertexPosition(_int iIndex) const
	{
		if (!m_pVertexPositions)
			return nullptr;

		if (iIndex < 0 || iIndex >= m_iNumVertices)
			return nullptr;

		return &m_pVertexPositions[iIndex];
	}

	_uint Get_NumIndices() const { return m_iNumIndices; }
	_uint Get_IndexStride() const { return m_iIndexStride; }

	_uint Get_NumVectices() const { return m_iNumVertices; }
	_uint Get_VertexStride() const { return m_iVertexStride; }

	_uint Get_NumVertexBuffers() const { return m_iNumVertexBuffers; }

	DXGI_FORMAT Get_IndexFormat() const { return m_eIndexFormat; }
	D3D11_PRIMITIVE_TOPOLOGY Get_Topology() const { return m_ePrimitive; }


public:
	virtual HRESULT Initialize_Prototype(MODEL_TYPE eType, const class CModel* pModel, const binMesh* pBinMesh, _fmatrix PreTransformMatrix);
	virtual HRESULT Initialize(void* pArg) override;
	HRESULT Bind_BoneMatrices(const vector<class CBone*>& Bones, class CShader* pShader, const _char* pConstantName);
	HRESULT Bind_OffsetMatrices(const vector<class CBone*>& Bones, class CShader* pShader, const _char* pConstantName);

private:
	_char							m_szName[MAX_PATH] = {};
	_uint							m_iMaterialIndex = {};			
	_uint							m_iNumBones = { };
	vector<_int>					m_BoneIndices;

	_float4x4*						m_pBoneMatrices = { nullptr };
	vector<_float4x4>				m_OffsetMatrices;

private:
	HRESULT Ready_VertexBuffer_For_NonAnim(const binMesh* pBinMesh, _fmatrix PreTransformMatrix);
	HRESULT Ready_VertexBuffer_For_Anim(const class CModel* pModel, const binMesh* pBinMesh);


public:
	static CMesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODEL_TYPE eType, const class CModel* pModel, const binMesh* pBinMesh, _fmatrix PreTransformMatrix);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END