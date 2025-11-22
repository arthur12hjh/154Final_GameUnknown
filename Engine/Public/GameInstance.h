#pragma once

#include "Prototype_Manager.h"

NS_BEGIN(Engine)

class CCamera;
class CInteraction_Component;
class CEventHandle;
class CCinemaTrack;

class ENGINE_DLL CGameInstance final : public CBase
{
	DECLARE_SINGLETON(CGameInstance);

private:
	CGameInstance();
	virtual ~CGameInstance() = default;

public:
	HRESULT Initialize_Engine(const ENGINE_DESC& EngineDesc, ID3D11Device** ppDevice, ID3D11DeviceContext** ppContext);
	void Update_Engine(_float fTimeDelta);
	HRESULT Draw();
	void Clear_Resources(_uint iLevelIndex);

	_float Random_Normal();
	_float Random(_float fMin, _float fMax);

#pragma region GRAPHIC_DEVICE
public:
	void Render_Begin(const _float4* pClearColor);
	void Render_End();
	
#pragma endregion

#pragma region INPUT_DEVICE
	// 마우스 입력 0 : 왼쪽 , 1 : 오른쪽 , 2 가운데 버튼
	// 키보드 입력은 DIK_키이름
	_bool		KeyDown(KEY_INPUT eType, _uint KeyState);
	_bool		KeyPressed(KEY_INPUT eType, _uint KeyState);
	_bool		KeyUp(KEY_INPUT eType, _uint KeyState);

	// 마우스 이동방향 0 : X 축    1  : Y 축    2 : Z축
	LONG		GetMouseAxis(_uint iAxis);

	// 인풋 포커스 세팅
	void		SetInputFoucs(KEY_INPUT eType, _bool bFlag);
	_bool		GetInputFoucs(KEY_INPUT eType);
#pragma endregion

#pragma region TIMER_MANAGER
	_float					Get_TimeDelta(const _wstring& strTimerTag);
	HRESULT					Add_Timer(const _wstring& strTimerTag);
	void					Compute_TimeDelta(const _wstring& strTimerTag);

	// 특정 시간 이후에 호출
	void					ADD_DelayFunction(const WCHAR* szTimerName, _float fAfterTime, function<void()> Function);

#pragma endregion

#pragma region LEVEL_MANAGER
public:
	HRESULT				Change_Level(class CLevel* pNewLevel);
	_uint				GetCurrentLevelID();

	// 이거 레퍼런스 카운트 증가합니다.
	class CGameHUD*		GetCurrentLevelHUD();
#pragma endregion

#pragma region PROTOTYPE_MANAGER
	HRESULT						Add_Prototype(_uint iLevelIndex, const _wstring& strPrototypeTag, class CBase* pPrototype);
	HRESULT						Add_SkeletalPrototype(_uint iLevelIndex, ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _wstring& strPrototypeTag, const _char* pModelFilePath, const string& strSkeletalPath, _fmatrix PreTransformMatrix);
	class CBase*				Clone_Prototype(PROTOTYPE ePrototype, _uint iLevelIndex, const _wstring& strPrototypeTag, void* pArg = nullptr);
	const map<const _wstring, class CBase*>* Get_Prototypes_InLevel(_uint iLevelIndex);
#pragma endregion

#pragma region OBJECT_MANAGER
	CComponent*								Get_PartObject_Component(_uint iLevelIndex, const _wstring& strLayerTag, const _wstring& strPartTag, const _wstring& strComponentTag, _uint iIndex = 0);
	HRESULT									Add_GameObject_ToLayer(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, _uint iLayerLevelIndex, const _wstring& strLayerTag, void* pArg = nullptr);
	
	// Layer에 있는 모든 오브젝트를 가져옴	
	list<CGameObject*>*						GetAllObejctToLayer(_uint iLayerIndex, const WCHAR* szLayerTag);
	map<const _wstring, class CLayer*>*		GetCurrentLevelLayer();
#pragma endregion

#pragma region RENDERER
	HRESULT Add_RenderGroup(RENDER eRenderGroup, class CGameObject* pRenderObject);
	const _float4x4* Get_Renderer_Matrix(D3DTS eType = D3DTS::END);
	HRESULT Set_ScreenSize(_uint iSizeX, _uint iSizeY);

#ifdef _DEBUG
	HRESULT Add_DebugComponent(class CComponent* pDebugCom);
	HRESULT Add_PhysxGeometry(class PxRigidActor* pActor, class PxShape* pShape);
	void	Set_DebugVisible(_bool isVisible);
#endif
#pragma endregion

#pragma region PIPELINE
	void								Set_Transform(D3DTS eState, _fmatrix TransformStateMatrix);
	const _float4x4*					Get_PreTransform_Float4x4(D3DTS eState);
	_matrix								Get_PreTransform_Matrix(D3DTS eState);
	const _float4x4*					Get_Transform_Float4x4(D3DTS eState);
	_matrix								Get_Transform_Matrix(D3DTS eState);
	const _float4x4*					Get_Transform_Float4x4_Inverse(D3DTS eState);
	_matrix								Get_Transform_Matrix_Inverse(D3DTS eState);
	const _float4*						Get_CamPosition();
	// 항등행렬 꺼내오기
	_matrix								GetIdentityMatrix();

	// 항등행렬 포인터 꺼내오기
	const _float4x4*					GetIdentityMatrixPtr();

#pragma endregion

#pragma region LIGHT_MANAGER
	HRESULT								Add_Light(const LIGHT_DESC& LightDesc, class CLight*	pOutLight = nullptr);
	HRESULT								Render_Lights(class CShader* pShader, class CVIBuffer* pVIBuffer);
	
	class CLight*						Find_Light(_uint iIndex);
	const	list<class CLight*>*		GetAllLight();
#ifdef _DEBUG
	void								Debug_LightRender();
#endif

#pragma endregion

#pragma region FONT_MANAGER
	HRESULT						Add_Font(const _wstring& strFontTag, const _tchar* pFontFilePath);
	HRESULT						Render_Text(const _wstring& strFontTag, const _tchar* pText, const _float2& vPosition, _fvector vColor = XMVectorSet(1.f, 1.f, 1.f ,1.f));
	_float2						Get_Text_Size(const _wstring& strFontTag, const _tchar* pText, bool bIgnoreWhitespace = true, float fScale = 1.f);
#pragma endregion

#pragma region TARGET_MANAGER
	HRESULT						Add_RenderTarget(const _wstring& strTargetTag, _uint iSizeX, _uint iSizeY, DXGI_FORMAT ePixelFormat, const _float4& vClearColor);
	HRESULT						Add_MRT(const _wstring& strMRTTag, const _wstring& strTargetTag);
	HRESULT						Begin_MRT(const _wstring& strMRTTag, ID3D11DepthStencilView* pDSV = nullptr);
	HRESULT						Load_MRT(const _wstring& strMRTTag, ID3D11DepthStencilView* pDSV = nullptr);
	HRESULT						End_MRT();
	HRESULT						Copy_RenderTarget(const _wstring& strTargetTag, ID3D11Texture2D* pTexture2D);
	HRESULT						Bind_RenderTarget(const _wstring& strTargetTag, class CShader* pShader, const _char* pConstantName);
	HRESULT						Clear_MRT(const _wstring& strMRTag);
#ifdef _DEBUG
	HRESULT						Ready_RT_Debug(const _wstring& strTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY);
	HRESULT						Render_RT_Debug(const _wstring& strMRTTag, CShader* pShader, CVIBuffer_Rect* pVIBuffer);

#endif

#pragma endregion

#pragma region PICKING
	_bool						isPicking(_float3* pOut);
	const POINT&				GetMousePoint();
#pragma endregion

#pragma region SHADOW
	HRESULT Ready_Shadow_Light(const SHADOW_LIGHT_DESC& Desc);
	HRESULT Bind_Shadow_Resource(class CShader* pShader, const _char* pConstantName, D3DTS eType);	
#pragma endregion

#pragma region FRUSTUM
	void				Transform_Frustum_ToLocalSpace(_fmatrix WorldMatrixInverse);
	_bool				isIn_WorldFrustum(_fvector vWorldPos, _float fRange = 0.f);
	_bool				isIn_LocalFrustum(_fvector vLocalPos, _float fRange = 0.f);
	_bool				isIn_WorldFrustum(class CCollider* pCollider);

#ifdef _DEBUG
	void				FrustomRender();
#endif

#pragma endregion

#pragma region Sound Manager
	// 효과음 및 배경음악 선택적 재생
	void			Manager_PlaySound(const TCHAR* pSoundKey, CHANNELID eID, float fVolume);
	// 배경음악 재생
	void			Manager_PlayBGM(const TCHAR* pSoundKey, float fVolume);

	// 특정 채널의 사운드를 정지
	void			Manager_StopSound(CHANNELID eID);

	// 모든 채널의 사운드를 정지
	void			Manager_StopAll();

	//	특정 채널의 사운드볼륨을 바꾸는 기능
	void			Manager_SetChannelVolume(CHANNELID eID, float fVolume);
#pragma endregion

#pragma region Effect Resource Manager
	void								ADD_ResourceManagerTexture(const WCHAR* TextureTag, CTexture* pTexture);
	void								ADD_ResourceManagerShader(const WCHAR* ShaderTag, CShader* pShader);
	void								ADD_ResourceManagerVIBuffer(const WCHAR* VIBufferTag, CComponent* pVIBuffer);

	CTexture*							Get_ResourceManagerTextureResource(const WCHAR* TextureTag);
	CShader*							Get_ResourceManagerShaderResource(const WCHAR* ShaderTag);
	CComponent*							Get_ResourceManagerVIBufferResource(const WCHAR* VIBufferTag);
										   
#pragma region _DEBUG					   
	map<const _wstring, CTexture*>*		Get_ALLResourceManagerTextureResource();
	map<const _wstring, CShader*>*		Get_ALLResourceManagerShaderResource();
	map<const _wstring, CComponent*>*	Get_ALLResourceManagerModelResource();
#pragma endregion
#pragma endregion

#pragma region Collision Manager
	void		ADD_Collider(class CCollider* pCollider);
#pragma endregion

#pragma region Thread Pool
	// 스레드 풀에 등록하면 등록 번호를 반환함
	// 이걸로 나중에 취소하거나 할수있음
	// Handle 받은거 작업 호출되면 nullptr로 바꿔주세요
	// 작업수행되서 작업리스트에서 빠지면 댕글링 포인터입니다.
	ThreadJobHandle*				Add_ThreadjobList(function<void(void*)> function);
	_bool							IsThreadPoolStop();
	size_t							GetThreadJobCount();
	_bool							IsWorkThread();
#pragma endregion

#pragma region Camera Manager
	HRESULT							Add_Camera(const WCHAR* szCameraTag, CCamera* pCamera);
	HRESULT							Remove_Camera(const WCHAR* szCameraTag);

	// Defaut 매개변수 있습니다.
	// 카메라 Tag 뒤에 행렬 매트릭스 넣으면 이전 카메라 정보 줍니다.
	HRESULT							SetMainCamera(const WCHAR* szCameraTag, _float4x4* pPreCameraMatrix = nullptr);

	//	카메라 매니저에서 카메라 포인터 받으면 래퍼런스 카운트 증가함
	//  가져갔으면 내려주세요
	CCamera*						GetCamrea(const WCHAR* szCameraTag);

	//	카메라 매니저에서 카메라 포인터 받으면 래퍼런스 카운트 증가함
	//  가져갔으면 내려주세요
	CCamera*						GetMainCamera();

	//메인카메라 월드 행렬 가져오기
	_matrix							GetMainCameraWorldMatrix();
	const _float4x4*				GetMainCameraWorldMatrixPtr();

	_matrix							GetCameraWorldMatrix(const WCHAR* szCameraTag);
	const _float4x4*				GetCameraWorldMatrixPtr(const WCHAR* szCameraTag);

	const unordered_map<_wstring, CCamera*>* GetAllCamera();

#pragma endregion

#pragma region Physx_Manager
	/* 피직스 싱글턴 객체 얻어오는 함수. */
	PxControllerManager* Get_PxCCTManager();
	PxScene*	Get_PxScene();
	PxPhysics*  Get_PxPhysics();
	/* 피직스 트랜스폼 변환함수. 어지간하면 건드리기 ㄴㄴ */
	PxTransform Convert_Matrix_ToPxTransform(_matrix WorldMatrix);
	/* 피직스 트랜스폼 변환함수. 어지간하면 건드리기 ㄴㄴ */
	_matrix		Convert_PxTransform_ToMatrix(PxTransform Transform);
	HRESULT		Add_CCT_ToPhysx(class CGameObject* pGameObject, class CCharacterController* pCCT);
	HRESULT		Add_RigidBody_ToPhysx(class CGameObject* pGameObject, class CRigidBody* pRigidBody);

#pragma endregion

#pragma region MODEL_PARSER
	HRESULT ReadFbx(const _char* pModelFilePath, MODEL_TYPE eType, binModel** ppOut);
	HRESULT ReadBin(const _char* pModelFilePath, MODEL_TYPE eType, binModel** ppOut);
	HRESULT WriteBin(const _char* pModelFilePath, MODEL_TYPE eType, binModel** ppOut);
	HRESULT ReadBinx(const _char* pModelFilePath, MODEL_TYPE eType, binModel** ppOut);
	HRESULT WriteBinx(const _char* pModelFilePath, MODEL_TYPE eType, binModel** ppOut);
#pragma endregion

#pragma region Interact Manager
	void								SetInteractionBaseObject(CGameObject* pObject);
	void								ADD_Interaction(CInteraction_Component* pInteraction_Com);
	void								Remove_Interaction(CInteraction_Component* pInteraction_Com);

	CInteraction_Component*				GetNearInteraction();
	vector<CInteraction_Component*>*	GetAllInteraction();
#pragma endregion 

#pragma region Cinema Manager
	HRESULT												ADD_ChinemaSceneData(const WCHAR* szSceneTag, const WCHAR* szTag, CCinemaTrack* pData);

	const CCinemaTrack*									GetCinemaSceneData(const WCHAR* szSceneTag, const WCHAR* szTag);
	const unordered_map<_wstring, CCinemaTrack*>*		GetCinemaSceneAllDatas(const WCHAR* szSceneTag);
	const unordered_map<_wstring, class CCutScene*>*	GetCinemaAllScenes();

	HRESULT												SaveCinemaSceneData(const WCHAR* szFilePath);
	HRESULT												LoadCinemaSceneData(const WCHAR* szFilePath);

#pragma endregion


#pragma region Event  Manager
	HRESULT									Add_Event(const WCHAR* szEventTag, CEventHandle* pEvent);
	HRESULT									Remove_Event(const WCHAR* szEventTag);

	HRESULT									Bind_Ovserver(const WCHAR* szEventTag, CEventHandle* pEvent);
	HRESULT									UnBind_Ovserver(const WCHAR* szEventTag, CEventHandle* pEvent);
#pragma endregion

	void							SetGamePause(_bool bFlag) { m_bIsPause = bFlag; }
	_bool							IsGamePasue() { return m_bIsPause; }

	_float							GetGameSpeedfRatio();
	void							ResetGameSpeed();

	// 델타 타임에 대해 곱셈 연산을 수행해서 느려지게 만들거나 빠르게 만들수있습니다.
	void							SetGameSpeed(_float fRatio);
	// 스크린 전체 사이즈
	const _uint2&					GetScreenSize();

	// 스크린 반절 사이즈
	const _uint2&					GetHalfScreenSize();
	const WCHAR*					GetFrameText();

#ifdef _DEBUG
	_float							GetLoopDurationTime(GAMELOOP_TYPE eType);
	void							ComputeLoopTime(GAMELOOP_TYPE eType);
#endif

private:
	class CGraphic_Device*			m_pGraphic_Device = { nullptr };
	class CInput_Device*			m_pInput_Device = { nullptr };
	class CTimer_Manager*			m_pTimer_Manager = { nullptr };
	class CLevel_Manager*			m_pLevel_Manager = { nullptr };
	class CPrototype_Manager*		m_pPrototype_Manager = { nullptr };
	class CObject_Manager*			m_pObject_Manager = { nullptr };
	class CRenderer*				m_pRenderer = { nullptr };
	class CPicking*					m_pPicking = { nullptr };
	class CCollisionManager*		m_pCollisionManager = { nullptr };
	class CSound_Manager*			m_pSoundManager = { nullptr };
	class CPipeLine*				m_pPipeLine = { nullptr };
	class CLight_Manager*			m_pLight_Manager = { nullptr };
	class CFont_Manager*			m_pFont_Manager = { nullptr };
	class CTarget_Manager*			m_pTarget_Manager = { nullptr };
	class CShadow*					m_pShadow = { nullptr };
	class CFrustum*					m_pFrustum = { nullptr };
	class CEffectResourceManager*	m_pEffect_ResourceManager = { nullptr };
	class CCameraManager*			m_pCameraManager = { nullptr };
	class CThreadPool*				m_pThreadPool = { nullptr };
	class CPhysx_Manager*			m_pPhysx_Manager = { nullptr };
	class CBinParser*				m_pBinParser = { nullptr };
	class CFbxParser*				m_pFbxParser = { nullptr };
	class CInteraction_Manager*		m_pInteract_Manager = { nullptr };
	class CCinematicManager*		m_pCinema_Manager = { nullptr };
	class CEventManager*			m_pEventManager = { nullptr };


	_bool							m_bIsPause = false;
	_float							m_fTimeRatio = { 1.f };
	_uint2							m_vScreenSize = {};
	_uint2							m_vHalfScreenSize = {};

	_uint							m_iDrawCnt = {};
	_float							m_fTimeAcc = {};
	_tchar							m_szFPS[MAX_PATH] = {};

#ifdef _DEBUG
	_float							m_fLoopTime[ENUM_CLASS(GAMELOOP_TYPE::END)];
#endif // _DEBUG

public:
	void							Release_Engine();
	virtual void					Free() override;
};

NS_END