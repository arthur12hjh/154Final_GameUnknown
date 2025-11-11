#ifndef Engine_Struct_h__
#define Engine_Struct_h__

namespace Engine
{
	typedef struct CameraInfo
	{
		float			fFov;
		float			fNear;
		float			fFar;
		float			fAspect;
	}CAMERA_INFO;

	typedef struct ThreadJob
	{
		unsigned int		iJobID;
		function<void()>	JobFunction;
		bool				bIsCanceled;
	}THREAD_JOB;
	using ThreadJobHandle = THREAD_JOB;

	typedef struct tagEngineDesc
	{
		HINSTANCE			hInstance;
		HWND				hWnd;
		unsigned int		iNumLevels;
		unsigned int		iWinSizeX, iWinSizeY;
		WINMODE				eWindowMode;
	}ENGINE_DESC;

	typedef struct tagLightDesc
	{
		LIGHT_TYPE			eType;
		XMFLOAT4			vDiffuse;
		XMFLOAT4			vAmbient;
		XMFLOAT4			vSpecular;

		XMFLOAT4			vPosition;
		XMFLOAT4			vDirection;        /* 광원의 빛의 방향 */
		float				fRange;            /* 빛의 최대 진행 거리 방향성 광원의 경우 의이없음 */
											   /* 루트 FLT_MAX의 값을 넘어갈수 없다.  */

		float				fFalloff;			/* 스포트 광원에서 사용 보통 1.0f 설정하며     */
												/* 안쪽 원뿔과 바깥쪽 원뿔간의 빛의 세기 차이  */

		float				fAttenuation0;		/* 상수 감쇠 계수 */
		float				fAttenuation1;		/* 선형 감쇠 계수 */
		float				fAttenuation2;		/* 이차 감쇠 계수 */
		float				fTheta;				/* 스포트 광원의 안쪽 원뿔의 각도 */
		float				fPhi;				/* 스포트 광원의 바깥쪽 원뿔의 각도 */
	}LIGHT_DESC;

	typedef struct tagShadowLight
	{
		XMFLOAT4		vEye, vAt;
		float			fNear, fFar, fFovy, fAspect;
	}SHADOW_LIGHT_DESC;

	typedef struct tagVertexPosition
	{
		XMFLOAT3			vPosition;	

		static constexpr unsigned int					iNumElements = { 1 };
		static constexpr D3D11_INPUT_ELEMENT_DESC		Elements[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			
		};

	}VTXPOS;

	//physx 용
	typedef struct tagVertexPositionColor
	{
		XMFLOAT3			vPosition;
		XMFLOAT4			vColor;

		static constexpr unsigned int					iNumElements = { 2 };
		static constexpr D3D11_INPUT_ELEMENT_DESC		Elements[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};

	}VTXPOSCOLOR;

	typedef struct tagVertexPositionTexcoord
	{
		XMFLOAT3			vPosition;
		XMFLOAT2			vTexcoord;

		static constexpr unsigned int					iNumElements = { 2 };
		static constexpr D3D11_INPUT_ELEMENT_DESC		Elements[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
		};

	}VTXPOSTEX;

	typedef struct tagVertexCube
	{
		XMFLOAT3			vPosition;
		XMFLOAT3			vTexcoord;

		static constexpr unsigned int					iNumElements = { 2 };
		static constexpr D3D11_INPUT_ELEMENT_DESC		Elements[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
		};

	}VTXCUBE;
	
	typedef struct tagVertexPositionNormalTexcoord
	{
		XMFLOAT3			vPosition;
		XMFLOAT3			vNormal;
		XMFLOAT2			vTexcoord;

		static constexpr unsigned int					iNumElements = { 3 };
		static constexpr D3D11_INPUT_ELEMENT_DESC		Elements[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0}
		};
	}VTXNORTEX;

	typedef struct tagVertexMesh
	{
		XMFLOAT3			vPosition;
		XMFLOAT3			vNormal;
		XMFLOAT3			vTangent;
		XMFLOAT3			vBinormal;
		XMFLOAT2			vTexcoord;		

		static constexpr unsigned int					iNumElements = { 5 };
		static constexpr D3D11_INPUT_ELEMENT_DESC		Elements[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{ "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 48, D3D11_INPUT_PER_VERTEX_DATA, 0}
		};
	}VTXMESH;


	typedef struct tagVertexAnimationMesh
	{
		XMFLOAT3			vPosition;
		XMFLOAT3			vNormal;
		XMFLOAT3			vTangent;
		XMFLOAT3			vBinormal;
		XMFLOAT2			vTexcoord;

		/* 이 정점이 어떤 뼈들의 상태를 받아야하는가? */
		XMUINT4				vBlendIndex;
		/* 각 뼈의 상태가 어떤 비율로 적용되야할지? */
		XMFLOAT4			vBlendWeight;

		static constexpr unsigned int					iNumElements = { 7 };
		static constexpr D3D11_INPUT_ELEMENT_DESC		Elements[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{ "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 48, D3D11_INPUT_PER_VERTEX_DATA, 0}, 
			{ "BLENDINDEX", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, 56, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "BLENDWEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 72, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};
	}VTXANIMMESH;

	typedef struct tagVertexInstance_Model
	{
		XMFLOAT4			vRight;
		XMFLOAT4			vUp;
		XMFLOAT4			vLook;
		XMFLOAT4			vTranslation;

	}VTX_INSTANCE_MODEL;

	typedef struct tagVertexInstance_Particle
	{
		XMFLOAT4			vRight;
		XMFLOAT4			vUp;
		XMFLOAT4			vLook;
		XMFLOAT4			vTranslation;

		XMFLOAT2			vLifeTime;
	}VTX_INSTANCE_PARTICLE;



	typedef struct tagVertexPosTexInstanceParticleDesc
	{		
		static constexpr unsigned int					iNumElements = { 7 };
		static constexpr D3D11_INPUT_ELEMENT_DESC		Elements[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},			
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
			
			{ "TEXCOORD", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
			{ "TEXCOORD", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
			{ "TEXCOORD", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
			{ "TEXCOORD", 4, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
			{ "TEXCOORD", 5, DXGI_FORMAT_R32G32_FLOAT, 1, 64, D3D11_INPUT_PER_INSTANCE_DATA, 1 }
		};
	}VTX_POSTEX_INSTANCE_PARTICLE;

	typedef struct tagVertexPosInstanceParticleDesc
	{
		static constexpr unsigned int					iNumElements = { 6 };
		static constexpr D3D11_INPUT_ELEMENT_DESC		Elements[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},			

			{ "WORLD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
			{ "WORLD", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
			{ "WORLD", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
			{ "WORLD", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 1, 64, D3D11_INPUT_PER_INSTANCE_DATA, 1 }
		};
	}VTX_POS_INSTANCE_PARTICLE;

	typedef struct tagKeyFrame
	{
		XMFLOAT3		vScale;
		XMFLOAT4		vRotation;
		XMFLOAT3		vTranslation;
		float			fTrackPosition;
	}KEYFRAME;

#pragma region MODEL_IMPORT

	typedef struct binFace
	{
		unsigned int				iNumIndices;
		vector<unsigned int>		vIndices;
	}BINFACE;

	typedef struct binNode
	{
		unsigned int		iNumChildren;
		XMFLOAT4X4			matTransformation;
		char				szName[MAX_PATH];
		vector<unsigned int>vChildrenIndex;
	}BINNODE;

	typedef struct binVertexWeight
	{
		unsigned int			iVertexId;
		float					fWeight;
	}BINVERTEXWEIGHT;

	typedef struct binVectorKey
	{
		float					fTime;
		XMFLOAT4				vValue;
	}BINVECTORKEY;

	typedef struct binBone
	{
		char					szName[MAX_PATH];
		unsigned int			iNumWeights;
		vector<binVertexWeight>	vWeights;
		XMFLOAT4X4				OffsetMatrix;
		//binNode*				pNode;
	}BINBONE;

	typedef struct binChannel
	{
		char szName[MAX_PATH];
		unsigned int iNumScalingKeys;
		unsigned int iNumRotationKeys;
		unsigned int iNumPositionKeys;
		vector<binVectorKey> cScalingKeys;
		vector<binVectorKey> cRotationKeys;
		vector<binVectorKey> cPositionKeys;
	}BINCHANNEL;

	typedef struct binAnimation
	{
		char					szName[MAX_PATH];
		float					fDuration;
		float					fTicksPerSecond;
		unsigned int			iNumChannels;
		vector<binChannel>		vChannels;
	}BINANIMATION;

	typedef struct binMesh
	{
		unsigned int				iNumVertices;
		unsigned int				iNumFaces;
		unsigned int				iNumBones;
		unsigned int				iMaterialIndex;
		vector<XMFLOAT3>			vPositions;
		vector<XMFLOAT3>			vNormals;
		vector<XMFLOAT3>			vTangents;
		vector<XMFLOAT3>			vBinormals;
		vector<vector<XMFLOAT2>>	vTextureCoords;
		vector<binFace>				vFaces;
		vector<binBone>				vBones;
		char						szName[MAX_PATH];
	}BINMESH;

	typedef struct binMaterial
	{
		enum TEXTURETYPE {
			NONE, DIFFUSE, SPECULAR, AMBIENT, EMISSIVE, HEIGHT,
			NORMALS, SHININESS, OPACITY, DISPLACEMENT,
			LIGHTMAP, REFLECTION, BASE_COLOR, NORMAL_CAMERA,
			EMISSIVE_COLOR, METALNESS, DIFFUSE_ROUGHNESS,
			AMBIENT_OCCLUSION, UNKNOWN, SHEEN, CLEARCOAT,
			TRANSMISSION, MAYA_BASE, MAYA_SPECULAR,
			MAYA_SPECULAR_COLOR, MAYA_SPECULAR_ROUGHNESS,
			ANISOTROPY, GLTF_METALLIC_ROUGHNESS, END
		};

		vector<unsigned int> vNumSRVs;
		vector<string> strTexturePaths[TEXTURETYPE::END];
	}BINMATERIAL;

	typedef struct binModel
	{
		unsigned int				iNumMeshes;
		unsigned int				iNumMaterials;
		unsigned int				iNumAnimations;
		unsigned int				iRootNodeIndex; //RootNode unsigned int 인덱스로 해야함
		vector<binNode>				vNodes;
		vector<binMesh>				vMeshes;
		vector<binMaterial>			vMaterials;
		vector<binAnimation>		vAnimations;
	}BINMODEL;

#pragma endregion

}

#endif // Engine_Struct_h__
