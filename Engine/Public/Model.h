#pragma once

#include "Component.h"
#include "Animation.h"

NS_BEGIN(Engine)

class CTransform;

class ENGINE_DLL CModel final : public CComponent
{
public:
	typedef struct ComputeBoneInfo
	{
		_int		iParentIndex;
		_int3		_padding;
	}COMPUTE_BONEINFO;

	typedef struct ComputeChannelInfo
	{
		_uint		iBoneIndex;
		_uint		iCurrentKeyFrameIndex;
		_uint		iNumKeyFrames;
		_uint		iKeyFrameOffset;
	}COMPUTE_CHANNELINFO;

	typedef struct ComputeKeyFrameInfo
	{
		_float3			vScale;
		_float			padding01;
		_float4			vRotation;
		_float3			vTranslation;
		_float			fTrackPosition;
	}COMPUTE_KEYFRAMEINFO;

	typedef struct AnimationGlobalBuffer
	{
		_float4x4		g_PreTransformMatrix;
		_float			g_fCurrentTrackPosition;
		_float			g_fTimeDelta;
		_float			g_fTickPerSecond;
		_float			g_fDuration;
		
		_uint			g_bIsLoop;
		_uint			g_iNumBones;
		_uint			g_iNumChannels;
		_uint			g_iRootIndex;
	}COMPUTE_GLOBALBUFFER;

	typedef struct BoneTransformMatrixOut
	{
		_float4x4 BoneLocalTransformMatrix;
		_float4x4 BoneCombinedTransformMatrix;
	}COMPUTE_BONEMATRIX_OUT;

private:
	CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CModel(const CModel& Prototype);
	virtual ~CModel() = default;

public:
	_uint Get_Mesh_MaterialIndex(_uint iIdx) const;
	const _char* Get_MeshName(_uint iIdx) const;
	const _char* Get_MaterialName(_uint iIdx) const;
	_uint Get_NumMeshes() const {
		return m_iNumMeshes;
	}

	_int Get_BoneIndex(const _char* pBoneName) const;

	vector<class CBone*>* Get_Bones();

	_uint Get_AnimationKeyFrameIndex() const;

	const _float4x4* Get_BoneMatrixPtr(const _char* pBoneName) const;

	void Attach_CombinedTransformationMatrix();

	// GPU 스키닝을 위한 ID3D11Buffer 전달함수
	ID3D11Buffer* Get_BoneMatrixGPUBuffer() { return m_pOutSource; }

	// 모델인스턴싱을 위한 최소한의 함수
	void				Copy_MeshBuffer(_uint iMeshNum, ID3D11Buffer** VIBuffer, ID3D11Buffer** IndexBuffer);
	_uint				Get_MeshIndices(_uint iMeshNum);
	_uint				Get_MeshVertexStride(_uint iMeshNum);
	DXGI_FORMAT			Get_MeshIndexFormat(_uint iMeshNum);
	_float				Get_AnimationRatio() { return m_Animations[m_iCurrentAnimIndex]->Get_SaturatedTrackPosition(); }

	binModel* Get_RawModelDesc() { return m_pModel; }

public:
	void Set_AnimationIndex(_int iAnimIndex, _bool isLoop = true) {
		if (m_iCurrentAnimIndex == iAnimIndex)
			return;

		m_iCurrentAnimIndex = iAnimIndex;
		m_isLoop = isLoop;
		
		if(AnimationChanged)
			AnimationChanged(m_Animations[m_iCurrentAnimIndex]->Get_Name());

		m_Animations[m_iCurrentAnimIndex]->Reset();
	}

	void Set_Animation(const _char* szAnimationTag, _bool isLoop = true);

	vector<class CAnimation*>* Get_AnimationList() { return &m_Animations; }

	HRESULT Import_Animations(vector<class CAnimation*>* pAnimations);

	HRESULT Import_Texture(_uint iMeshIndex, TEXTURE_TYPE eType, const _char* pTextureFilePath, const _char* pBindTag = nullptr, _bool bIsSaved = FALSE);

	HRESULT Mapping_OffsetMatrix();
public:
	virtual HRESULT Initialize_Prototype(MODEL_TYPE eType, const _char* pModelFilePath, _fmatrix PreTransformMatrix, CModel* pSkeletonModel);
	virtual HRESULT Initialize(void* pArg) override;
	HRESULT Bind_BoneMatrices(_uint iMeshIndex, class CShader* pShader, const _char* pConstantName);
	HRESULT Bind_BoneSRV(_uint iMeshIndex, class CShader* pShader, const _char* pConstantName);
	HRESULT Bind_Material(_uint iMeshIndex, class CShader* pShader, const _char* pConstantName, aiTextureType eType, _uint iTextureIndex);
	HRESULT Bind_AllMaterials(_uint iMeshIndex, class CShader* pShader, _uint iTextureIndex);
	_bool Play_Animation(_float fTimeDelta, CTransform* pTransform = nullptr,  _float fRootMotionMagnification = 0.f);

	HRESULT Bind_MaterialTag(TEXTURE_TYPE eType, const _char* szBindTag);
	HRESULT Bind_BoneMatrixSRV(CShader* pShader, const _char* pConstantName);
	HRESULT Bind_PreBoneMatrixSRV(CShader* pShader);
	HRESULT Bind_GlobalOffsetMatrices(CShader* pShader);

	_bool			IsAnimationFinished() { return m_isFinish; }

	aiTextureType Convert_TextureType(TEXTURE_TYPE eType);


	virtual HRESULT Render(_uint iMeshIndex);

public:
	std::function<void(const _char*)> AnimationChanged;

private:
	MODEL_TYPE					m_eType = {};
	_float4x4					m_PreTransformMatrix = {};

	binModel* m_pModel;
	class CComputeShader*		m_pComputeShaderCom = { nullptr };

	COMPUTE_GLOBALBUFFER		m_GlobalBuffer;

	ID3D11Buffer*				m_pBoneSource = { nullptr };
	ID3D11Buffer*				m_pChannelSource = { nullptr };
	ID3D11Buffer*				m_pKeyFrameSource = { nullptr };
	ID3D11Buffer*				m_pOutSource = { nullptr };
	ID3D11Buffer*				m_pRootSource = { nullptr };
	ID3D11Buffer*				m_pPreBoneMatrices = { nullptr };
	ID3D11Buffer*				m_pOutReadBack = { nullptr };

	ID3D11ShaderResourceView*	m_pBoneMatricesSRV = { nullptr };
	ID3D11ShaderResourceView*	m_pPreBoneMatricesSRV = { nullptr };

	_uint						m_iNumMeshes = {};
	vector<class CMesh*>		m_Meshes;

	_uint						m_iNumMaterials = {};
	vector<class CMaterial*>	m_Materials;

	vector<class CBone*>		m_Bones;
	vector<_float4x4>			m_GlobalOffsetMatrices;

	_float4x4					m_PreRootMatrix{};
	_float4x4					m_CurRootMatrix{};

	_int						m_iCurrentAnimIndex = { -1 };
	_uint						m_iNumAnimations = {};
	_bool						m_isLoop = { false };
	_bool						m_isFinish = { false };
	_int						m_iFlagPreRootModified = ROOTFLAG_RESET;
	vector<class CAnimation*>	m_Animations;

	_char						m_szBindTags[ENUM_CLASS(TEXTURE_TYPE::END)][MAX_PATH];

#ifdef _DEBUG
	_char						m_ModelFilePath[MAX_PATH];
#endif

private:
	HRESULT Ready_Meshes();
	HRESULT Ready_Meshes(CModel* pSkeleton);
	HRESULT Ready_Materials(const _char* pModelFilePath);
	HRESULT Ready_Bones(binNode* pNode, _int iParentIndex);
	HRESULT Ready_Animations();
	HRESULT Ready_ComputeShader();

	HRESULT Mapping_Skeleton(CModel* pSkeletonModel);

	HRESULT Mapping_Animation(class CAnimation* pAnimation);
	HRESULT Bind_ComputeShader(_float fTimeDelta);

	HRESULT Apply_RootMotion(CTransform* pTransform, _float fRootMotionMagnification);

	


public:
	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODEL_TYPE eType, const _char* pModelFilePath, _fmatrix PreTransformMatrix = XMMatrixIdentity(), CModel* pSkeletonModel = nullptr);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free(); public:
};

NS_END