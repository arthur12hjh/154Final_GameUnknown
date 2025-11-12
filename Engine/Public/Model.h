#pragma once

#include "Component.h"
#include "Animation.h"

NS_BEGIN(Engine)

class ENGINE_DLL CModel final : public CComponent
{
private:
	CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CModel(const CModel& Prototype);
	virtual ~CModel() = default;

public:
	_uint Get_NumMeshes() const {
		return m_iNumMeshes;
	}

	_int Get_BoneIndex(const _char* pBoneName) const;
	
	vector<class CBone*>* Get_Bones();
	
	_uint Get_AnimationKeyFrameIndex() const;

	const _float4x4* Get_BoneMatrixPtr(const _char* pBoneName) const;

	void Attach_CombinedTransformationMatrix();

	// 모델인스턴싱을 위한 최소한의 함수
	void				Copy_MeshBuffer(_uint iMeshNum, ID3D11Buffer** VIBuffer, ID3D11Buffer** IndexBuffer);
	_uint				Get_MeshIndices(_uint iMeshNum);
	_uint				Get_MeshVertexStride(_uint iMeshNum);
	DXGI_FORMAT			Get_MeshIndexFormat(_uint iMeshNum);

public:
	void Set_AnimationIndex(_int iAnimIndex, _bool isLoop = true) {
		if (m_iCurrentAnimIndex == iAnimIndex)
			return;

		m_iCurrentAnimIndex = iAnimIndex;
		m_isLoop = isLoop;

		m_Animations[m_iCurrentAnimIndex]->Reset();
	}

	void Set_Animation(const _char* szAnimationTag);

	vector<class CAnimation*>* Get_AnimationList() { return &m_Animations; }

public:
	virtual HRESULT Initialize_Prototype(MODEL_TYPE eType, const _char* pModelFilePath, _fmatrix PreTransformMatrix);
	virtual HRESULT Initialize(void* pArg) override;
	HRESULT Bind_BoneMatrices(_uint iMeshIndex, class CShader* pShader, const _char* pConstantName);
	HRESULT Bind_Material(_uint iMeshIndex, class CShader* pShader, const _char* pConstantName, aiTextureType eType, _uint iTextureIndex);
	_bool Play_Animation(_float fTimeDelta);
	virtual HRESULT Render(_uint iMeshIndex);
	
private:
	MODEL_TYPE					m_eType = {};
	_float4x4					m_PreTransformMatrix = {};

	binModel*					m_pModel;

	_uint						m_iNumMeshes = {};
	vector<class CMesh*>		m_Meshes;

	_uint						m_iNumMaterials = {};
	vector<class CMaterial*>	m_Materials;

	vector<class CBone*>		m_Bones;

	_int						m_iCurrentAnimIndex = { -1 };
	_uint						m_iNumAnimations = {};
	_bool						m_isLoop = { false };
	_bool						m_isFinish = { false };
	vector<class CAnimation*>	m_Animations;	
private:
	HRESULT Ready_Meshes();
	HRESULT Ready_Materials(const _char* pModelFilePath);
	HRESULT Ready_Bones(binNode* pNode, _int iParentIndex);
	HRESULT Ready_Animations();


public:
	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODEL_TYPE eType, const _char* pModelFilePath, _fmatrix PreTransformMatrix = XMMatrixIdentity());
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free(); public:
};

NS_END