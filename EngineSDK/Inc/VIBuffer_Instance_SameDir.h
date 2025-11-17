#pragma once
#include "VIBuffer_Instance.h"

NS_BEGIN(Engine)
class CModel;
class CShader;

class ENGINE_DLL CVIBuffer_Instance_SameDir : public CVIBuffer_Instance
{
public:
	typedef struct tagModelInstanceDesc : public INSTANCE_DESC
	{
		const char* pModelFilePath;
		_matrix							PreModelMatrix;

		//나중에 리타겟팅 해야하면 리타겟팅하는거 추가
	}MODEL_INSTANCE_DESC;

private:
	CVIBuffer_Instance_SameDir(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Instance_SameDir(const CVIBuffer_Instance_SameDir& Prototype);
	virtual ~CVIBuffer_Instance_SameDir() = default;

public:
	virtual HRESULT						Initialize_Prototype(const INSTANCE_DESC* pInstanceDesc) override;
	virtual HRESULT						Initialize(void* pArg) override;
	virtual HRESULT						Render(_uint iIndex);

	HRESULT								Bind_MatrialTexture(CShader* pShader, _uint iMeshIndex, const _char* pConstantName, TEXTURE_TYPE eTextureType, _uint TextureIndex);
	_uint								GetModelNumMeshes();

private:
	VTX_INSTANCE_MODEL* m_pInstanceVertices = { nullptr };
	CModel* m_pModel = nullptr;

	_float3								m_vPivot = {};
	_bool								m_bFlag = { true };

private:
	HRESULT								Bind_Resource(_uint iMeshIndex);

public:
	static CVIBuffer_Instance_SameDir* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const INSTANCE_DESC* pInstanceDesc);
	virtual CComponent* Clone(void* pArg) override;
	virtual void						Free() override;

};
NS_END