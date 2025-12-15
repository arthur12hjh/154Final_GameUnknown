#include "GameInstance.h"

#include "Prototype_Manager.h"
#include "Object_Manager.h"
#include "Graphic_Device.h"

#include "Target_Manager.h"
#include "Level_Manager.h"
#include "Timer_Manager.h"
#include "Light_Manager.h"
#include "Input_Device.h"
#include "Font_Manager.h"
#include "CollisionManager.h"
#include "EffectResourceManager.h"
#include "Sound_Manager.h"
#include "Renderer.h"
#include "PipeLine.h"
#include "Frustum.h"
#include "Occlusion.h"
#include "ThreadPool.h"
#include "CameraManager.h"
#include "Level.h"
#include "Picking.h"
#include "Shadow.h"
#include "Physx_Manager.h"
#include "BinParser.h"
#include "Interaction_Manager.h"
#include "CinematicManager.h"
#include "EventManager.h"
#include "FbxParser.h"

IMPLEMENT_SINGLETON(CGameInstance)

mt19937 CGameInstance::m_RandomDevice(random_device{}());
uniform_real_distribution<_float> CGameInstance::m_distribution(0.f, 1.f);

CGameInstance::CGameInstance()
{
}

HRESULT CGameInstance::Initialize_Engine(const ENGINE_DESC& EngineDesc, ID3D11Device** ppDevice, ID3D11DeviceContext** ppContext)
{
	m_pGraphic_Device = CGraphic_Device::Create(EngineDesc.hWnd, EngineDesc.eWindowMode, EngineDesc.iWinSizeX, EngineDesc.iWinSizeY, ppDevice, ppContext);
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_vScreenSize = { EngineDesc.iWinSizeX, EngineDesc.iWinSizeY };
	m_vHalfScreenSize = { m_vScreenSize.x >> 1 , m_vScreenSize.y >> 1};

	m_pBinParser = CBinParser::Create();
	if (nullptr == m_pBinParser)
		return E_FAIL;

	m_pFbxParser = CFbxParser::Create();
	if (nullptr == m_pFbxParser)
		return E_FAIL;

	m_pPhysx_Manager = CPhysx_Manager::Create();
	if (nullptr == m_pPhysx_Manager)
		return E_FAIL;

	m_pFrustum = CFrustum::Create(*ppDevice, *ppContext);

	m_pOcculusion = COcclusion::Create(*ppDevice, *ppContext);
	if (nullptr == m_pOcculusion)
		return E_FAIL;

#ifdef _DEBUG
	m_pLight_Manager = CLight_Manager::Create(*ppDevice, *ppContext);
#else
	m_pLight_Manager = CLight_Manager::Create();
#endif // DEBUG
	if (nullptr == m_pLight_Manager || nullptr == m_pFrustum)
		return E_FAIL;

	m_pInput_Device = CInput_Device::Create(EngineDesc.hInstance, EngineDesc.hWnd);
	if (nullptr == m_pInput_Device)
		return E_FAIL;

	m_pThreadPool = CThreadPool::Create(*ppDevice, *ppContext, 8);
	if (nullptr == m_pThreadPool)
		return E_FAIL;

	m_pPipeLine = CPipeLine::Create();
	if (nullptr == m_pPipeLine)
		return E_FAIL;

	m_pTimer_Manager = CTimer_Manager::Create();
	if (nullptr == m_pTimer_Manager)
		return E_FAIL;

	m_pPrototype_Manager = CPrototype_Manager::Create(EngineDesc.iNumLevels);
	if (nullptr == m_pPrototype_Manager)
		return E_FAIL;

	m_pObject_Manager = CObject_Manager::Create(EngineDesc.iNumLevels);
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	m_pLevel_Manager = CLevel_Manager::Create();
	if (nullptr == m_pLevel_Manager)
		return E_FAIL;

	m_pTarget_Manager = CTarget_Manager::Create(*ppDevice, *ppContext);
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;
	
	m_pRenderer = CRenderer::Create(*ppDevice, *ppContext);
	if (nullptr == m_pRenderer)
		return E_FAIL;

	m_pFont_Manager = CFont_Manager::Create(*ppDevice, *ppContext);
	if (nullptr == m_pFont_Manager)
		return E_FAIL;

	m_pPicking = CPicking::Create(*ppDevice, *ppContext, EngineDesc.hWnd, EngineDesc.iWinSizeX, EngineDesc.iWinSizeY);
	if (nullptr == m_pPicking)
		return E_FAIL;

	m_pShadow = CShadow::Create();
	if (nullptr == m_pShadow)
		return E_FAIL;

	m_pInteract_Manager = CInteraction_Manager::Create();
	if (nullptr == m_pInteract_Manager)
		return E_FAIL;

	m_pCollisionManager = CCollisionManager::Create();
	if (nullptr == m_pCollisionManager)
		return E_FAIL;

	m_pSoundManager = CSound_Manager::Create();
	if (nullptr == m_pSoundManager)
		return E_FAIL;

	m_pEffect_ResourceManager = CEffectResourceManager::Create(*ppDevice, *ppContext);
	if (nullptr == m_pEffect_ResourceManager)
		return E_FAIL;

	m_pCameraManager = CCameraManager::Create();
	if (nullptr == m_pCameraManager)
		return E_FAIL;

	m_pCinema_Manager = CCinematicManager::Create(*ppDevice, *ppContext);
	if (nullptr == m_pCinema_Manager)
		return E_FAIL;

	m_pEventManager = CEventManager::Create();
	if (nullptr == m_pEventManager)
		return E_FAIL;

#ifdef _DEBUG
	m_pTimer_Manager->Add_Timer(TEXT("Priority_Time"));
	m_pTimer_Manager->Add_Timer(TEXT("Update_Time"));
	m_pTimer_Manager->Add_Timer(TEXT("LateUpdate_Time"));
	m_pTimer_Manager->Add_Timer(TEXT("Collision_Time"));
	m_pTimer_Manager->Add_Timer(TEXT("Render_Time"));
#endif // _DEBUG

	return S_OK;
}

void CGameInstance::Update_Engine(_float fTimeDelta)
{
	_float fGameSpeed = fTimeDelta * m_fTimeRatio;

	if (m_bIsHitStopDurationTime)
	{
		m_iHitStopFrame.x++;
		if (m_iHitStopFrame.x > m_iHitStopFrame.y)
		{
			if (m_bIsPause)
				m_bIsPause = false;

			m_vLerpTime.x += fTimeDelta * m_fHitStopReturnSpeed;
			_float fRatio = Clamp<_float>(m_vLerpTime.x / m_vLerpTime.y, 0.f, 1.f);
			m_fTimeRatio = Lerp<_float>(m_fTimeRatio, 1.f, fRatio);
			if (1 <= fRatio)
				m_bIsHitStopDurationTime = false;
		}
	}

	if (false == m_bIsPause)
	{
		m_pInput_Device->UpdateKeyFrame();
		m_pPicking->Update();
		m_pCameraManager->Priority_Update(fGameSpeed);

		//Priority Update 디버그
#ifdef _DEBUG
		ComputeLoopTime(GAMELOOP_TYPE::PRIORITY);
		m_fLoopTime[ENUM_CLASS(GAMELOOP_TYPE::PRIORITY)] = GetLoopDurationTime(GAMELOOP_TYPE::PRIORITY);
		m_pObject_Manager->Priority_Update(fGameSpeed);
		ComputeLoopTime(GAMELOOP_TYPE::PRIORITY);
		m_fLoopTime[ENUM_CLASS(GAMELOOP_TYPE::PRIORITY)] -= GetLoopDurationTime(GAMELOOP_TYPE::PRIORITY);
#else
		m_pObject_Manager->Priority_Update(fGameSpeed);
#endif

		m_pPipeLine->Update();
		m_pTimer_Manager->Update_Timer(fGameSpeed);
	
		m_pFrustum->Update();
		m_pCameraManager->Update(fGameSpeed);

		//Update 디버그
#ifdef _DEBUG
		ComputeLoopTime(GAMELOOP_TYPE::UPDATE);
		m_fLoopTime[ENUM_CLASS(GAMELOOP_TYPE::UPDATE)] = GetLoopDurationTime(GAMELOOP_TYPE::UPDATE);
		m_pObject_Manager->Update(fGameSpeed);
		ComputeLoopTime(GAMELOOP_TYPE::UPDATE);
		m_fLoopTime[ENUM_CLASS(GAMELOOP_TYPE::UPDATE)] -= GetLoopDurationTime(GAMELOOP_TYPE::UPDATE);
#else
		m_pObject_Manager->Update(fGameSpeed);

#endif
	}

	m_pCameraManager->Late_Update(fGameSpeed);

	//Late_Update 디버그
#ifdef _DEBUG
	ComputeLoopTime(GAMELOOP_TYPE::LATE_UPDATE);
	m_fLoopTime[ENUM_CLASS(GAMELOOP_TYPE::LATE_UPDATE)] = GetLoopDurationTime(GAMELOOP_TYPE::LATE_UPDATE);
	m_pObject_Manager->Late_Update(fGameSpeed);
	ComputeLoopTime(GAMELOOP_TYPE::LATE_UPDATE);
	m_fLoopTime[ENUM_CLASS(GAMELOOP_TYPE::LATE_UPDATE)] -= GetLoopDurationTime(GAMELOOP_TYPE::LATE_UPDATE);
#else
	m_pObject_Manager->Late_Update(fGameSpeed);
#endif

	//충돌 로직 디버그
#ifdef _DEBUG
		ComputeLoopTime(GAMELOOP_TYPE::COLLISION);
		m_fLoopTime[ENUM_CLASS(GAMELOOP_TYPE::COLLISION)] = GetLoopDurationTime(GAMELOOP_TYPE::COLLISION);
		m_pCollisionManager->Compute_Collision();
		ComputeLoopTime(GAMELOOP_TYPE::COLLISION);
		m_fLoopTime[ENUM_CLASS(GAMELOOP_TYPE::COLLISION)] -= GetLoopDurationTime(GAMELOOP_TYPE::COLLISION);
#else
	m_pCollisionManager->Compute_Collision();
#endif

	m_pInteract_Manager->Update();


	m_pPhysx_Manager->Update(fGameSpeed); // isdead 체크해서 뺴고
	m_pObject_Manager->Clear_DeadObj(); // -> 죽은 객체 빠지고
	m_pLight_Manager->Clear_DeadLight(); // -> 죽은 객체 빠지고

	m_pRenderer->Update(fTimeDelta);

	m_pThreadPool->Update_Async();
	m_pLevel_Manager->Update(fTimeDelta);

	m_fTimeAcc += fTimeDelta;
}

HRESULT CGameInstance::Draw()
{
	//랜더 로직 디버그
#ifdef _DEBUG
	ComputeLoopTime(GAMELOOP_TYPE::RENDER);
	m_fLoopTime[ENUM_CLASS(GAMELOOP_TYPE::RENDER)] = GetLoopDurationTime(GAMELOOP_TYPE::RENDER);
	
	m_pRenderer->Render();

	ComputeLoopTime(GAMELOOP_TYPE::RENDER);
	m_fLoopTime[ENUM_CLASS(GAMELOOP_TYPE::RENDER)] -= GetLoopDurationTime(GAMELOOP_TYPE::RENDER);
#else
	m_pRenderer->Render();
#endif

	++m_iDrawCnt;
	if (m_fTimeAcc >= 1.f)
	{
		wsprintf(m_szFPS, TEXT("FPS : %d"), m_iDrawCnt);

		m_iDrawCnt = 0;
		m_fTimeAcc = 0.f;
	}

	m_pLevel_Manager->Render();
	return S_OK;
}

void CGameInstance::Clear_Resources(_uint iLevelIndex)
{
	m_pPrototype_Manager->Clear(iLevelIndex);
	m_pObject_Manager->Clear(iLevelIndex);
}

_float CGameInstance::Random_Normal()
{
	return m_distribution(m_RandomDevice);
}

_float CGameInstance::Random(_float fMin, _float fMax)
{
	return fMin + Random_Normal() * (fMax - fMin);	
}

#pragma region GRAPHIC_DEVICE

void CGameInstance::Render_Begin(const _float4* pClearColor)
{
	m_pGraphic_Device->Clear_BackBuffer_View(pClearColor);
	m_pGraphic_Device->Clear_DepthStencil_View();
}

void CGameInstance::Render_End()
{
	m_pGraphic_Device->Present();
}

#pragma endregion

#pragma region Input Device
_bool CGameInstance::KeyDown(KEY_INPUT eType, _uint KeyState)
{
	return m_pInput_Device->KeyDown(eType, KeyState);
}

_bool CGameInstance::KeyPressed(KEY_INPUT eType, _uint KeyState)
{
	return m_pInput_Device->KeyPressed(eType, KeyState);
}

_bool CGameInstance::KeyUp(KEY_INPUT eType, _uint KeyState)
{
	return m_pInput_Device->KeyUp(eType, KeyState);
}

LONG CGameInstance::GetMouseAxis( _uint iAxis)
{
	return m_pInput_Device->GetMouseAxis(iAxis);
}

void CGameInstance::SetInputFoucs(KEY_INPUT eType, _bool bFlag)
{
	m_pInput_Device->SetInputFoucs(eType, bFlag);
}

_bool CGameInstance::GetInputFoucs(KEY_INPUT eType)
{
	return m_pInput_Device->GetInputFoucs(eType);
}
#pragma endregion


#pragma region TIMER_MANAGER

_float CGameInstance::Get_TimeDelta(const _wstring& strTimerTag)
{
	return m_pTimer_Manager->Get_TimeDelta(strTimerTag);
}

HRESULT CGameInstance::Add_Timer(const _wstring& strTimerTag)
{
	return m_pTimer_Manager->Add_Timer(strTimerTag);
}

void CGameInstance::Compute_TimeDelta(const _wstring& strTimerTag)
{
	m_pTimer_Manager->Compute_TimeDelta(strTimerTag);
}

void CGameInstance::ADD_DelayFunction(const WCHAR* szTimerName, _float fAfterTime, function<void()> Function)
{
	m_pTimer_Manager->ADD_DelayFunction(szTimerName, fAfterTime, Function);
}

#pragma endregion

#pragma region LEVEL_MANAGER

HRESULT CGameInstance::Clear_LevelResource(_bool bIsClearPrototypeData)
{
	m_pCameraManager->Clear_Cameras();
	if (bIsClearPrototypeData)
		return m_pLevel_Manager->Clear_LevelResource();

	return S_OK;
}

HRESULT CGameInstance::Change_Level(CLevel* pNewLevel)
{
	return m_pLevel_Manager->Change_Level(pNewLevel);
}

_uint CGameInstance::GetCurrentLevelID()
{
	return m_pLevel_Manager->GetCurrentLevelID();
}

CGameHUD* CGameInstance::GetCurrentLevelHUD()
{
	return m_pLevel_Manager->GetCurrentLevel()->GetHUD();
}

#pragma endregion

#pragma region PROTOTYPE_MANAGER

HRESULT CGameInstance::Add_Prototype(_uint iLevelIndex, const _wstring& strPrototypeTag, CBase* pPrototype)
{
	return m_pPrototype_Manager->Add_Prototype(iLevelIndex, strPrototypeTag, pPrototype);
}

HRESULT CGameInstance::Add_SkeletalPrototype(_uint iLevelIndex, ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _wstring& strPrototypeTag, const _char* pModelFilePath, const string& strSkeletalPath, vector<_wstring>& szPartPrototypeTagList, vector<string>& szPartModelFilePathList, _fmatrix PreTransformMatrix)
{
	return m_pPrototype_Manager->Add_SkeletalPrototype(iLevelIndex, pDevice, pContext,  strPrototypeTag, pModelFilePath, strSkeletalPath, szPartPrototypeTagList, szPartModelFilePathList, PreTransformMatrix);
}

CBase* CGameInstance::Clone_Prototype(PROTOTYPE ePrototype, _uint iLevelIndex, const _wstring& strPrototypeTag, void* pArg)
{
	return m_pPrototype_Manager->Clone_Prototype(ePrototype, iLevelIndex, strPrototypeTag, pArg);
}

const map<const _wstring, class CBase*>* CGameInstance::Get_Prototypes_InLevel(_uint iLevelIndex)
{
	return m_pPrototype_Manager->Get_Prototypes_InLevel(iLevelIndex);
}

CBase* CGameInstance::Get_Prototype(_uint iLevelIndex, const _wstring& strPrototypeTag)
{
	return m_pPrototype_Manager->Get_Prototype(iLevelIndex, strPrototypeTag);
}

#pragma endregion

#pragma region OBJECT_MANAGER

CComponent* CGameInstance::Get_PartObject_Component(_uint iLevelIndex, const _wstring& strLayerTag, const _wstring& strPartTag, const _wstring& strComponentTag, _uint iIndex)
{
	return m_pObject_Manager->Get_PartObject_Component(iLevelIndex, strLayerTag, strPartTag, strComponentTag, iIndex);
}

HRESULT CGameInstance::Add_GameObject_ToLayer(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, _uint iLayerLevelIndex, const _wstring& strLayerTag, void* pArg)
{
	return m_pObject_Manager->Add_GameObject_ToLayer(iPrototypeLevelIndex, strPrototypeTag, iLayerLevelIndex, strLayerTag, pArg);
}

CGameObject* CGameInstance::Add_Get_GameObject(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, _uint iLayerLevelIndex, const _wstring& strLayerTag, void* pArg)
{
	return m_pObject_Manager->Add_Get_GameObject(iPrototypeLevelIndex, strPrototypeTag, iLayerLevelIndex, strLayerTag, pArg);
}

void CGameInstance::ADD_ToLayer(_uint iLayerLevelIndex, const _wstring& strLayerTag, CGameObject* pObject)
{
	m_pObject_Manager->ADD_ToLayer(iLayerLevelIndex, strLayerTag, pObject);
}

list<CGameObject*>* CGameInstance::GetAllObejctToLayer(_uint iLayerIndex, const WCHAR* szLayerTag)
{
	return m_pObject_Manager->Get_LayerObjects(iLayerIndex, szLayerTag);
}

map<const _wstring, class CLayer*>* CGameInstance::GetCurrentLevelLayer()
{
	return m_pObject_Manager->GetLayer();
}

#pragma endregion

#pragma region RENDERER

HRESULT CGameInstance::Add_RenderGroup(RENDER eRenderGroup, CGameObject* pRenderObject)
{
	return m_pRenderer->Add_RenderGroup(eRenderGroup, pRenderObject);
}

const _float4x4* CGameInstance::Get_Renderer_Matrix(D3DTS eType)
{
	return m_pRenderer->Get_Renderer_Matrix(eType);
}

HRESULT CGameInstance::Set_ScreenSize(_uint iSizeX, _uint iSizeY)
{
	return m_pRenderer->Set_ScreenSize(iSizeX, iSizeY);
}

void CGameInstance::Active_RadialBlur(_float fLifeTime, _uint iSampleCount, _float fSamplePower)
{
	return m_pRenderer->Active_RadialBlur(fLifeTime, iSampleCount, fSamplePower);
}

#ifdef _DEBUG

HRESULT CGameInstance::Add_DebugComponent(CComponent* pDebugCom)
{
	return m_pRenderer->Add_DebugComponent(pDebugCom);
}
HRESULT CGameInstance::Add_PhysxGeometry(CGameObject* pGameObject, PxRigidActor* pActor, PxShape* pShape)
{
	return m_pRenderer->Add_PhysxGeometry(pGameObject, pActor, pShape);
}

void CGameInstance::Set_DebugVisible(_bool isVisible)
{
	m_pRenderer->Set_DebugVisible(isVisible);
}

void CGameInstance::Set_DebugColliderVisible(_bool isVisible)
{
	m_pRenderer->Set_DebugColliderVisible(isVisible);
}

void* CGameInstance::Get_DoF_Desc()
{
	return m_pRenderer->Get_DoF_Desc();
}

void* CGameInstance::Get_Bloom_Desc()
{
	return m_pRenderer->Get_Bloom_Desc();
}

void* CGameInstance::Get_Fog_Desc()
{
	return m_pRenderer->Get_Fog_Desc();
}

void* CGameInstance::Get_SSAO_Desc()
{
	return m_pRenderer->Get_SSAO_Desc();
}

void* CGameInstance::Get_MotionBlur_Desc()
{
	return m_pRenderer->Get_MotionBlur_Desc();
}

void* CGameInstance::Get_Volumetric_Desc()
{
	return m_pRenderer->Get_Volumetric_Desc();
}

void* CGameInstance::Get_HDR_Desc()
{
	return m_pRenderer->Get_HDR_Desc();
}

#endif

#pragma endregion

#pragma region PIPELINE

void CGameInstance::Set_Transform(D3DTS eState, _fmatrix TransformStateMatrix)
{
	m_pPipeLine->Set_Transform(eState, TransformStateMatrix);
}

const _float4x4* CGameInstance::Get_PreTransform_Float4x4(D3DTS eState)
{
	return m_pPipeLine->Get_PreTransform_Float4x4(eState);
}

_matrix CGameInstance::Get_PreTransform_Matrix(D3DTS eState)
{
	return m_pPipeLine->Get_PreTransform_Matrix(eState);
}

const _float4x4* CGameInstance::Get_Transform_Float4x4(D3DTS eState)
{
	return m_pPipeLine->Get_Transform_Float4x4(eState);
}

_matrix CGameInstance::Get_Transform_Matrix(D3DTS eState)
{
	return m_pPipeLine->Get_Transform_Matrix(eState);
}

const _float4x4* CGameInstance::Get_Transform_Float4x4_Inverse(D3DTS eState)
{
	return m_pPipeLine->Get_Transform_Float4x4_Inverse(eState);
}

_matrix CGameInstance::Get_Transform_Matrix_Inverse(D3DTS eState)
{
	return m_pPipeLine->Get_Transform_Matrix_Inverse(eState);
}

const _float4* CGameInstance::Get_CamRight()
{
	return m_pPipeLine->Get_CamRight();
}

const _float4* CGameInstance::Get_CamUp()
{
	return m_pPipeLine->Get_CamUp();
}

const _float4* CGameInstance::Get_CamLook()
{
	return m_pPipeLine->Get_CamLook();
}

const _float4* CGameInstance::Get_CamPosition()
{
	return m_pPipeLine->Get_CamPosition();
}

const _float4* CGameInstance::Get_PreCamRight()
{
	return m_pPipeLine->Get_PreCamRight();
}

const _float4* CGameInstance::Get_PreCamUp()
{
	return m_pPipeLine->Get_PreCamUp(); 
}

const _float4* CGameInstance::Get_PreCamLook()
{
	return m_pPipeLine->Get_PreCamLook(); 
}

const _float4* CGameInstance::Get_PreCamPosition()
{
	return m_pPipeLine->Get_PreCamPosition();
}

_matrix CGameInstance::GetIdentityMatrix()
{
	return m_pPipeLine->GetIdentityMatrix();
}

const _float4x4* CGameInstance::GetIdentityMatrixPtr()
{
	return m_pPipeLine->GetIdentityMatrixPtr();
}

const CAMERA_INFO& CGameInstance::Get_CurrentCamInfo()
{
	return m_pPipeLine->Get_CurrentCamInfo();
}

#pragma endregion

#pragma region LIGHT_MANAGER

HRESULT CGameInstance::Add_Light(const LIGHT_DESC& LightDesc, class CLight* pOutLight)
{
	return m_pLight_Manager->Add_Light(LightDesc, pOutLight);
}

CLight* CGameInstance::Find_Light(_uint iIndex)
{
	return m_pLight_Manager->Find_Light(iIndex);
}

const list<class CLight*>* CGameInstance::GetAllLight()
{
	return m_pLight_Manager->GetAllLight();
}

LIGHT_DESC* CGameInstance::Get_Directional_Desc()
{
	return m_pLight_Manager->Get_Directional_Desc();
}

HRESULT CGameInstance::Render_Lights(CShader* pShader, CVIBuffer* pVIBuffer)
{
	return m_pLight_Manager->Render_Lights(pShader, pVIBuffer);
}

HRESULT CGameInstance::Render_VolumetricLights(CShader* pShader, CVIBuffer* pVIBuffer)
{
	return m_pLight_Manager->Render_VolumetricLights(pShader, pVIBuffer);
}

#ifdef _DEBUG
void CGameInstance::Select_LightRender(CLight* pSelectLight)
{
	return m_pLight_Manager->Select_LightRender(pSelectLight);
}

void CGameInstance::Debug_LightRender()
{
	m_pLight_Manager->Debug_LightRender();
}
#endif // _DEBUG
#pragma endregion

#pragma region FONT_MANAGER
HRESULT CGameInstance::Add_Font(const _wstring& strFontTag, const _tchar* pFontFilePath)
{
	return m_pFont_Manager->Add_Font(strFontTag, pFontFilePath);
}

HRESULT CGameInstance::Render_Text(const _wstring& strFontTag, const _tchar* pText, const _float2& vPosition, _fvector vColor, _float fScale)
{
	return m_pFont_Manager->Render(strFontTag, pText, vPosition, vColor, fScale);
}

_float2 CGameInstance::Get_Text_Size(const _wstring& strFontTag, const _tchar* pText, bool bIgnoreWhitespace, float fScale)
{
	return m_pFont_Manager->Get_Text_Size(strFontTag, pText, bIgnoreWhitespace, fScale);
}
#pragma endregion

#pragma region TARGET_MANAGER

HRESULT CGameInstance::Add_RenderTarget(const _wstring& strTargetTag, _uint iSizeX, _uint iSizeY, DXGI_FORMAT ePixelFormat, const _float4& vClearColor)
{
	return m_pTarget_Manager->Add_RenderTarget(strTargetTag, iSizeX, iSizeY, ePixelFormat, vClearColor);
}

HRESULT CGameInstance::Add_MRT(const _wstring& strMRTTag, const _wstring& strTargetTag)
{
	return m_pTarget_Manager->Add_MRT(strMRTTag, strTargetTag);
}

HRESULT CGameInstance::Begin_MRT(const _wstring& strMRTTag, ID3D11DepthStencilView* pDSV)
{
	return m_pTarget_Manager->Begin_MRT(strMRTTag, pDSV);
}

HRESULT CGameInstance::Load_MRT(const _wstring& strMRTTag, ID3D11DepthStencilView* pDSV)
{
	return m_pTarget_Manager->Load_MRT(strMRTTag, pDSV);
}

HRESULT CGameInstance::End_MRT()
{
	return m_pTarget_Manager->End_MRT();
}

HRESULT CGameInstance::Copy_RenderTarget(const _wstring& strTargetTag, ID3D11Texture2D* pTexture2D)
{
	return m_pTarget_Manager->Copy_RenderTarget(strTargetTag, pTexture2D);
}

HRESULT CGameInstance::Bind_RenderTarget(const _wstring& strTargetTag, CShader* pShader, const _char* pConstantName)
{
	return m_pTarget_Manager->Bind_RenderTarget(strTargetTag, pShader, pConstantName);
}

HRESULT CGameInstance::Clear_MRT(const _wstring& strMRTTag)
{
	return m_pTarget_Manager->Clear_MRT(strMRTTag);
}


#ifdef _DEBUG

HRESULT CGameInstance::Ready_RT_Debug(const _wstring& strTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY)
{
	return m_pTarget_Manager->Ready_Debug(strTargetTag, fX, fY, fSizeX, fSizeY);
}


HRESULT CGameInstance::Render_RT_Debug(const _wstring& strMRTTag, CShader* pShader, CVIBuffer_Rect* pVIBuffer)
{
	return m_pTarget_Manager->Render_Debug(strMRTTag, pShader, pVIBuffer);
}

#endif
#pragma endregion

#pragma region PICKING
_bool CGameInstance::isPicking(_float3* pOut)
{
	// 포커스 들어갔을때만 

	return m_pPicking->isPicking(pOut);
}

const POINT& CGameInstance::GetMousePoint()
{
	return m_pPicking->GetMousePoint();
}

HRESULT CGameInstance::Ready_Shadow_Light(const SHADOW_LIGHT_DESC& Desc)
{
	return m_pShadow->Ready_Shadow_Light(Desc);
}

HRESULT CGameInstance::Bind_Shadow_Resource(CShader* pShader, const _char* pConstantName, D3DTS eType)
{
	return m_pShadow->Bind_Shader_Resource(pShader, pConstantName, eType);
}

#pragma endregion

#pragma region FRUSTUM

void CGameInstance::Transform_Frustum_ToLocalSpace(_fmatrix WorldMatrixInverse)
{
	m_pFrustum->Transform_Frustum_ToLocalSpace(WorldMatrixInverse);
}

_bool CGameInstance::isIn_WorldFrustum(_fvector vWorldPos, _float fRange)
{
	return m_pFrustum->isIn_WorldFrustum(vWorldPos, fRange);
}

_bool CGameInstance::isIn_LocalFrustum(_fvector vLocalPos, _float fRange)
{
	return m_pFrustum->isIn_LocalFrustum(vLocalPos, fRange);
}

_bool CGameInstance::isIn_WorldFrustum(CCollider* pCollider)
{
	return m_pFrustum->isIn_WorldFrustum(pCollider);
}

_bool CGameInstance::isIn_DistanceFrustum(_vector vPoint, _float fDistance)
{
	return m_pFrustum->isIn_DistanceFrustum(vPoint, fDistance);
}



#ifdef _DEBUG
void CGameInstance::FrustomRender()
{
	return m_pFrustum->FrustomRender();
}
#endif // _DEBUG
#pragma endregion

#pragma region Occlusion
void CGameInstance::Begin_Query()
{
	m_pOcculusion->Begin_Query();
}
void CGameInstance::End_Query()
{
	m_pOcculusion->End_Query();
}
HRESULT CGameInstance::Get_Result(_bool* pIsVisible)
{
	m_pOcculusion->Get_Result(pIsVisible);

	return S_OK;
}
#pragma endregion

#pragma region Sound Manager
void CGameInstance::Manager_PlaySound(const TCHAR* pSoundKey, CHANNELID eID, float fVolume)
{
	m_pSoundManager->Manager_PlaySound(pSoundKey, eID, fVolume);
}

void CGameInstance::Manager_PlayBGM(const TCHAR* pSoundKey, float fVolume)
{
	m_pSoundManager->Manager_PlayBGM(pSoundKey, fVolume);
}

void CGameInstance::Manager_StopSound(CHANNELID eID)
{
	m_pSoundManager->Manager_StopSound(eID);
}

void CGameInstance::Manager_StopAll()
{
	m_pSoundManager->Manager_StopAll();
}

void CGameInstance::Manager_SetChannelVolume(CHANNELID eID, float fVolume)
{
	m_pSoundManager->Manager_SetChannelVolume(eID, fVolume);
}

#pragma endregion

#pragma region Effect Resource Manager
void CGameInstance::ADD_ResourceManagerTexture(const WCHAR* TextureTag, CTexture* pTexture)
{
	m_pEffect_ResourceManager->ADD_TextureResource(TextureTag, pTexture);
}

void CGameInstance::ADD_ResourceManagerShader(const WCHAR* ShaderTag, CShader* pShader)
{
	m_pEffect_ResourceManager->ADD_ShaderResource(ShaderTag, pShader);
}

void CGameInstance::ADD_ResourceManagerVIBuffer(const WCHAR* VIBufferTag, CComponent* pVIBuffer)
{
	m_pEffect_ResourceManager->ADD_VIBufferResource(VIBufferTag, pVIBuffer);
}

CTexture* CGameInstance::Get_ResourceManagerTextureResource(const WCHAR* TextureTag)
{
	return m_pEffect_ResourceManager->GetTextureResource(TextureTag);
}

CShader* CGameInstance::Get_ResourceManagerShaderResource(const WCHAR* ShaderTag)
{
	return m_pEffect_ResourceManager->GetShaderResource(ShaderTag);
}

CComponent* CGameInstance::Get_ResourceManagerVIBufferResource(const WCHAR* VIBufferTag)
{
	return m_pEffect_ResourceManager->GetVIBufferResource(VIBufferTag);
}

map<const _wstring, CTexture*>* CGameInstance::Get_ALLResourceManagerTextureResource()
{
	return m_pEffect_ResourceManager->GetALLTextureResource();
}

map<const _wstring, CShader*>* CGameInstance::Get_ALLResourceManagerShaderResource()
{
	return m_pEffect_ResourceManager->GetALLShaderResource();
}

map<const _wstring, CComponent*>* CGameInstance::Get_ALLResourceManagerModelResource()
{
	return m_pEffect_ResourceManager->GetALLVIBufferResource();
}

#pragma endregion

#pragma region Collision Manager
void CGameInstance::ADD_Collider(CCollider* pCollider)
{
	m_pCollisionManager->ADD_CollisionList(pCollider);
}
#pragma endregion

#pragma region Thread Pool
ThreadJobHandle* CGameInstance::Add_ThreadjobList(function<void(void*)> function)
{
	return m_pThreadPool->Add_jobList(function);
}

_bool CGameInstance::IsThreadPoolStop()
{
	return m_pThreadPool->IsThreadPoolStop();
}

size_t CGameInstance::GetThreadJobCount()
{
	return m_pThreadPool->GetThreadJobCount();
}

_bool CGameInstance::IsWorkThread()
{
	return m_pThreadPool->IsWorkThread();
}

#pragma endregion

#pragma region Camera Manager
void CGameInstance::Shake_Camera(_float fShakeTime, _float fIntensity)
{
	return m_pCameraManager->Shake(fShakeTime, fIntensity);
}
HRESULT CGameInstance::Add_Camera(const WCHAR* szCameraTag, CCamera* pCamera)
{
	return m_pCameraManager->Add_Camera(szCameraTag, pCamera);
}
HRESULT CGameInstance::Remove_Camera(const WCHAR* szCameraTag)
{
	return m_pCameraManager->Remove_Camera(szCameraTag);
}
HRESULT CGameInstance::SetMainCamera(const WCHAR* szCameraTag, _float4x4* pPreCameraMatrix)
{
	return m_pCameraManager->SetMainCamera(szCameraTag, pPreCameraMatrix);
}
CCamera* CGameInstance::GetCamrea(const WCHAR* szCameraTag)
{
	return m_pCameraManager->GetCamrea(szCameraTag);
}
CCamera* CGameInstance::GetMainCamera()
{
	return m_pCameraManager->GetMainCamera();
}
_bool CGameInstance::IsMainCamera(CCamera* pCamera)
{
	return m_pCameraManager->IsMainCamera(pCamera);
}
_matrix CGameInstance::GetMainCameraWorldMatrix()
{
	return m_pCameraManager->GetMainCameraWorldMatrix();
}
const _float4x4* CGameInstance::GetMainCameraWorldMatrixPtr()
{
	return m_pCameraManager->GetMainCameraWorldMatrixPtr();
}
_matrix CGameInstance::GetCameraWorldMatrix(const WCHAR* szCameraTag)
{
	return m_pCameraManager->GetCameraWorldMatrix(szCameraTag);
}
const _float4x4* CGameInstance::GetCameraWorldMatrixPtr(const WCHAR* szCameraTag)
{
	return m_pCameraManager->GetCameraWorldMatrixPtr(szCameraTag);
}
const unordered_map<_wstring, CCamera*>* CGameInstance::GetAllCamera()
{
	return m_pCameraManager->GetAllCamera();
}

#pragma region Physx_Manager

PxControllerManager* CGameInstance::Get_PxCCTManager()
{
	return m_pPhysx_Manager->Get_PxCCTManager();
}

PxScene* CGameInstance::Get_PxScene()
{
	return m_pPhysx_Manager->Get_PxScene();
}

PxPhysics* CGameInstance::Get_PxPhysics()
{
	return m_pPhysx_Manager->Get_PxPhysics();
}

PxTransform CGameInstance::Convert_Matrix_ToPxTransform(_matrix WorldMatrix)
{
	return m_pPhysx_Manager->Convert_Matrix_ToPxTransform(WorldMatrix);
}

_matrix CGameInstance::Convert_PxTransform_ToMatrix(PxTransform Transform)
{
	return m_pPhysx_Manager->Convert_PxTransform_ToMatrix(Transform);
}

HRESULT CGameInstance::Add_CCT_ToPhysx(CGameObject* pGameObject, CCharacterController* pCCT)
{
	return m_pPhysx_Manager->Add_CCT_ToPhysx(pGameObject, pCCT);
}

HRESULT CGameInstance::Add_RigidBody_ToPhysx(CGameObject* pGameObject, CRigidBody* pRigidBody)
{
	return m_pPhysx_Manager->Add_RigidBody_ToPhysx(pGameObject, pRigidBody);
}

HRESULT CGameInstance::Add_Terrain_ToPhysx(CVIBuffer_Terrain* pTerrainVIBuffer)
{
	return m_pPhysx_Manager->Add_Terrain_ToPhysx(pTerrainVIBuffer);
}

#pragma endregion

#pragma region BINPARSER

HRESULT CGameInstance::ReadFbx(const _char* pModelFilePath, MODEL_TYPE eType, binModel** ppOut)
{
	return m_pFbxParser->ReadFbx(pModelFilePath, eType, ppOut);
}

HRESULT CGameInstance::ReadBin(const _char* pModelFilePath, MODEL_TYPE eType, binModel** ppOut)
{
	return m_pBinParser->ReadBin(pModelFilePath, eType, ppOut);
}

HRESULT CGameInstance::WriteBin(const _char* pModelFilePath, MODEL_TYPE eType, binModel** ppOut)
{
	return m_pBinParser->WriteBin(pModelFilePath, eType, ppOut);
}

HRESULT CGameInstance::ReadBinx(const _char* pModelFilePath, MODEL_TYPE eType, binModel** ppOut)
{
	return m_pBinParser->ReadBinx(pModelFilePath, eType, ppOut);
}

HRESULT CGameInstance::WriteBinx(const _char* pModelFilePath, MODEL_TYPE eType, binModel** ppOut)
{
	return m_pBinParser->WriteBinx(pModelFilePath, eType, ppOut);
}

#pragma endregion

#pragma region Interact Manager
void CGameInstance::SetInteractionBaseObject(CGameObject* pObject)
{
	m_pInteract_Manager->SetInteractionBaseObject(pObject);
}

void CGameInstance::ADD_Interaction(CInteraction_Component* pInteraction_Com)
{
	m_pInteract_Manager->ADD_Interaction(pInteraction_Com);
}

void CGameInstance::Remove_Interaction(CInteraction_Component* pInteraction_Com)
{
	return m_pInteract_Manager->Remove_Interaction(pInteraction_Com);
}

CInteraction_Component* CGameInstance::GetNearInteraction()
{
	return m_pInteract_Manager->GetNearInteraction();
}

vector<CInteraction_Component*>* CGameInstance::GetAllInteraction()
{
	return m_pInteract_Manager->GetAllInteraction();
}

#pragma endregion

#pragma region Cinema Manager
HRESULT CGameInstance::ADD_ChinemaSceneData(const WCHAR* szSceneTag, const WCHAR* szTag, CCinemaTrack* pData)
{
	return m_pCinema_Manager->ADD_CutSceneData(szSceneTag, szTag, pData);
}
const CCinemaTrack* CGameInstance::GetCinemaSceneData(const WCHAR* szSceneTag, const WCHAR* szTag)
{
	return m_pCinema_Manager->GetCutSceneData(szSceneTag, szTag);
}

const unordered_map<_wstring, CCinemaTrack*>* CGameInstance::GetCinemaSceneAllDatas(const WCHAR* szSceneTag)
{
	return m_pCinema_Manager->GetSceneAllDatas(szSceneTag);
}

const unordered_map<_wstring, class CCutScene*>* CGameInstance::GetCinemaAllScenes()
{
	return m_pCinema_Manager->GetAllScenes();
}
HRESULT CGameInstance::SaveCinemaSceneData(const WCHAR* szFilePath)
{
	return m_pCinema_Manager->SaveCutSceneData(szFilePath);
}
HRESULT CGameInstance::LoadCinemaSceneData(const WCHAR* szFilePath)
{
	return m_pCinema_Manager->LoadCutSceneData(szFilePath);
}

#pragma endregion

#pragma region Event
HRESULT CGameInstance::Add_Event(const WCHAR* szEventTag, CEventHandle* pEvent)
{
	return m_pEventManager->Add_Event(szEventTag, pEvent);
}

HRESULT CGameInstance::Remove_Event(const WCHAR* szEventTag)
{
	return m_pEventManager->Remove_Event(szEventTag);
}
HRESULT CGameInstance::Bind_Observer(const WCHAR* szEventTag, CEventHandle* pEvent)
{
	return m_pEventManager->Bind_Observer(szEventTag, pEvent);
}
HRESULT CGameInstance::UnBind_Observer(const WCHAR* szEventTag, CEventHandle* pEvent)
{
	return m_pEventManager->UnBind_Observer(szEventTag, pEvent);
}
#pragma endregion


void CGameInstance::GamePauseDurationTime(_uint fStopCnt, _float fTimeRatio, _float fReturnSpeed)
{
	m_bIsHitStopDurationTime = true;
	m_fHitStopReturnSpeed = fReturnSpeed;
	m_fTimeRatio = fTimeRatio;
	m_vLerpTime.x = 0.f;
	m_iHitStopFrame.x = 0;
	m_iHitStopFrame.y = fStopCnt;
}

_float CGameInstance::GetGameSpeedfRatio()
{
	return m_fTimeRatio;
}

void CGameInstance::ResetGameSpeed()
{
	m_fTimeRatio = 1.f;
}

void CGameInstance::SetGameSpeed(_float fRatio)
{
	fRatio = Clamp<_float>(fRatio, 0.001f, 3.f);
	m_fTimeRatio = fRatio;
}

const _uint2& CGameInstance::GetScreenSize()
{
	return m_vScreenSize;

}
const _uint2& CGameInstance::GetHalfScreenSize()
{
	return m_vHalfScreenSize;
}

const WCHAR* CGameInstance::GetFrameText()
{
	return m_szFPS;
}

#ifdef _DEBUG
_float CGameInstance::GetLoopDurationTime(GAMELOOP_TYPE eType)
{
	switch (eType)
	{
	case GAMELOOP_TYPE::PRIORITY:
		return m_pTimer_Manager->Get_TimeDelta(TEXT("Priority_Time"));
	case GAMELOOP_TYPE::UPDATE:
		return m_pTimer_Manager->Get_TimeDelta(TEXT("Update_Time"));;
	case GAMELOOP_TYPE::LATE_UPDATE:
		return m_pTimer_Manager->Get_TimeDelta(TEXT("LateUpdate_Time"));;
	case GAMELOOP_TYPE::COLLISION:
		return m_pTimer_Manager->Get_TimeDelta(TEXT("Collision_Time"));;
	case GAMELOOP_TYPE::RENDER:
		return m_pTimer_Manager->Get_TimeDelta(TEXT("Render_Time"));;
	}

	return -1.f;
}

void CGameInstance::ComputeLoopTime(GAMELOOP_TYPE eType)
{
	switch (eType)
	{
	case GAMELOOP_TYPE::PRIORITY:
		return m_pTimer_Manager->Compute_TimeDelta(TEXT("Priority_Time"));
	case GAMELOOP_TYPE::UPDATE:
		return m_pTimer_Manager->Compute_TimeDelta(TEXT("Update_Time"));;
	case GAMELOOP_TYPE::LATE_UPDATE:
		return m_pTimer_Manager->Compute_TimeDelta(TEXT("LateUpdate_Time"));;
	case GAMELOOP_TYPE::COLLISION:
		return m_pTimer_Manager->Compute_TimeDelta(TEXT("Collision_Time"));;
	case GAMELOOP_TYPE::RENDER:
		return m_pTimer_Manager->Compute_TimeDelta(TEXT("Render_Time"));;
	}
}
#endif

void CGameInstance::Release_Engine()
{
	DestroyInstance();

	Safe_Release(m_pLevel_Manager);
	Safe_Release(m_pThreadPool);
	Safe_Release(m_pFrustum);
	Safe_Release(m_pCameraManager);
	Safe_Release(m_pShadow);
	Safe_Release(m_pPicking);
	Safe_Release(m_pTarget_Manager);
	Safe_Release(m_pFont_Manager);
	Safe_Release(m_pLight_Manager);
	Safe_Release(m_pPipeLine);
	Safe_Release(m_pTimer_Manager);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pSoundManager);
	Safe_Release(m_pEffect_ResourceManager);
	Safe_Release(m_pCollisionManager);
	Safe_Release(m_pPrototype_Manager);
	Safe_Release(m_pObject_Manager);
	Safe_Release(m_pInteract_Manager);
	Safe_Release(m_pEventManager);
	Safe_Release(m_pBinParser);
	Safe_Release(m_pCinema_Manager);
	Safe_Release(m_pFbxParser);
	Safe_Release(m_pInput_Device);
	Safe_Release(m_pGraphic_Device);
	Safe_Release(m_pPhysx_Manager);
}

void CGameInstance::Free()
{
	__super::Free();

	
}
