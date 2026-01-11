#pragma once

#include "Component.h"
#include "Animation.h"

NS_BEGIN(Engine)

class CTransform;

class ENGINE_DLL CModel : public CComponent
{
private:
	CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CModel(const CModel& Prototype);
	virtual ~CModel() = default;

public:
	// <Mesh 정보 받는 함수들>
	_uint Get_Mesh_MaterialIndex(_uint iIdx) const;
	const _char* Get_MeshName(_uint iIdx) const;
	const _uint Get_NumMeshes() const { return m_iNumMeshes; }
	vector<class CMesh*>* Get_MeshList() { return &m_Meshes; }
	class CMesh* Get_Mesh(_int iMeshIndex = 0) { return m_Meshes[iMeshIndex]; }
	// </end>

	// <머티리얼 정보 받는 함수들>
	vector<class CMaterial*>* Get_Materials();
	const _char* Get_MaterialName(_uint iIdx) const;
	const _uint Get_NumMaterials() const { return m_Materials.size(); }
	// </end>

	// <애니메이션 함수들>
	vector<class CAnimation*>* Get_Animations();


	_float Get_CurrentMorphDuration();

	_uint Get_CurrentMorphTrackPosition();
	_float	Get_fCurrentMorphTrackPosition();
	_float Get_CurrentMorphSaturatedTrackPosition();
	void Set_MorphTrackPosition(_float fTrackPosition);
	// </end>

	// <Bone 정보 받는 함수들>
	vector<class CBone*>* Get_Bones();
	_int Get_BoneIndex(const _char* pBoneName) const;
	// </end>

	// <ShapeKey, MorphAnimation 전용 함수들>
	_int Get_ShapeIndex(const _char* pShapeName) const; //안써시발아
	void Set_ShapeWeightIndex(_uint iShapeKeyIndex, _float fWeight);
	void Set_ShapeWeight(const _char* szShapeTag, _float fWeight);
	void Bind_ShapeWeight(_uint iMeshIndex, class CShader* pShader);

	void Reset_ShapeWeight();

	unordered_map<string, _int>* Get_ShapeKeyNames() { return &m_ShapeKeyIndexMap; }
	// </end>

	_uint Get_AnimationKeyFrameIndex() const;
	_float Get_fTrackPosition() const;

	const _float4x4* Get_BoneMatrixPtr(const _char* pBoneName);
	const _float4x4* Get_LocalBoneMatrixPtr(const _char* pBoneName);
	// 더 이상 쓰지 않음.
	//누구맘대로누구맘대로누구맘대로누구맘대로누구맘대로누구맘대로누구맘대로누구맘대로
	void Attach_CombinedTransformationMatrix();

	// 피직스 본 매핑을 위함.
	void Set_CombinedTransformationMatrix(const _char* pBoneName, _fmatrix CombinedMatrix);
	void Override_CombinedTransformationMatrix(const _char* pBoneName, _fmatrix CombinedMatrix);
	// GPU 스키닝을 위한 ID3D11Buffer 전달함수
	ID3D11Buffer* Get_BoneMatrixGPUBuffer() { return m_pOutSource; }

	HRESULT Set_AnimationKeyFrameIndex(_uint iKeyFrameIndex);

	// 모델인스턴싱을 위한 최소한의 함수
	void				Copy_MeshBuffer(_uint iMeshNum, ID3D11Buffer** VIBuffer, ID3D11Buffer** IndexBuffer);
	_uint				Get_MeshIndices(_uint iMeshNum);
	_uint				Get_MeshVertexStride(_uint iMeshNum);
	DXGI_FORMAT			Get_MeshIndexFormat(_uint iMeshNum);
	_float				Get_AnimationRatio();

	// binModel 구조체로 받는 함수. 만들긴 했는데 쓸모는 없을듯
	binModel* Get_RawModelDesc() { return m_pModel; }

	void Set_PreTransformMatrix(_fmatrix PreTransformMatrix) { XMStoreFloat4x4(&m_PreTransformMatrix, PreTransformMatrix); }

public:
	// <애니메이션 관련 함수들. 인덱스, wstring, string. 자세한 정보는 디코 자료탭 존난애강 참고>
	void Set_AnimationIndex(_int iAnimIndex,
		_bool isLoop = true,
		_float fLerpDuration = 0.12f,
		_bool bIsRestart = FALSE,
		_float fEndTrackPosition = -1.f,
		_float fStartTrackPosition = 0.f,
		_bool isResetTrackPosition = TRUE,
		_bool isRootMotionUpdated = TRUE);

	void Set_Animation(const _wstring& strAnimationTag,
		_bool isLoop = true,
		_float fAnimationPlayRate = 1.f,
		_float fLerpDuration = 0.12f,
		_bool bIsRestart = FALSE,
		_float fEndTrackPosition = -1.f,
		_float fStartTrackPosition = 0.f,
		_bool isResetTrackPosition = TRUE,
		_bool isRootMotionUpdated = TRUE);

	void Set_Animation(const _char* szAnimationTag,
		_bool isLoop = true,
		_float fAnimationPlayRate = 1.f,
		_float fLerpDuration = 0.12f,
		_bool bIsRestart = FALSE,
		_float fEndTrackPosition = -1.f,
		_float fStartTrackPosition = 0.f,
		_bool isResetTrackPosition = TRUE,
		_bool isRootMotionUpdated = TRUE);
	// </end>

	// <모프 애니메이션 설정하는 함수들>
	void Set_MorphAnimationIndex(_int iAnimIndex,
		_bool isLoop = true,
		_float fAnimationPlayRate = 1.f,
		_float fLerpDuration = 0.12f,
		_bool bIsRestart = FALSE,
		_float fEndTrackPosition = -1.f,
		_float fStartTrackPosition = 0.f,
		_bool isResetTrackPosition = TRUE);

	void Set_MorphAnimation(const _char* szAnimationTag,
		_bool isLoop = true,
		_float fAnimationPlayRate = 1.f,
		_float fLerpDuration = 0.12f,
		_bool bIsRestart = FALSE,
		_float fEndTrackPosition = -1.f,
		_float fStartTrackPosition = 0.f,
		_bool isResetTrackPosition = TRUE);
	// </end>


	vector<class CAnimation*>* Get_AnimationList() { return &m_Animations; }

	HRESULT Initialize_ShapeKeyIndexMap();
	HRESULT Initialize_AnimationIndexMap();
	HRESULT Initialize_AnimationBufferResource();

	HRESULT Release_AnimationChannel();

	// <모델의 기존 값들을 변경하는 함수들>
	// Import_Animations를 제외하면, 나머지는 binx에 저장된다.
	HRESULT Import_Animations(vector<class CAnimation*>* pAnimations, _char* szName);
	HRESULT Import_Texture(_uint iMeshIndex, TEXTURE_TYPE eType, const _char* pTextureFilePath, const _char* pBindTag = nullptr, _bool bIsSaved = FALSE);
	HRESULT Change_BoneTag(const _char* szAfterBoneTag, const vector<string>& szTargetTagList);
	// </end>

	// <본매핑을 위해 사용할 본들이 누구들인지 입력하는 함수>
	HRESULT AddCount_PartialBone(const _char* pBoneName);
	HRESULT ReleaseCount_PartialBone(const _char* pBoneName);

public:
	virtual HRESULT Initialize_Prototype(MODEL_TYPE eType, const _char* pModelFilePath, _fmatrix PreTransformMatrix, CModel* pSkeletonModel);
	virtual HRESULT Initialize(void* pArg) override;
	HRESULT Bind_BoneMatrices(_uint iMeshIndex, class CShader* pShader, const _char* pConstantName);
	HRESULT Bind_BoneSRV(_uint iMeshIndex, class CShader* pShader, const _char* pConstantName);
	HRESULT Bind_ShapeKeys(_uint iMeshIndex, class CShader* pShader, const _char* pConstantName);
	HRESULT Bind_Material(_uint iMeshIndex, class CShader* pShader, const _char* pConstantName, aiTextureType eType, _uint iTextureIndex);
	HRESULT Bind_AllMaterials(_uint iMeshIndex, class CShader* pShader, _uint iTextureIndex);
	_bool Play_Animation(_float fTimeDelta, CTransform* pTransform = nullptr, _float fRootMotionMagnification = 0.f);
	_bool Play_MorphAnimation(_float fTimeDelta);

	// 애니메이션이 GPU단에서 처리됨에 따라, 각 Mesh에 별개로 있는 오프셋 매트릭스를 Bone처럼 한 곳에 묶어 인덱스 매핑해주는 초기화 함수
	HRESULT Mapping_OffsetMatrix();

	HRESULT Bind_MaterialTag(TEXTURE_TYPE eType, const _char* szBindTag);
	HRESULT Bind_BoneMatrixSRV(CShader* pShader, const _char* pConstantName);
	HRESULT Bind_PreBoneMatrixSRV(CShader* pShader);
	HRESULT Bind_GlobalOffsetMatrices(CShader* pShader);

	_bool			IsAnimationFinished() { return m_isFinish; }
	_bool			CompareAnimationTag(const _char* szAnimationTag);
	aiTextureType	Convert_TextureType(TEXTURE_TYPE eType);

	// 이거 모션트레일 전용 PreBoneMatrix Copy해오는겁니다.
	ID3D11Buffer* Get_PreBoneMatrix();


	virtual HRESULT Render(_uint iMeshIndex);


public:
	std::function<void(const _char*)> AnimationChanged;

private:
	MODEL_TYPE					m_eType = {};
	_float4x4					m_PreTransformMatrix = {};

	binModel* m_pModel;
	class CComputeShader* m_pComputeShaderCom = { nullptr };
	class CComputeShader* m_pCombinedMatrixComputeShaderCom = { nullptr };

	COMPUTE_GLOBALBUFFER			m_GlobalBuffer;

	ID3D11Buffer* m_pOutSource = { nullptr };
	ID3D11Buffer* m_pRootSource = { nullptr };
	ID3D11Buffer* m_pPreBoneMatrices = { nullptr };
	ID3D11Buffer* m_pLerpBoneMatrices = { nullptr };
	ID3D11Buffer* m_pOutReadBack = { nullptr };
	ID3D11Buffer* m_pOutRootReadBack = { nullptr };

	ID3D11ShaderResourceView* m_pBoneMatricesSRV = { nullptr };
	ID3D11ShaderResourceView* m_pPreBoneMatricesSRV = { nullptr };

	unordered_map<string, _int>		m_AnimationIndexMap;
	unordered_map<_int, _int>		m_PartialBoneCountMap;

	_uint							m_iNumMeshes = {};
	vector<class CMesh*>			m_Meshes;

	_uint							m_iNumMaterials = {};
	vector<class CMaterial*>		m_Materials;

	vector<class CBone*>			m_Bones;
	vector<_float4x4>				m_GlobalOffsetMatrices;

	vector<ID3D11ShaderResourceView*>		m_pChannelSRVList;
	vector<ID3D11ShaderResourceView*>		m_pKeyFrameSRVList;

#pragma region FACIAL METHOD
	// Mesh단으로 내려가서 주석처리한 변수들
	//_uint							m_iNumShapeKeys;
	//vector<class CShapeKey*>		m_ShapeKeys;
	//vector<_float>					m_ShapeKeyWeights;
	unordered_map<string, _int>		m_ShapeKeyIndexMap;

	_uint							m_iNumMorphAnimations;
	_int							m_iNumShapeKeys;
	_int							m_iCurrentMorphAnimIndex = { -1 };
	_bool							m_isMorphLoop = { false };
	_bool							m_isMorphFinish = { false };
	_float							m_fMorphAnimationPlayRate = 1.f;
	vector<class CMorphAnimation*>	m_MorphAnimations;

	_float						m_fMorphEndTrackPosition = -1.f;
	_float						m_fMorphStartTrackPosition = 0.f;

	_float							m_ShapeKeyWeights[MAX_SHAPEKEY];
	ID3D11Buffer*					m_pShapeKeyWeightBuffer = { nullptr };
	ID3D11ShaderResourceView*		m_pShapeKeyWeightSRV = { nullptr };
#pragma endregion

	_float4x4					m_PreRootMatrix{};
	_float4x4					m_CurRootMatrix{};

	_float						m_fAnimationPlayRate = 1.f;
	_int						m_iRootIndex = 0;

	_float						m_fBlendElapsed = 0.f;
	_float						m_fBlendRatio = 0.f;
	_float						m_fBlendDuration = 0.12f;

	_float						m_fEndTrackPosition = -1.f;
	_float						m_fStartTrackPosition = 0.f;

	_int						m_iCurrentAnimIndex = { -1 };
	_uint						m_iNumAnimations = {};
	_int						m_iFlagPreRootModified = iFLAG_ROOT_RESET;
	_bool						m_isLoop = { false };
	_bool						m_isFinish = { false };
	vector<class CAnimation*>	m_Animations;

	_char						m_szBindTags[ENUM_CLASS(TEXTURE_TYPE::END)][MAX_PATH];

	_bool						m_isLerp = { FALSE };
	_bool						m_isRootMotionUpdated = { TRUE };

#ifdef _DEBUG
	_char						m_ModelFilePath[MAX_PATH];
#endif

private:
	HRESULT Ready_Meshes();
	HRESULT Ready_Materials(const _char* pModelFilePath);
	HRESULT Ready_Bones(binNode* pNode, _int iParentIndex);
	HRESULT Ready_Animations();

#pragma region FACIAL METHOD
	// 페이셜 모델 관련 처리들. ShapeKey(AnimMesh)와 MorphAnimation은 사실 Mesh단에서 처리해줘야한다.
	// 굳이 그런 복잡한 로직 별로임 ㅇㅅㅇ.. 걍 모델로 끌고 나오자
	void Apply_MorphState(const MORPH_KEYFRAME* pCurrent);
	
	HRESULT Bind_MorphAnimations(_float fTimeDelta);
	
	HRESULT Ready_MorphAnimations();
	HRESULT Ready_ShapeKeyWeightBuffer();
#pragma endregion

	HRESULT Ready_ComputeShader();

	HRESULT Ready_SkeletonBones(CModel* pSkeleton);

	HRESULT Mapping_Animation(class CAnimation* pAnimation);
	HRESULT Bind_ComputeShader(_float fTimeDelta);

	HRESULT Apply_RootMotion(CTransform* pTransform, _float fRootMotionMagnification);

	HRESULT Bind_ChannelAndKeyFrameBuffer();

	HRESULT Update_BoneMatrices();

	_int Find_Animation(const _char* szAnimationTag);
	
	_uint Get_TotalAnimationKeyFrame(class CAnimation* pAnimation);


public:
	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODEL_TYPE eType, const _char* pModelFilePath, _fmatrix PreTransformMatrix = XMMatrixIdentity(), CModel* pSkeletonModel = nullptr);
	virtual CComponent* Clone(void* pArg) override;
virtual void Free(); public:
};

NS_END