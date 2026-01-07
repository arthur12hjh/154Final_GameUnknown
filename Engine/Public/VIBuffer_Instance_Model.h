#pragma once
#include "VIBuffer_Instance.h"

NS_BEGIN(Engine)
class CModel;
class CShader;

class ENGINE_DLL CVIBuffer_Instance_Model : public CVIBuffer_Instance
{
public:
	typedef struct tagModelInstanceDesc : public INSTANCE_DESC
	{
		const char* pModelFilePath;
		_matrix							PreModelMatrix;
		//나중에 리타겟팅 해야하면 리타겟팅하는거 추가
	}MODEL_INSTANCE_DESC;

public:

private:
	CVIBuffer_Instance_Model(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Instance_Model(const CVIBuffer_Instance_Model& Prototype);
	virtual ~CVIBuffer_Instance_Model() = default;

public:
	virtual HRESULT						Initialize_Prototype(const INSTANCE_DESC* pInstanceDesc) override;
	virtual HRESULT						Initialize(void* pArg) override;
	virtual HRESULT						Render(_uint iIndex);

	HRESULT								Bind_MatrialTexture(CShader* pShader, _uint iMeshIndex, const _char* pConstantName, aiTextureType eTextureType, _uint TextureIndex);
	_uint								GetModelNumMeshes();
	_uint								Get_NumInstance() const { return m_iNumInstance; }
	VTX_INSTANCE_MODEL* Get_InstanceVertices() const { return m_pInstanceVertices; }

private:
	VTX_INSTANCE_MODEL*					m_pInstanceVertices = { nullptr };
	CModel* m_pModel = nullptr;

	_float3								m_vPivot = {};
	_bool								m_bFlag = { true };

private:
	HRESULT								Bind_Resource(_uint iMeshIndex);

public:
	static CVIBuffer_Instance_Model* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const INSTANCE_DESC* pInstanceDesc);
	virtual CComponent* Clone(void* pArg) override;
	virtual void						Free() override;

};
NS_END