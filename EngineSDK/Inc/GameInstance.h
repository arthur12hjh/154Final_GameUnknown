#pragma once

#include "Prototype_Manager.h"

NS_BEGIN(Engine)

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
#pragma endregion

#pragma region TIMER_MANAGER
	_float					Get_TimeDelta(const _wstring& strTimerTag);
	HRESULT					Add_Timer(const _wstring& strTimerTag);
	void					Compute_TimeDelta(const _wstring& strTimerTag);
	// 특정 시간 이후에 호출

#pragma endregion

#pragma region LEVEL_MANAGER
public:
	HRESULT Change_Level(class CLevel* pNewLevel);
#pragma endregion

#pragma region PROTOTYPE_MANAGER
	HRESULT						Add_Prototype(_uint iLevelIndex, const _wstring& strPrototypeTag, class CBase* pPrototype);
	class CBase*				Clone_Prototype(PROTOTYPE ePrototype, _uint iLevelIndex, const _wstring& strPrototypeTag, void* pArg = nullptr);
#pragma endregion

#pragma region OBJECT_MANAGER
	CComponent*					Get_PartObject_Component(_uint iLevelIndex, const _wstring& strLayerTag, const _wstring& strPartTag, const _wstring& strComponentTag, _uint iIndex = 0);
	HRESULT						Add_GameObject_ToLayer(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, _uint iLayerLevelIndex, const _wstring& strLayerTag, void* pArg = nullptr);
	
	// Layer에 있는 모든 오브젝트를 가져옴
	list<CGameObject*>*			GetAllObejctToLayer(_uint iLayerIndex, const WCHAR* szLayerTag);

#pragma endregion

#pragma region RENDERER
	HRESULT Add_RenderGroup(RENDER eRenderGroup, class CGameObject* pRenderObject);
#ifdef _DEBUG
	HRESULT Add_DebugComponent(class CComponent* pDebugCom);
#endif
#pragma endregion

#pragma region PIPELINE
	void						Set_Transform(D3DTS eState, _fmatrix TransformStateMatrix);
	const _float4x4*			Get_Transform_Float4x4(D3DTS eState);
	_matrix						Get_Transform_Matrix(D3DTS eState);
	const _float4x4*			Get_Transform_Float4x4_Inverse(D3DTS eState);
	_matrix						Get_Transform_Matrix_Inverse(D3DTS eState);
	const _float4*				Get_CamPosition();
#pragma endregion

#pragma region LIGHT_MANAGER
	const LIGHT_DESC*			Get_LightDesc(_uint iIndex) const;
	HRESULT						Add_Light(const LIGHT_DESC& LightDesc);
	HRESULT						Render_Lights(class CShader* pShader, class CVIBuffer* pVIBuffer);
#pragma endregion

#pragma region FONT_MANAGER
	HRESULT						Add_Font(const _wstring& strFontTag, const _tchar* pFontFilePath);
	HRESULT						Render_Text(const _wstring& strFontTag, const _tchar* pText, const _float2& vPosition, _fvector vColor = XMVectorSet(1.f, 1.f, 1.f ,1.f));
#pragma endregion

#pragma region TARGET_MANAGER
	HRESULT						Add_RenderTarget(const _wstring& strTargetTag, _uint iSizeX, _uint iSizeY, DXGI_FORMAT ePixelFormat, const _float4& vClearColor);
	HRESULT						Add_MRT(const _wstring& strMRTTag, const _wstring& strTargetTag);
	HRESULT						Begin_MRT(const _wstring& strMRTTag, ID3D11DepthStencilView* pDSV = nullptr);
	HRESULT						End_MRT();
	HRESULT						Copy_RenderTarget(const _wstring& strTargetTag, ID3D11Texture2D* pTexture2D);
	HRESULT						Bind_RenderTarget(const _wstring& strTargetTag, class CShader* pShader, const _char* pConstantName);

#ifdef _DEBUG
	HRESULT						Ready_RT_Debug(const _wstring& strTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY);
	HRESULT						Render_RT_Debug(const _wstring& strMRTTag, CShader* pShader, CVIBuffer_Rect* pVIBuffer);
#endif

#pragma endregion

#pragma region PICKING
	_bool isPicking(_float3* pOut);

#pragma endregion

#pragma region SHADOW
	HRESULT Ready_Shadow_Light(const SHADOW_LIGHT_DESC& Desc);
	HRESULT Bind_Shadow_Resource(class CShader* pShader, const _char* pConstantName, D3DTS eType);	
#pragma endregion

#pragma region FRUSTUM
	void Transform_Frustum_ToLocalSpace(_fmatrix WorldMatrixInverse);
	_bool isIn_WorldFrustum(_fvector vWorldPos, _float fRange = 0.f);
	_bool isIn_LocalFrustum(_fvector vLocalPos, _float fRange = 0.f);

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

	CTexture*							GetResourceManagerTextureResource(const WCHAR* TextureTag);
	CShader*							GetResourceManagerShaderResource(const WCHAR* ShaderTag);
	CComponent*							GetResourceManagerVIBufferResource(const WCHAR* VIBufferTag);

#pragma region _DEBUG
	map<const _wstring, CTexture*>*		GetALLResourceManagerTextureResource();
	map<const _wstring, CShader*>*		GetALLResourceManagerShaderResource();
	map<const _wstring, CComponent*>*	GetALLResourceManagerModelResource();
#pragma endregion
#pragma endregion

#pragma region Collision Manager
	void		ADD_Collider(class CCollider* pCollider);
#pragma endregion


	void							SetGamePause(_bool bFlag) { m_bIsPause = bFlag; }
	_bool							IsGamePasue() { return m_bIsPause; }

	const _uint2&					GetScreenSize();
	const _uint2&					GetHalfScreenSize();

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

	_bool							m_bIsPause = false;
	_uint2							m_vScreenSize = {};
	_uint2							m_vHalfScreenSize = {};

public:
	void							Release_Engine();
	virtual void					Free() override;
};

NS_END