#pragma once
#include "VIBuffer_Instance.h"

NS_BEGIN(Engine)
class CModel;
class CShader;

class ENGINE_DLL CVIBuffer_Instance_MeshParticle : public CVIBuffer_Instance
{
public:
	typedef struct tagMeshInstanceDesc : public INSTANCE_DESC
	{
		_float2							vLifeTime;
		_float2							vSpeed;
	}MESH_PARTICLE_INSTANCE_DESC;

	typedef struct tagModelInstance_Particle final : public VTX_INSTANCE_MODEL
	{
		_float2		vLifeTime;

		_float4x4	WorldMat = {};
		_float4	vRoot;
		_float4	vStart;
		_float2 vSpeeds;
		_float	vSize;
	}VTX_INSTANCE_MODEL_PARTICLE;

public:

private:
	CVIBuffer_Instance_MeshParticle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Instance_MeshParticle(const CVIBuffer_Instance_MeshParticle& Prototype);
	virtual ~CVIBuffer_Instance_MeshParticle() = default;

public:
	virtual HRESULT						Initialize_Prototype(const INSTANCE_DESC* pInstanceDesc) override;
	virtual HRESULT						Initialize(void* pArg) override;
	virtual HRESULT						Render(_uint iIndex);

	HRESULT								Bind_MatrialTexture(CShader* pShader, _uint iMeshIndex, const _char* pConstantName, aiTextureType eTextureType, _uint TextureIndex);
	_uint								GetModelNumMeshes();
	_uint								Get_NumInstance() const { return m_iNumInstance; }
	VTX_INSTANCE_MODEL_PARTICLE* Get_InstanceVertices() const { return m_pInstanceVertices; }
	void								Set_Model(CModel* pModel) { m_pModel = pModel; }

private:
	VTX_INSTANCE_MODEL_PARTICLE* m_pInstanceVertices = { nullptr };
	CModel* m_pModel = nullptr;

	_float3								m_vPivot = {};
	_bool								m_bFlag = { true };

private:
	HRESULT								Bind_Resource(_uint iMeshIndex);

public:
	static CVIBuffer_Instance_MeshParticle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const INSTANCE_DESC* pInstanceDesc);
	virtual CComponent* Clone(void* pArg) override;
	virtual void						Free() override;

};
NS_END